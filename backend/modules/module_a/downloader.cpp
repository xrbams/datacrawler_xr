#include "downloader.hpp"

Downloader::~Downloader(){
    stop = true;
    for (auto & w : workers){
        if (w.joinable()) w.join();
    }
    
}

static void init_openssl(){
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

SSL_CTX* create_ssl_context(){
    SSL_CTX* ctx = SSL_CTX_new(TLS_method());
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);  // Force TLS 1.2+
    SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);
    if(!ctx){
        std::cerr << "Error creating SSL contect\n";
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

std::string extract_hostname(const std::string& url) { 
    std::string clean_url = url;
    if (clean_url.find("https://") == 0) {
        clean_url = clean_url.substr(8);
    } else if (clean_url.find("http://") == 0) {
        clean_url = clean_url.substr(7);
    }

    size_t slash_pos = clean_url.find('/');
    if (slash_pos != std::string::npos) {
        return clean_url.substr(0, slash_pos);  // Extract only the domain
    }
    return clean_url;
}

std::string extract_path(const std::string& url) {
    size_t slash_pos = url.find('/', 8);  // Skip 'https://' or 'http://'
    if (slash_pos != std::string::npos) {
        return url.substr(slash_pos);  // Keep everything after the domain
    }
    return "/";  // Default to root "/"
}
int Downloader::status_code = 0;


std::string http_get(const std::string &hostname, const std::string &path){
    struct addrinfo hints{}, *res;
    int sockfd;

    //Zero out the hints structure
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // ipv4 AF_UNSPEC - for it to choose any
    hints.ai_socktype = SOCK_STREAM; // TCP

    // Resolve the hostname to an IP address
    cout << " Pulled: " << hostname.c_str() << endl;

    int status = getaddrinfo(hostname.c_str(), "443", &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(status) << std::endl;
        return "";
    }

    // Create a socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0){
        perror("socket failed");
        freeaddrinfo(res);
        return "";
    }

    // Connect to the server
    if(connect(sockfd, res->ai_addr, res->ai_addrlen) < 0){
        perror("connect failed");
        close(sockfd);
        freeaddrinfo(res);
        return "";
    }

    freeaddrinfo(res); // We dont need address info anymore

    // Initialize OpenSSL
    init_openssl();
    SSL_CTX* ctx = create_ssl_context();
    SSL* ssl = SSL_new(ctx);
    SSL_set_tlsext_host_name(ssl, hostname.c_str());
    SSL_set_fd(ssl, sockfd);

    // Perform SSL handshake
    if(SSL_connect(ssl) != 1){
        std::cerr << "SSL connection failed \n";
        ERR_print_errors_fp(stderr);
        close(sockfd);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return "";
    }

    
    // Construct a HTTP GET req
    std::string request = "GET " + path + " HTTP/1.1\r\n";
    request += "Host: " + hostname + "\r\n";
    request += "Connection: close\r\n\r\n";

    // Send request, Alternative to send()
    SSL_write(ssl, request.c_str(), request.length());

    // Receive response
    char buffer[BUFFER_SIZE];
    string response;
    ssize_t bytes_received;

    // Alternative to Recv for https connections
    while ((bytes_received = SSL_read(ssl, buffer, BUFFER_SIZE - 1)) > 0){ 
        buffer[bytes_received] = '\0';
        response += buffer;
    }
    
    if (bytes_received < 0){
        perror("recv failed");
    }

    {
        size_t line_end = response.find("\r\n");
        if (line_end != std::string::npos) {
            std::string status_line = response.substr(0, line_end);

            // Split the status line by spaces
            size_t first_space = status_line.find(" ");
            size_t second_space = status_line.find(" ", first_space + 1);
            
            if (first_space != std::string::npos && second_space != std::string::npos) {
                // Extract the status code as a string and convert to an integer
                std::string status_code_str = status_line.substr(first_space + 1, second_space - first_space - 1);
                try {
                    int scode = std::stoi(status_code_str);
                    Downloader::setStatusCode(scode);
                    // std::cout << "Status Code: " << scode << std::endl;
                } catch (const std::exception &e) {
                    std::cerr << "Failed to convert status code: " << e.what() << std::endl;
                }
                // std::cout << "Status Code: " << scode << std::endl;
            } else {
                std::cerr << "Failed to parse status code" << std::endl;
            }
        } else {
            std::cerr << "No status line found in response" << std::endl;
        }
    }
    

    // Cleanup
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sockfd);
    
    return response;
}

static size_t writeCallback(void* ptr, size_t size, size_t nmemb, void* userdata){
    size_t t_size = size * nmemb;
    unique_ptr<string>* response = static_cast<std::unique_ptr<std::string>*>(userdata);
    response->get()->append(static_cast<char*>(ptr), t_size); // store data

    return t_size; 
}

// this right now gets triggered in main, 
// but ideally we want the scheduler to handle this.
void Downloader::enqueueUrl(const string& url) {
    url_queue.push(url);
}

void Downloader::start(int num){
    for (int i = 0; i < num; ++i)
    {
        workers.emplace_back(&Downloader::worker, this);
    }
    
}

void Downloader::worker(){
    // call storage instance
    Database db;
    while (!stop)
    {
        string url;
        if(url_queue.pop(url)) {
            CURL* curl = curl_easy_init();
            if (!curl) continue;
            std::string hostname = extract_hostname(url); //just for http_get
            std::string path = extract_path(url);
            std::string html = http_get(hostname, path); //fetch(url); //url

            if (!html.empty()) {
                // std::cout << "Downloaded: " << url << std::endl;
                // 🔥 Store the result in a queue
                std::lock_guard<std::mutex> lock(resultsMutex);
                results.push({url, html});

                Content content = Parser::parse_content(html, url);

                auto header = std::make_unique<Header>("text/html", html.length());
                auto crawl = std::make_unique<CrawlMetadata>(0, url, getCurrentTimestamp());
                Metadata meta(std::move(header), std::make_unique<Content>(content), std::move(crawl));

                json jsonData = serializer::serialize(meta);
                db.saveData(jsonData);

                // 🔥 Save extracted links to MongoDB and enqueue them
                for (const std::string& link : content.links) {
                    db.saveLink(link, url);  // Store link in MongoDB
                    url_queue.push(link);    // Add link to queue for crawling
                }

            }else {
                std::cerr << "Failed to download: " << url << std::endl;
            }


        }
    }
}

string Downloader::fetch(const std::string& url) { //
    CURL* curl = curl_easy_init();

    if(!curl) {
        std::cerr << "Failed to initialize CURL \n"; 
        return "";
    }
    auto res_data = make_unique<string>();

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // specify URL, change it to const char*
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects for SSL/TLS handshake
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback); // collect response
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res_data);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    // int max_retries = 3;
    // int attempts = 0;
    // CURLcode res;

    // do {
    //     res = curl_easy_perform(curl);
    //     if (res != CURLE_OK) {
    //         std::cerr << "Attempt " << (attempts + 1) << " failed: " 
    //                   << curl_easy_strerror(res) << "\n";
    //     }
    //     attempts++;
    // } while (res != CURLE_OK && attempts < max_retries);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        return "";
    }

    //std::cout << "\nFull Response:\n" << *res_data << std::endl;

    // Get HTTP status code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    curl_easy_cleanup(curl);

    if (status_code != 200) {
        std::cerr << "HTTP error: " << status_code << " for " << url << "\n";
        return "";
    }

    return *res_data;
}

string Downloader::getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&now_c), "%Y-%m-%dT%H:%M:%SZ"); // ISO 8601
        return oss.str();
}


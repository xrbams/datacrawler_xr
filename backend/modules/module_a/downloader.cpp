#include "downloader.hpp"

Downloader::~Downloader(){}

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
// int Downloader::status_code = 0;


std::string http_get(const std::string &hostname, const std::string &path){
    struct addrinfo hints{}, *res;
    int sockfd;

    //Zero out the hints structure
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // ipv4 AF_UNSPEC - for it to choose any
    hints.ai_socktype = SOCK_STREAM; // TCP

    // Resolve the hostname to an IP address
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
                std::cout << "url: " << hostname.c_str() << path.c_str() << " Status Code: " << status_code_str << std::endl; // lazy solution but if it works 
           
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

std::string http_get_robots(const std::string &hostname){
    struct addrinfo hints{}, *res;
    int sockfd;

    //Zero out the hints structure
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // ipv4 AF_UNSPEC - for it to choose any
    hints.ai_socktype = SOCK_STREAM; // TCP

    // Resolve the hostname to an IP address
    // cout << " Pulled: " << hostname.c_str() << endl;

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

    
    // Construct an HTTP GET request for the robots.txt
    std::string request = "GET /robots.txt HTTP/1.1\r\n";
    request += "Host: " + hostname + "\r\n";
    request += "Connection: close\r\n";
    request += "User-Agent: DataCrawler_XR/1.0 (Created by xrbams)\r\n\r\n";  // Optional: Add a custom User-Agent

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
        // Extract the body from the response (skip headers)
        size_t body_start = response.find("\r\n\r\n"); // Find where headers end
        if (body_start != std::string::npos) {
            response = response.substr(body_start + 4); // Get the content after the headers
        }
    }
    

    // Cleanup
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sockfd);
    
    return response;


}


string Downloader::getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&now_c), "%Y-%m-%dT%H:%M:%SZ"); // ISO 8601
        return oss.str();
}


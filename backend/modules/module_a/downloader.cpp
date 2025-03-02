#include "downloader.hpp"

Downloader::~Downloader(){
    stop = true;
    for (auto & w : workers){
        if (w.joinable()) w.join();
    }
    
}

static size_t writeCallback(void* ptr, size_t size, size_t nmemb, void* userdata){
    size_t t_size = size * nmemb;
    unique_ptr<string>* response = static_cast<std::unique_ptr<std::string>*>(userdata);
    response->get()->append(static_cast<char*>(ptr), t_size); // store data

    return t_size; 
}

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
    while (!stop)
    {
        string url;
        if(url_queue.pop(url)) {
            CURL* curl = curl_easy_init();
            if (!curl) continue;

            std::string html = fetch(url); //url

            if (!html.empty()) {
                std::cout << "Downloaded: " << url << std::endl;
                // 🔥 Store the result in a queue
                std::lock_guard<std::mutex> lock(resultsMutex);
                results.push({url, html}); 
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


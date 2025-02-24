//Multi-threaded Downloader

/*

- downloads webpages from URLs
- Parse HTML
- send extracted links to module_c
- handles robot.txt and site-specific rate limits

Networking: libcurl or Boost.Asio (asynchronous networking).
Parsing: Gumbo, libxml2, or HTML Tidy.
Multithreading: std::thread, std::async, or Boost.Thread.

*/

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <curl/curl.h>  // Requires libcurl

/*
- Fetching Content
- handling status codes
- returning data.
*/
class Downloader
{
private:
std::string url = "";
int status_code;
std::string timestamp; // timestamp use time

// CURL callback function (not storing content yet)
static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp){
    return size * nmemb; // Discard data for now
}
    
public:
    explicit Downloader(const std::string& url) : url(url), status_code(0), timestamp(getCurrentTimestamp()) {}

    ~Downloader();

    bool fetch() {
        CURL* curl = curl_easy_init();

        if(!curl) std::cerr << "Failed to initialize CURL \n";

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << "\n";
            curl_easy_cleanup(curl);
            return false;
        }

        // Get HTTP status code
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
        curl_easy_cleanup(curl);

        return true;
    }

    // Getters
    std::string getUrl() const { return url; }
    int getStatusCode() const { return status_code; }
    std::string getTimestamp() const { return timestamp; }

    // Static method to get current timestamp
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&now_c), "%Y-%m-%dT%H:%M:%SZ"); // ISO 8601
        return oss.str();
    }


};

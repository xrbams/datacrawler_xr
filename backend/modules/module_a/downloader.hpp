#ifndef DOWNLOADER_H
#define DOWNLOADER_H

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
// include libraries
#include <iostream>
#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <sstream>
#include <iomanip>
#include <thread>
#include <curl/curl.h>
#include <queue>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "../module_c/queue.hpp"
#include "../module_b/serializer.hpp"
#include "../module_b/storage.hpp"

using namespace std;

/*
- Fetching Content
- handling status codes
- returning data.
*/
#define BUFFER_SIZE 4096 

std::string http_get(const std::string &hostname, const std::string &path);
std::string http_get_robots(const std::string &hostname);
std::string extract_hostname(const std::string& url);
std::string extract_path(const std::string& url);

class Downloader
{
private:
std::string url = "";
int status_code;
std::string timestamp; 

Sec_Queue<std::string> url_queue;
vector<thread> workers;
bool stop;

std::queue<std::pair<std::string, std::string>> results;  // (html, url)
std::mutex resultsMutex;

public:
    Downloader() : timestamp(getCurrentTimestamp()), status_code(0) {}
    explicit Downloader(const std::string& url) : url(url), status_code(0), timestamp(getCurrentTimestamp()), stop(false) {}
    ~Downloader();
    // fetch data and create timestamp.
    string fetch(const std::string& url); // const std::string& url 
    // std::string http_get(const std::string &hostname, const std::string &path); 
    static std::string getCurrentTimestamp();

    void enqueueUrl(const string& url);
    void start(int num);
    void worker();

    // for debugging purposes
    bool hasResults() { 
        std::lock_guard<std::mutex> lock(resultsMutex);
        return !results.empty();
    }

    // for debugging purposes
    std::pair<std::string, std::string> getResult() {
        std::lock_guard<std::mutex> lock(resultsMutex);
        if (results.empty()) return {"", ""};
        
        auto result = results.front();
        results.pop();
        return result;
    }

    // Setters
    void setStatusCode(int code){ status_code = code; }

    // Getters
    std::string getUrl() const { return url; }
    int getStatusCode() const { return status_code; }
    std::string getTimestamp() const { return timestamp; }

};

#endif
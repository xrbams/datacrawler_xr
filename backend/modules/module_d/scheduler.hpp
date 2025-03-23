// Module D: Scheduler

/*
- Select URLs from Module C (Queue) and assign them to Module A (Downloader).
- Apply rate limiting and site-specific policies (respect robots.txt).
- Implement crawl depth limits (so it doesn’t go infinitely deep).

- Task Scheduling: std::thread or Boost.Asio.
- Rate Limiting: Token bucket algorithm (e.g., leaky bucket logic).
- Concurrency Control: Mutex locks or thread-safe queues (e.g., boost::lockfree::queue).

*/

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <sstream>
#include <regex>

#include "../module_a/downloader.hpp"
#include "../module_a/metadata.hpp"
#include "../module_a/parser.hpp"
#include "../module_b/serializer.hpp"
#include "../module_b/storage.hpp"

// Robot parser
class Robot{
    std::unordered_map<std::string, std::vector<std::string>> rules;
    std::string userAgent = "DataCrawler_XR/1.0 (Created by xrbams)\r\n\r\n";

public:
    void parser(const std::string& content);
    bool isAllowed(const std::string& url) const;

};

// Token Buckett Rate limiter
class TokenBucket{
    double tokens;
    double maxTokens;
    double refillRate;
    std::chrono::steady_clock::time_point lastRefill;

public:
    TokenBucket(double rate) : tokens(rate), maxTokens(rate), refillRate(rate) {
        lastRefill = std::chrono::steady_clock::now();
    }

    void waitForToken(){
        while(true){
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed = now - lastRefill;

            // Refill tokens
            tokens += elapsed.count() * refillRate;
            std::cout << "Tokens: " << tokens << std::flush;
            if(tokens > maxTokens) tokens = maxTokens;
            lastRefill = now;

            if(tokens >= 1.0){
                tokens -= 1.0;
                std::cout << " Token granted, proceeding to crawl!" << std::endl;  // Add confirmation here
                return;
            }
            // Sleep for a short
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

    }

};

// Depth Tracker
class DepthTracker{
    std::unordered_map<std::string, int> urlDepth;
    int maxDepth;

public:
    DepthTracker(int max) : maxDepth(maxDepth){}
    bool canCrawl(const std::string &url){
        if (urlDepth.find(url) == urlDepth.end()) {
            urlDepth[url] = 0;  // Initialize new URL with depth 0
        }
        int curr = urlDepth[url];
        if(curr >= maxDepth) return false;
        urlDepth[url] = curr++;
        return true;
    }

    int getDepth(const std::string& url) const { 
        // Step 1: Remove query parameters and fragments
        size_t queryPos = url.find('?');
        size_t fragmentPos = url.find('#');

        // Find the earliest occurrence of '?' or '#'
        size_t endPos = std::min(queryPos == std::string::npos ? url.length() : queryPos,
                                fragmentPos == std::string::npos ? url.length() : fragmentPos);

        // Step 2: Extract the path part of the URL (ignore query params and fragments)
        std::string path = url.substr(0, endPos);

        // Step 3: Calculate depth based on slashes in the path
        int depth = 0;
        size_t pos = 0;

        while ((pos = url.find('/', pos)) != std::string::npos) {
            ++depth;
            ++pos;
        }

        return depth;
    }
};

// Scheduler
class Scheduler{
    Sec_Queue<std::string> url_queue;
    vector<thread> workers;
    bool stop;

    std::queue<std::tuple<std::string, std::string, std::string>> results;  // (html, url)
    std::mutex resultsMutex;
public:
    Scheduler() = default;
    ~Scheduler();
    // for debugging purposes
    bool hasResults() { 
        std::lock_guard<std::mutex> lock(resultsMutex);
        return !results.empty();
    }
    static std::string getCurrentTimestamp();
    void enqueueUrl(const string& url);
    void start(int num);
    void worker();
    void scheduler();
};

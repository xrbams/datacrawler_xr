#include "scheduler.hpp"

Scheduler::~Scheduler(){
    stop = true;
    for (auto & w : workers){
        if (w.joinable()) w.join();
    }
}
std::string getPath(const std::string& url){
        auto pos = url.find("://");
        if (pos != std::string::npos) {
            pos = url.find("/", pos + 3);
            if (pos != std::string::npos) {
                return url.substr(pos);
            }
        }
        return "/";
    }

bool Robot::isAllowed(const std::string& url) const {
        std::string path = getPath(url); // Extract path from URL

        auto it = rules.find(userAgent);
        if (it != rules.end()) {
            for (const auto& i : it->second) {
                if (i.empty()) continue;  // Empty disallow = everything allowed
                if (path == i || (path.compare(0, i.size(), i) == 0)) return false;  // Proper path match
            }
        }
        return true;

    }


void Robot::parser(const std::string& content){
        std::istringstream ss(content);
        std::string line;
        std::string curr = "*";

        while (std::getline(ss, line))
        {
            line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), ""); 
            if (line.empty() || line[0] == '#') continue;

            auto pos = line.find(":");
            if(pos == std::string::npos) continue;

            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            key = std::regex_replace(key, std::regex("^\\s+|\\s+$"), "");
            value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");

            if(key == "User-Agent"){
                curr = value;
            }else if(key == "Disallow"){
                rules[curr].push_back(value);
            }
            
        }

    }


void Scheduler::enqueueUrl(const string& url) {
    url_queue.push(url);
}

void Scheduler::start(int num){
    for (int i = 0; i < num; ++i)
    {
        workers.emplace_back(&Scheduler::worker, this);
    }
}

string Scheduler::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&now_c), "%Y-%m-%dT%H:%M:%SZ"); // ISO 8601
    return oss.str();
}


void Scheduler::worker(){
    // call storage instance
    // Database db;
    Robot robo;
    TokenBucket limit(6);
    DepthTracker tracker(6);
    while (!stop)
    {
        string url;
        if(url_queue.pop(url)) {
            CURL* curl = curl_easy_init();
            if (!curl) continue;
            std::string hostname = extract_hostname(url); //just for http_get
            std::string path = extract_path(url);
            std::string robotxt = http_get_robots(hostname);
            std::string html;

            if(!robotxt.empty()){
                robo.parser(robotxt);
                if(!robo.isAllowed(url)){
                    cout << "Blocked by robots: " << url << endl;
                    // continue;
                }

                if(!tracker.canCrawl(hostname + path)){
                    std::cout << "Checking URL: " << hostname + path << " Depth: " << tracker.getDepth(hostname + path) << std::endl;
                    // continue;
                }

                // Rate limiting
                limit.waitForToken();

                // Download the good urls
                html = http_get(hostname, path); // url: and status code come from here
            }
            
        
            if (!html.empty()) {
                // std::cout << "Downloaded: " << url << std::endl;
                // 🔥 Store the result in a queue
                // std::lock_guard<std::mutex> lock(resultsMutex);
                // results.push((url, html));

                Content content = Parser::parse_content(html, url);

                auto header = std::make_unique<Header>("text/html", html.length());
                auto crawl = std::make_unique<CrawlMetadata>(0, url, getCurrentTimestamp());
                Metadata meta(std::move(header), std::make_unique<Content>(content), std::move(crawl));

                // Save to JSON send to db.
                // json jsonData = serializer::serialize(meta);
                // db.saveData(jsonData);

                // 🔥 Save extracted links to MongoDB and enqueue them
                
                for (const std::string& link : content.links) {
                    // db.saveLink(link, url);  // Store link in MongoDB
                    // std::cout << "Links: " << link << std::endl;
                    url_queue.push(link);    // Add link to queue for crawling
                }

            }else {
                std::cerr << "Failed to download: " << url << std::endl;
            }


        }
    }
}

// Scheduler
void Scheduler::scheduler(){
    
    Downloader d;
    start(7);

    enqueueUrl("https://www.janestreet.com");
    enqueueUrl("https://leetcode.com/");
    enqueueUrl("https://tryhackme.com");
    enqueueUrl("https://www.google.com");
    enqueueUrl("https://neetcode.io");
    enqueueUrl("https://www.youtube.com");

    std::this_thread::sleep_for(std::chrono::seconds(10));

    // std::vector<std::string> links = {
    //     "https://www.janestreet.com/welcome-guide/",
    //     "https://www.janestreet.com/bonus-problems/",
    //     "https://www.janestreet.com/apply-quantitative-researcher/",
    //     "https://www.janestreet.com/watch?v=dQw4w9WgXcQ/wabo/erfg/wsdf",
    //     "https://www.janestreet.com/results?search_query=robotics"
    // };

    // std::string url = "https://www.janestreet.com";
    // std::string hostname = extract_hostname(url);

    // std::string robotxt = http_get_robots(hostname);

}
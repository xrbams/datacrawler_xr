#include "scheduler.hpp"

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
                if (path.compare(0, i.size(), i) == 0) return false;  // Proper path match
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

// Scheduler
void scheduler(){
    Robot robo;
    TokenBucket limit(5);
    DepthTracker tracker(5);
    // Downloader d; //(url)
    // d.start(7);

    // d.enqueueUrl("https://www.janestreet.com");
    // d.enqueueUrl("https://www.leetcode.com");
    // d.enqueueUrl("https://tryhackme.com");
    // d.enqueueUrl("https://www.google.com");
    // d.enqueueUrl("https://www.neetcode.io");
    // d.enqueueUrl("https://youtube.com");

    std::vector<std::string> links = {
        "https://www.janestreet.com/welcome-guide/",
        "https://www.janestreet.com/bonus-problems/",
        "https://www.janestreet.com/apply-quantitative-researcher/",
        "https://www.janestreet.com/watch?v=dQw4w9WgXcQ/wabo/erfg/wsdf",
        "https://www.janestreet.com/results?search_query=robotics"
    };

    std::string url = "https://www.janestreet.com";
    std::string hostname = extract_hostname(url);

    std::string robotxt = http_get_robots(hostname);
    robo.parser(robotxt);
     for (const auto &i : links){
        if(!robo.isAllowed(i)){
            cout << "Blocked by robots: " << i << endl;
             continue;
        }

        if(!tracker.canCrawl(i)){
            std::cout << "Checking URL: " << i << " Depth: " << tracker.getDepth(i) << std::endl;
             continue;
        }

        // Rate limiting
        limit.waitForToken();
     }

}
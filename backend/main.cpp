#include "./modules/module_d/scheduler.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    cout << "This is my first go at Concurrent Web Crawler" << endl;

    Scheduler s;
    s.scheduler();
    // string url = "https://www.databricks.com";
    // std::string hostname = extract_hostname(url); //just for http_get
    // std::string path = extract_path(url);
    // string html = http_get(hostname, path);
    // cout << "HTML: " << html << endl;

    // Downloader d; //(url)
    // d.start(4);

    // d.enqueueUrl("https://www.xxl.fi");
    // d.enqueueUrl("https://www.leetcode.com");
    // d.enqueueUrl("https://tryhackme.com");

    // std::this_thread::sleep_for(std::chrono::seconds(10));
    
    // while (true){

    //     auto [url, html] = d.getResult();

    //     if(!html.empty()){
    //         cout << "URL: " << url << endl;
    //         cout << "Status: " << d.getStatusCode() << endl;
    //         cout << "Crawled At: " << d.getTimestamp() << endl;
            
    //         Content cont = Parser::parse_content(html, url);

    //         auto header = std::make_unique<Header>("text/html", html.length());
    //         auto content = std::make_unique<Content>(cont);
    //         auto crawl = std::make_unique<CrawlMetadata>(0, url, d.getTimestamp());
    //         Metadata meta(std::move(header), std::move(content), std::move(crawl));

    //         meta.display();
    //     } 
    // }

    return 0;
}

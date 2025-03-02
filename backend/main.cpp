#include "modules/module_a/downloader.hpp"
// #include "modules/module_a/metadata.hpp"
#include "modules/module_a/parser.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    cout << "This is my first go at Concurrent Web Crawler" << endl;
    string url = "";
    Downloader d; //(url)
    d.start(4);

    d.enqueueUrl("https://www.youtube.com");
    d.enqueueUrl("https://www.google.com");
    d.enqueueUrl("https://www.neetcode.io");

    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    while (d.hasResults()){

        auto [url, html] = d.getResult();

        if(!html.empty()){
            cout << "URL: " << url << endl;
            cout << "Status: " << d.getStatusCode() << endl;
            cout << "Crawled At: " << d.getTimestamp() << endl;
            
            Content cont = Parser::parse_content(html, url);

            auto header = std::make_unique<Header>("text/html", html.length());
            auto content = std::make_unique<Content>(cont);
            auto crawl = std::make_unique<CrawlMetadata>(0, url, d.getTimestamp());
            Metadata meta(std::move(header), std::move(content), std::move(crawl));

            meta.display();
        } 
    }

    return 0;
}

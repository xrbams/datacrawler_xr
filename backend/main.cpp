#include "modules/module_a/downloader.cpp"
#include "modules/module_a/metadata.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    cout << "This is my first go at Concurrent Web Crawler" << endl;
    Downloader d("https://www.google.com");
    string html = d.fetch();
    

    if(!html.empty()){
        cout << "URL: " << d.getUrl() << endl;
        cout << "Status: " << d.getStatusCode() << endl;
        cout << "Crawled At: " << d.getTimestamp() << endl;
        
        Metadata data;
        Content cont = data.parse_content(html);

        std::cout << "Title: " << cont.title << std::endl;
        std::cout << "Meta Description: " << cont.meta_description << std::endl;
        std::cout << "Extracted Links: " << std::endl;
        for (const auto& link : cont.links) {
            std::cout << "  - " << link << std::endl;
        }
    }
    //getstorage();
    return 0;
}

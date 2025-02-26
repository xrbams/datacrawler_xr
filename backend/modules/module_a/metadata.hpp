#ifndef METADATA_H
#define METADATA_H

#include <iostream>
#include <vector>
#include <memory>
using namespace std;

struct Header
{
    std::string content_type;
    int content_length;

    // Constructor for initialization
    Header(std::string type, int length) : content_type(std::move(type)), content_length(length) {}
};

struct Content
{
    std::string title;
    std::string meta_description;
    std::string text;
    std::vector<std::string> links;
    Content() = default;
    Content(std::string t, std::string desc, std::string txt, std::vector<std::string> lnks) :
        title(std::move(t)), meta_description(std::move(desc)), text(std::move(txt)), links(std::move(lnks)) {}
};

struct CrawlMetadata 
{
   int depth;
   std::string parent_url;
   std::string crawled_at; //timestamp

   CrawlMetadata() = default;
   CrawlMetadata(int d, std::string parent, std::string timestamp)
    : depth(d), parent_url(std::move(parent)), crawled_at(std::move(timestamp)) {}
};

class Metadata
{
private:
    std::unique_ptr<Header> header;
    std::unique_ptr<Content> content;
    CrawlMetadata crawl;
    
public:
    Metadata()
        : header(std::make_unique<Header>("", 0)),
          content(std::make_unique<Content>("", "", "", std::vector<std::string>())) {}

    Metadata(std::unique_ptr<Header> h, std::unique_ptr<Content> c, CrawlMetadata cm)
        : header(std::move(h)), content(std::move(c)), crawl(std::move(cm)) {}
    ~Metadata() = default;
    
    Header parse_header(const std::string& html);
    Content parse_content(const std::string& html);
    CrawlMetadata parse_metadata(const std::string& html);

    // Display function (For Debugging)
    // void display() const {
    //     std::cout << "Header:\n  Content-Type: " << header->content_type
    //               << "\n  Content-Length: " << header->content_length << "\n";

    //     std::cout << "Content:\n  Title: " << content->title
    //               << "\n  Meta Description: " << content->meta_description
    //               << "\n  Text Snippet: " << content->text.substr(0, 50) << "...\n  Links:\n";
    //     for (const auto& link : content->links)
    //         std::cout << "    " << link << "\n";

    //     std::cout << "Crawl Metadata:\n  Depth: " << crawl->depth
    //               << "\n  Parent URL: " << crawl->parent_url
    //               << "\n  Crawled At: " << crawl->crawled_at << "\n";
    // }
};

// Metadata::Metadata(){}
// Metadata::~Metadata(){}

#endif
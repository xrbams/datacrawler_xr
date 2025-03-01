// metadata.hpp
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
    std::unique_ptr<CrawlMetadata> crawl;
    
public:
    Metadata()
        : header(std::make_unique<Header>("", 0)),
          content(std::make_unique<Content>("", "", "", std::vector<std::string>())) {}

    Metadata(std::unique_ptr<Header> h, std::unique_ptr<Content> c, std::unique_ptr<CrawlMetadata> cm)
        : header(std::move(h)), content(std::move(c)), crawl(std::move(cm)) {}
    ~Metadata() = default;
    
    void display() const {
        std::cout << "Title: " << content->title << "\n";
        std::cout << "Description: " << content->meta_description << "\n";
        std::cout << "Links: ";
        for (const auto& link : content->links) {
            std::cout << "  - " << link << std::endl;
        }
    }
};

#endif
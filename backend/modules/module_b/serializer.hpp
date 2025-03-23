#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <nlohmann/json.hpp>  
#include "../module_a/parser.hpp"

using json = nlohmann::json;

class serializer
{
private:
    /* data */
public:
    serializer(/* args */) = default;
    ~serializer() = default;

    static json serialize(const Metadata& meta){
        json j;
        j["url"] = meta.getCrawlMetadata()->parent_url;  // FIXED
        j["status"] = 200;
        j["timestamp"] = meta.getCrawlMetadata()->crawled_at;  // FIXED

        j["headers"] = {
            {"content-type", meta.getHeader()->content_type},
            {"content-length", meta.getHeader()->content_length}
        };

        j["content"] = {
            {"title", meta.getContent()->title},
            {"meta_description", meta.getContent()->meta_description},
            {"text", meta.getContent()->text},
            {"links", meta.getContent()->links} // FIXED: Directly accessing vector
        };

        j["crawl_metadata"] = {
            {"depth", meta.getCrawlMetadata()->depth},
            {"parent_url", meta.getCrawlMetadata()->parent_url},
            {"crawled_at", meta.getCrawlMetadata()->crawled_at}
        };
        return j;
    }


};

#endif
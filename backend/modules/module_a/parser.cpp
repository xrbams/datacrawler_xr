#include <gumbo.h>

#include "downloader.cpp"
#include "metadata.cpp"

string extractText(unique_ptr<GumboNode>* node){
    if(node->get()->type == GUMBO_NODE_TEXT) {
        return node->get()->v.text.text;
    }
    if (node->get()->type == GUMBO_NODE_ELEMENT 
            && node->get()->v.element.children.length > 0)
    {
        return extractText(static_cast<unique_ptr<GumboNode>*>(node->get()->v.element.children.data[0]));
    }
    return "";
}

void extractLinks(unique_ptr<GumboNode>* node, vector<string>& links) {
    if(node->get()->type != GUMBO_NODE_ELEMENT) return;

    if(node->get()->v.element.tag == GUMBO_TAG_A){
        GumboAttribute* href = gumbo_get_attribute(&node->get()->v.element.attributes, "href");
        if(href) links.push_back(href->value);
    }
    GumboVector children = node->get()->v.element.children;
    for (size_t i = 0; i < children.length; ++i)
    {
        extractLinks(static_cast<unique_ptr<GumboNode>*>(children.data[i]), links);
    }
}

Header Metadata::parse_header(const std::string& html) {}
Content Metadata::parse_content(const std::string& html) {}
CrawlMetadata Metadata::parse_metadata(const std::string& html) {}
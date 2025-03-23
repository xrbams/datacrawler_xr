// parser.cpp
#include "parser.hpp"


 string extractText(GumboNode* node){
    if(node->type == GUMBO_NODE_TEXT) {
        return node->v.text.text;
    }
    if (node->type == GUMBO_NODE_ELEMENT 
            && node->v.element.children.length > 0)
    {
        return extractText(static_cast<GumboNode*>(node->v.element.children.data[0]));
    }
    return "";
}

std::string cleanURL(const std::string& url, const std::string& base_url) {
    if (url.empty()) return "";

    // Convert relative URL to absolute URL
    if (url[0] == '/') {
        return base_url + url;
    }

    // Remove tracking parameters (e.g., utm_source, utm_medium, etc.)
    std::regex tracking_params(R"([?&](utm_[^&]+|ref=[^&]+))");
    return std::regex_replace(url, tracking_params, "");
}

void extractLinks(GumboNode* node, std::unordered_set<std::string>& unique_links, const std::string& base_url) {
    if (node->type != GUMBO_NODE_ELEMENT) return;

    if (node->v.element.tag == GUMBO_TAG_A) {
        GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (href) {
            std::string cleaned = cleanURL(href->value, base_url);
            unique_links.insert(cleaned);
        }
    }

    GumboVector children = node->v.element.children;
    for (size_t i = 0; i < children.length; ++i) {
        extractLinks(static_cast<GumboNode*>(children.data[i]), unique_links, base_url);
    }
}


Header Parser::parse_header(const std::string& html) {}

Content Parser::parse_content(const std::string& html, const std::string& base_url) {
    Content content("", "", "", {});
    std::unordered_set<std::string> unique_links;

    GumboOutput* output = gumbo_parse(html.c_str());
    GumboVector* rootChildren = &output->root->v.element.children;


    // Parse the Title
    GumboNode* head = nullptr;
    for (size_t i = 0; i < rootChildren->length; ++i) {
        GumboNode* node = static_cast<GumboNode*>(rootChildren->data[i]);
        if (node->type == GUMBO_NODE_ELEMENT && node->v.element.tag == GUMBO_TAG_HEAD) {
            head = node; 
            break;
        }
    }
    if (head) {  // Now, look inside head for <title>
        GumboVector* headChildren = &head->v.element.children;
        for (size_t i = 0; i < headChildren->length; ++i) {
            GumboNode* node = static_cast<GumboNode*>(headChildren->data[i]);
            if (node->type == GUMBO_NODE_ELEMENT && node->v.element.tag == GUMBO_TAG_META) {
                GumboAttribute* nameAttr = gumbo_get_attribute(&node->v.element.attributes, "name");
                GumboAttribute* contentAttr = gumbo_get_attribute(&node->v.element.attributes, "content");
                if (nameAttr && contentAttr) {
                    // Store the description if it's a meta description tag
                    if (std::string(nameAttr->value) == "description") {
                        content.meta_description = contentAttr->value;
                    }
                }
            }
            
            if (node->type == GUMBO_NODE_ELEMENT && node->v.element.tag == GUMBO_TAG_TITLE) {
                
                content.title = extractText(node);
            }
        }
    }
    // Parse the extracted links 
    for (size_t i = 0; i < rootChildren->length; ++i)
    {
        GumboNode* node = static_cast<GumboNode*>(rootChildren->data[i]);

        if (node->type == GUMBO_NODE_ELEMENT)
        {
            extractLinks(node, unique_links, base_url);
        }
        
    }

    // Move unique links into the content structure
    content.links.assign(unique_links.begin(), unique_links.end());

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return content;
}

CrawlMetadata Parser::parse_metadata(const std::string& html) {}
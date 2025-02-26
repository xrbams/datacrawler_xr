#include "gumbo.h"
#include "metadata.hpp"

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

void extractLinks(GumboNode* node, vector<string>& links) {
    if(node->type != GUMBO_NODE_ELEMENT) return;

    if(node->v.element.tag == GUMBO_TAG_A){
        GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if(href) links.push_back(href->value);
    }
    GumboVector children = node->v.element.children;
    for (size_t i = 0; i < children.length; ++i)
    {
        extractLinks(static_cast<GumboNode*>(children.data[i]), links);
    }
}

Header Metadata::parse_header(const std::string& html) {}

Content Metadata::parse_content(const std::string& html) {
    Content content("", "", "", {});

    GumboOutput* output = gumbo_parse(html.c_str());
    GumboVector* rootChildren = &output->root->v.element.children;
    for (size_t i = 0; i < rootChildren->length; ++i)
    {
        GumboNode* node = static_cast<GumboNode*>(rootChildren->data[i]);

        if (node->type == GUMBO_NODE_ELEMENT)
        {
            if (node->v.element.tag == GUMBO_TAG_TITLE)
            {
                content.title = extractText(node); //map to title
            }
            if (node->v.element.tag == GUMBO_TAG_META)
            {
                GumboAttribute* nameAttr = gumbo_get_attribute(&node->v.element.attributes, "name");
                GumboAttribute* contentAttr = gumbo_get_attribute(&node->v.element.attributes, "content");
                if (nameAttr && contentAttr && string(nameAttr->value) == "description")
                {
                    content.meta_description = contentAttr->value;
                }
            }
            extractLinks(node, content.links);
        }
        
    }

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return content;
}

CrawlMetadata Metadata::parse_metadata(const std::string& html) {}
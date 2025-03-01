//parser.hpp
#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <regex>
#include <algorithm>
#include "gumbo.h"
#include "metadata.hpp"  // Import Metadata so we can return structured data


class Parser
{
private:

public:
    Parser() = default;
    ~Parser()  = default;

    static Header parse_header(const std::string& html);
    static Content parse_content(const std::string& html, const std::string& base_url);
    static CrawlMetadata parse_metadata(const std::string& html);
};


#endif
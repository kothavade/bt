#pragma once

#include <map>
#include <string>
#include <vector>
#include <rva/variant.hpp>

#include "Aliases.h"

class Bencode
{
public:
    using Value = rva::variant<
    std::string,
    i64,
    std::vector<rva::self_t>,
    std::map<std::string, rva::self_t>>;
    using List = std::vector<Value>;
    using Dict = std::map<std::string, Value>;
    static auto parse_bencode(std::string_view data) -> Value;
private:
    struct ParseResult
    {
        Value value;
        std::string_view rest;
    };
    static auto parse_inner(std::string_view data) -> ParseResult;
};
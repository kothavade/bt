#pragma once

#include <map>
#include <string>
#include <vector>
#include <rva/variant.hpp>

#include "Aliases.h"

namespace bt
{
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
        static auto decode(std::string_view data) -> Value;
        static auto encode(const Value& value) -> std::string;

    private:
        struct ParseResult
        {
            Value value;
            std::string_view rest;
        };

        static auto decode_inner(std::string_view data) -> ParseResult;
    };
}

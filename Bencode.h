#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

#include "Aliases.h"

struct Bencode
{
    using List = std::vector<Bencode>;
    using Dictionary = std::unordered_map<std::string_view, Bencode>;
    using Value = std::variant<i64, std::string_view, List, Dictionary>;
    Value value;

    explicit Bencode(Value value) : value(std::move(value))
    {
    }

    explicit Bencode(std::string_view data);
};


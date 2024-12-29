#include "Bencode.h"

#include <fmt/format.h>

auto parse(const std::string_view data) -> std::pair<Bencode, std::string_view>
{
    if (data.empty())
        throw std::runtime_error("Expected data, got empty string");

    switch (data[0])
    {
    case 'i':
        {
            const auto pos = data.find('e');
            if (pos == std::string_view::npos)
                throw std::runtime_error("Expected end of integer, got end of data");
            // TODO: don't copy
            const auto integer = std::string(data.substr(1, pos - 1));
            return {Bencode{std::stoll(integer)}, data.substr(pos + 1)};
        }
    case 'l':
        {
            std::vector<Bencode> list;
            auto rest = data.substr(1);
            while (rest[0] != 'e')
            {
                const auto& [bencode, new_rest] = parse(rest);
                list.push_back(bencode);
                rest = new_rest;
            }
            return {Bencode{list}, rest.substr(1)};
        }
    case 'd':
        {
            std::unordered_map<std::string_view, Bencode> dict;
            auto rest = data.substr(1);
            while (rest[0] != 'e')
            {
                const auto& [key, key_rest] = parse(rest);
                const auto& [value, value_rest] = parse(key_rest);
                const auto key_str = std::get<std::string_view>(key.value);
                dict.emplace(key_str, value);
                rest = value_rest;
            }
            return {Bencode{dict}, rest.substr(1)};
        }
    // String
    default:
        {
            const auto pos = data.find(':');
            if (pos == std::string_view::npos)
                throw std::runtime_error(fmt::format("Expected end of string length, got {}", data));
            const auto length = std::stoll(std::string(data.substr(0, pos)));
            const auto value = Bencode::Value{data.substr(pos + 1, length)};
            const auto rest = data.substr(pos + 1 + length);
            return {Bencode{value}, rest};
        }
    }
}

Bencode::Bencode(const std::string_view data)
{
    const auto [bencode, rest] = parse(data);
    if (!rest.empty())
        throw std::runtime_error(fmt::format("Expected end of data, got {}", rest));
    value = bencode.value;
}

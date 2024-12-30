#include "Bencode.h"

#include <sstream>
#include <fmt/format.h>

namespace bt
{
    // TODO: iterative
    auto Bencode::decode_inner(const std::string_view data) -> ParseResult
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
                const auto integer = std::string(data.substr(1, pos - 1));
                return {Value{std::stoll(integer)}, data.substr(pos + 1)};
            }
        case 'l':
            {
                List list;
                auto rest = data.substr(1);
                while (rest[0] != 'e')
                {
                    const auto& [bencode, new_rest] = decode_inner(rest);
                    list.push_back(bencode);
                    rest = new_rest;
                }
                return {Value{list}, rest.substr(1)};
            }
        case 'd':
            {
                Dict dict;
                auto rest = data.substr(1);
                while (rest[0] != 'e')
                {
                    const auto& [key, key_rest] = decode_inner(rest);
                    const auto& [value, value_rest] = decode_inner(key_rest);
                    const auto key_str = rva::get<std::string>(key);
                    dict.emplace(key_str, value);
                    rest = value_rest;
                }
                return {Value{dict}, rest.substr(1)};
            }
            // String
        default:
            {
                const auto pos = data.find(':');
                if (pos == std::string_view::npos)
                    throw std::runtime_error(fmt::format("Expected end of string length, got {}", data));
                const auto length = std::stoll(std::string(data.substr(0, pos)));
                const auto value = Value{std::string(data.substr(pos + 1, length))};
                const auto rest = data.substr(pos + 1 + length);
                return {Value{value}, rest};
            }
        }
    }

    auto Bencode::decode(const std::string_view data) -> Value
    {
        const auto [bencode, rest] = decode_inner(data);
        if (!rest.empty())
            throw std::runtime_error(fmt::format("Expected end of data, got {}", rest));
        return bencode;
    }

    auto Bencode::encode(const Value& value) -> std::string
    {
        std::ostringstream result;
        rva::visit([&result](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>)
            {
                result << v.size() << ':' << v;
            }
            else if constexpr (std::is_same_v<T, i64>)
            {
                result << 'i' << v << 'e';
            }
            else if constexpr (std::is_same_v<T, List>)
            {
                result << 'l';
                for (const auto& item : v)
                    result << encode(item);
                result << 'e';
            }
            else if constexpr (std::is_same_v<T, Dict>)
            {
                result << 'd';
                for (const auto& [key, value] : v)
                {
                    result << encode(key) << encode(value);
                }
                result << 'e';
            }
        }, value);
        return result.str();
    }
}
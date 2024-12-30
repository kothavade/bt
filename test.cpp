#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "Bencode.h"
#include <string_view>
using namespace std::literals;

using B = Bencode;


TEST_CASE("Bencode")
{
    SUBCASE("Integer")
    {
        CHECK_EQ(B{"i42e"sv}, 42LL);
    }

    // SUBCASE("String")
    // {
    //     checkBencodeValue(B{"4:spam"sv}, "spam"sv);
    //     checkBencodeValue(B{"0:"sv}, ""sv);
    // }

    // SUBCASE("List")
    // {
    //     const Bencode b1{"le"sv};
    //     CHECK(std::holds_alternative<Bencode::List>(b1.value));
    //     CHECK(std::get<Bencode::List>(b1.value).empty());
    //
    //     const Bencode b2{"li42ee"sv};
    //     CHECK(std::holds_alternative<Bencode::List>(b2.value));
    //     const auto& list = std::get<Bencode::List>(b2.value);
    //     REQUIRE(list.size() == 1);
    //     CHECK(std::holds_alternative<i64>(list[0].value));
    //     CHECK(std::get<i64>(list[0].value) == 42);
    //
    //     checkBencodeValue(B{"li42e4:spamli42eee"sv}, B::List{
    //                           B{42LL},
    //                           B{"spam"sv},
    //                           B{B::List{B{42LL}}}
    //                       });
    // }
}

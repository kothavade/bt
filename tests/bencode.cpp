#include "doctest.h"
#include "Bencode.h"
using namespace bt;
using namespace std::literals;
using Value = Bencode::Value;

template <typename T>
void check_bc(const std::string& s, const T& expected)
{
    auto v = Bencode::parse(s);
    CHECK_EQ(rva::get<T>(v), expected);
}


TEST_CASE("Integer")
{
    check_bc("i42e", 42LL);
    check_bc("i-42e", -42LL);
}

TEST_CASE("String")
{
    check_bc("4:spam", "spam"s);
    check_bc("0:", ""s);
}

TEST_CASE("List")
{
    check_bc("l4:spam4:eggse", Bencode::List{"spam"s, "eggs"s});
    check_bc("le", Bencode::List{});
    check_bc("li42el4:spam4:eggsee", Bencode::List{42LL, Bencode::List{"spam"s, "eggs"s}});
}

TEST_CASE("Dictionary")
{
    check_bc("d3:cow3:moo4:spam4:eggse", Bencode::Dict{{"cow"s, "moo"s}, {"spam"s, "eggs"s}});
    check_bc("d4:spaml1:a1:bee", Bencode::Dict{{"spam"s, Bencode::List{"a"s, "b"s}}});
    check_bc("d9:publisher3:bob17:publisher-webpage15:www.example.com18:publisher.location4:homee",
             Bencode::Dict{
                 {"publisher"s, "bob"s},
                 {"publisher-webpage"s, "www.example.com"s},
                 {"publisher.location"s, "home"s}
             });
}

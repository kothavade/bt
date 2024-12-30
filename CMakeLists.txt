cmake_minimum_required(VERSION 3.30)
project(bt)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG e3ddede6c4ee818825c4e5a6dfa1d384860c27d9)
FetchContent_Declare(
        rva
        GIT_REPOSITORY https://github.com/codeinred/recursive-variant.git
        GIT_TAG main)
FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.11)
FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(rva)
FetchContent_MakeAvailable(doctest)


set(SOURCE_FILES
        src/Bencode.cpp
        src/Torrent.cpp
)

add_compile_options(-fsanitize=address,undefined)
add_link_options(-fsanitize=address,undefined)

include_directories(include)

add_executable(bt
        src/main.cpp
        ${SOURCE_FILES}
)
target_link_libraries(bt PRIVATE fmt::fmt rva::rva)

add_executable(test
        tests/test.cpp
        ${SOURCE_FILES}
)
target_link_libraries(test PRIVATE fmt::fmt rva::rva)
target_include_directories(test PRIVATE ${doctest_SOURCE_DIR}/doctest)

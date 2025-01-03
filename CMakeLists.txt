cmake_minimum_required(VERSION 3.30)
project(bt)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        rva
        GIT_REPOSITORY https://github.com/codeinred/recursive-variant.git
        GIT_TAG main)
FetchContent_MakeAvailable(rva)
# FetchContent_Declare(
#         doctest
#         GIT_REPOSITORY https://github.com/doctest/doctest.git
#         GIT_TAG v2.4.11)
# FetchContent_MakeAvailable(doctest)
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(fmt REQUIRED IMPORTED_TARGET fmt)



set(SOURCE_FILES
        src/Bencode.cpp
        src/Torrent.cpp
        src/Tracker.cpp
)

add_compile_options(-fsanitize=address,undefined)
add_link_options(-fsanitize=address,undefined)

include_directories(include)

find_package(fmt CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(ada CONFIG REQUIRED)

add_executable(bt
        src/main.cpp
        ${SOURCE_FILES}
)
target_link_libraries(bt PRIVATE fmt::fmt rva::rva asio::asio ada::ada)

add_executable(test
        tests/test.cpp
        ${SOURCE_FILES}
)
target_link_libraries(test PRIVATE fmt::fmt doctest::doctest rva::rva)
target_include_directories(test PRIVATE ${doctest_SOURCE_DIR}/doctest)

cmake_minimum_required(VERSION 3.30)
project(bt)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG        e3ddede6c4ee818825c4e5a6dfa1d384860c27d9) # 11.1.1
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        rva
        GIT_REPOSITORY https://github.com/codeinred/recursive-variant.git
        GIT_TAG        main
)
FetchContent_MakeAvailable(rva)

include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
        doctest
        PREFIX ${CMAKE_BINARY_DIR}/doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        TIMEOUT 10
        UPDATE_COMMAND ${GIT_EXECUTABLE} pull
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
)

ExternalProject_Get_Property(doctest source_dir)
set(DOCTEST_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")


add_executable(bt main.cpp Bencode.cpp Torrent.cpp)
target_link_libraries(bt PRIVATE fmt::fmt rva::rva)

add_executable(test test.cpp Bencode.cpp Torrent.cpp)
target_link_libraries(test PRIVATE fmt::fmt rva::rva)
target_include_directories(test PUBLIC ${DOCTEST_INCLUDE_DIR})

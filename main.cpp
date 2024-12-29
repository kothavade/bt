#include <filesystem>
#include <fstream>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "Torrent.h"


auto main(const int argc, char* argv[]) -> int
{
    if (argc < 2)
    {
        fmt::println("Usage: {} <torrent file>", argv[0]);
        return 1;
    }

    const fs::path file{argv[1]};
    if (!exists(file))
    {
        fmt::println("File {} does not exist", file.string());
        return 1;
    }
    if (!is_regular_file(file))
    {
        fmt::println("File {} is not a regular file", file.string());
        return 1;
    }
    if (file.extension() != ".torrent")
    {
        fmt::println("File {} has an invalid extension", file.string());
        return 1;
    }

    std::ifstream in(file);
    if (!in)
    {
        fmt::println("Failed to open file {}", file.string());
        return 1;
    }
    const auto size = file_size(file);
    std::string torrent_file(size, '\0');
    in.read(torrent_file.data(), static_cast<std::streamsize>(size));
    in.close();

    try
    {
        Torrent torrent{torrent_file};
        fmt::println("Torrent: {}", torrent);
    }
    catch (const std::exception& e)
    {
        fmt::println("Failed to parse torrent file: {}", e.what());
        return 1;
    }

    return 0;
}

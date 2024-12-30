#include <filesystem>
#include <fmt/format.h>

#include "Torrent.h"


auto main(const int argc, char* argv[]) -> int
{
    if (argc < 2)
    {
        fmt::println("Usage: {} <torrent file>", argv[0]);
        return 1;
    }

    const fs::path file{argv[1]};

    try
    {
        const auto torrent = Torrent{file};
        fmt::print("{}\n", torrent);
    } catch (const std::exception& e)
    {
        fmt::print(stderr, "Error: {}\n", e.what());
        return 1;
    }

    return 0;
}

#include <Bencode.h>
#include <Tracker.h>
#include <ada.h>
#include <fmt/format.h>

#include <asio.hpp>
#include <filesystem>

#include "Torrent.h"

using namespace Typhoon;

auto main(const int argc, char *argv[]) -> int {
    if (argc < 2) {
        fmt::println("Usage: {} <torrent file>", argv[0]);
        return 1;
    }

    try {
        const auto torrent = Torrent::from_file(argv[1]);
        auto io_context = asio::io_context{};
        auto tracker = HttpTracker{io_context};

        co_spawn(
            io_context,
            [&]() -> asio::awaitable<void> {
                auto response = co_await tracker.announce(torrent, false);
                fmt::println("Tracker response: {}", response);
            },
            asio::detached);

        io_context.run();
    } catch (const std::exception &e) {
        fmt::print(stderr, "Exception: {}\n", e.what());
        return 1;
    }

    return 0;
}

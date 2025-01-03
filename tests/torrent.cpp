#include "Torrent.h"

#include "doctest.h"

using namespace Typhoon;

TEST_CASE("Torrent") {
    for (const auto &entry : fs::directory_iterator{"../examples"})
        CHECK_NOTHROW(Torrent::from_file(entry.path()));
}

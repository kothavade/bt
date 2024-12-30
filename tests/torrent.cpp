#include "doctest.h"

#include "Torrent.h"

TEST_CASE("Torrent")
{
    for (const auto& entry : fs::directory_iterator{"../examples"})
        CHECK_NOTHROW(Torrent{entry.path()});
}

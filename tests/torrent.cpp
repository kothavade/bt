#include "doctest.h"
#include "Torrent.h"

using namespace bt;

TEST_CASE("Torrent")
{
    for (const auto& entry : fs::directory_iterator{"../examples"})
        CHECK_NOTHROW(Torrent{entry.path()});
}

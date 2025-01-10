#include <doctest/doctest.h>

#include "../src/headers.hpp"

using namespace tls;

TEST_CASE("Entity initialization")
{
    // sanity checks
    Core core;

    core.add_entity("Philip II of Macedon", -382, -336);
    core.add_entity("Alexander the Great", -356, -323);
    core.add_entity("Aristoteles", -384, -322);
    core.add_entity("Plato", -428, -348);
    core.add_entity("Macedonia", -808, -168);
    core.add_entity("Minna the Great", -3200, 2019);
    CHECK_EQ(core.data.size(), 6);
}

TEST_CASE("Logic errors")
{
    Core core;
    CHECK_THROWS(core.add_entity("foo", 1900, 1810));
}

TEST_CASE("calendars")
{


}
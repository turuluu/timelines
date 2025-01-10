#include <doctest/doctest.h>

#include "../src/headers.hpp"

using namespace tls;

TEST_CASE("Entity initialization")
{
    // sanity checks
    Core core;
    core.add(Entity("foo") | 1900 | 1910);
    core.add("foo"_e | 1900 | 1910);
    CHECK_EQ(core.data[0].name, "foo");
    CHECK_EQ(core.data[0].start_year, 1900);
    CHECK_EQ(core.data[0].end_year, 1910);

    CHECK_EQ(core.data[0].name, core.data[1].name);

    core.add("Philip II of Macedon"_e | -382 | -336);
    core.add("Alexander the Great"_e | -356 | -323);
    core.add("Aristoteles"_e | -384 | -322);
    core.add("Plato"_e | -428 | -348);
    core.add("Macedonia"_e | -808 | -168);
    core.add("Minna the Great"_e | -3200 | 2019);
    CHECK_EQ(core.data.size(), 8);
}

TEST_CASE("Logic errors")
{
    Core core;
    CHECK_THROWS(core.add(Entity("foo") | 1900 | 1810));
}

TEST_CASE("calendars")
{


}
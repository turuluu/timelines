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
}

TEST_CASE("Logic errors")
{
    Core core;
    CHECK_THROWS(core.add(Entity("foo") | 1900 | 1810));
}

TEST_CASE("calendars")
{


}
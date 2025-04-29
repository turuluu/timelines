#include <doctest/doctest.h>

#include "../src/headers.hpp"

using namespace tls;

TEST_CASE("Entity initialization")
{
    // sanity checks
    struct core core;

    auto Y = clock::from_iso;
    core.add_entity("Philip II of Macedon", Y(-382), Y(-336));
    core.add_entity("Alexander the Great", Y(-356), Y(-323));
    core.add_entity("Aristoteles", Y(-384), Y(-322));
    core.add_entity("Plato", Y(-428), Y(-348));
    core.add_entity("Macedonia", Y(-808), Y(-168));
    core.add_entity("Minna the Great", Y(-3200), Y(2019));
    CHECK_EQ(core.data.size(), 6);
}

TEST_CASE("Logic errors")
{
    core core;
    CHECK_THROWS(core.add_entity("foo", clock::from_iso(1900), clock::from_iso(1810)));
}

TEST_CASE("select")
{
    struct core core;

    auto Y = clock::from_iso;
    core.add_entity("Philip II of Macedon", Y(-382), Y(-336));
    core.add_entity("Alexander the Great", Y(-356), Y(-323));
    scoped_graphics gfx(100, 100);
    gui rc(core);
    auto& v = rc.make<vertical>();
    CHECK_EQ(2, v.lanes_in_interval({Y(-400), Y(0)}));
    CHECK_EQ(1, v.lanes_in_interval({Y(-356), Y(-300)}));
    CHECK_EQ(0, v.lanes_in_interval({Y(0), Y(400)}));
}
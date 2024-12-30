#include <cassert>

#include "entities.hpp"
#include "core.hpp"

#if 0
#include "doctest/doctest.h"

int factorial(int number) { return number <= 1 ? number : factorial(number - 1) * number; }


TEST_CASE("testing the factorial function") {
    CHECK(factorial(1) == 1);
    CHECK(factorial(2) == 2);
    CHECK(factorial(3) == 6);
    CHECK(factorial(10) == 3628800);
}

TEST_CASE("testing entities")
{
    Entity e("foo", 0, 1);
    CHECK(e.name == "foo");
}
#endif

namespace tls
{

Entity::Entity(std::string _name)
  : name(std::move(_name))
  , id(ids++)
{
    std::cout << "Creating " << name << ", id: " << ids - 1 << "\n";
}

Entity::Entity(std::string _name, int _start, int _end)
  : name(std::move(_name))
  , start_year(_start)
  , end_year(_end)
  , id(ids++)
{
    // birth before death, right?
    assert(start_year <= end_year);
}

Entity::~Entity() { std::cout << "DTOR '" << name << "'\n"; }

Entity::Property
operator|(Entity::Property lhs, Entity::Property rhs)
{
    return static_cast<Entity::Property>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

Entity::Property
operator&(Entity::Property lhs, Entity::Property rhs)
{
    return static_cast<Entity::Property>(
      static_cast<std::underlying_type<Entity::Property>::type>(lhs) &
      static_cast<std::underlying_type<Entity::Property>::type>(rhs));
}

Entity::Property
operator|=(Entity::Property& lhs, Entity::Property rhs)
{
    lhs = static_cast<Entity::Property>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
    return lhs;
}

Entity
operator""_e(const char* text, size_t)
{
    Entity e = Entity{ text };
    e.properties |= Entity::Property::Has_name_and_id;
    return e;
}

Entity
operator|(Entity e, int year)
{
    if (e.properties == Entity::Property::Has_start_year)
    {
        // end year should be after start year
        assert(year > e.start_year);

        // year should remain within set limits
        std::cout << year_limits(year) << "\n";
        assert(year_limits(year) == year);

        e.end_year = year;
        e.properties |= Entity::Property::Has_end_year;
        std::cout << "emplacing (" << e.name << ", " << e.start_year << ", " << e.end_year << ")\n";
        //        Years::instance()->insert(&e);
        return e;
    }
    else if (e.properties == Entity::Property::Has_end_year)
    {
        std::cout << "warning: tried to set even though all parameters are set\n";
        return e;
    }
    else
    {
        e.start_year = year;
        e.properties |= Entity::Property::Has_start_year;
        return e;
    }
}

void populate_entities_test(Core& core)
{
    core.add("Philip II of Macedon"_e | -382 | -336);
    core.add("Alexander the Great"_e | -356 | -323);
    core.add("Aristoteles"_e | -384 | -322);
    core.add("Plato"_e | -428 | -348);
    core.add("Macedonia"_e | -808 | -168);
    core.add("Minna the Great"_e | -3200 | 2019);
}

}
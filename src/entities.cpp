#include <cassert>

#include "core.hpp"
#include "entities.hpp"
#include <exception>

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
    switch (e.properties)
    {
        case Entity::Property::Has_start_year:
        {

            // end year should be after start year
            // assert(year > e.start_year);

            if (year < e.start_year)
                throw std::logic_error("Entity's end year < start year");

            // year should remain within set limits
            std::cout << year_limits(year) << "\n";
            assert(year_limits(year) == year);

            e.end_year = year;
            e.properties |= Entity::Property::Has_end_year;
            std::cout << "emplacing (" << e.name << ", " << e.start_year << ", " << e.end_year
                      << ")\n";
            //        Years::instance()->insert(&e);
            return e;
        }
        break;
        case Entity::Property::Has_end_year:
        {
            std::cout << "warning: tried to set even though all parameters are set\n";
            return e;
        }
        break;
        default:
        {
            e.start_year = year;
            e.properties |= Entity::Property::Has_start_year;
            return e;
        }
        break;
    }
}
}
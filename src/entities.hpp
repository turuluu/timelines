#pragma once

#include <set>
#include <vector>

namespace tls
{
static size_t ids = 0;

struct Dimensions
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
};

struct Entity
{
    explicit Entity(std::string _name);

    Entity(std::string _name, int _start, int _end);

    ~Entity();

    enum class Property : unsigned
    {
        None = 0x0,
        Has_name_and_id = 0x1,
        Has_start_year = 0x2,
        Has_end_year = 0x4
    };

    const size_t id;
    std::string name;
    int start_year = 0;
    int end_year = 0;
    Property properties = Property::None;

    Dimensions bounds;
    Entity* parent = nullptr;
};

///////////////////////////
Entity::Property operator|(Entity::Property lhs, Entity::Property rhs);

Entity::Property operator&(Entity::Property lhs, Entity::Property rhs);

Entity::Property operator|=(Entity::Property& lhs, Entity::Property rhs);

Entity operator""_e(const char* text, size_t);

/**
 * First usage will default to passing <start year> and second
 * to <end year>. When passing the end year, the pointer is emplaced
 * in a container passing the ownership to it.
 *
 * @param e
 * @param year
 * @return
 */
Entity operator|(Entity e, int year);

}
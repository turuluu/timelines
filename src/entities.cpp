#include <cassert>

#include "core.hpp"
#include "entities.hpp"
#include "errors.hpp"

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
    if (start_year > end_year)
    {
        // birth before death, right?
        error::timescale_issue err;
        err.set_code_file(__FILE__);
        err.set_line_n(__LINE__);
        err.set_interval(start_year, end_year);
        throw err;
    }

}

Entity::~Entity() { std::cout << "DTOR '" << name << "'\n"; }

}
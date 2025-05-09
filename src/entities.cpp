#include <cassert>
#include <iostream>

#include "core.hpp"
#include "entities.hpp"
#include "errors.hpp"

namespace tls
{

entity::entity(std::string _name)
  : name(std::move(_name))
  , id(ids++)
{
    utlz::dbg("Creating ", name, ", id: ", ids - 1);
}

entity::entity(std::string _name, time_point _start, time_point _end)
  : name(std::move(_name))
  , interval{ _start, _end }
  , id(ids++)
{
    if (interval.start > interval.end)
    {
        // birth before death, right?
        error::timescale_issue err;
        err.set_code_file(__FILE__);
        err.set_line_n(__LINE__);
        err.set_interval(interval);
        throw err;
    }
}

entity::~entity() { utlz::dbg("DTOR '", name, "'"); }

} // namespace tls
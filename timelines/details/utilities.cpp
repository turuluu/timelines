#include <thread>

#include "utilities.hpp"

namespace util
{

std::string
get_thread_id()
{
    return to_string(std::this_thread::get_id());
}

size_t
length(const char* cstr)
{
    size_t _length = 0;
    char c = 'a';
    for (; c != 0; ++_length)
        c = cstr[_length];

    return _length;
}
}
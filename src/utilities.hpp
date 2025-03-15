#pragma once

#include <algorithm>
#include <cmath>
#include <sstream>

namespace util
{
const double pi = std::acos(-1);

template<typename Type>
Type
limit(Type min, Type max, Type value)
{
    return std::min(max, std::max(min, value));
}

size_t length(const char* cstr);

template<typename... T>
std::string
to_string(T... obj)
{
    return (std::stringstream{} << ... << obj).str();
}

std::string get_thread_id();

} // namespace util

// Thread, Stack, Location debug print helper
#define TSL_PRINT_ENABLED 0
#if TSL_PRINT_ENABLED
#ifndef PRINT_TSL
#if defined(_MSC_VER)
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#endif

#ifndef FILE_LOC
#define FILE_LOC util::to_string(__FILE__, ":", __LINE__)
#endif

#define PRINT_TSL                                                                                  \
    do                                                                                             \
    {                                                                                              \
        printf("T[ %s ]\tS[ %s ]\tL[ %s ]\n",                                                      \
               util::get_thread_id().c_str(),                                                      \
               __PRETTY_FUNCTION__,                                                                \
               FILE_LOC.c_str());                                                                  \
        fflush(NULL);                                                                              \
    } while (0);
#else
#define PRINT_TSL ;
#endif // TSL
#endif

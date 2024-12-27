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

template <typename T>
std::string stream_to_string(T obj)
{
    return (std::stringstream {} << obj).str();
}

std::string get_thread_id();

}

// Convenience logging, for example for clion which can create hyperlinks from these print outs
#ifndef FILE_LOC
    #define FILE_LOC (std::stringstream {} << __FILE__ << ":" << __LINE__).str()

    #if defined(_MSC_VER)
        #ifndef __PRETTY_FUNCTION__
            #define __PRETTY_FUNCTION__ __FUNCSIG__
        #endif
    #endif

    // Print thread, class and location (file:line_n). This is not bound to DEBUG and will log even
    // in pace builds - which is intentional use TCL_PRINT_ENABLED dummy definition to disable the
    // annoying logs for local builds
    #define TCL_PRINT_ENABLED 1
    #if TCL_PRINT_ENABLED
        #define PRINT_TCL                          \
            do                                     \
            {                                      \
                printf(                            \
                    "T[ %s ]\tC[ %s ]\tL[ %s ]\n", \
                    util::get_thread_id().c_str(), \
                    __PRETTY_FUNCTION__,           \
                    FILE_LOC.c_str()               \
                );                                 \
                fflush(NULL);                      \
            } while (0);
    #else
        #define PRINT_TCL ;
    #endif // TCL_PRINT_ENABLED
#endif

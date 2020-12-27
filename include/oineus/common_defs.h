#pragma once

#include <cassert>
#include <atomic>
#include <vector>
#include <memory>
#include <string>
#include <sstream>

#ifdef OINEUS_USE_SPDLOG

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
namespace spd = spdlog;

template<typename... Args>
inline void debug(const Args& ... args)
{
    spd::debug(args...);
}

template<typename... Args>
inline void info(const Args& ... args)
{
    spd::info(args...);
}

#else

template<typename... Args>
inline void debug([[maybe_unused]] const Args& ... args)
{
    ;
}

template<typename... Args>
inline void info([[maybe_unused]] const Args& ... args)
{
    ;
}

#endif

namespace oineus {

using dim_type = size_t;

template<typename Real>
struct RPoint {
    Real x{0};
    Real y{0};

    RPoint() = default;

    RPoint(Real _x, Real _y)
            :x(_x), y(_y) { };

    const Real& operator[](int i) const
    {
        switch (i) {
        case 0:
            return x;
            break;
        case 1:
            return y;
            break;
        default:
            throw std::out_of_range("RPoint has only 2 coords");
        }
    }

    Real& operator[](int i)
    {
        switch (i) {
        case 0:
            return x;
            break;
        case 1:
            return y;
            break;
        default:
            throw std::out_of_range("RPoint has only 2 coords");
        }
    }
};

//template<typename Out>
//void split_by_delim(const std::string& s, char delim, Out result)
//{
//    std::stringstream ss(s);
//    std::string item;
//    while(std::getline(ss, item, delim)) {
//        *(result++) = item;
//    }
//}

//inline std::vector<std::string> split_by_delim(const std::string& s, char delim)
//{
//    std::vector<std::string> elems;
//    split_by_delim(s, delim, std::back_inserter(elems));
//    return elems;
//}
}

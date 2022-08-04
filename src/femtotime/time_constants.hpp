/**
 * @file time_constants.hpp
 * @author Patrick Norton <pnorton@lanl.gov>
 * @date 1 Aug 2022
*/
#pragma once

namespace femtotime {

#if __GNUC__
    typedef __int128 int128_t;
    typedef unsigned __int128 uint128_t;
#elif __clang__
    typedef __int128_t int128_t;
    typedef __uint128_t uint128_t;
#else
    #error "128-bit integers are only supported on GCC and Clang"
#endif

static const int128_t fs_per_ns = 1'000'000;
static const int128_t ns_per_sec = 1'000'000'000;
static const int128_t sec_per_min = 60;
static const int128_t min_per_hour = 60;
static const int128_t hours_per_day = 24;
static const int128_t days_per_year = 365;
static const int128_t days_per_leap = 366;

static const int128_t fs_per_sec = fs_per_ns * ns_per_sec;
static const int128_t fs_per_min = fs_per_sec * sec_per_min;
static const int128_t fs_per_hour = fs_per_min * min_per_hour;
static const int128_t fs_per_day = fs_per_hour * hours_per_day;
static const int128_t fs_per_year = fs_per_day * days_per_year;
static const int128_t fs_per_leap = fs_per_day * days_per_leap;

static const int128_t fs_per_ms = 1'000'000'000'000;
static const int128_t fs_per_us = 1'000'000'000;

static const int128_t secs_per_hour = sec_per_min * min_per_hour;
static const int128_t secs_per_day = secs_per_hour * hours_per_day;
static const int128_t secs_per_year = secs_per_day * days_per_year;

} // femtotime

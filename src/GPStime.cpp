/**
 * @file GPStime.cpp
 * @author Patrick Norton <pnorton@lanl.gov>
 * @date 10 July 2022
*/

// [femtotime headers]
#include "femtotime/GPStime.hpp"

// [C++ headers]
#include <algorithm>
#include <iostream>

// [fmt]
#include <fmt/printf.h>

// [Namespaces]
using namespace std;

namespace femtotime {

/**
 * @brief The times (in GPS) that a leap second was added to UTC time.
 *
 * This keeps the time in GPS, not UTC. This means that the leap second times
 * are slowly drifting out of sync with the expected UTC times. If you need
 * UTC times, use `utc_time_t::leap_seconds` instead.
 *
 * NOTE FOR FUTURE MAINTAINERS: This and `utc_time_t::leap_seconds` need to be
 * kept in sync. This vector contains the leap second times as their GPS
 * equivalents, which means that a proper conversion is necessary. In
 * particular, none of the times in this column should have a 60 in the seconds
 * column. Additionally, the conversion code in this file assumes that the list
 * is sorted (it uses `std::upper_bound`), so any new leap seconds should be
 * added to the bottom of both vectors.
 */
std::vector<gps_time_t> gps_time_t::leap_seconds = {
  gps_time_t(1972, 6, 30,  23, 59, 51),
  gps_time_t(1972, 12, 31, 23, 59, 52),
  gps_time_t(1973, 12, 31, 23, 59, 53),
  gps_time_t(1974, 12, 31, 23, 59, 54),
  gps_time_t(1975, 12, 31, 23, 59, 55),
  gps_time_t(1976, 12, 31, 23, 59, 56),
  gps_time_t(1977, 12, 31, 23, 59, 57),
  gps_time_t(1978, 12, 31, 23, 59, 58),
  gps_time_t(1979, 12, 31, 23, 59, 59),
  gps_time_t(1981, 7, 1, 0, 0, 0),
  gps_time_t(1982, 7, 1, 0, 0, 1),
  gps_time_t(1983, 7, 1, 0, 0, 2),
  gps_time_t(1985, 7, 1, 0, 0, 3),
  gps_time_t(1988, 1, 1, 0, 0, 4),
  gps_time_t(1990, 1, 1, 0, 0, 5),
  gps_time_t(1991, 1, 1, 0, 0, 6),
  gps_time_t(1992, 7, 1, 0, 0, 7),
  gps_time_t(1993, 7, 1, 0, 0, 8),
  gps_time_t(1994, 7, 1, 0, 0, 9),
  gps_time_t(1996, 1, 1, 0, 0, 10),
  gps_time_t(1997, 7, 1, 0, 0, 11),
  gps_time_t(1999, 1, 1, 0, 0, 12),
  gps_time_t(2006, 1, 1, 0, 0, 13),
  gps_time_t(2009, 1, 1, 0, 0, 14),
  gps_time_t(2012, 7, 1, 0, 0, 15),
  gps_time_t(2015, 7, 1, 0, 0, 16),
  gps_time_t(2017, 1, 1, 0, 0, 17),
};
#warning "We need to apply a doubt formalism to the leap seconds, or an"
#warning " assertion mechanism to make sure we are not past their validity."

/**
 * @brief The UTC times of every leap second.
 *
 * If the GPS-time equivalents of these are needed, use
 * `gps_time_t::leap_seconds`.
 *
 * NOTE FOR FUTURE MAINTAINERS: See `gps_time_t::leap_seconds`.
 */
std::vector<utc_time_t> utc_time_t::leap_seconds = {
  utc_time_t(1972, 6, 30,  23, 59, 60),
  utc_time_t(1972, 12, 31, 23, 59, 60),
  utc_time_t(1973, 12, 31, 23, 59, 60),
  utc_time_t(1974, 12, 31, 23, 59, 60),
  utc_time_t(1975, 12, 31, 23, 59, 60),
  utc_time_t(1976, 12, 31, 23, 59, 60),
  utc_time_t(1977, 12, 31, 23, 59, 60),
  utc_time_t(1978, 12, 31, 23, 59, 60),
  utc_time_t(1979, 12, 31, 23, 59, 60),
  utc_time_t(1981, 6, 30,  23, 59, 60),
  utc_time_t(1982, 6, 30,  23, 59, 60),
  utc_time_t(1983, 6, 30,  23, 59, 60),
  utc_time_t(1985, 6, 30,  23, 59, 60),
  utc_time_t(1987, 12, 31, 23, 59, 60),
  utc_time_t(1989, 12, 31, 23, 59, 60),
  utc_time_t(1990, 12, 31, 23, 59, 60),
  utc_time_t(1992, 6, 30,  23, 59, 60),
  utc_time_t(1993, 6, 30,  23, 59, 60),
  utc_time_t(1994, 6, 30,  23, 59, 60),
  utc_time_t(1995, 12, 31, 23, 59, 60),
  utc_time_t(1997, 6, 30,  23, 59, 60),
  utc_time_t(1998, 12, 31, 23, 59, 60),
  utc_time_t(2005, 12, 31, 23, 59, 60),
  utc_time_t(2008, 12, 31, 23, 59, 60),
  utc_time_t(2012, 6, 30,  23, 59, 60),
  utc_time_t(2015, 6, 30,  23, 59, 60),
  utc_time_t(2016, 12, 31, 23, 59, 60)
};

static constexpr array<int, 12> _basic_month_durations = {
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static constexpr array<int, 12> _leap_month_durations = {
  31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static constexpr std::pair<int64_t, int64_t>
euclidean_div(int64_t x, int64_t y)
{
  auto quot = x / y;
  auto rem = x % y;
  if (rem >= 0) {
    return std::pair(quot, rem);
  } else {
    return std::pair(quot - 1, rem + y);
  }
}

static constexpr std::pair<int128_t, int128_t>
euclidean_div(int128_t x, int128_t y)
{
  auto quot = x / y;
  auto rem = x % y;
  if (rem >= 0) {
    return std::pair(quot, rem);
  } else {
    return std::pair(quot - 1, rem + y);
  }
}

static constexpr int64_t days_per_400_years = 146097;
static constexpr int64_t days_per_100_years = 36524;
static constexpr int64_t days_per_4_years = 1461;

// Our days_of_month vector is rotated here so Febuary is last--this means we
// don't have to deal with leap years specially because Feb. 29 is represented
// as day 366, making it the last day in our list.
static constexpr array<int, 12> rotated_month_days = {
  31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29
};

static std::tuple<int, int, int>
dayToDate(int64_t total_days, int64_t y2000_epoch)
{
  // High-level overview of this function:
  //
  // The general way this function works is by dividing the total number of days
  // elapsed into "cycles": periods each the length of one of the leap year
  // rules (400 years, 100 years, 4 years, and 1 year). This function works by
  // dividing down each cycle and getting the remaining days: for example, every
  // 400 years, exactly 146,097 days pass, so the date (day and month) of any
  // day count `x` is exactly the same as the date for day `x % 146097`. We
  // repeat this for each of the cycles, slowly shrinking down until we get to a
  // cycle of 1 year, at which point we can determine the day and month.
  //
  // One of the least intuitive parts of this function is how it deals with leap
  // days. The first thing it does is that it modifies the day count to be
  // relative to March 1, 2000 instead of the GPS epoch (January 6, 1980). This
  // is because that day is immediately after a leap day and at the beginning of
  // a 400-year cycle. This means that the "special" leap days (e.g. the missing
  // leap day every 100 years, or the no-longer-missing one every 400) are
  // always the very last day in their respective cycles. This makes them much
  // easier to deal with--as an example, the math is identical for dealing with
  // 100-year cycles whether or not the last leap day is missing (as it is 3 out
  // of 4 times), because all other days come before it, so the number of days
  // elapsed from the beginning of the cycle is the same for all of them.

  // Adjusts the date to be relative to Mar. 1, 2000
  // This date was chosen because it is the day after a leap day, on a year
  // divisible by 400--this makes doing math much easier
  auto adjusted_days = total_days - y2000_epoch;

  // Euclidean division here to account for days before the year-2000 epoch
  auto [y400_cycles, y400_rem] =
    euclidean_div(adjusted_days, days_per_400_years);

  auto [y100_cycles, y100_rem] = euclidean_div(y400_rem, days_per_100_years);
  if (y100_cycles == 4) {
    // This is the case if and only if the date is the Feb. 29, 2000 (or off by
    // 400 years). If that's the case, we need to adjust our times appropriately
    y100_cycles = 3;
    y100_rem += days_per_100_years;
  }

  auto [y4_cycles, y4_rem] = euclidean_div(y100_rem, days_per_4_years);
  // The max y100_rem can be is 36524, which is less than 25*1461 (= 26525).
  assert(y4_cycles < 25);

  auto [years, days] = euclidean_div(y4_rem, 365);
  if (years == 4) {
    // If years == 4, we have a leap day--celebrate (or don't)
    years = 3;
    days += 365;
  }

  // Now that we have our day relative to March 2000, we need to actually
  // convert that to standard year-month-day format

  // Then, use the day of the year to calculate the day of the month
  // Our days_of_month vector is rotated here so Febuary is last--this means we
  // don't have to deal with leap years specially because Feb. 29 is represented
  // as day 366, making it the last day in our list.
  int day_of_month = days;
  int month = 0;
  for (; day_of_month >= rotated_month_days[month]; month++) {
    day_of_month -= rotated_month_days[month];
  }
  // Adjust for the fact that our month vector is off by 2
  month = (month + 2) % 12;
  // Because our months are off by 2, we need to adjust the year for dates in
  // January and Feburary
  bool adjust_year = month < 2;
  auto year = years + 4*y4_cycles + 100*y100_cycles + 400*y400_cycles;
  // Adjust our year from being relative to Mar. 2000 to standard Gregorian
  year += 2000 + adjust_year;
  // We need to adjust our month and day to 1-indexed because that's what people
  // expect when dealing with dates
  return {year, month + 1, day_of_month + 1};
}

static std::tuple<int, int, int> gpsDayToDate(int64_t total_days)
{
  // The GPS epoch is 7360 days before Mar. 1, 2000
  return dayToDate(total_days, 7360);
}

static std::tuple<int, int, int> utcDayToDate(int64_t total_days)
{
  // The GPS epoch is 11017 days before Mar. 1, 2000
  return dayToDate(total_days, 11017);
}

static int64_t dateToDays(int64_t year, int month, int day, int64_t y2000_epoch)
{
  // Like daysToDate, this converts first to days relative to Mar. 1, 2000
  // This puts us immediately after a leap day, so leap day is now the last
  // day of the year, making conversion easier
  year -= (month <= 2); // Early days are part of the previous "year"
  month = (month + 9) % 12; // Convert months so March is 0
  year -= 2000; // Convert year so it's relative to 2000
  auto day_of_year = day - 1;
  for (int i = 0; i < month; i++) {
    day_of_year += rotated_month_days[i];
  }
  auto [y400_cycles, y400_rem] = euclidean_div(year, 400);
  // Convert the remaining years to the day within the 400-year cycle
  // Every 4 years, there is an extra leap day, and then every 100, there is one
  // day fewer (missing leap day).
  auto day_of_cycle =
    365 * y400_rem + y400_rem / 4 - y400_rem / 100 + day_of_year;
  // Get the number of days elapsed since Mar. 1, 2000
  // This just adds on the elapsed 400-year cycles
  auto relative_day = y400_cycles * days_per_400_years + day_of_cycle;
  // Converts the relative day back to being relative to the GPS epoch, then
  // returns
  return relative_day + y2000_epoch;
}

static int64_t dateToGPSDays(int64_t year, int month, int day)
{
  return dateToDays(year, month, day, 7360);
}

static int64_t dateToUTCDays(int64_t year, int month, int day)
{
  return dateToDays(year, month, day, 11017);
}

femtosecs_t DateTime2femtosecs(int year, int month, int day,
                                 int hours, int minutes, long double secs)
{
  femtosecs_t fs;
  fs = (secs * fs_per_sec);
  fs += (minutes * fs_per_min);
  fs += (hours * fs_per_hour);
  auto days_since_epoch = dateToGPSDays(year, month, day);
  fs += (days_since_epoch * fs_per_day);
  return fs;
}

femtosecs_t DateTime2UTC(int year, int month, int day,
                         int hours, int minutes, long double secs)
{
  femtosecs_t fs;
  fs = (secs * fs_per_sec);
  fs += (minutes * fs_per_min);
  fs += (hours * fs_per_hour);
  auto days_since_epoch = dateToUTCDays(year, month, day);
  fs += (days_since_epoch * fs_per_day);
  return fs;
}

gps_time_t FromDateTimeNumbers(int year, int month, int day,
                                 int hours, int minutes, long double secs)
{
  auto fs = DateTime2femtosecs(year, month, day, hours, minutes, secs);
  return gps_time_t(fs);
}

static constexpr bool is_leap_year(int year)
{
  return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

static constexpr const array<int, 12> &year2month_durations(int year)
{
  if (is_leap_year(year)) {
    return _leap_month_durations;
  } else {
    return _basic_month_durations;
  }
}

/**
 * @brief Convert a day/month/year in Gregorian format to the number of days
 * since the start of the given year.
 */
static int date2doy(int year, int month, int day)
{
  int day_of_year = 0;
  const auto &month_durations = year2month_durations(year);
  for (int mo = 0; mo < (month - 1); ++mo) {
    day_of_year += month_durations[mo];
  }
  day_of_year += (day - 1);
  return day_of_year;
}
/**
 * @brief The femtosecond conversion factor needed to adjust from UTC to GPS.
 *
 * Note that this is not the same as the seconds *elapsed* between the two
 * epochs, because this conversion ignores leap seconds (both time types ignore
 * leap seconds internally).
 */
static constexpr femtosecs_t epoch_adjust = 315'964'800 * fs_per_sec;

/**
 * @brief This is the GPS epoch on Jan. 6, 1980 at midnight
*/
gps_time_t gps_time_t::gps_epoch = gps_time_t(0);

/**
 * @brief The GPS epoch at Jan. 6, 1980 at midnight.
 */
utc_time_t utc_time_t::gps_epoch = utc_time_t(1980, 1, 6, 0, 0, 0, 0);

/**
 * @brief The UTC epoch at Jan. 1, 1970 at midnight.
 */
gps_time_t gps_time_t::utc_epoch = FromUTC(utc_time_t::utc_epoch);

/**
 * @brief The UTC epoch at Jan. 1, 1970 at midnight.
 */
utc_time_t utc_time_t::utc_epoch = utc_time_t(0);

/**
 * @brief The next leap second to occur on or after the given time.
 *
 * If the given time is a leap second, it will return the given time.
 */
static std::vector<gps_time_t>::const_iterator
next_leap_second(const gps_time_t &gps_time)
{
  return std::upper_bound(
    gps_time_t::leap_seconds.begin(), gps_time_t::leap_seconds.end(), gps_time
  );
}

/**
 * @brief The next leap second to occur on or after the given time.
 *
 * If the given time is a leap second, it will return the given time.
 */
static std::vector<utc_time_t>::const_iterator
next_leap_second(const utc_time_t &utc_time)
{
  return std::upper_bound(
    utc_time_t::leap_seconds.begin(), utc_time_t::leap_seconds.end(), utc_time
  );
}

/**
 * @brief The number of leap seconds elapsed between the GPS epoch and the given
 * time, as well as whether or not the given time is within a leap second.
 *
 * If a time before the GPS epoch is given, a negative number is returned.
 * If the time given is within a leap second, it is calculated as if that second
 * has occurred.
 */
static std::pair<ptrdiff_t, bool>
elapsed_leap_seconds(const gps_time_t &gps_time)
{
  // The first leap second after the GPS epoch.
  static auto EPOCH_BEGIN_LEAP_SECOND = next_leap_second(gps_time_t::gps_epoch);
  auto next = next_leap_second(gps_time);
  if (next == gps_time_t::leap_seconds.begin()) {
    // The given time is before the first leap second, therefore it is not a
    // leap second.
    return {std::distance(EPOCH_BEGIN_LEAP_SECOND, next), false};
  } else {
    auto prev = next - 1;
    // No need to check the difference is positive: prev < gps_time because if
    // it weren't, it would have been returned by `next_leap_second` (and
    // therefore be in `next`, not `prev`).
    bool is_leap = (gps_time - *prev) < duration_t::from_secs(1);
    return {std::distance(EPOCH_BEGIN_LEAP_SECOND, next), is_leap};
  }
}

/**
 * @brief The number of leap seconds elapsed between the GPS epoch and the given
 * time, as well as whether or not the given time is within a leap second.
 *
 * If a time before the GPS epoch is given, a negative number is returned.
 * If the time given is within a leap second, it is calculated as if that second
 * has occurred.
 */
static std::pair<ptrdiff_t, bool>
elapsed_leap_seconds(const utc_time_t &utc_time)
{
  // The first leap second after the GPS epoch.
  static auto EPOCH_BEGIN_LEAP_SECOND = next_leap_second(utc_time_t::gps_epoch);
  auto next = next_leap_second(utc_time);
  if (next == utc_time_t::leap_seconds.begin()) {
    // The given time is before the first leap second, therefore it is not a
    // leap second.
    return {std::distance(EPOCH_BEGIN_LEAP_SECOND, next), false};
  } else {
    return {std::distance(EPOCH_BEGIN_LEAP_SECOND, next), utc_time.is_leap()};
  }
}

int LeapSecondsBetween(const utc_time_t& time1, const utc_time_t& time2)
{
  auto [elapsed1, leap1] = elapsed_leap_seconds(time1);
  auto [elapsed2, leap2] = elapsed_leap_seconds(time2);
  return elapsed2 - elapsed1;
}

/**
 * Check if date string is modified Julian or UTC format
*/
bool IsJulian(const std::string &date_string)
{
  return date_string.find("-") == std::string::npos;
}

static uint64_t pow10(int n)
{
  static constexpr uint64_t pow_table[] = {
    1u, 10u, 100u, 1000u, 10'000u,
    100'000u, 1'000'000u, 10'000'000u,
    100'000'000u, 1'000'000'000u,
    10'000'000'000u, 100'000'000'000u,
    1'000'000'000'000u, 10'000'000'000'000u,
    100'000'000'000'000u,
    1'000'000'000'000'000u,
    10'000'000'000'000'000u,
    100'000'000'000'000'000u,
    1'000'000'000'000'000'000u,
    10'000'000'000'000'000'000u,
  };
  return pow_table[n];
}

/** @brief Convert a UTC time string to a gps_time_t */
gps_time_t gps_time_t::FromUTCString(const std::string& utc_time)
{
  if (IsJulian(utc_time)) {
    throw std::runtime_error(
      "Modified-Julian conversion is no longer supported");
  }

  int year, month, day, hour, min, seconds;

  // Scan the string into constituent parts
  int result;
  long nanos;
  auto dot_pos = utc_time.find(".");
  if (dot_pos == std::string::npos) {
    // If no "." in string, the number of seconds is an integer, so we can avoid
    // the issues with precision altogether.
    result = sscanf(utc_time.c_str(), "%04d-%02d-%02dT%02d:%02d:%02d",
                    &year, &month, &day, &hour, &min, &seconds);
    nanos = 0;

    if (result != 6) {
      auto msg = fmt::format("Cannot parse string '{}' as UTC time", utc_time);
      throw std::runtime_error(msg);
    }
  } else {
    // If there is a dot, scanning gets much more tricky. Because neither
    // `double` nor `long double` has the precision we need, we need to parse
    // the time after the dot as a separate integer. However, we cannot simply
    // parse before and after the dot as integers, as we wouldn't know how many
    // digits of precision the post-dot integer was given as. For example,
    // `.0001` and `.1` would both parse as `1` here. As such, we split the
    // string at the dot, parse the pre-dot portion normally, and then parse the
    // post-decimal portion seperately.
    long unscaled_partials;
    int pre_partials, post_partials;

    result = sscanf(utc_time.c_str(), "%04d-%02d-%02dT%02d:%02d:%02d.%n%ld%nZ",
                    &year, &month, &day, &hour, &min, &seconds,
                    &pre_partials, &unscaled_partials, &post_partials);

    if (result != 7) { // Note that %n isn't counted here
      auto msg = fmt::format(
        "Cannot parse string '{}' as UTC time (result {}, post {}, len {})",
        utc_time, result, post_partials, utc_time.length());
      throw std::runtime_error(msg);
    }

    auto partial_len = post_partials - pre_partials;
    const int nano_digits = 9;
    auto scaling_factor = nano_digits - partial_len;
    if (scaling_factor < 0) {
      nanos = unscaled_partials / pow10(-scaling_factor);
    } else {
      nanos = unscaled_partials * pow10(scaling_factor);
    }
  }
  utc_time_t parsed_time(year, month, day, hour, min, seconds, nanos);
  return FromUTC(parsed_time);
}

/**
 * @brief Deprecated compatibility adapter for `gps_time_t::FromUTCString`.
 */
gps_time_t FromUTCString(const std::string& utc_time)
{
  return gps_time_t::FromUTCString(utc_time);
}

/** @brief Convert a UTC time string to a gps_time_t */
gps_time_t gps_time_t::FromGPSString(const std::string& gps_time)
{

  int year, month, day, hour, min, seconds;

  // Scan the string into constituent parts
  int result;
  long nanos;
  auto dot_pos = gps_time.find(".");
  if (dot_pos == std::string::npos) {
    // If no "." in string, the number of seconds is an integer, so we can avoid
    // the issues with precision altogether.
    result = sscanf(gps_time.c_str(), "GPS_%04d-%02d-%02dT%02d:%02d:%02dZ",
                    &year, &month, &day, &hour, &min, &seconds);
    nanos = 0;

    if (result != 6) {
      auto msg = fmt::format("Cannot parse string '{}' as UTC time", gps_time);
      throw std::runtime_error(msg);
    }
  } else {
    // If there is a dot, scanning gets much more tricky. Because neither
    // `double` nor `long double` has the precision we need, we need to parse
    // the time after the dot as a separate integer. However, we cannot simply
    // parse before and after the dot as integers, as we wouldn't know how many
    // digits of precision the post-dot integer was given as. For example,
    // `.0001` and `.1` would both parse as `1` here. As such, we add specifiers
    // to count the number of characters parsed before and after the decimal,
    // to determine the length.
    unsigned long long unscaled_partials;
    int pre_partials, post_partials;

    result = sscanf(gps_time.c_str(),
                    "GPS_%04d-%02d-%02dT%02d:%02d:%02d.%n%llu%nZ",
                    &year, &month, &day, &hour, &min, &seconds,
                    &pre_partials, &unscaled_partials, &post_partials);

    if (result != 7) { // Note that %n isn't counted here
      auto msg = fmt::format("Cannot parse string '{}' as GPS time (result {})",
                             gps_time, result);
      throw std::runtime_error(msg);
    }

    auto partial_len = post_partials - pre_partials;
    const int nano_digits = 9;
    auto scaling_factor = nano_digits - partial_len;
    if (scaling_factor < 0) {
      nanos = unscaled_partials / pow10(-scaling_factor);
    } else {
      nanos = unscaled_partials * pow10(scaling_factor);
    }
  }
  return gps_time_t(year, month, day, hour, min, seconds, nanos);
}

/**
 * @brief Deprecated compatibility adapter for `gps_time_t::FromGPSString`.
 */
gps_time_t FromGPSString(const std::string& gps_time)
{
  return gps_time_t::FromGPSString(gps_time);
}

gps_time_t gps_time_t::FromISOString(const std::string& iso_time)
{
  int year, month, day, hour, min, seconds;

  auto result = sscanf(iso_time.c_str(), "%04d%02d%02dT%02d%02d%02d",
                       &year, &month, &day, &hour, &min, &seconds);

  if (result != 6) {
      auto msg = fmt::format("Cannot parse string '{}' as ISO time (result {})",
                             iso_time, result);
      throw std::runtime_error(msg);
  }

  return FromUTC(utc_time_t(year, month, day, hour, min, seconds, 0));
}

/**
 * @brief Convert a gps_time_t to a UTC string
 * 
 * @param gps_time The input GPS time to convert to UTC
 *
 * This function returns the time (stored in Diorama's internal
 * representation, which is the GPS time) as a string (in ISO 8601
 * format) with the UTC time corresponding to that GPS time.  The main
 * part of this conversion is to add in the leap seconds, since GPS
 * time ignores leap seconds, while UTC accounts for them.
 *
 * @return The UTC time as an ISO 8601 string YYYY-MM-DDTHH:MM:SS.SZ
*/
std::string gps_time_t::ToUTCString() const
{
  return ToUTC().ToString();
}

/**
 * @brief Deprecated compatibility adapter for `gps_time_t::ToUTCString`.
 */
std::string ToUTCString(const gps_time_t &gps_time)
{
  return gps_time.ToUTCString();
}

/** @brief Convert a gps_time_t to a utc_time_t */
utc_time_t gps_time_t::ToUTC() const
{
  auto [elapsed, is_leap] = elapsed_leap_seconds(*this);
  auto adjusted_time = _femtosecs - elapsed * fs_per_sec;
  return utc_time_t(adjusted_time + epoch_adjust, is_leap);
}

/**
 * @brief Deprecated compatibility adapter for `gps_time_t::ToUTC`.
 */
utc_time_t ToUTC(const gps_time_t &gps_time)
{
  return gps_time.ToUTC();
}

/** @brief Convert a utc_time_t to a gps_time_t */
gps_time_t gps_time_t::FromUTC(const utc_time_t &utc_time)
{
  auto [elapsed, is_leap] = elapsed_leap_seconds(utc_time);
  assert(is_leap == utc_time.is_leap());
  auto adjusted_time = utc_time.get_fs() + elapsed * fs_per_sec;
  return gps_time_t(adjusted_time - epoch_adjust);
}

/**
 * @brief Deprecated compatibility adapter for `gps_time_T::FromUTC`
 */
gps_time_t FromUTC(const utc_time_t &utc_time)
{
  return gps_time_t::FromUTC(utc_time);
}

gps_time_t gps_time_t::FromTime(time_t time)
{
  utc_time_t utc_time(time * fs_per_sec); // time_t is seconds since Unix epoch
  return gps_time_t::FromUTC(utc_time);
}

static long julian_epoch = 51604;

/** @brief Return a Gregorian date nday from the modified Julian epoch */
void Julian2UTC(const long &nday, int &year, int &month, int &day)
{
  auto [y, m, d] = dayToDate(nday, julian_epoch);

  year = y;
  month = m;
  day = d;
}

/**
 * @brief Inspect a date string and convert to YYYY-MM-DD format if Julian
*/
std::string String2Date(const std::string& date_string)
{
  int year, month, day;
  std::string result("NOT_A_DATE_STRING");
  try {
  
    if (IsJulian(date_string)) { 
    
      // this is Modified Julian Day format
      auto iday = stol(date_string);
      
      // get UTC year, month, day from integer Julian day
      Julian2UTC(iday, year, month, day);

      // form the string
      result = fmt::format("{:04}-{:02}-{:02}", year, month, day);

    } else {
      result = date_string;
    }    
  } catch (std::exception &e) {
    auto msg = fmt::format("in {}() -- {}", __FUNCTION__, e.what());
    throw runtime_error(msg);
  }
  return result;
}

gps_time_t::gps_time_t(int year, int month, int day,
                           int hours, int minutes, long double secs)
{
  static_assert(sizeof(int128_t) == 16);
  _femtosecs = DateTime2femtosecs(year, month, day, hours, minutes, secs);
}

gps_time_t::gps_time_t(int year, int month, int day,
                           int hours, int minutes, int secs, int nanos)
{
  static_assert(sizeof(int128_t) == 16);
  _femtosecs = DateTime2femtosecs(year, month, day, hours, minutes, secs);
  _femtosecs += static_cast<femtosecs_t>(nanos) * fs_per_ns;
}

femtosecs_t gps_time_t::get_fs() const
{
  return _femtosecs;
}

/** @brief get the year */
int gps_time_t::Year() const
{
  // FIXME: Can this be done without calculating the month/day?
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = gpsDayToDate(total_days);
  return year;
}

int gps_time_t::Month() const
{
  // FIXME: Can this be done without calculating the year/day?
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = gpsDayToDate(total_days);
  return month;
}

int gps_time_t::Day() const
{
  // FIXME: Can this be done without calculating the year/month?
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = gpsDayToDate(total_days);
  return day;
}

int gps_time_t::Hour() const
{
  auto [days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  return partial_days / fs_per_hour;
}

int gps_time_t::Minute() const
{
  auto [hours, partial_hours] = euclidean_div(_femtosecs, fs_per_hour);
  return partial_hours / fs_per_min;
}

double gps_time_t::Seconds() const
{
  auto [minutes, partial_minutes] = euclidean_div(_femtosecs, fs_per_min);
  return static_cast<double>(partial_minutes) / fs_per_sec;
}

int gps_time_t::WholeSeconds() const
{
  auto [minutes, partial_minutes] = euclidean_div(_femtosecs, fs_per_min);
  return partial_minutes / fs_per_sec;
}

int gps_time_t::Nanoseconds() const
{
  auto [seconds, partial_seconds] = euclidean_div(_femtosecs, fs_per_sec);
  return partial_seconds / fs_per_ns;
}

int gps_time_t::DayOfYear() const
{
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = gpsDayToDate(total_days);
  // date2doy returns 0-indexed, we want 1-indexed
  return date2doy(year, month, day) + 1;
}

long double gps_time_t::SecondsSinceEpoch() const
{
  return static_cast<long double>(_femtosecs + epoch_adjust) / fs_per_sec;
}

long double gps_time_t::SecondsSinceYear() const
{
  // FIXME: Is this the most efficient way to do this?
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = gpsDayToDate(total_days);
  auto day_of_year = date2doy(year, month, day);
  auto partial_day_secs = static_cast<long double>(partial_days) / fs_per_sec;
  auto full_day_secs = day_of_year * secs_per_year;
  return full_day_secs + partial_day_secs;
}

long double gps_time_t::SecondsSinceDay() const
{
  auto [days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  return static_cast<long double>(partial_days) / fs_per_sec;
}

string gps_time_t::ToString() const
{
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = gpsDayToDate(total_days);
  auto [hours, partial_hours] = euclidean_div(partial_days, fs_per_hour);
  auto [mins, partial_mins] = euclidean_div(partial_hours, fs_per_min);
  auto [secs, femtos] = euclidean_div(partial_mins, fs_per_sec);

  return fmt::format(
    "GPS_{:04}-{:02}-{:02}T{:02}:{:02}:{:02}.{:015}Z",
    year, month, day, hours, mins, secs, femtos
  );
}

string gps_time_t::DateString() const
{
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = gpsDayToDate(total_days);
  return fmt::format("{:04}-{:02}-{:02}", year, month, day);
}

/**
 * @brief Determine if the year is a leap year
 *
 * Leap years occur every 4 years
 * However, years that are evenly divisible by 100 are not leap years.
 * The exception to this rule are years that are evenly divisible by 400.
 * These special rules for every 100 and 400 years are to get the average
 * length of the year over long timescale to match the actual length of a
 * year (~365.24).
 *
 * @return Boolean determining whether or not the year is a leap year
*/
bool gps_time_t::IsLeapYear() const
{
  return is_leap_year(Year());
}

bool gps_time_t::operator==(const gps_time_t &other) const
{
  return _femtosecs == other._femtosecs;
}

bool gps_time_t::operator!=(const gps_time_t &other) const
{
  return !this->operator==(other);
}

bool gps_time_t::operator<(const gps_time_t &other) const
{
  return _femtosecs < other._femtosecs;
}

bool gps_time_t::operator<=(const gps_time_t &other) const
{
  return _femtosecs <= other._femtosecs;
}

bool gps_time_t::operator>(const gps_time_t &other) const
{
  return _femtosecs > other._femtosecs;
}

bool gps_time_t::operator>=(const gps_time_t &other) const
{
  return _femtosecs >= other._femtosecs;
}

gps_time_t gps_time_t::operator+(const duration_t &other) const
{
  return gps_time_t(_femtosecs + other.get_fs());
}

gps_time_t gps_time_t::operator-(const duration_t &other) const
{
  return gps_time_t(_femtosecs - other.get_fs());
}

duration_t gps_time_t::operator-(const gps_time_t &other) const
{
  return duration_t(_femtosecs - other._femtosecs);
}

gps_time_t& gps_time_t::operator+=(const duration_t &other)
{
  _femtosecs += other.get_fs();
  return *this;
}

/**
 * @brief Compute decimal year
 *
 * This computes the decimal year as the current year plus the
 * fraction of seconds elapsed so far during the year divided by
 * the total number of seconds during the year.
 *
 * This function uses the IsLeapYear function to determine whether
 * the current year is a leap year and whether an additional day
 * needs to be added to properly compute the total number of seconds
 * during the year.
 *
 * @return Decimal year as a boost::units::metric::year_base_unit quantity
 */
double gps_time_t::DecimalYear() const
{
  // TODO: Remove
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = gpsDayToDate(total_days);
  auto day_of_year = date2doy(year, month, day);
  auto days_in_year = is_leap_year(year) ? 366 : 365;
  auto days_through_year = static_cast<double>(day_of_year) / days_in_year;
  auto partial = static_cast<double>(partial_days) / fs_per_day;
  auto through_year = days_through_year + partial / days_in_year;
  return year + through_year;
}

std::tuple<int, int, int> gps_time_t::ToDate() const
{
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  return gpsDayToDate(total_days);
}

/** @brief Constructor */
utc_time_t::utc_time_t(int y, int mon, int d, int h, int min, int s, int n)
  : _leap(s == 60)
{
  s -= _leap;
  _femtosecs = DateTime2UTC(y, mon, d, h, min, s);
  _femtosecs += n * fs_per_ns;
}

utc_time_t::utc_time_t(int year, int month, int day,
                           int hours, int minutes, long double secs)
  : _leap(secs >= 60)
{
  secs -= _leap;
  _femtosecs = DateTime2UTC(year, month, day, hours, minutes, secs);
}

femtosecs_t utc_time_t::get_fs() const
{
  return _femtosecs;
}

bool utc_time_t::is_leap() const
{
  return _leap;
}

int utc_time_t::Year() const
{
  // FIXME: Can this be done without calculating the month/day?
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = utcDayToDate(total_days);
  return year;
}

int utc_time_t::Month() const
{
  // FIXME: Can this be done without calculating the year/day?
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = utcDayToDate(total_days);
  return month;
}

int utc_time_t::Day() const
{
  // FIXME: Can this be done without calculating the year/month?
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = utcDayToDate(total_days);
  return day;
}

int utc_time_t::Hour() const
{
  auto [days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  return partial_days / fs_per_hour;
}

int utc_time_t::Minute() const
{
  auto [hours, partial_hours] = euclidean_div(_femtosecs, fs_per_hour);
  return partial_hours / fs_per_min;
}

double utc_time_t::Seconds() const
{
  auto [minutes, partial_minutes] = euclidean_div(_femtosecs, fs_per_min);
  return static_cast<double>(partial_minutes) / fs_per_sec + _leap;
}

int utc_time_t::WholeSeconds() const
{
  auto [minutes, partial_minutes] = euclidean_div(_femtosecs, fs_per_min);
  return partial_minutes / fs_per_sec + _leap;
}

int utc_time_t::Nanoseconds() const
{
  auto [seconds, partial_seconds] = euclidean_div(_femtosecs, fs_per_sec);
  return partial_seconds / fs_per_ns;
}

int utc_time_t::DayOfYear() const
{
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = utcDayToDate(total_days);
  // date2doy returns 0-indexed, we want 1-indexed
  return date2doy(year, month, day) + 1;
}

gps_time_t utc_time_t::ToGPS() const
{
  return gps_time_t::FromUTC(*this);
}

std::string utc_time_t::ToString() const
{
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = utcDayToDate(total_days);
  auto [hours, partial_hours] = euclidean_div(partial_days, fs_per_hour);
  auto [mins, partial_mins] = euclidean_div(partial_hours, fs_per_min);
  auto [secs, femtos] = euclidean_div(partial_mins, fs_per_sec);

  return fmt::format(
    "{:04}-{:02}-{:02}T{:02}:{:02}:{:02}.{:015}Z",
    year, month, day, hours, mins, secs + _leap, femtos
  );
}

std::string utc_time_t::DateString() const
{
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  auto [year, month, day] = utcDayToDate(total_days);
  return fmt::format("{:04}-{:02}-{:02}", year, month, day);
}

std::tuple<int, int, int> utc_time_t::ToDate() const
{
  auto [total_days, partial_days] = euclidean_div(_femtosecs, fs_per_day);
  return utcDayToDate(total_days);
}

bool utc_time_t::operator==(const utc_time_t &other) const {
  return _femtosecs == other._femtosecs;
}

bool utc_time_t::operator!=(const utc_time_t &other) const {
  return !this->operator==(other);
}

bool utc_time_t::operator<(const utc_time_t &other) const {
  // Because of how we store leap seconds here, this gets a little bit
  // complicated. If the two types have the same leap-second status, then the
  // calculation is simple. Otherwise, we need to modify one of them so they're
  // using the same basis (leap seconds are one second off from non-leap
  // seconds).
  if (_leap == other._leap) {
    return _femtosecs < other._femtosecs;
  } else {
    femtosecs_t delta = _leap - other._leap;
    return _femtosecs + delta * fs_per_sec < other._femtosecs;
  }
}

femtosecs_t duration_t::get_fs() const
{
  return _femtosecs;
}

long duration_t::total_days() const
{
  return _femtosecs / fs_per_day;
}

long duration_t::total_hours() const
{
  return _femtosecs / fs_per_hour;
}

long duration_t::total_seconds() const
{
  return _femtosecs / fs_per_sec;
}

long duration_t::total_milliseconds() const
{
  return _femtosecs / fs_per_ms;
}

long duration_t::total_microseconds() const
{
  return _femtosecs / fs_per_us;
}

long duration_t::total_nanoseconds() const
{
  return _femtosecs / fs_per_ns;
}

long duration_t::hours() const
{
  auto partial_days = _femtosecs % fs_per_day;
  return partial_days / fs_per_hour;
}

long duration_t::minutes() const
{
  auto partial_hours = _femtosecs % fs_per_hour;
  return partial_hours / fs_per_min;
}

long duration_t::seconds() const
{
  auto partial_mins = _femtosecs % fs_per_min;
  return partial_mins / fs_per_sec;
}

duration_t duration_t::from_years(int years)
{
  femtosecs_t femtos = years * fs_per_year;
  return duration_t(femtos);
}

duration_t duration_t::from_hours(femtosecs_t hours)
{
  femtosecs_t femtos = hours * fs_per_hour;
  return duration_t(femtos);
}

duration_t duration_t::from_mins(femtosecs_t mins)
{
  femtosecs_t femtos = mins * fs_per_min;
  return duration_t(femtos);
}

duration_t duration_t::from_mins(int mins)
{
  femtosecs_t femtos = static_cast<femtosecs_t>(mins) * fs_per_min;
  return duration_t(femtos);
}

duration_t duration_t::from_mins(long mins)
{
  femtosecs_t femtos = static_cast<femtosecs_t>(mins) * fs_per_min;
  return duration_t(femtos);
}

duration_t duration_t::from_mins_f(double mins)
{
  femtosecs_t femtos = mins * fs_per_min;
  return duration_t(femtos);
}

duration_t duration_t::from_mins_f(long double mins)
{
  femtosecs_t femtos = mins * fs_per_min;
  return duration_t(femtos);
}

duration_t duration_t::from_secs(long double seconds)
{
  femtosecs_t femtos = seconds * fs_per_sec;
  return duration_t(femtos);
}

duration_t duration_t::from_millis(femtosecs_t milliseconds)
{
  auto femtos = milliseconds * fs_per_ms;
  return duration_t(femtos);
}

duration_t duration_t::from_micros(femtosecs_t microseconds)
{
  auto femtos = microseconds * fs_per_us;
  return duration_t(femtos);
}

duration_t duration_t::from_nanos(femtosecs_t nanoseconds)
{
  auto femtos = nanoseconds * fs_per_ns;
  return duration_t(femtos);
}

long double duration_t::f_days() const
{
  auto whole_days = _femtosecs / fs_per_day;
  auto partial_days = _femtosecs % fs_per_day;
  return whole_days + static_cast<long double>(partial_days) / fs_per_day;
}

long double duration_t::f_minutes() const
{
  auto whole_mins = _femtosecs / fs_per_min;
  auto partial_mins = _femtosecs % fs_per_min;
  return whole_mins + static_cast<long double>(partial_mins) / fs_per_min;
}

long double duration_t::f_seconds() const
{
  auto whole_secs = _femtosecs / fs_per_sec;
  auto partial_secs = _femtosecs % fs_per_sec;
  return whole_secs + static_cast<long double>(partial_secs) / fs_per_sec;
}

duration_t duration_t::invert_sign() const
{
  return duration_t(-_femtosecs);
}

bool duration_t::is_negative() const
{
  return _femtosecs < 0;
}

bool duration_t::operator==(const duration_t &other) const
{
  return _femtosecs == other._femtosecs;
}

bool duration_t::operator<(const duration_t &other) const
{
  return _femtosecs < other._femtosecs;
}

duration_t duration_t::operator+(const duration_t& other) const
{
  return duration_t(_femtosecs + other._femtosecs);
}

duration_t duration_t::operator-(const duration_t& other) const
{
  return duration_t(_femtosecs - other._femtosecs);
}

duration_t duration_t::operator/(double other) const
{
  return duration_t(_femtosecs / other);
}

/** @brief Output operator */
std::ostream& operator<<(std::ostream& os, const gps_time_t& t) {
  os << t.ToString();           // ToString() already has a GPS_ in it
  return os;
}

/** @brief Output operator */
std::ostream& operator<<(std::ostream& os, const utc_time_t& t) {
  os << t.ToString();
  return os;
}

std::ostream& operator<<(std::ostream& os, const duration_t& t) {
  // TODO: Better representation of this
  os << t.f_minutes() << " mins";
  return os;
}

} /** namespace femtotime */

/**
 * @file GPStime.hpp
 * @author Patrick Norton <pnorton@lanl.gov>
 * @date 10 July 2022
*/
#pragma once

// [C/C++ headers]
#include <time.h>
#include <vector>
#include <cmath>
#include <string>

// [Femtotime headers]
#include "femtotime/time_constants.hpp"

// [Namespaces]
namespace femtotime {

typedef int128_t femtosecs_t;

// Forward declarations
class utc_time_t;
class duration_t;

/**
 * @class gps_time_t
 *
 * The purpose of this class is to wrap our scalar 128bit femtosecond
 * operaitons to represent GPS time (no leap seconds). This is
 * motivated by Los Alamos's Diorama library which is expected to use
 * GPS time internally.
 *
 */
class gps_time_t
{
public:
  /** @brief The list of leap seconds in GPS time */
  static std::vector<gps_time_t> leap_seconds;
  /** @brief The GPS epoch */
  static gps_time_t gps_epoch;
  /** @brief The UTC epoch */
  static gps_time_t utc_epoch;
  /** @brief constructor from a 128bit integer (femtoseconds since epoch) */
  explicit gps_time_t(femtosecs_t fs = 0) : _femtosecs(fs)
  {}
  /** @brief Constructor from timestamp with nanoseconds */
  gps_time_t(int y, int m, int d, int h, int min, int s, int n);
  /** @brief Constructor from timestamp with double-valued seconds */
  gps_time_t(int year, int month, int day,
             int hours, int minutes, long double secs);
  /** @brief get the femtoseconds since the epoch */
  femtosecs_t get_fs() const;
  /** @brief get the year */
  int Year() const;
  /** @brief get the month of year */
  int Month() const;
  /** @brief get the day of week */
  int Day() const;
  /** @brief get the hour of the day, 0-23 */
  int Hour() const;
  /** @brief get the minute of the hour */
  int Minute() const;
  /** @brief get the double-precision seconds of the minute */
  double Seconds() const;
  /** @brief get the integer seconds of the minute */
  int WholeSeconds() const;
  /** @brief get the nanoseconds of the second */
  int Nanoseconds() const;
  /** @brief get the days since the start of the year */
  int DayOfYear() const;
  /** @brief Get the double precision seconds since epoch */
  long double SecondsSinceEpoch() const;
  /** @brief Get the double precision seconds into the year */
  long double SecondsSinceYear() const;
  /** Get the double precision seconds into the year */
  long double SecondsSinceDay() const;
  /** @brief convert the time structure to a string */
  std::string ToString() const;
  /** @brief convert the time structure to a string with fewer digits */
  std::string ToStringBrief() const;
  /** @brief convert time structure to a date string */
  std::string DateString() const;
  /** @brief Determine if the year is a leap year */
  bool IsLeapYear() const;
  /** @brief Compute decimal year */
  double DecimalYear() const;
  /** @brief Get the date portion of the time */
  std::tuple<int, int, int> ToDate() const;
  /** @brief Convert a UTC time string to a gps_time_t */
  static gps_time_t FromUTCString(const std::string& utc_time);
  /** @brief Convert a GPS time string to a gps_time_t */
  static gps_time_t FromGPSString(const std::string& gps_time);
  // FIXME? Should this be a method on utc_time_t instead?
  /** @brief Convert an ISO-format UTC string to a gps_time_t */
  static gps_time_t FromISOString(const std::string& utc_time);
  /** @brief Convert a gps_time_t to a UTC string with fewere digits */
  std::string ToUTCStringBrief() const;
  /** @brief Convert a gps_time_t to a UTC string */
  std::string ToUTCString() const;
  /** @brief Convert a gps_time_t to a utc_time_t */
  utc_time_t ToUTC() const;
  /** @brief Convert a utc_time_t to a gps_time_t */
  static gps_time_t FromUTC(const utc_time_t &utc_time);
  /** @brief Convert a time_t (secs from UTC epoch) to a gps_time_t */
  static gps_time_t FromTime(time_t time);
  /** @brief Convert a POSIX struct timespec (secs and nanosecs from
      UTC epoch) to a gps_time_t */
  static gps_time_t FromTimespec(struct timespec *ts);
  bool operator==(const gps_time_t &other) const;
  bool operator!=(const gps_time_t &other) const;
  bool operator<(const gps_time_t &other) const;
  bool operator<=(const gps_time_t &other) const;
  bool operator>(const gps_time_t &other) const;
  bool operator>=(const gps_time_t &other) const;
  gps_time_t operator+(const duration_t &other) const;
  gps_time_t operator-(const duration_t &other) const;
  duration_t operator-(const gps_time_t &other) const;
  gps_time_t& operator+=(const duration_t &other);
private:
  femtosecs_t _femtosecs;
};

/**
 * @class utc_time_t
 *
 * Provide a class for storing UTC times. Seconds should be <61.
 */
class utc_time_t
{
public:
  /** @brief Constructor from individual fields */
  utc_time_t(int y, int mon, int d, int h, int min, int s, int n);
  /** @brief Constructor from individual fields */
  utc_time_t(int y, int mon, int d, int h, int min, long double s);
  /** @brief Default constructor */
  explicit utc_time_t(femtosecs_t femtos = 0) : _femtosecs(femtos), _leap(false)
  {}
  /** @brief Constructor with leap seconds */
  utc_time_t(femtosecs_t femtos, bool leap) : _femtosecs(femtos), _leap(leap)
  {}
  /** @brief The UTC times of leap seconds */
  static std::vector<utc_time_t> leap_seconds;
  /** @brief The GPS epoch, as a UTC time */
  static utc_time_t gps_epoch;
  /** @brief The UTC epoch */
  static utc_time_t utc_epoch;
  /** @brief The number of femtoseconds elapsed since the UTC epoch */
  femtosecs_t get_fs() const;
  /** @brief If the time is during a leap second */
  bool is_leap() const;
  /** @brief get the year */
  int Year() const;
  /** @brief get the month */
  int Month() const;
  /** @brief get the day */
  int Day() const;
  /** @brief get the hour */
  int Hour() const;
  /** @brief get the minute */
  int Minute() const;
  /** @brief get the seconds (including fractional part) */
  double Seconds() const;
  /** @brief get the integer part of the seconds */
  int WholeSeconds() const;
  /** @brief get the nanoseconds */
  int Nanoseconds() const;
  /** @brief get the days since the start of the year */
  int DayOfYear() const;
  /** @brief convert to gps_time_t */
  gps_time_t ToGPS() const;
  /** @brief convert the time structure to a UTC string */
  std::string ToString() const;
  /** @brief convert the time structure to a UTC string with fewer digits */
  std::string ToStringBrief() const;
  /** @brief convert time structure to a date string */
  std::string DateString() const;
  /** @brief Get the date portion of the time */
  std::tuple<int, int, int> ToDate() const;

  bool operator ==(const utc_time_t &other) const;
  bool operator !=(const utc_time_t &other) const;

  bool operator <(const utc_time_t &other) const;
  bool operator <=(const utc_time_t &other) const;
  bool operator >(const utc_time_t &other) const;
  bool operator >=(const utc_time_t &other) const;

private:
  // A quick note on the representation of leap seconds:
  //
  // Internally, this timekeeping method ignores leap seconds. This is because
  // doing things this way means that the timestamp-to-external-representation
  // relationship is kept consistent (and this is how Unix time is stored
  // internally). This allows us to reuse much of the conversion machinery from
  // `gps_time_t`, because leap seconds are ignored there as well. Because leap
  // seconds need to be represented, they are stored as the *previous* second,
  // with `_leap = true`. While this would complicate duration calculations,
  // that is not implemented in this class, so it is not something we need to
  // worry about. However, care should be taken when editing the comparison
  // methods (which are needed for `FromUTC`).
  femtosecs_t _femtosecs;
  bool _leap;
};

/**
 * @class duration_t
 *
 * A class representating time durations.
 */
class duration_t
{
public:
  duration_t(femtosecs_t femtos) : _femtosecs(femtos)
  {;}
  /** @brief The total number of femtoseconds in the duration */
  femtosecs_t get_fs() const;
  /** @brief The number of complete days (86400 seconds) */
  long total_days() const;
  /** @brief The number of complete hours */
  long total_hours() const;
  /** @brief The number of complete seconds */
  long total_seconds() const;
  /** @brief The number of complete milliseconds */
  long total_milliseconds() const;
  /** @brief The number of complete microseconds */
  long total_microseconds() const;
  /** @brief The number of complete nanoseconds */
  long total_nanoseconds() const;
  /** @brief The hour of the day (0-23) */
  long hours() const;
  /** @brief The minute of the current hour (0-59) */
  long minutes() const;
  /** @brief The second of the current minute (0-59) */
  long seconds() const;
  /** @brief The total number of days elapsed, including partial days */
  long double f_days() const;
  /** @brief The total number of minutes elapsed, including partial mins */
  long double f_minutes() const;
  /** @brief The total number of seconds elapsed, including partial secs */
  long double f_seconds() const;
  /** @brief Returns the additive inverse of the duration */
  duration_t invert_sign() const;
  /** @brief Returns if the duration is negative */
  bool is_negative() const;
  /** @brief Constructs a duration from an integer number of years */
  static duration_t from_years(int years);
  /** @brief Constructs a duration from an integer number of hours*/
  static duration_t from_hours(femtosecs_t hours);
  /** @brief Constructs a duration from an integer number of minutes */
  static duration_t from_mins(femtosecs_t mins);
  /** @brief Constructs a duration from an integer number of minutes */
  static duration_t from_mins(int mins);
  /** @brief Constructs a duration from an integer number of minutes */
  static duration_t from_mins(long mins);
  /** @brief Intentionally deleted overload--use from_mins_f instead */
  static duration_t from_mins(double mins) = delete;
  /** @brief Constructs a duration from a number of minutes */
  static duration_t from_mins_f(double mins);
  /** @brief Constructs a duration from a number of minutes */
  static duration_t from_mins_f(long double mins);
  /** @brief Constructs a duration from a number of seconds */
  static duration_t from_secs(long double seconds);
  /** @brief Constructs a duration from a number of milliseconds */
  static duration_t from_millis(femtosecs_t milliseconds);
  /** @brief Constructs a duration from an integer number of microseconds */
  static duration_t from_micros(femtosecs_t microseconds);
  /** @brief Constructs a duration from an integer number of nanoseconds */
  static duration_t from_nanos(femtosecs_t nanoseconds);
  /** @brief Constructs a duration from a POSIX timespec */
  static duration_t from_timespec(struct timespec *ts);
  bool operator<(const duration_t &other) const;
  bool operator<=(const duration_t &other) const;
  bool operator>(const duration_t &other) const;
  bool operator>=(const duration_t &other) const;
  bool operator==(const duration_t &other) const;
  bool operator!=(const duration_t &other) const;

  duration_t operator+(const duration_t &other) const;
  duration_t operator-(const duration_t &other) const;
  duration_t operator*(double other) const;
  duration_t operator*(femtosecs_t other) const;
  duration_t operator/(double other) const;
  duration_t operator/(femtosecs_t other) const;

private:
  femtosecs_t _femtosecs;
};

/** @brief Convert a UTC time string to a gps_time_t */
gps_time_t FromUTCString(const std::string& utc_time);
/** @brief Convert a UTC time string to a gps_time_t */
gps_time_t FromGPSString(const std::string& utc_time);
/** @brief Convert a gps_time_t to a UTC string */
std::string ToUTCString(const gps_time_t &gps_time);
/** @brief Convert a gps_time_t to a UTC string */
std::string ToUTCStringBrief(const gps_time_t &gps_time);

/** @brief Convert a gps_time_t to a utc_time_t */
utc_time_t ToUTC(const gps_time_t &gps_time);

/** @brief Convert a utc_time_t to a gps_time_t */
gps_time_t FromUTC(const utc_time_t &utc_time);

/** @brief Inspect a date string and convert to YYYY-MM-DD format if Julian*/
std::string String2Date(const std::string& date_string);

/** @brief Check if date string is modified Julian or UTC format */
bool IsJulian(const std::string &date_string);

/** @brief Calculate the number of leap seconds elapsed between two times */
int LeapSecondsBetween(const utc_time_t& time1, const utc_time_t& t2);

std::ostream& operator<<(std::ostream& os, const gps_time_t& t);
std::ostream& operator<<(std::ostream& os, const utc_time_t& t);
std::ostream& operator<<(std::ostream& os, const duration_t& t);

} /** namespace femtotime */

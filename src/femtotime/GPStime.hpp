/**
 * @file GPStime.hpp
 * @author Patrick Norton <pnorton@lanl.gov>
 * @date 10 July 2022
*/
#pragma once

// [C++ headers]
#include <vector>
#include <cmath>

// [MessagePack headers]
#include <msgpack.hpp>

// [DATUR headers]
#include <datur/datur.hpp>
#include <datur/msgpack_helpers.hpp>

// [TLE headers]
#include <tle/time_constants.hpp>

// [Namespaces]
namespace tle {
namespace time {

typedef int128_t femtosecs_t;

// Forward declarations
class utc_time_t;
class duration_t;

/**
 * @class gps_time_t
 *
 * The purpose of this class is to wrap the BOOST ptime structure and
 * use it to represent GPS time (no leap seconds). Diorama is expected to use
 * GPS time internally. 
 *
 * BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG is defined during the build
 * so that nanosecond resolution is available.  This is sufficient
 * (and better than GPS time precision) for absolute timestamps.  
 * Sub-nanosecond relative time is still available via
 * boost::units::quantity<si::time, double>.
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

  gps_time_t(int y, int m, int d, int h, int min, int s, int n);

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

  /** @brief convert time structure to a date string */
  std::string DateString() const;

  /** @brief Determine if the year is a leap year */
  bool IsLeapYear() const;

  /** @brief Compute decimal year */
  datur::units::year_time_type DecimalYear() const;

  /** @brief Get the date portion of the time */
  std::tuple<int, int, int> ToDate() const;

  /** @brief Convert a UTC time string to a gps_time_t */
  static gps_time_t FromUTCString(const std::string& utc_time);

  /** @brief Convert a GPS time string to a gps_time_t */
  static gps_time_t FromGPSString(const std::string& gps_time);

  // FIXME? Should this be a method on utc_time_t instead?
  /** @brief Convert an ISO-format UTC string to a gps_time_t */
  static gps_time_t FromISOString(const std::string& utc_time);

  /** @brief Convert a gps_time_t to a UTC string */
  std::string ToUTCString() const;

  /** @brief Convert a gps_time_t to a utc_time_t */
  utc_time_t ToUTC() const;

  /** @brief Convert a utc_time_t to a gps_time_t */
  static gps_time_t FromUTC(const utc_time_t &utc_time);

  /** @brief Convert a time_t (secs from UTC epoch) to a gps_time_t */
  static gps_time_t FromTime(time_t time);

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

  // /** @brief Construct from BOOST posix_time ptime. */
  // utc_time_t(const boost::posix_time::ptime& t);

  /** @brief The UTC times of leap seconds */
  static std::vector<utc_time_t> leap_seconds;

  /** @brief The GPS epoch, as a UTC time */
  static utc_time_t gps_epoch;

  /** @brief The UTC epoch */
  static utc_time_t utc_epoch;

  femtosecs_t get_fs() const;

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
  
  /** @brief convert time structure to a date string */
  std::string DateString() const;

  /** @brief Get the date portion of the time */
  std::tuple<int, int, int> ToDate() const;

  bool operator ==(const utc_time_t &other) const;
  bool operator !=(const utc_time_t &other) const;

  bool operator <(const utc_time_t &other) const;

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

  femtosecs_t get_fs() const;

  long total_days() const;
  long total_hours() const;
  long total_seconds() const;
  long total_milliseconds() const;
  long total_microseconds() const;
  long total_nanoseconds() const;

  long hours() const;
  long minutes() const;
  long seconds() const;

  long double f_days() const;
  long double f_minutes() const;
  long double f_seconds() const;

  duration_t invert_sign() const;
  bool is_negative() const;

  static duration_t from_years(int years);
  static duration_t from_hours(femtosecs_t hours);
  static duration_t from_mins(femtosecs_t mins);
  static duration_t from_mins(int mins);
  static duration_t from_mins(long mins);
  static duration_t from_mins(double mins) = delete;
  static duration_t from_mins_f(double mins);
  static duration_t from_mins_f(long double mins);
  static duration_t from_secs(long double seconds);
  static duration_t from_millis(femtosecs_t milliseconds);
  static duration_t from_micros(femtosecs_t microseconds);
  static duration_t from_nanos(femtosecs_t nanoseconds);

  bool operator<(const duration_t &other) const;
  bool operator==(const duration_t &other) const;

  duration_t operator+(const duration_t &other) const;
  duration_t operator-(const duration_t &other) const;
  duration_t operator/(double other) const;

private:
  femtosecs_t _femtosecs;
};

/** @brief Convert a UTC time string to a gps_time_t */
gps_time_t FromUTCString(const std::string& utc_time);

/** @brief Convert a UTC time string to a gps_time_t */
gps_time_t FromGPSString(const std::string& utc_time);

/** @brief Convert a gps_time_t to a UTC string */
std::string ToUTCString(const gps_time_t &gps_time);

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

std::ostream& operator<<(std::ostream& os, const tle::time::gps_time_t& t);
std::ostream& operator<<(std::ostream& os, const tle::time::utc_time_t& t);
std::ostream& operator<<(std::ostream& os, const tle::time::duration_t& t);

} /** namespace time */
} /** namespace tle */

// Hacky MessagePack operators
// TODO: Remove MessagePack altogether
namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(v2) {
namespace adaptor {

template<>
struct pack<tle::time::gps_time_t> {
  template<typename Stream>
  packer<Stream>& operator()(msgpack::packer<Stream>& o,
                             const tle::time::gps_time_t &t) const {
    // NOTE: Technically, this won't work properly on systems that use
    // ones-complement for their arithmetic (it's not UB, just
    // implementation-defined). There is no way that we are compiling on one of
    // those systems (and it will be illegal in C++20 anyawys).
    auto femtos = static_cast<tle::time::uint128_t>(t.get_fs());
    auto high_bits = static_cast<uint64_t>(femtos >> 64);
    auto low_bits = static_cast<uint64_t>(femtos);
    o.pack_array(2);
    o.pack_uint64(high_bits);
    o.pack_uint64(low_bits);
    return o;
  }
};

template<>
struct convert<tle::time::gps_time_t> {
  msgpack::object const& operator()(msgpack::object const& o,
                                    tle::time::gps_time_t& t) const {
    if (o.type != msgpack::type::ARRAY || o.via.array.size != 2) {
      throw msgpack::type_error();
    }
    auto high_bits = o.via.array.ptr[0].via.u64;
    auto low_bits = o.via.array.ptr[1].via.u64;
    auto femtos = (static_cast<tle::time::uint128_t>(high_bits) << 64)
      + static_cast<tle::time::uint128_t>(low_bits);
    t = tle::time::gps_time_t(static_cast<tle::time::int128_t>(femtos));
    return o;
  }
};

} // namespace adaptor
} // namespace MSGPACK_API_VERSION_NAMESPACE
} // namespace msgpack


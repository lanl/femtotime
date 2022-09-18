/**
 * @file   test_utc_time.cpp
 * @author Patrick Norton <pnorton@lanl.gov>
 * @brief  UTC time tests
 *
 */

// [CPPUNIT headers]
#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

// [TLE headers]
#include "femtotime/GPStime.hpp"

// [Namespaces]
using namespace std;
using namespace femtotime;

/**
 * @class UTCTimeCppUnit
 */
class UTCTimeCppUnit : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(UTCTimeCppUnit);
  CPPUNIT_TEST(test_day_of_year);
  CPPUNIT_TEST(test_to_date);
  CPPUNIT_TEST(test_to_string);
  CPPUNIT_TEST(test_is_leap);
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp() {}
  void tearDown() {}
  void test_day_of_year();
  void test_to_date();
  void test_to_string();
  void test_is_leap();
};
CPPUNIT_TEST_SUITE_REGISTRATION(UTCTimeCppUnit);

void UTCTimeCppUnit::test_day_of_year()
{
  utc_time_t time1(2000, 1, 1, 0, 0, 0, 0);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Jan. 1 returns wrong day of year", 1, time1.DayOfYear()
  );

  utc_time_t time2(2000, 2, 14, 0, 0, 0, 0);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Feb. 14 returns wrong day of year", 45, time2.DayOfYear()
  );

  utc_time_t time3(2000, 1, 1, 12, 34, 56, 0);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Partial days not working for DayOfYear()", 1, time3.DayOfYear()
  );

  utc_time_t time4(2000, 2, 29, 0, 0, 0, 0);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Leap day returns wrong day of year", 60, time4.DayOfYear()
  );

  utc_time_t time5(2001, 3, 1, 0, 0, 0, 0);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Mar. 1 returns wrong day of year in non-leap year", 60, time5.DayOfYear()
  );

  utc_time_t time6(2000, 3, 1, 0, 0, 0, 0);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Mar. 1 returns wrong day of year in leap year", 61, time6.DayOfYear()
  );

  utc_time_t time7(2001, 12, 31, 0, 0, 0, 0);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Last day of year is incorrect in non-leap year", 365, time7.DayOfYear()
  );

  utc_time_t time8(2000, 12, 31, 0, 0, 0, 0);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Last day of year is incorrect in leap year", 366, time8.DayOfYear()
  );

  utc_time_t time9(1900, 3, 1, 0, 0, 0, 0);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Mar. 1, 1900 incorrect (not a leap year)", 60, time9.DayOfYear()
  );
}

void UTCTimeCppUnit::test_to_date()
{
  std::vector<utc_time_t> times = {
    utc_time_t(2000, 1, 1, 0, 0, 0, 0),
    utc_time_t(2000, 2, 14, 0, 0, 0, 0),
    utc_time_t(2000, 1, 1, 12, 34, 56, 0),
    utc_time_t(2001, 3, 1, 0, 0, 0, 0),
    utc_time_t(2000, 3, 1, 0, 0, 0, 0),
    utc_time_t(2001, 12, 31, 0, 0, 0, 0),
    utc_time_t(2001, 12, 31, 0, 0, 0, 0),
    utc_time_t(1900, 3, 1, 0, 0, 0, 0),
  };

  std::vector<std::tuple<int, int, int>> expected = {
    {2000, 1, 1},
    {2000, 2, 14},
    {2000, 1, 1},
    {2001, 3, 1},
    {2000, 3, 1},
    {2001, 12, 31},
    {2001, 12, 31},
    {1900, 3, 1},
  };

  for (size_t i = 0; i < times.size(); i++) {
    std::string msg = "ToDate() conversion incorrect for UTC time "
        + times[i].ToString() + ": ";
    auto [t_year, t_mon, t_day] = times[i].ToDate();
    auto [e_year, e_mon, e_day] = expected[i];

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
       msg + "Year is incorrect", t_year, e_year
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
       msg + "Month is incorrect", t_mon, e_mon
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
       msg + "Day is incorrect", t_day, e_day
    );
  }
}

void UTCTimeCppUnit::test_to_string()
{
  std::vector<utc_time_t> times = {
    utc_time_t(2000, 1, 1, 0, 0, 0, 0),
    utc_time_t(2000, 2, 19, 0, 0, 0, 0),
    utc_time_t(2000, 2, 29, 0, 0, 0, 0),
    utc_time_t(2000, 3, 1, 0, 0, 0, 0),
    utc_time_t(2001, 3, 1, 0, 0, 0, 0),
    utc_time_t(2003, 7, 12, 5, 14, 23, 500'000'000),
    utc_time_t(2005, 12, 31, 23, 59, 60, 0),
  };

  std::vector<std::string> expected = {
    "2000-01-01T00:00:00.000000000000000Z",
    "2000-02-19T00:00:00.000000000000000Z",
    "2000-02-29T00:00:00.000000000000000Z",
    "2000-03-01T00:00:00.000000000000000Z",
    "2001-03-01T00:00:00.000000000000000Z",
    "2003-07-12T05:14:23.500000000000000Z",
    "2005-12-31T23:59:60.000000000000000Z",
  };

  for (size_t i = 0; i < times.size(); i++) {
    CPPUNIT_ASSERT_EQUAL(expected[i], times[i].ToString());
  }
}

void UTCTimeCppUnit::test_is_leap()
{
  std::vector<utc_time_t> non_leap_times = {
    utc_time_t(2000, 1, 1, 0, 0, 0, 0),
    utc_time_t(2000, 2, 29, 0, 0, 0, 0), // Leap second, not leap day :)
    utc_time_t(2005, 2, 15, 0, 0, 0, 0),
    utc_time_t(2005, 12, 31, 23, 59, 59, 0),
    utc_time_t(1945, 3, 17, 12, 0, 0, 0),
    utc_time_t(0, false),
    utc_time_t(48'828'799 * fs_per_sec, false), // 1980-06-30T23:59:59Z
    utc_time_t::utc_epoch,
    utc_time_t::gps_epoch,
  };

  for (const auto& time : non_leap_times) {
    std::string msg = "Time " + time.ToString()
        + " should not be a leap second";
    CPPUNIT_ASSERT_MESSAGE(msg, !time.is_leap());
  }

  for (const auto& time : utc_time_t::leap_seconds) {
    std::string msg = std::string("All values in utc_time_t::leap_seconds ")
        + "should be leap seconds, but " + time.ToString() + " isn't";
    CPPUNIT_ASSERT_MESSAGE(msg, time.is_leap());
  }
}

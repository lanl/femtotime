/**
 * @file   test_gps_time.cpp
 * @author Patrick Norton <pnorton@lanl.gov>
 * @brief  GPS time tests
 *
 */

#include <string.h>

// [CPPUNIT headers]
#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#ifdef HAVE_MSGPACK
// [MessagePack headers]
#include <msgpack.hpp>
#endif // HAVE_MSGPACK

// [TLE headers]
#include "femtotime/GPStime.hpp"

// [Namespaces]
using namespace std;
using namespace femtotime;

namespace test {

/** 
 * @class GPSTimeCppUnit 
*/
class GPSTimeCppUnit : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(GPSTimeCppUnit);
  CPPUNIT_TEST(test_leap_seconds);
  CPPUNIT_TEST(test_utc);
  CPPUNIT_TEST(test_t_to_gps_and_back);
  CPPUNIT_TEST(test_timespec_to_gps);
  CPPUNIT_TEST(test_isleapyear);
  CPPUNIT_TEST(test_decimalyear);
  CPPUNIT_TEST(test_leap_seconds_equal);
  CPPUNIT_TEST(test_gps_epoch);
  CPPUNIT_TEST(test_from_utc);
  CPPUNIT_TEST(test_to_utc);
  CPPUNIT_TEST(test_leap_second_order);
  CPPUNIT_TEST(test_from_gps_str);
  CPPUNIT_TEST_SUITE_END();
public: 
  void setUp(){}
  void tearDown(){}
  void test_leap_seconds();
  void test_utc();
  void test_t_to_gps_and_back();
  void test_timespec_to_gps();
  void test_isleapyear();
  void test_decimalyear();
  void test_leap_seconds_equal();
  void test_gps_epoch();
  void test_from_utc();
  void test_to_utc();
  void test_leap_second_order();
  void test_from_gps_str();
};
CPPUNIT_TEST_SUITE_REGISTRATION(GPSTimeCppUnit);

/**
 * @brief Test leap second integration
 */
void GPSTimeCppUnit::test_leap_seconds()
{
  // 1. Test one second before and after a leap second
  auto time1 = FromUTCString("2015-06-30T23:59:59.0Z");
  auto time2 = FromUTCString("2015-07-01T00:00:00.0Z");  
  auto dt = time2 - time1;
  cout << time1 << "   " << time2 << "   dt: " << dt
       << "   dt.seconds(): " << dt.seconds()
       << "   true?: " << (dt.seconds() == 2) << "\n";
  CPPUNIT_ASSERT_EQUAL(long(dt.seconds()), 2l);

  time1 = FromUTC(utc_time_t(2015, 6, 30, 23, 59, 59, 0));
  time2 = FromUTC(utc_time_t(2015, 7, 1, 0, 0, 0, 0));
  dt = time2 - time1;
  cout << time1 << "   " << time2 << "   dt: " << dt
       << "   dt.seconds(): " << dt.seconds()
       << "   true?: " << (dt.seconds() == 2) << "\n";
  CPPUNIT_ASSERT_EQUAL(long(dt.seconds()), 2l);

  // 2. Test before and after all leap seconds (announced as of 2016-12-31)
  time1 = FromUTCString("1970-01-01T00:00:00.0Z");
  time2 = FromUTCString("2017-01-01T00:00:00.0Z");  
  dt = time2 - time1;
  // Note that the number of seconds ends in 27--that is the number of leap
  // seconds that have elapsed between these timestamps
  CPPUNIT_ASSERT_EQUAL(dt.total_milliseconds(), 1'483'228'827'000l);
    
  // 3. Test at a fractional second straddling a leap second
  time1 = FromUTCString("2015-06-30T23:59:59.5Z");
  time2 = FromUTCString("2015-07-01T00:00:00.0Z");
  dt = time2 - time1;
  CPPUNIT_ASSERT_EQUAL(dt.total_milliseconds(), 1500l);
    
  // 4. check the end point being fractionally below a leap second
  time1 = FromUTCString("2015-06-30T23:59:58.0Z");
  time2 = FromUTCString("2015-06-30T23:59:59.5Z");
  dt = time2 - time1;
  CPPUNIT_ASSERT_EQUAL(dt.total_milliseconds(), 1500l);
    
  // 5. Test a negative time difference
  time1 = FromUTCString("2015-07-01T00:00:00.0Z");
  time2 = FromUTCString("2015-06-30T23:59:59.0Z");
  dt = time2 - time1;
  CPPUNIT_ASSERT_EQUAL(long(dt.seconds()), -2l);
    
  // 6. Test the subtraction of leap seconds before the GPS epoch
  time1 = FromUTCString("1979-12-31T23:59:59.0Z");
  time2 = FromUTCString("1980-01-01T00:00:00.0Z");
  dt = time2 - time1;
  CPPUNIT_ASSERT_EQUAL(long(dt.seconds()), 2l);
    
  // 7. Test conversion from GPS to UTC
  time1 = gps_time_t(1981,07,01, 00,00,00);
  std::string stime = ToUTCString(time1);
  std::string expected = "1981-06-30T23:59:60.000000000000000Z";
  CPPUNIT_ASSERT_EQUAL(expected, stime);
    
  // 8. Test the calculation of seconds since the epoch
  time1 = gps_time_t(1970,01,01, 12,00,00);
  auto test_seconds_e = time1.SecondsSinceEpoch();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("SecondsSinceEpoch calculation failed",
                               test_seconds_e, 12*60*60.l);

  // 9. Test the calculation of seconds since the beginning of the year
  time2 = gps_time_t(2014,01,01, 12,00,00);
  auto test_seconds_y = time2.SecondsSinceYear();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("SecondsSinceYear calculation failed",
                               test_seconds_y, 12*60*60.l);

  // 10. test addition operations in gps_time_t
  time1 = gps_time_t(2014,11,14,0,0,0,0);
  time2 = gps_time_t(2014,11,15,0,0,0,0);
  duration_t td_1_day(fs_per_day);

  auto time3 = time1 + td_1_day;
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "gps_time_t::operator(time_duration) failed", time3, time2
  );

  std::string date1=time1.DateString();
  std::string expected1 = "2014-11-14";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "gps_time_t::operator(DateString) failed", expected1, date1
  );

  auto time6 = gps_time_t(1980,1,5,0,0,0,0);
  auto time6_utc = ToUTCString(time6);       
  cout<<"gps_epoch="<<ToUTCString(gps_time_t::gps_epoch);
  cout<<"time6_utc="<<time6_utc;
}

/**
 * @brief Test utc_time_t conversions
 */
void GPSTimeCppUnit::test_utc() 
{
  // check the converting to and from ptime works
  string timestr = "2015-06-01T12:10:08.123456789000000Z";
  gps_time_t t_gps = FromUTCString(timestr);
  utc_time_t t_utc = ToUTC(t_gps);
    
  // compare UTC string outputs    
  string sout = t_utc.ToString();
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "UTC time conversion failed", timestr, sout
  );
}

void GPSTimeCppUnit::test_t_to_gps_and_back()
{
  // check the converting from gps_time_t to string and back works
  gps_time_t tin = FromUTCString("2015-06-01T12:10:08.123456789Z");
  string t_str = tin.ToString();
  // make sure format is good
  CPPUNIT_ASSERT(strncmp(t_str.c_str(), "GPS_", 4) == 0);
  gps_time_t t_return = FromGPSString(t_str);
  CPPUNIT_ASSERT_EQUAL(t_return, tin);
  cout << "\n" << "Time [in]  : " << tin << "\n"
       << "Time [out] : " << t_return << "\n";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "gps_time_t->gps_time_string->gps_time_t conversion failed",
    tin, t_return
  );
}

void GPSTimeCppUnit::test_timespec_to_gps()
{
  // check the converting from POSIX timespec to gps_time_t
  // stuct timeval tm;
  // timerclear(&tm);
  struct timespec ts;
  // strptime("2001-11-12 18:31:01", "%Y-%m-%d %H:%M:%S", &tm);
  // strftime(buf, sizeof(buf), "%d %b %Y %H:%M", &tm);
  // puts(buf);
  time_t unix_time_now = time(NULL);
  // gps_time_t gt = gps_time_t::FromTime(unix_time_now);
  ts.tv_sec = unix_time_now;
  ts.tv_nsec = 17'000'720;
  gps_time_t gtspec = gps_time_t::FromTimespec(&ts);
  // IMPROVEME: here we could test the POSIX time conversion to years
  // and hours and so forth, but for now I just test that the
  // nanoseconds are correct
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "nanoseconds from timespec were not properly recorded",
    (long) gtspec.Nanoseconds(), ts.tv_nsec);
}

/**
 * @brief Test leap year check
 */
void GPSTimeCppUnit::test_isleapyear() {
  // Setup inputs and results
  std::vector<gps_time_t> times = {
  gps_time_t(1600, 1, 1, 0, 0, 0),
  gps_time_t(1608, 1, 1, 0, 0, 0),
  gps_time_t(1703, 1, 1, 0, 0, 0),
  gps_time_t(1900, 1, 1, 0, 0, 0),
  gps_time_t(1902, 1, 1, 0, 0, 0),
  gps_time_t(1904, 1, 1, 0, 0, 0),
  gps_time_t(2000, 1, 1, 0, 0, 0),
  gps_time_t(2004, 1, 1, 0, 0, 0)};

  std::vector<bool> computed;
  std::vector<bool> expected = {true, true, false, false,
  false, true, true, true};

  // Calculate whether each date is a leap year
  for(size_t i=0; i<times.size(); i++) {
    computed.push_back(times[i].IsLeapYear());
  }

  // Compare the computed and expected results
  for(size_t i=0; i<expected.size(); i++) {
    std::string msg = "IsLeapYear() check failed for " + times[i].ToString();
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, expected[i], computed[i]);
  }
}


/**
 * @brief Test decimal year calculation
 */
void GPSTimeCppUnit::test_decimalyear() {
    // Setup inputs and results
  std::vector<gps_time_t> times = {
  gps_time_t(1600, 1, 1, 1, 1, 1),
  gps_time_t(1608, 2, 2, 2, 2, 2),
  gps_time_t(1703, 3, 3, 3, 3, 3),
  gps_time_t(1900, 4, 4, 4, 4, 4),
  gps_time_t(1902, 5, 5, 5, 5, 5),
  gps_time_t(1904, 6, 6, 6, 6, 6),
  gps_time_t(2000, 7, 7, 7, 7, 7),
  gps_time_t(2004, 8, 8, 8, 8, 8)};

  std::vector<double> computed;
  std::vector<double> expected = {
    1600.00011570, 1608.08766324,
    1703.16747156, 1900.25525888,
    1902.34030648, 1904.42965638,
    2000.51447161, 2004.60201908
  };

  // Calculate decimal year for each date
  for(size_t i=0; i<times.size(); i++) {
    computed.push_back(times[i].DecimalYear());
  }

  // Compare the computed and expected results
  for(size_t i=0; i<expected.size(); i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
      "DecimalYear() check failed",
      expected[i], computed[i], 1.0e-6
    );
  }
}

void GPSTimeCppUnit::test_leap_seconds_equal() {
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Leap second lists should have equal length",
    gps_time_t::leap_seconds.size(), utc_time_t::leap_seconds.size()
  );

  for (size_t i = 0; i < gps_time_t::leap_seconds.size(); i++) {
    const auto& gps_leap = gps_time_t::leap_seconds[i];
    const auto& utc_leap = utc_time_t::leap_seconds[i];

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "ToUTC() conversion failed",
      utc_leap, ToUTC(gps_leap)
    );
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "FromUTC() conversion failed",
      gps_leap, FromUTC(utc_leap)
    );
  }
}

void GPSTimeCppUnit::test_gps_epoch() {
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "gps_time_t::gps_epoch has the incorrect date/time",
    gps_time_t(1980, 1, 6, 0, 0, 0, 0),
    gps_time_t::gps_epoch
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "utc_time_t::gps_epoch has the incorrect date/time",
    utc_time_t(1980, 1, 6, 0, 0, 0, 0),
    utc_time_t::gps_epoch
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Incorrect conversion of GPS epoch to UTC",
    utc_time_t::gps_epoch, ToUTC(gps_time_t::gps_epoch)
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Incorrect conversion of GPS epoch from UTC",
    gps_time_t::gps_epoch, FromUTC(utc_time_t::gps_epoch)
  );
}

void GPSTimeCppUnit::test_from_utc() {
  // Test the GPS epoch
  auto time1 = gps_time_t(1980, 1, 6, 0, 0, 0, 0);
  auto time2 = FromUTC(utc_time_t(1980, 1, 6, 0, 0, 0, 0));
  auto time3 = FromUTCString("1980-01-06T00:00:00.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "GPS epoch converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "GPS epoch converts incorrectly from string",
    time1, time3
  );

  // Test before all leap seconds--UTC should be 9 seconds ahead
  time1 = gps_time_t(1972, 1, 1, 0, 0, 0, 0);
  time2 = FromUTC(utc_time_t(1972, 1, 1, 0, 0, 9, 0));
  time3 = FromUTCString("1972-01-01T00:00:09.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Times before all leap seconds convert incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Times before all leap seconds convert incorrectly from string",
    time1, time3
  );

  // Test after all leap seconds (at time of writing)
  // UTC should be 18 seconds behind
  time1 = gps_time_t(2020, 1, 1, 0, 0, 18, 0);
  time2 = FromUTC(utc_time_t(2020, 1, 1, 0, 0, 0, 0));
  time3 = FromUTCString("2020-01-01T00:00:00.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Times after all leap seconds convert incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Times after all leap seconds convert incorrectly from string",
    time1, time3
  );

  // Test the beginning of the first leap second after the epoch
  time1 = gps_time_t(1981, 7, 1, 0, 0, 0, 0);
  time2 = FromUTC(utc_time_t(1981, 6, 30, 23, 59, 60, 0));
  time3 = FromUTCString("1981-06-30T23:59:60.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the first leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the first leap second converts incorrectly from string",
    time1, time3
  );

  // Test the middle of the first leap second after the epoch
  time1 = gps_time_t(1981, 7, 1, 0, 0, 0, 500'000'000);
  time2 = FromUTC(utc_time_t(1981, 6, 30, 23, 59, 60, 500'000'000));
  time3 = FromUTCString("1981-06-30T23:59:60.5Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the first leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the first leap second converts incorrectly from string",
    time1, time3
  );

  // Test immediately after the first leap second after the epoch
  // UTC should now be off by 1 second
  time1 = gps_time_t(1981, 7, 1, 0, 0, 1, 0);
  time2 = FromUTC(utc_time_t(1981, 7, 1, 0, 0, 0, 0));
  time3 = FromUTCString("1981-07-01T00:00:00.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the first leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the first leap second converts incorrectly from string",
    time1, time3
  );

  // Test the beginning of a later leap second
  time1 = gps_time_t(2006, 1, 1, 0, 0, 13, 0);
  time2 = FromUTC(utc_time_t(2005, 12, 31, 23, 59, 60, 0));
  time3 = FromUTCString("2005-12-31T23:59:60.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the 2005-06 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the 2005-06 leap second converts incorrectly from string",
    time1, time3
  );

  // Test the middle of a later leap second
  time1 = gps_time_t(2006, 1, 1, 0, 0, 13, 500'000'000);
  time2 = FromUTC(utc_time_t(2005, 12, 31, 23, 59, 60, 500'000'000));
  time3 = FromUTCString("2005-12-31T23:59:60.5Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the 2005-06 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the 2005-06 leap second converts incorrectly from string",
    time1, time3
  );

  // Test immediately after a later leap second
  // UTC should now be off by 1 additional second
  time1 = gps_time_t(2006, 1, 1, 0, 0, 14, 0);
  time2 = FromUTC(utc_time_t(2006, 1, 1, 0, 0, 0, 0));
  time3 = FromUTCString("2006-01-01T00:00:00.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the 2005-06 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the 2005-06 leap second converts incorrectly from string",
    time1, time3
  );

  // Test the beginning of an earlier leap second
  time1 = gps_time_t(1976, 12, 31, 23, 59, 56, 0);
  time2 = FromUTC(utc_time_t(1976, 12, 31, 23, 59, 60, 0));
  time3 = FromUTCString("1976-12-31T23:59:60.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the 1976-77 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the 1976-77 leap second converts incorrectly from string",
    time1, time3
  );

  // Test the middle of a later leap second
  time1 = gps_time_t(1976, 12, 31, 23, 59, 56, 500'000'000);
  time2 = FromUTC(utc_time_t(1976, 12, 31, 23, 59, 60, 500'000'000));
  time3 = FromUTCString("1976-12-31T23:59:60.5Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the 1976-77 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the 1976-77 leap second converts incorrectly from string",
    time1, time3
  );

  // Test immediately after a later leap second
  // UTC should now be off by 1 additional second
  time1 = gps_time_t(1976, 12, 31, 23, 59, 57, 0);
  time2 = FromUTC(utc_time_t(1977, 1, 1, 0, 0, 0, 0));
  time3 = FromUTCString("1977-01-01T00:00:00.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the 1976-77 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the 1976-77 leap second converts incorrectly from string",
    time1, time3
  );
}

void GPSTimeCppUnit::test_to_utc() {
  // Test the GPS epoch
  auto time1 = utc_time_t(1980, 1, 6, 0, 0, 0, 0);
  auto timegps = gps_time_t(1980, 1, 6, 0, 0, 0, 0);
  auto time2 = ToUTC(timegps);
  auto time3 = ToUTCString(timegps);
  std::string timestr = "1980-01-06T00:00:00.000000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "GPS epoch converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "GPS epoch converts incorrectly to string",
    timestr, time3
  );

  // Test before all leap seconds--UTC should be 9 seconds ahead
  time1 = utc_time_t(1972, 1, 1, 0, 0, 9, 0);
  timegps = gps_time_t(1972, 1, 1, 0, 0, 0, 0);
  time2 = ToUTC(timegps);
  time3 = ToUTCString(timegps);
  timestr = "1972-01-01T00:00:09.000000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Times before all leap seconds convert incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Times before all leap seconds convert incorrectly to string",
    timestr, time3
  );

  // Test after all leap seconds (at time of writing)
  // UTC should be 18 seconds behind
  time1 = utc_time_t(2020, 1, 1, 0, 0, 0, 0);
  timegps = gps_time_t(2020, 1, 1, 0, 0, 18, 0);
  time2 = ToUTC(timegps);
  time3 = ToUTCString(timegps);
  timestr = "2020-01-01T00:00:00.000000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Times after all leap seconds convert incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Times after all leap seconds convert incorrectly from string",
    timestr, time3
  );

  // Test the beginning of the first leap second after the epoch
  time1 = utc_time_t(1981, 6, 30, 23, 59, 60, 0);
  timegps = gps_time_t(1981, 7, 1, 0, 0, 0, 0);
  time2 = ToUTC(timegps);
  time3 = ToUTCString(timegps);
  timestr = "1981-06-30T23:59:60.000000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the first leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the first leap second converts incorrectly to string",
    timestr, time3
  );

  // Test the middle of the first leap second after the epoch
  time1 = utc_time_t(1981, 6, 30, 23, 59, 60, 500'000'000);
  timegps = gps_time_t(1981, 7, 1, 0, 0, 0, 500'000'000);
  time2 = ToUTC(timegps);
  time3 = ToUTCString(timegps);
  timestr = "1981-06-30T23:59:60.500000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the first leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the first leap second converts incorrectly to string",
    timestr, time3
  );

  // Test immediately after the first leap second after the epoch
  // UTC should now be off by 1 second
  time1 = utc_time_t(1981, 7, 1, 0, 0, 0, 0);
  timegps = gps_time_t(1981, 7, 1, 0, 0, 1, 0);
  time2 = ToUTC(timegps);
  time3 = ToUTCString(timegps);
  timestr = "1981-07-01T00:00:00.000000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the first leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the first leap second converts incorrectly to string",
    timestr, time3
  );

  // Test the beginning of a later leap second
  time1 = utc_time_t(2005, 12, 31, 23, 59, 60, 0);
  timegps = gps_time_t(2006, 1, 1, 0, 0, 13, 0);
  time2 = ToUTC(timegps);
  time3 = ToUTCString(timegps);
  timestr = "2005-12-31T23:59:60.000000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the 2005-06 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the 2005-06 leap second converts incorrectly to string",
    timestr, time3
  );

  // Test the middle of a later leap second
  time1 = utc_time_t(2005, 12, 31, 23, 59, 60, 500'000'000);
  timegps = gps_time_t(2006, 1, 1, 0, 0, 13, 500'000'000);
  time2 = ToUTC(timegps);
  time3 = ToUTCString(timegps);
  timestr = "2005-12-31T23:59:60.500000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the 2005-06 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the 2005-06 leap second converts incorrectly to string",
    timestr, time3
  );

  // Test immediately after a later leap second
  // UTC should now be off by 1 additional second
  time1 = utc_time_t(2006, 1, 1, 0, 0, 0, 0);
  timegps = gps_time_t(2006, 1, 1, 0, 0, 14, 0);
  time2 = ToUTC(timegps);
  time3 = ToUTCString(timegps);
  timestr = "2006-01-01T00:00:00.000000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the 2005-06 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the 2005-06 leap second converts incorrectly to string",
    timestr, time3
  );

  // Test the beginning of an earlier leap second
  time1 = utc_time_t(1976, 12, 31, 23, 59, 60, 0);
  timegps = gps_time_t(1976, 12, 31, 23, 59, 56, 0);
  time2 = ToUTC(timegps);
  time3 = ToUTCString(timegps);
  timestr = "1976-12-31T23:59:60.000000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the 1976-77 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the 1976-77 leap second converts incorrectly to string",
    timestr, time3
  );

  // Test the middle of a later leap second
  time1 = utc_time_t(1976, 12, 31, 23, 59, 60, 500'000'000);
  timegps = gps_time_t(1976, 12, 31, 23, 59, 56, 500'000'000);
  time2 = ToUTC(timegps);
  time3 = ToUTCString(timegps);
  timestr = "1976-12-31T23:59:60.500000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the 1976-77 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the 1976-77 leap second converts incorrectly to string",
    timestr, time3
  );

  // Test immediately after a later leap second
  // UTC should now be off by 1 additional second
  time1 = utc_time_t(1977, 1, 1, 0, 0, 0, 0);
  timegps = gps_time_t(1976, 12, 31, 23, 59, 57, 0);
  time2 = ToUTC(timegps);
  time3 = ToUTCString(timegps);
  timestr = "1977-01-01T00:00:00.000000000000000Z";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the 1976-77 leap second converts incorrectly",
    time1, time2
  );
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the 1976-77 leap second converts incorrectly to string",
    timestr, time3
  );
}

void GPSTimeCppUnit::test_leap_second_order() {
  for (size_t i = 0; i < gps_time_t::leap_seconds.size() - 1; i++) {
    const auto& time1 = gps_time_t::leap_seconds[i];
    const auto& time2 = gps_time_t::leap_seconds[i + 1];
    CPPUNIT_ASSERT_MESSAGE(
      "Leap seconds are not in order", time1 < time2
    );
  }
  for (size_t i = 0; i < utc_time_t::leap_seconds.size() - 1; i++) {
    const auto& time1 = utc_time_t::leap_seconds[i];
    const auto& time2 = utc_time_t::leap_seconds[i + 1];
    CPPUNIT_ASSERT_MESSAGE(
      "Leap seconds are not in order", time1 < time2
    );
  }
}

void GPSTimeCppUnit::test_from_gps_str() {
  // Test the GPS epoch
  auto time1 = gps_time_t(1980, 1, 6, 0, 0, 0, 0);
  auto time2 = FromGPSString("GPS_1980-01-06T00:00:00.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "GPS epoch converts incorrectly",
    time1, time2
  );

  // Test before all leap seconds
  time1 = gps_time_t(1972, 1, 1, 0, 0, 0, 0);
  time2 = FromGPSString("GPS_1972-01-01T00:00:00.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Times before all leap seconds convert incorrectly",
    time1, time2
  );

  // Test after all leap seconds (at time of writing)
  time1 = gps_time_t(2020, 1, 1, 0, 0, 0, 0);
  time2 = FromGPSString("GPS_2020-01-01T00:00:00.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Times after all leap seconds convert incorrectly",
    time1, time2
  );

  // Test the beginning of the first leap second after the epoch
  time1 = gps_time_t(1981, 7, 1, 0, 0, 0, 0);
  time2 = FromGPSString("GPS_1981-07-01T00:00:00.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the first leap second converts incorrectly",
    time1, time2
  );

  // Test the middle of the first leap second after the epoch
  time1 = gps_time_t(1981, 7, 1, 0, 0, 0, 500'000'000);
  time2 = FromGPSString("GPS_1981-07-01T00:00:00.5Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the first leap second converts incorrectly",
    time1, time2
  );

  // Test immediately after the first leap second after the epoch
  time1 = gps_time_t(1981, 7, 1, 0, 0, 1, 0);
  time2 = FromGPSString("GPS_1981-07-01T00:00:01.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the first leap second converts incorrectly",
    time1, time2
  );

  // Test the beginning of a later leap second
  time1 = gps_time_t(2006, 1, 1, 0, 0, 13, 0);
  time2 = FromGPSString("GPS_2006-01-01T00:00:13.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the 2005-06 leap second converts incorrectly",
    time1, time2
  );

  // Test the middle of a later leap second
  time1 = gps_time_t(2006, 1, 1, 0, 0, 13, 500'000'000);
  time2 = FromGPSString("GPS_2006-01-01T00:00:13.5Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the 2005-06 leap second converts incorrectly",
    time1, time2
  );

  // Test immediately after a later leap second
  // UTC should now be off by 1 additional second
  time1 = gps_time_t(2006, 1, 1, 0, 0, 14, 0);
  time2 = FromGPSString("GPS_2006-01-01T00:00:14.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the 2005-06 leap second converts incorrectly",
    time1, time2
  );

  // Test the beginning of an earlier leap second
  time1 = gps_time_t(1976, 12, 31, 23, 59, 56, 0);
  time2 = FromGPSString("GPS_1976-12-31T23:59:56.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Beginning of the 1976-77 leap second converts incorrectly",
    time1, time2
  );

  // Test the middle of a later leap second
  time1 = gps_time_t(1976, 12, 31, 23, 59, 56, 500'000'000);
  time2 = FromGPSString("GPS_1976-12-31T23:59:56.5Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Middle of the 1976-77 leap second converts incorrectly",
    time1, time2
  );

  // Test immediately after a later leap second
  // UTC should now be off by 1 additional second
  time1 = gps_time_t(1976, 12, 31, 23, 59, 57, 0);
  time2 = FromGPSString("GPS_1976-12-31T23:59:57.0Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "End of the 1976-77 leap second converts incorrectly",
    time1, time2
  );

  time1 = gps_time_t(2022, 1, 1, 12, 34, 56, 789'012'345) + duration_t(678'901);
  time2 = FromGPSString("GPS_2022-1-1T12:34:56.789012345678901Z");
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
    "Femtosecond-precision GPS string converts incorrectly",
    time1, time2
  );
}
} /* namespace test */

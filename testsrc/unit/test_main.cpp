/**
 * @file   test_main.cpp
 * @author Mark Galassi <markgalassi@lanl.gov>
 * @date   2020-09-29
 */

// [CPPUNIT headers]
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestResult.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/BriefTestProgressListener.h>

/**
 * @brief Main unit test harness
 */
int main(int argc, char **argv)
{
  CppUnit::TestResultCollector  collector;
  CppUnit::TestFactoryRegistry &registry
    = CppUnit::TestFactoryRegistry::getRegistry();
  CppUnit::TestResult result;
  result.addListener(&collector);

  // Add a listener that prints info as we enter and exit tests
  CppUnit::BriefTestProgressListener brief_progress;
  result.addListener( &brief_progress );

  // the main cppunit test feature: the runner
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(registry.makeTest());
  runner.run(result);

  // write xml output files; these are used by jenkins to compile test
  // failure/success history
  std::ofstream xmlFileOut("cppunitTestResults.xml");
  CppUnit::XmlOutputter xmlOut(&collector, xmlFileOut);
  xmlOut.write();
  CppUnit::TextOutputter textOut(&collector, std::cerr);
  textOut.write();
  return collector.wasSuccessful() ? 0 : 1;
}

/*
 * DelayRandomTest.cpp
 *
 */


#define BOOST_TEST_MODULE DelayRandomTest
#include <boost/test/unit_test.hpp>
#include <letitgo.h>

BOOST_AUTO_TEST_SUITE(test_random_graph)

BOOST_AUTO_TEST_CASE(random_615623606) {
	  LETModel sample = generate_LET(GeneratorRequest (5,5,615623606, automotive_dt));
	  AgeLatencyResult delay = compute_age_latency(sample);
	  BOOST_CHECK_GT(delay.age_latency, 0);
      std::string svg_str = sample.getSVG();
}


BOOST_AUTO_TEST_SUITE_END()

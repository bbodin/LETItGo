/*
 * DelayRandomTest.cpp
 *
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE DelayRandomTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>

BOOST_AUTO_TEST_SUITE(test_random_graph)

BOOST_AUTO_TEST_CASE(test_random_615623606) {
	  LETModel sample = generate_LET(GeneratorRequest (5,5,615623606, automotive_dt));
	  auto delay = ComputeAgeLatency(sample);
	  BOOST_CHECK_GT(delay.age_latency, 0);
}


BOOST_AUTO_TEST_SUITE_END()

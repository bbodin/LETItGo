/*
 * DelayRandomTest.cpp
 *
 */

#define BOOST_TEST_MODULE DelayRandomTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>

BOOST_AUTO_TEST_SUITE(test_random_graph)

BOOST_AUTO_TEST_CASE(test_random_615623606) {

	  commons::set_verbose_mode(5);
	  size_t n = 5;
	  size_t m = 5;
	  size_t seed = 615623606;
	  LETModel sample = generate_LET(n,m,seed);
	  auto delay = ComputeAgeLatency(sample);
	  BOOST_CHECK_GT(delay.age_latency, 0);
}


BOOST_AUTO_TEST_SUITE_END()

/*
 * BenchmarkTest.cpp
 *
 */

#define BOOST_TEST_MODULE BenchmarkTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>
#include <map>
#include <chrono>
#include <boost/timer/progress_display.hpp>
#include <benchmark.h>

BOOST_AUTO_TEST_SUITE(test_benchmark)

BOOST_AUTO_TEST_CASE(benchmark_Expansion) {

	 // size_t begin_n = 5;
	 // size_t end_n   = 20;
	 // size_t step_n  = 5;
	 // size_t sample_count = 2;
	 // size_t iter_count   = 2;

	  // TODO: fix bug first ...
	//main_benchmark_expansion ( begin_n,  end_n,  step_n,  sample_count,  iter_count, 123) ;
}

BOOST_AUTO_TEST_CASE(benchmark_AgeLatency) {

	  size_t begin_n = 5;
	  size_t end_n   = 20;
	  size_t step_n  = 5;
	  size_t sample_count = 2;
	  size_t iter_count   = 2;

	main_benchmark_age_latency ( begin_n,  end_n,  step_n,  sample_count,  iter_count, 123) ;
}

BOOST_AUTO_TEST_SUITE_END()

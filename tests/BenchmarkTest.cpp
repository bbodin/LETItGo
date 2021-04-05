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

	AgeLantencyBenchmarkConfiguration config;

	config.begin_n       = 4;
	config.end_n         = 10;
	config.step_n        = 2;
	config.sample_count  = 2;
	config.iter_count    = 2;
	config.seed          = 123;

	main_benchmark_age_latency ( config ) ;
}


BOOST_AUTO_TEST_CASE(test_benchmark_expansion) {

	GenerateExpansionFun fun = generate_partial_constraint_graph;
	size_t sample_count = 2;
	size_t iter_count = 2;
	size_t n = 5;
	size_t m = 10;
	size_t seed = 123;
	ExpansionBenchmarkResult res = benchmark_expansion (generate_partial_constraint_graph, sample_count , iter_count ,  n,  m, LETDatasetType::automotive_dt, false,   seed);

	BOOST_CHECK_GT(res.total_edge_count, 0);
}






BOOST_AUTO_TEST_SUITE_END()

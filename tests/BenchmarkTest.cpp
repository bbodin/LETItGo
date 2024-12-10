/*
 * BenchmarkTest.cpp
 *
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BenchmarkTest
#include <boost/test/unit_test.hpp>
#include <letitgo.h>
#include <benchmark.h>

BOOST_AUTO_TEST_SUITE(test_benchmark)


BOOST_AUTO_TEST_CASE(benchmark_AgeLatency) {
    letitgo::utils::set_verbose_mode(10);
	AgeLantencyBenchmarkConfiguration config;

	config.begin_n       = 4;
	config.end_n         = 10;
	config.step_n        = 2;
	config.sample_count  = 2;
	config.iter_count    = 2;
	config.seed          = 123;
        config.detailed     = true;
        config.dryrun     = false;
        config.kind = LETDatasetType::automotive_dt;
	main_benchmark_age_latency ( config ) ;
}

BOOST_AUTO_TEST_SUITE_END()

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
#include "benchmark_utils.h"

BOOST_AUTO_TEST_SUITE(test_benchmark)


BOOST_AUTO_TEST_CASE(test_empty) {
	if (getenv("VERBOSE")) {
		commons::set_verbose_mode(atoi(getenv("VERBOSE")));
	}


  size_t begin_n = 5;
  size_t end_n   = 100;
  size_t step_n  = 5;
  size_t sample_count = 1;

  if (getenv("SAMPLE_COUNT")) {
	  sample_count = atoi(getenv("SAMPLE_COUNT"));
  }

  size_t total = sample_count * (end_n - begin_n + step_n) / step_n;
  VERBOSE_INFO("Start benchmark of " << total << " runs.");

  //boost::timer::progress_display show_progress( total );

  std::cout << "index\tltime\titer\tsize\thtime\titer\tsize" << std::endl;

  for (auto n = begin_n ; n <= end_n ; n+= step_n) {

	  size_t high_density_m = (n * (n - 1)) / 3;
	  size_t low_density_m = (n * (n - 1)) / 4;
	  auto bench_low  = benchmark ( sample_count, n, low_density_m) ;
	  auto bench_high = benchmark ( sample_count, n, high_density_m) ;

	  std::cout << n << "\t" << std::setprecision(2) << std::fixed << bench_low.sum_time << "\t"  << std::setprecision(2)  << bench_low.sum_iter << "\t" << bench_low.sum_size
			         << "\t"  <<std::setprecision(2) << std::fixed << bench_high.sum_time << "\t" << std::setprecision(2) << bench_high.sum_iter << "\t" << bench_high.sum_size << std::endl;

  }


}

BOOST_AUTO_TEST_SUITE_END()

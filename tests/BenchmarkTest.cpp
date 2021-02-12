/*
 * BenchmarkTest.cpp
 *
 */

#define BOOST_TEST_MODULE BenchmarkTest
#include <boost/test/included/unit_test.hpp>
#include <generator.h>
#include <letitgo.h>
#include <map>
#include <chrono>
#include <boost/timer/progress_display.hpp>

BOOST_AUTO_TEST_SUITE(test_benchmark)

struct BenchmarkResult {

	  double sum_time = 0;
	  double sum_iter = 0;
	  double sum_size = 0;

	  BenchmarkResult (double sum_time, double sum_iter, double sum_size) : sum_time(sum_time) , sum_iter(sum_iter)  , sum_size(sum_size) {}

};

INTEGER_TIME_UNIT getLCM (LETModel& m) {

	INTEGER_TIME_UNIT lcm = 1;
	for (Task t : m.tasks()) {
		lcm = std::lcm(lcm, t.getT());
	}
	return lcm;
}

INTEGER_TIME_UNIT getSumN (LETModel& m) {

	INTEGER_TIME_UNIT lcm = getLCM(m);
	INTEGER_TIME_UNIT sum = 0;
	for (Task t : m.tasks()) {
		sum += lcm / t.getT();
	}
	return sum;
}


BenchmarkResult benchmark (size_t sample_count, size_t n, size_t m) {

	  double sum_time = 0;
	  double sum_iter = 0;
	  double sum_size = 0;

	  VERBOSE_DEBUG("Start benchmark with n=" << n << " and " << " m=" << m);
	  for (size_t i = 0 ; i < sample_count ; i ++ ) {
			  //++pd;
			  LETModel sample = generate_LET(n,m);
			  auto lcm = getLCM(sample);
			  VERBOSE_DEBUG("lcm=" << lcm);
			  auto sum_n = getSumN (sample);


			  auto t1 = std::chrono::high_resolution_clock::now();
			  auto res = ComputeAgeLatency(sample);
			  auto t2 = std::chrono::high_resolution_clock::now();
			  auto duration = t2 - t1;
			  sum_time += duration.count() / 1000000;
			  sum_iter += res.required_iterations;
			  sum_size += (double) res.expansion_size / (double) sum_n;
			  VERBOSE_DEBUG("    **** dur=" << duration.count() << " iter=" << res.required_iterations << " part=" <<  res.expansion_size << " size=" << sum_n);

	  }

	  return BenchmarkResult((double)sum_time / (double)sample_count, (double)sum_iter / (double)sample_count, (double)sum_size / (double)sample_count);
}


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

  std::cout << "index" << "\tltime\titer\tsize"
		         << "\thtime\titer\tsize" << std::endl;

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

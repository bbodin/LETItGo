/*
 * benchmark_utils.h
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */

#ifndef TESTS_BENCHMARK_UTILS_H_
#define TESTS_BENCHMARK_UTILS_H_

#include <model.h>
#include <numeric>
#include <chrono>
#include <letitgo.h>

struct BenchmarkResult {

	  double sum_time = 0;
	  double sum_iter = 0;
	  double sum_size = 0;

	  BenchmarkResult (double sum_time, double sum_iter, double sum_size) : sum_time(sum_time) , sum_iter(sum_iter)  , sum_size(sum_size) {}

};

template <typename entier>
entier getLCM (LETModel& m) {

	entier lcm = 1;
	for (Task t : m.tasks()) {
		lcm = std::lcm(lcm, t.getT());
	}
	return lcm;
}

template <typename entier>
entier getSumN (LETModel& m) {

	entier lcm = getLCM<entier>(m);
	entier sum = 0;
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
			  LETModel sample = generate_LET(n,m);
			  INTEGER_TIME_UNIT lcm = getLCM<INTEGER_TIME_UNIT>(sample);
			  VERBOSE_DEBUG("LCM=" << lcm);
			  INTEGER_TIME_UNIT sum_n = getSumN<INTEGER_TIME_UNIT> (sample);


			  auto t1 = std::chrono::high_resolution_clock::now();
			  auto res = ComputeAgeLatency(sample);
			  auto t2 = std::chrono::high_resolution_clock::now();
			  auto duration = t2 - t1;
			  sum_time += duration.count() / 1000000;
			  sum_iter += res.required_iterations;
			  sum_size += (double) res.expansion_size / (double) sum_n;
			  VERBOSE_DEBUG("    **** duration=" << duration.count() << " iteration=" << res.required_iterations << " part=" <<  res.expansion_size << " size=" << sum_n);

	  }

	  return BenchmarkResult((double)sum_time / (double)sample_count, (double)sum_iter / (double)sample_count, (double)sum_size / (double)sample_count);
}



#endif /* TESTS_BENCHMARK_UTILS_H_ */

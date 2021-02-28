/*
 * benchmark.cpp
 *
 *  Created on: Feb 28, 2021
 *      Author: toky
 */

#include <benchmark.h>
#include <letitgo.h>
#include <chrono>

/**
 *
 * Run the Age latency multiple time on the same graph for n iterations
 * and return the average.
 *
 */

double get_age_latency_execution_time (AgeLatencyFun fun, LETModel sample, size_t n) {
	double sum_time = 0;
	for (size_t i = 0 ; i < n; i++) {
		 auto t1 = std::chrono::high_resolution_clock::now();
		 fun(sample, generate_partial_constraint_graph);
		 auto t2 = std::chrono::high_resolution_clock::now();
		 auto duration = t2 - t1;
		 sum_time += duration.count();
	}
	return (sum_time / n);
}


ExpansionBenchmarkResult  benchmark_expansion   (GenerateExpansionFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m, size_t seed) {
	  double sum_time = 0;

	  VERBOSE_DEBUG("Start benchmark with n=" << n << " and " << " m=" << m);
	  for (size_t i = 0 ; i < sample_count ; i ++ ) {
		  LETModel sample = generate_LET(n,m, seed + i);
		  auto K = generate_periodicity_vector(sample, 2);
		  INTEGER_TIME_UNIT lcm = getLCM<INTEGER_TIME_UNIT>(sample);
		  VERBOSE_DEBUG("LCM=" << lcm);

		  double sub_sum_time = 0;
		  for (size_t i = 0 ; i < iter_count; i++) {
			  auto t1 = std::chrono::high_resolution_clock::now();
			  fun(sample, K);
			  auto t2 = std::chrono::high_resolution_clock::now();
			  auto duration = t2 - t1;
			  sub_sum_time += duration.count();
		  }

		  auto duration =  (sub_sum_time / n);
		  PartialConstraintGraph res = fun(sample, K);

		  auto original = generate_partial_constraint_graph(sample, K);
		  VERBOSE_ASSERT_EQUALS(res, original);

		  sum_time += duration / 1000000;
		  VERBOSE_DEBUG("    **** duration=" << duration);

	  }

	  return ExpansionBenchmarkResult((double)sum_time / (double)sample_count, 0);
}

AgeLatencyBenchmarkResult benchmark_age_latency (AgeLatencyFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m, size_t seed) {

	  double sum_time = 0;
	  double sum_iter = 0;
	  double sum_size = 0;

	  VERBOSE_DEBUG("Start benchmark with n=" << n << " and " << " m=" << m);
	  for (size_t i = 0 ; i < sample_count ; i ++ ) {
			  LETModel sample = generate_LET(n,m, seed + i);
			  INTEGER_TIME_UNIT lcm = getLCM<INTEGER_TIME_UNIT>(sample);
			  VERBOSE_DEBUG("LCM=" << lcm);
			  INTEGER_TIME_UNIT sum_n = getSumN<INTEGER_TIME_UNIT> (sample);
			  auto duration = get_age_latency_execution_time (fun, sample, iter_count);
			  auto res = fun(sample, generate_partial_constraint_graph);
			  sum_time += duration / 1000000;
			  sum_iter += res.required_iterations;
			  sum_size += (double) res.expansion_size / (double) sum_n;
			  VERBOSE_DEBUG("    **** duration=" << duration << " iteration=" << res.required_iterations << " part=" <<  res.expansion_size << " size=" << sum_n);

	  }

	  return AgeLatencyBenchmarkResult((double)sum_time / (double)sample_count, (double)sum_iter / (double)sample_count, (double)sum_size / (double)sample_count);
}

/*
 * benchmark.h
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */

#ifndef INCLUDE_BENCHMARK_H_
#define INCLUDE_BENCHMARK_H_

#include <model.h>
#include <numeric>
#include <chrono>
#include <letitgo.h>
#include <functional>
typedef std::function<AgeLatencyResult(const LETModel &model, GenerateExpansionFun fun)> AgeLatencyFun;

struct ExpansionBenchmarkResult {
	double average_time;
	size_t size;
	ExpansionBenchmarkResult (double average_time, size_t size) : average_time(average_time) , size(size) {}
};
struct AgeLatencyBenchmarkResult {
	  double sum_time = 0;
	  double sum_iter = 0;
	  double sum_size = 0;
	  AgeLatencyBenchmarkResult (double sum_time, double sum_iter, double sum_size) : sum_time(sum_time) , sum_iter(sum_iter)  , sum_size(sum_size) {}
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


AgeLatencyBenchmarkResult benchmark_age_latency (AgeLatencyFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m, size_t seed);
ExpansionBenchmarkResult  benchmark_expansion   (GenerateExpansionFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m, size_t seed);



#endif /* INCLUDE_BENCHMARK_H_ */

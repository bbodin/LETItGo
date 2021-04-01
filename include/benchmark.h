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

struct BenchmarkConfiguration  {
	size_t begin_n;
	size_t end_n;
	size_t step_n;
	size_t sample_count;
	size_t iter_count;
	size_t seed;
};
struct ExpansionBenchmarkConfiguration : public BenchmarkConfiguration {

};

struct AgeLantencyBenchmarkConfiguration : public BenchmarkConfiguration {

};

struct ExpansionBenchmarkResult {
	size_t sample_count;
	Algorithm2_statistics algo2_stats;
	double average_time;
	size_t total_vertex_count;
	size_t total_edge_count;
	ExpansionBenchmarkResult (size_t sample_count, Algorithm2_statistics algo2_stats, double average_time, size_t total_vertex_count, size_t total_edge_count) : sample_count(sample_count), algo2_stats(algo2_stats), average_time(average_time) , total_vertex_count(total_vertex_count), total_edge_count(total_edge_count) {}
};
struct AgeLatencyBenchmarkResult {
	  double time  = 0;
	  double iter  = 0;
	  double size  = 0;
	  double bound = 0;
	  double g_ctime  = 0;
	  double p_ctime = 0;
	  AgeLatencyBenchmarkResult (double t, double it, double s, double b, double g, double p) : time(t) , iter(it)  , size(s), bound(b) , g_ctime(g), p_ctime(p) {}
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
ExpansionBenchmarkResult  benchmark_expansion   (GenerateExpansionFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m,  bool harmonized_periodicity, size_t seed);

void main_benchmark_age_latency (AgeLantencyBenchmarkConfiguration config);
void main_benchmark_expansion (ExpansionBenchmarkConfiguration config);


#endif /* INCLUDE_BENCHMARK_H_ */

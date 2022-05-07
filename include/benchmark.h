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
	bool detailed;
	LETDatasetType kind;
    std::string logfile;
};
struct ExpansionBenchmarkConfiguration : public BenchmarkConfiguration {

};

struct AgeLantencyBenchmarkConfiguration : public BenchmarkConfiguration {

};

struct ExpansionBenchmarkResult {
	size_t sample_count;
	double sum_n; // Max Possible Expansion size
	Algorithm2_statistics algo2_stats;
	double average_time;
	size_t total_vertex_count;
	size_t total_edge_count;
	ExpansionBenchmarkResult (size_t sample_count, double sum_n,  Algorithm2_statistics algo2_stats, double average_time, size_t total_vertex_count, size_t total_edge_count) : sample_count(sample_count), sum_n(sum_n), algo2_stats(algo2_stats), average_time(average_time) , total_vertex_count(total_vertex_count), total_edge_count(total_edge_count) {}
};

/**
 * bench_res.time  += duration;
		bench_res.iter  += fun_res.required_iterations;
		bench_res.size  += (double) fun_res.expansion_size / (double) sum_n;
		bench_res.bound += (double) fun_res.first_bound_error / (double) fun_res.age_latency;
		bench_res.g_ctime += fun_res.graph_computation_time;
		bench_res.p_ctime += fun_res.path_computation_time;

 */
struct AgeLatencyBenchmarkResult {

		// Fixed
	  size_t n = 0, m = 0;
	  LETDatasetType dt = LETDatasetType::unknown_dt;

	    // Could be average
	  double time  = 0; // Execution Time of the algorithm
	  double iter  = 0; // Number of required iteration
	  double sum_n  = 0; // Max Possible Expansion size
	  double size  = 0; // Expansion size as percent of V / N
	  double bound = 0; // Error margin from the first bound t'ill the final result
	  double g_ctime  = 0; // Graph generation time
	  double p_ctime = 0; // Path computation time

	  AgeLatencyBenchmarkResult (size_t n, size_t m, LETDatasetType dt) :
		  n(n), m(m), dt(dt), time(0) , iter(0)  , sum_n(0),  size(0), bound(0) , g_ctime(0), p_ctime(0) {}

	  AgeLatencyBenchmarkResult (size_t n, size_t m, LETDatasetType dt, double t, double it, double sn, double s, double b, double g, double p) :
		  n(n), m(m), dt(dt), time(t) , iter(it)  , sum_n(sn),  size(s), bound(b) , g_ctime(g), p_ctime(p) {}
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


AgeLatencyBenchmarkResult benchmark_age_latency (AgeLatencyFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m, LETDatasetType dt, size_t seed);
ExpansionBenchmarkResult  benchmark_expansion   (GenerateExpansionFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m,  LETDatasetType dt, bool harmonized_periodicity, size_t seed);

void main_benchmark_age_latency (AgeLantencyBenchmarkConfiguration config);
void main_benchmark_expansion (ExpansionBenchmarkConfiguration config);


#endif /* INCLUDE_BENCHMARK_H_ */

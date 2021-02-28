/*
 * benchmarkNewExpansion.cpp
 *
 *  Created on: Feb 28, 2021
 *      Author: toky
 */

#include <verbose.h>
#include <partial_constraint_graph.h>
#include <benchmark.h>
#include <gflags/gflags.h>
#include <iomanip>

DEFINE_int32(verbose,         0, "Specify the verbosity level (0-10)");
DEFINE_int32(begin_n,         5, "Minimal task count");
DEFINE_int32(end_n,         100, "Maximum task count");
DEFINE_int32(step_n,          5, "Step of task count");
DEFINE_int32(sample_count,  100, "How many graph to generate per size (variety)");
DEFINE_int32(iter_count,     50, "How many run per graph (precision)");





int main (int argc , char * argv[]) {
	gflags::SetUsageMessage("LETItGo: LET Analysis tool");
	gflags::SetVersionString("1.0.0");
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	utils::set_verbose_mode(FLAGS_verbose);


	size_t begin_n = FLAGS_begin_n;
	size_t end_n = FLAGS_end_n;
	size_t step_n = FLAGS_step_n;
	size_t sample_count = FLAGS_sample_count;
	size_t iter_count = FLAGS_iter_count;
	size_t seed = 123;


	size_t total = sample_count * (end_n - begin_n + step_n) / step_n;
	VERBOSE_INFO("Start benchmark of " << total << " runs.");

	std::cout << "n\tm\torig\tnew\tratio" << std::endl;

	for (size_t n = begin_n ; n <= end_n ; n+= step_n) {

		size_t m = (n * (n - 1)) / 3;
		ExpansionBenchmarkResult bench_res1  = benchmark_expansion ( generate_partial_constraint_graph , sample_count, iter_count, n, m, seed) ;
		ExpansionBenchmarkResult bench_res2  = benchmark_expansion ( opt_new_generate_partial_constraint_graph , sample_count, iter_count, n, m, seed) ;

		std::cout << n << "\t" << m
				<< "\t"  << std::setprecision(2) << std::fixed << bench_res1.average_time
			   	<< "\t"  << std::setprecision(2) << std::fixed << bench_res2.average_time
			   	<< "\t"  << std::setprecision(2) << std::fixed << bench_res2.average_time /  bench_res1.average_time
				<< std::endl;

		 m = (n * (n - 1)) / 4;
		 bench_res1  = benchmark_expansion ( generate_partial_constraint_graph , sample_count, iter_count, n, m, seed) ;
		 bench_res2  = benchmark_expansion ( opt_new_generate_partial_constraint_graph , sample_count, iter_count, n, m, seed) ;

		 std::cout << n << "\t" << m
						<< "\t"  << std::setprecision(2) << std::fixed << bench_res1.average_time
					   	<< "\t"  << std::setprecision(2) << std::fixed << bench_res2.average_time
					   	<< "\t"  << std::setprecision(2) << std::fixed << bench_res2.average_time /  bench_res1.average_time
						<< std::endl;
	  }





	gflags::ShutDownCommandLineFlags();
	return 0;

}


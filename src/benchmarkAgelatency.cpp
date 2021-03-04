/*
 * benchmarkAgelatency.cpp
 *
 *  Created on: Mar 4, 2021
 *      Author: toky
 */



#include <verbose.h>
#include <letitgo.h>
#include <benchmark.h>
#include <gflags/gflags.h>
#include <iomanip>

DEFINE_int32(verbose,         0, "Specify the verbosity level (0-10)");
DEFINE_int32(begin_n,         5, "Minimal task count");
DEFINE_int32(end_n,         100, "Maximum task count");
DEFINE_int32(step_n,          5, "Step of task count");
DEFINE_int32(sample_count,  100, "How many graph to generate per size (variety)");
DEFINE_int32(iter_count,     50, "How many run per graph (precision)");
DEFINE_int32(seed,          123, "Value of the first seed.");





int main (int argc , char * argv[]) {
	gflags::SetUsageMessage("LETItGo: LET Analysis tool");
	gflags::SetVersionString("1.0.0");
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	utils::set_verbose_mode(FLAGS_verbose);


	main_benchmark_age_latency ( FLAGS_begin_n,  FLAGS_end_n,  FLAGS_step_n,  FLAGS_sample_count,  FLAGS_iter_count, FLAGS_seed) ;


	gflags::ShutDownCommandLineFlags();
	return 0;

}

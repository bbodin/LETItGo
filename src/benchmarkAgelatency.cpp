/*
 * benchmarkAgelatency.cpp
 *
 *  Created on: Mar 4, 2021
 *      Author: toky
 */



#include <verbose.h>
#include <letitgo.h>
#include <benchmark.h>
#include <iomanip>
#include <gflags/gflags.h>

DEFINE_int32(verbose,         0, "Specify the verbosity level (0-10)");
DEFINE_int32(begin_n,         5, "Minimal task count");
DEFINE_int32(end_n,         100, "Maximum task count");
DEFINE_int32(step_n,          5, "Step of task count");
DEFINE_int32(sample_count,  100, "How many graph to generate per size (variety)");
DEFINE_int32(iter_count,     50, "How many run per graph (precision)");
DEFINE_int32(seed,          123, "Value of the first seed.");
DEFINE_bool(detailed,      false, "printout every sample");
DEFINE_string(kind,  "automotive", "Kind of dataset to generate (automotive,generic,harmonic)");





int main (int argc , char * argv[]) {
	gflags::SetUsageMessage("LETItGo: LET Analysis tool");
	gflags::SetVersionString("1.0.0");
	gflags::ParseCommandLineFlags(&argc, &argv, true);


	letitgo::utils::set_verbose_mode(FLAGS_verbose);


	AgeLantencyBenchmarkConfiguration config;

	config.begin_n       = FLAGS_begin_n;
	config.end_n         = FLAGS_end_n;
	config.step_n        = FLAGS_step_n;
	config.sample_count  = FLAGS_sample_count;
	config.iter_count    = FLAGS_iter_count;
	config.seed          = FLAGS_seed;
	config.kind          = str2kind(FLAGS_kind);
	config.detailed      = FLAGS_detailed;
	main_benchmark_age_latency (config ) ;


	gflags::ShutDownCommandLineFlags();
	return 0;

}

/*
 * letitgo.cpp
 *
 *  Created on: Apr 5, 2021
 *      Author: toky
 */


#include <verbose.h>
#include <letitgo.h>
#include <benchmark.h>
#include <gflags/gflags.h>
#include <iomanip>
#include <fstream>

DEFINE_int32(verbose,   0, "Specify the verbosity level (0-10)");
DEFINE_string(filename, "",
                        "Path Location of the file to open. (Supports LETItGo XML Format)");
DEFINE_bool(agelatency, false,
                        "Perform age latency analysis");
DEFINE_bool(outputxml, false,
                        "Perform age latency analysis");





int main (int argc , char * argv[]) {
	gflags::SetUsageMessage("LETItGo: LET Analysis tool");
	gflags::SetVersionString("1.0.0");
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	letitgo::utils::set_verbose_mode(FLAGS_verbose);

	std::ifstream t(FLAGS_filename);
	std::string str((std::istreambuf_iterator<char>(t)),
	                 std::istreambuf_iterator<char>());

	LETModel* instance = new LETModel(str);
	VERBOSE_ASSERT(instance, "Could not construct LET Instance");

	AgeLatencyFun original = (AgeLatencyFun) ComputeAgeLatency;
	if (FLAGS_agelatency) {
		original(*instance, generate_partial_constraint_graph);

	}
	if (FLAGS_outputxml) {
		std::cout << *instance;
	}

	gflags::ShutDownCommandLineFlags();
	return 0;

}



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
            "Output the XML of the LET model");
DEFINE_bool(outputdot, false,
            "Output the DOT of the LET model");
DEFINE_bool(outputsvg, false,
            "Output the SVG of the LET model");
DEFINE_bool(outputtikz, false,
            "Output the Tikz of the LET model");
DEFINE_int32(schedule_duration,   0, "length if tikz schedule");

DEFINE_int32(n,          4, "Generated case: Value of N");
DEFINE_int32(m,          4, "Generated case: Value of M");
DEFINE_int32(seed,       1, "Generated case: Value of seed");
DEFINE_string(kind,  "automotive", "Generated case: Kind of dataset to generate (automotive,generic,harmonic)");
DEFINE_bool(DiEqualTi,      false, "Generated case: Every Di = Ti");

int main (int argc , char * argv[]) {
	gflags::SetUsageMessage("LETItGo: LET Analysis tool");
	gflags::SetVersionString("1.0.0");
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	letitgo::utils::set_verbose_mode(FLAGS_verbose);

    LETModel *instance = nullptr;

    if (FLAGS_filename != "") {
        VERBOSE_INFO("Load file " << FLAGS_filename);
        std::ifstream t(FLAGS_filename);
        std::string str((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());

        instance = new LETModel(str);
    } else {
        GeneratorRequest r (FLAGS_n,FLAGS_m,FLAGS_seed, str2kind(FLAGS_kind), FLAGS_DiEqualTi);
        VERBOSE_INFO("Generate instance " << r);
        instance = new LETModel(generate_LET(r));
    }

    VERBOSE_ASSERT(instance, "Could not construct LET Instance");

    AgeLatencyFun original = (AgeLatencyFun) compute_age_latency;
	if (FLAGS_agelatency) {
		auto res = original(*instance);
        std::cout << "// Age Latency:" << res.age_latency << std::endl;
	}
    if (FLAGS_outputxml) {
        std::cout << instance->getXML();
    }
    if (FLAGS_outputdot) {
        std::cout << instance->getDOT();
    }
    if (FLAGS_outputsvg) {
        std::cout << instance->getSVG();
    }
    if (FLAGS_outputtikz) {
        VERBOSE_ASSERT_GreaterThan(FLAGS_schedule_duration,0);
        std::cout << instance->getTikz(FLAGS_schedule_duration);
    }

    delete instance;

	gflags::ShutDownCommandLineFlags();
	return 0;

}



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
DEFINE_bool(outputtikzdag, false,
            "Output the Tikz of the LET model");
DEFINE_bool(outputtikzPEG, false,
            "Output the Tikz of the Partial Expansion graphs");
DEFINE_bool(outputtikzschedule, false,
            "Output the Tikz of the LET model");
DEFINE_bool(outputtabular, false,
            "Output the Latex tabular of the LET model");
DEFINE_bool(outputalphas, false,
            "Output the Latex tabular of the PEG Alphas.");

DEFINE_int32(schedule_duration,   0, "length if tikz schedule");

DEFINE_int32(n,          4, "Generated case: Value of N");
DEFINE_int32(m,          4, "Generated case: Value of M");
DEFINE_int32(seed,       1, "Generated case: Value of seed");
DEFINE_string(kind,  "automotive", "Generated case: Kind of dataset to generate (automotive,generic,harmonic)");
DEFINE_bool(DiEqualTi,      false, "Generated case: Every Di = Ti");
DEFINE_string(peg, "",
            "Generate the PEG with a particular periodicity vector");

bool onPCGCreated (const PartialConstraintGraph& pcg) {

    auto upper_res = FindLongestPath(pcg, upper_wt);
    auto lower_res = FindLongestPath(pcg, lower_wt);
    std::vector<long> rv_vec;
    for (auto v : compute_repetition_vector(pcg.getModel())) {
        rv_vec.push_back(v.second);
    }
    std::cout << "// N=" <<  rv_vec
            << " K=" << pcg.get_periodicity_vector()
            << " LB/UP: " << lower_res.second << "/" << upper_res.second
            << " len(UP): " << upper_res.first.size() - 2
    << std::endl;
    if (FLAGS_outputalphas) {
        std::cout << "// Alpha values " << std::endl;
        for (Dependency e : pcg.getModel().dependencies()) {
            std::cout << "// Alpha values for the first edge " << e << std::endl;
            std::cout << pcg.getAlphasAsLatex(e.getId()) << std::endl;
        }
    }

    if (FLAGS_outputtikzPEG) std::cout << "// Partial expansion Graph Low/Up"<< std::endl;
    if (FLAGS_outputtikzPEG) std::cout << pcg.getTikZ();

    if (FLAGS_outputsvg || FLAGS_outputdot) std::cout << "// Upper bound graph"<< std::endl;
    if (FLAGS_outputsvg) std::cout << pcg.getSVG(upper_wt);
    if (FLAGS_outputdot) std::cout << pcg.getDOT(upper_wt);

    if (FLAGS_outputsvg || FLAGS_outputdot) std::cout << "// Lower bound graph"<< std::endl;
    if (FLAGS_outputsvg) std::cout << pcg.getSVG(lower_wt);
    if (FLAGS_outputdot) std::cout << pcg.getDOT(lower_wt);

    return true;
}


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

    AgeLatencyWithHookFun original = (AgeLatencyWithHookFun) compute_age_latency_with_hook;
    PEGOnCreatedFun on_created = onPCGCreated;



    if (FLAGS_agelatency) {
		auto res = original(*instance, on_created);
        std::cout << "// Age Latency:" << res.age_latency << std::endl;
	}

    if (!FLAGS_peg.empty()) {
        //PeriodicityVector K = generate_periodicity_vector(*instance);
        PeriodicityVector K = parse_periodicity_vector(FLAGS_peg);
        PartialConstraintGraph PKG = PartialConstraintGraph(*instance, K);
        onPCGCreated(PKG);
        std::cout << "// End of PEG" << std::endl;
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


    if (FLAGS_outputtikzschedule) {
        VERBOSE_ASSERT_GreaterThan(FLAGS_schedule_duration,0);
        std::cout << instance->getTikzSchedule(FLAGS_schedule_duration);
    }

    if (FLAGS_outputtikzdag) {
        std::cout << instance->getTikzDAG();
    }

    if (FLAGS_outputtabular) {
        std::cout << instance->getTabular();
    }
    delete instance;

	gflags::ShutDownCommandLineFlags();
	return 0;

}



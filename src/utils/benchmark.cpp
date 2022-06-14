/*
 * benchmark.cpp
 *
 *  Created on: Feb 28, 2021
 *      Author: toky
 */

#include <benchmark.h>
#include <letitgo.h>
#include <chrono>
#include <iomanip>
#include <fstream>

/**
 *
 * Run the Age latency multiple time on the same graph for n iterations
 * and return the average.
 *
 * execution time in millisecond
 *
 */


inline TIME_UNIT sumT(std::vector<TIME_UNIT> K) {
    TIME_UNIT sum = 0;
    for (auto it: K) {
        sum += it;
    }
    return sum;
}

double get_age_latency_execution_time (AgeLatencyFun fun, LETModel sample, size_t n) {
	double sum_time = 0;
	for (size_t i = 0 ; i < n; i++) {
		auto t1 = std::chrono::high_resolution_clock::now();
		fun(sample);
		auto t2 = std::chrono::high_resolution_clock::now();
		auto duration = t2 - t1;
		sum_time += duration.count();
	}
	return (sum_time / n) / 1000000;
}


AgeLatencyBenchmarkResult benchmark_age_latency (AgeLatencyFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m, LETDatasetType dt,  bool DiEqualTi,size_t seed) {

	//double sum_time = 0;
	//double sum_iter = 0;
	//double sum_size = 0;


	AgeLatencyBenchmarkResult bench_res (n,m,dt);

	VERBOSE_DEBUG("Start benchmark with n=" << n << " and " << " m=" << m);
	for (size_t i = 0 ; i < sample_count ; i ++ ) {
		VERBOSE_INFO ("Run generate with arguments n=" << n << ", m=" << m << ", dt=" << dt << ", seed=" << seed + i);

        GeneratorRequest r (n,m,seed + i, dt,  DiEqualTi);
		LETModel sample = Generator::getInstance().generate(r);
		INTEGER_TIME_UNIT lcm = getLCM<INTEGER_TIME_UNIT>(sample);
		VERBOSE_DEBUG("LCM=" << lcm);
		INTEGER_TIME_UNIT sum_n = getSumN<INTEGER_TIME_UNIT> (sample);

		VERBOSE_INFO ("Run get_age_latency_execution_time");
		auto duration = get_age_latency_execution_time (fun, sample, iter_count);

		VERBOSE_INFO ("Run get_age_latency one last time");
		AgeLatencyResult fun_res = fun(sample);
		VERBOSE_DEBUG("AgeLatencyResult = " << fun_res);
		bench_res.time  += duration;
		bench_res.iter  += fun_res.expansion_edge_count.size();
		bench_res.sum_n  += sum_n;
		bench_res.size  += (double) fun_res.expansion_vertex_count.back() / (double) sum_n;
		double bound_error = (double) fun_res.upper_bounds.front() - (double) fun_res.lower_bounds.front();
		bench_res.bound +=  bound_error / (double) fun_res.age_latency;
		bench_res.g_ctime += sumT(fun_res.graph_computation_time);
		bench_res.p_ctime += sumT(fun_res.upper_computation_time);

	}

	bench_res.time  /= (double) sample_count;
	bench_res.iter  /= (double) sample_count;
	bench_res.sum_n  /= (double) sample_count;
	bench_res.size  /= (double) sample_count;
	bench_res.bound /= (double) sample_count;
	bench_res.g_ctime  /= (double) sample_count;
	bench_res.p_ctime /= (double) sample_count;

	return bench_res;
}



inline void print_detailed_al_header(std::ostream& out_stream) {
    out_stream
                     << "seed"
            <<  ";" << "kind"
            <<  ";"  << "DiEqTi"
			<< ";" << "n"
			<< ";" << "m"
			<< ";" << "sum_n"
			<< ";" << "AgeLatency"
			<< ";" << "IterationCount"
			<< ";" << "ExpansionVertex"
			<< ";" << "ExpansionEdges"
			<< ";" << "LowerBounds"
			<< ";" << "UpperBounds"
			<< ";" << "gen_time"
            << ";" << "lbp_time"
            << ";" << "ubp_time"
            << ";" << "total_time"
			<< std::endl;
}


inline void print_detailed_al_row(GeneratorRequest r,
		  AgeLatencyResult res, std::ostream& out_stream) {

    VERBOSE_ASSERT(r.n == res.n, "Wrong graph generated");
    VERBOSE_ASSERT(r.m == res.m, "Wrong graph generated");

    out_stream     << r.seed  << ";" << r.t  << ";" << r.DiEqualTi
				  << ";"  << res.n
				  << ";"  << res.m
				  << ";"  << res.sum_n;
    out_stream << ";"  << std::fixed << res.age_latency;
    out_stream << ";"  << res.expansion_vertex_count.size()   ;
    out_stream << ";"  << "\"" << res.expansion_vertex_count << "\""   ;
    out_stream << ";"  << "\"" << res.expansion_edge_count << "\""   ;
    out_stream << ";"  << "\"" << res.lower_bounds  << "\""  ;
    out_stream << ";"  << "\"" << res.upper_bounds << "\""   ;
    out_stream << ";"  << "\"" << std::setprecision(2) << std::fixed << res.graph_computation_time << "\"" ;
    out_stream << ";"  << "\"" << std::setprecision(2) << std::fixed << res.lower_computation_time << "\"" ;
    out_stream << ";"  << "\"" << std::setprecision(2) << std::fixed << res.upper_computation_time  << "\"" ;
    out_stream << ";"  << std::setprecision(2) << std::fixed << res.total_time  ;
    out_stream << std::endl;


}

inline void print_al_header(std::ostream& out_stream) {
    out_stream
    << std::setw(5) << "kind"
	<< std::setw(5) << "n"
			<< std::setw(5) << "m"
			<< std::setw(10) << "sumN"
			<< std::setw(10) << "time"
			<< std::setw(10) << "iter"
			<< std::setw(10) << "size"
			<< std::setw(10) << "bound"
			<< std::setw(10) << "gen_time"
			<< std::setw(10) << "sp_time"
			<< std::endl;
}

inline void print_al_row(AgeLatencyBenchmarkResult bench, std::ostream& out_stream) {
    out_stream
			  << std::setw(5) << bench.dt
			  << std::setw(5) << bench.n
				  << std::setw(5) << bench.m << std::flush;
    out_stream
				  << std::setw(10) << std::fixed << std::setprecision(1)  << bench.sum_n
				  << std::setw(10) << std::setprecision(2)  << std::fixed << bench.time
				  << std::setw(10) << std::setprecision(2)  << std::fixed << bench.iter
				  << std::setw(10) << bench.size
				  << std::setw(10) << bench.bound
				  << std::setw(10) << bench.g_ctime
				  << std::setw(10) << bench.p_ctime

				  << std::endl;
}



void main_benchmark_age_latency (AgeLantencyBenchmarkConfiguration config) {

	size_t begin_n = config.begin_n ;
	size_t end_n        = config.end_n ;
	size_t step_n       = config.step_n     ;
	size_t sample_count = config.sample_count;
	size_t iter_count   = config.iter_count  ;
	size_t fseed         = config.seed       ;
	LETDatasetType       dt = config.kind;
    bool  DiEqualTi = config. DiEqualTi;
	AgeLatencyFun original = (AgeLatencyFun) compute_age_latency; //

    
    std::ofstream out_file = std::ofstream (config.logfile);
    std::ostream* output = &std::cout;
    if (out_file.is_open()) {
        output = dynamic_cast<std::ostream*>(&out_file);
    }
    std::ostream& out_stream = *output;
    


	size_t total = sample_count * (end_n - begin_n + step_n) / step_n;
	VERBOSE_INFO("Start benchmark of " << total << " runs.");

    std::cout << "######################################################################################################" << std::endl;
    std::cout << "########## LET it Go Age latency Benchmarking                                      ###################" << std::endl;
    std::cout << "######################################################################################################" << std::endl;

    std::cout << "#     begin_n = " << begin_n << "" << std::endl;
    std::cout << "#     end_n = " << end_n << "" << std::endl;
    std::cout << "#     step_n = " << step_n << "" << std::endl;
    std::cout << "#     sample_count = " << sample_count << "" << std::endl;
    std::cout << "#     iter_count = " << iter_count << "" << std::endl;
    std::cout << "#     fseed = " << fseed << "" << std::endl;
    std::cout << "#     DiEqualTi = " << DiEqualTi << "" << std::endl;
    std::cout << "#     dt = " << config.kind << "" << std::endl;
    std::cout << "######################################################################################################" << std::endl;

    if (out_file.is_open()) {
        VERBOSE_SET_PROGRESS(begin_n, end_n);
    }
    if (config.detailed) {
		print_detailed_al_header(out_stream);
	} else {
		print_al_header(out_stream);
	}

    VERBOSE_ASSERT(end_n % step_n == 0, "The step is not rounding perfectly. I have OCDs, please be kind.");
	for (auto n = begin_n ; n <= end_n ; n+= step_n) {

        if (out_file.is_open()) {
            VERBOSE_UPDATE_PROGRESS(n);
        }

		size_t seed = fseed + n;
		size_t high_m = (n * (n - 1)) / 3;
		size_t medium_m = (n * (n - 1)) / 4;
		size_t low_m = (n * (n - 1)) / 5;

		for (size_t m : {low_m, medium_m, high_m} ) {

				VERBOSE_INFO ("Run benchmark_age_latency with arguments " << sample_count << "," << iter_count << "," << n << "," << m << "," << dt << "," << seed );

				if (config.detailed) {
					for (size_t i = 0 ; i < sample_count ; i ++ ) {
                        GeneratorRequest r (n,m,seed+i,dt, DiEqualTi);
						LETModel sample = Generator::getInstance().generate(r);
                        AgeLatencyResult fun_res = config.dryrun?AgeLatencyResult(sample):original(sample);
                        print_detailed_al_row(r,fun_res, out_stream);
					}
				} else {
					AgeLatencyBenchmarkResult bench  = benchmark_age_latency ( original, sample_count, iter_count, n, m, dt,  DiEqualTi, seed) ;
					print_al_row(bench, out_stream);
				}
			}
		}

        VERBOSE_FINISH_PROGRESS();

}


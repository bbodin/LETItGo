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

/**
 *
 * Run the Age latency multiple time on the same graph for n iterations
 * and return the average.
 *
 * execution time in millisecond
 *
 */

double get_age_latency_execution_time (AgeLatencyFun fun, LETModel sample, size_t n) {
	double sum_time = 0;
	for (size_t i = 0 ; i < n; i++) {
		auto t1 = std::chrono::high_resolution_clock::now();
		fun(sample, generate_partial_constraint_graph);
		auto t2 = std::chrono::high_resolution_clock::now();
		auto duration = t2 - t1;
		sum_time += duration.count();
	}
	return (sum_time / n) / 1000000;
}

ExpansionBenchmarkResult  benchmark_expansion   (GenerateExpansionFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m,  LETDatasetType dt, bool harmonized_periodicity, size_t seed) {
	double sum_time = 0;
	long sum_edge = 0;
	long sum_vertex = 0;

	Generator& g = Generator::getInstance();

	VERBOSE_DEBUG("Start benchmark with n=" << n << " and " << " m=" << m << " seed=" << seed);
	INTEGER_TIME_UNIT sum_n = 0 ;
	Algorithm2_statistics total_stats;
	total_stats.total_case1 = 0;
	total_stats.total_case2 = 0;
	total_stats.total_case3 = 0;
	for (size_t i = 0 ; i < sample_count ; i ++ ) {

		// Prepare problem instance
		LETModel sample = g.generate(dt, n,m, seed + i);
		auto K = harmonized_periodicity ?  generate_random_ni_periodicity_vector(sample, seed) : generate_random_periodicity_vector(sample, seed);
		INTEGER_TIME_UNIT lcm = getLCM<INTEGER_TIME_UNIT>(sample);
		sum_n += getSumN<INTEGER_TIME_UNIT> (sample);
		VERBOSE_DEBUG("LCM=" << lcm);
		VERBOSE_DEBUG("K=" << K);

		for (Task t : sample.tasks()) {
			auto tid = t.getId();
			VERBOSE_ASSERT(K[tid] , 0);
		}
		// Check the instance can be solved and retrieve algo2 stats

		auto original = generate_partial_constraint_graph(sample, K);
		Algorithm2_statistics::getSingleton().clear();
		PartialConstraintGraph res = fun(sample, K);
		total_stats = total_stats + Algorithm2_statistics::getSingleton();

		if (res != original) {
			std::cout << "Failed with: "  << std::endl
					<< " sample seed =" << seed + i << std::endl
					<< " N =" << n << std::endl
					<< " M =" << m << std::endl
					<< " K seed =" << seed << std::endl
					<< " harmonized_periodicity =" << harmonized_periodicity << std::endl
					<< " K =" << K << std::endl
					<< sample << std::endl;
		}
		VERBOSE_ASSERT_EQUALS(res, original);


		//Get timings and statistics
		double sub_sum_time = 0;
		for (size_t i = 0 ; i < iter_count; i++) {
			auto t1 = std::chrono::high_resolution_clock::now();
			fun(sample, K);
			auto t2 = std::chrono::high_resolution_clock::now();
			auto duration = t2 - t1;
			sub_sum_time += duration.count();
		}
		auto duration =  (sub_sum_time / n);

		sum_vertex += original.getExecutions().size();
		sum_edge += original.getConstraints().size();

		sum_time += duration / 1000000;
		VERBOSE_DEBUG("    **** duration=" << duration);

	}

	return ExpansionBenchmarkResult(sample_count,(double) sum_n / (double)sample_count, total_stats, (double)sum_time / (double)sample_count, sum_vertex, sum_edge);
}

AgeLatencyBenchmarkResult benchmark_age_latency (AgeLatencyFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m, LETDatasetType dt, size_t seed) {

	//double sum_time = 0;
	//double sum_iter = 0;
	//double sum_size = 0;

	GenerateExpansionFun expFun = (GenerateExpansionFun) generate_partial_constraint_graph;

	AgeLatencyBenchmarkResult bench_res (0,0,0,0,0,0,0);

	VERBOSE_DEBUG("Start benchmark with n=" << n << " and " << " m=" << m);
	for (size_t i = 0 ; i < sample_count ; i ++ ) {
		VERBOSE_INFO ("Run generate with arguments n=" << n << ", m=" << m << ", dt=" << dt << ", seed=" << seed + i);
		LETModel sample = Generator::getInstance().generate(dt, n , m , seed + i);
		INTEGER_TIME_UNIT lcm = getLCM<INTEGER_TIME_UNIT>(sample);
		VERBOSE_DEBUG("LCM=" << lcm);
		INTEGER_TIME_UNIT sum_n = getSumN<INTEGER_TIME_UNIT> (sample);

		VERBOSE_INFO ("Run get_age_latency_execution_time");
		auto duration = get_age_latency_execution_time (fun, sample, iter_count);

		VERBOSE_INFO ("Run get_age_latency one last time");
		AgeLatencyResult fun_res = fun(sample, expFun);
		bench_res.time  += duration;
		bench_res.iter  += fun_res.required_iterations;
		bench_res.sum_n  += sum_n;
		bench_res.size  += (double) fun_res.expansion_size / (double) sum_n;
		bench_res.bound += (double) fun_res.first_bound_error / (double) fun_res.age_latency;
		bench_res.g_ctime += fun_res.graph_computation_time;
		bench_res.p_ctime += fun_res.path_computation_time;
		VERBOSE_DEBUG("    **** duration=" << duration << " iteration=" << fun_res.required_iterations
				<< " part=" <<  fun_res.expansion_size << " size=" << sum_n << " first_bound_error=" << fun_res.first_bound_error);

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


void main_benchmark_expansion (ExpansionBenchmarkConfiguration config) {

	size_t begin_n = config.begin_n ;
	size_t end_n        = config.end_n ;
	size_t step_n       = config.step_n     ;
	size_t sample_count = config.sample_count;
	size_t iter_count   = config.iter_count  ;
	size_t fseed         = config.seed       ;

	size_t total = sample_count * (end_n - begin_n + step_n) / step_n;
	VERBOSE_INFO("Start benchmark of " << total << " runs.");

	//boost::timer::progress_display show_progress( total );
	std::cout << "############################################################################################" << std::endl;
	std::cout << "########## LET it Go Age Expansion Benchmarking                                          ###" << std::endl;
	std::cout << "############################################################################################" << std::endl;
	std::cout << "#     begin_n = " << begin_n << "" << std::endl;
	std::cout << "#     end_n = " << end_n << "" << std::endl;
	std::cout << "#     step_n = " << step_n << "" << std::endl;
	std::cout << "#     sample_count = " << sample_count << "" << std::endl;
	std::cout << "#     iter_count = " << iter_count << "" << std::endl;
	std::cout << "#     fseed = " << fseed << "" << std::endl;
	std::cout << "############################################################################################" << std::endl;

	GenerateExpansionFun f_original          = (GenerateExpansionFun) generate_partial_constraint_graph;
	GenerateExpansionFun f_new               = (GenerateExpansionFun) new_generate_partial_constraint_graph;
	GenerateExpansionFun f_new_and_optimized = (GenerateExpansionFun) opt_new_generate_partial_constraint_graph;

	std::cout
		<< std::setw(4) << "dt"
		<< std::setw(4) << "KdN"
		<< std::setw(5) << "n"
			<< std::setw(5) << "m"
			<< std::setw(10) << "lcm"
			<< std::setw(10) << "V"
			<< std::setw(10) << "E"
			<< std::setw(10) << "orig"
			<< std::setw(10) << "new"
			<< std::setw(10) << "opt"
			<< std::setw(7) << "ratio"
			<< std::setw(7) << "TC1"
			<< std::setw(7) << "TC2"
			<< std::setw(7) << "TC3"
			<< std::endl;

	for (size_t n = begin_n ; n <= end_n ; n+= step_n) {

		size_t low_m  = (n * (n - 1)) / 4;
		size_t high_m = (n * (n - 1)) / 3;

		size_t seed = fseed + n;

		for (size_t m : {low_m, high_m} ) {
			for (bool hpf : {false, true}) {
				for (LETDatasetType dt : {LETDatasetType::automotive_dt, LETDatasetType::harmonic_dt} ) {

					std::cout
					<< std::setw(4)  << dt
					<< std::setw(4)  << hpf
					<< std::setw(5)  << n
							<< std::setw(5)  << m  << std::flush;

				ExpansionBenchmarkResult bench_res1  = benchmark_expansion ( f_original , sample_count, iter_count, n, m,dt,  hpf,  seed) ;
				ExpansionBenchmarkResult bench_res2  = benchmark_expansion ( f_new , sample_count, iter_count, n, m, dt, hpf,  seed) ;
				ExpansionBenchmarkResult bench_res3  = benchmark_expansion ( f_new_and_optimized , sample_count, iter_count, n, m,dt,  hpf,  seed) ;
				std::cout
				<< std::setw(10) << bench_res1.sum_n / (double) bench_res1.sample_count
						<< std::setw(10) << bench_res1.total_vertex_count / (double) bench_res1.sample_count
						<< std::setw(10) << bench_res1.total_edge_count / (double) bench_res1.sample_count
						<< std::setw(10) << std::setprecision(2) << std::fixed << bench_res1.average_time
						<< std::setw(10) << std::setprecision(2) << std::fixed << bench_res2.average_time
						<< std::setw(10) << std::setprecision(2) << std::fixed << bench_res3.average_time
						<< std::setw(7)  << std::setprecision(2) << std::fixed << bench_res3.average_time /  bench_res1.average_time
						<< std::setw(7)  << std::setprecision(2) << std::fixed << bench_res2.algo2_stats.total_case1 /  (double) (bench_res2.sample_count * m)
						<< std::setw(7)  << std::setprecision(2) << std::fixed << bench_res2.algo2_stats.total_case2 /  (double) (bench_res2.sample_count * m)
						<< std::setw(7)  << std::setprecision(2) << std::fixed << bench_res2.algo2_stats.total_case3 /  (double) (bench_res2.sample_count * m)
						<< std::endl;
				}
			}

		}

	}


}

void main_benchmark_age_latency (AgeLantencyBenchmarkConfiguration config) {

	size_t begin_n = config.begin_n ;
	size_t end_n        = config.end_n ;
	size_t step_n       = config.step_n     ;
	size_t sample_count = config.sample_count;
	size_t iter_count   = config.iter_count  ;
	size_t fseed         = config.seed       ;

	AgeLatencyFun original = (AgeLatencyFun) ComputeAgeLatency;


	size_t total = sample_count * (end_n - begin_n + step_n) / step_n;
	VERBOSE_INFO("Start benchmark of " << total << " runs.");

	//boost::timer::progress_display show_progress( total );
	std::cout << "#######################################################################################################################################" << std::endl;
	std::cout << "########## LET it Go Age latency Benchmarking                                      ####################################################" << std::endl;
	std::cout << "#######################################################################################################################################" << std::endl;
	std::cout << "#     begin_n = " << begin_n << "" << std::endl;
	std::cout << "#     end_n = " << end_n << "" << std::endl;
	std::cout << "#     step_n = " << step_n << "" << std::endl;
	std::cout << "#     sample_count = " << sample_count << "" << std::endl;
	std::cout << "#     iter_count = " << iter_count << "" << std::endl;
	std::cout << "#     fseed = " << fseed << "" << std::endl;
	std::cout << "#######################################################################################################################################" << std::endl;
	std::cout
	<< std::setw(5) << "kind"
	<< std::setw(5) << "n"
			<< std::setw(5) << "m"
			<< std::setw(10) << "lcm"
			<< std::setw(10) << "time"
			<< std::setw(10) << "iter"
			<< std::setw(10) << "size"
			<< std::setw(10) << "bound"
			<< std::setw(10) << "gen_time"
			<< std::setw(10) << "sp_time"
			<< std::endl;

	for (auto n = begin_n ; n <= end_n ; n+= step_n) {

		size_t seed = fseed + n;
		size_t high_m = (n * (n - 1)) / 3;
		size_t low_m = (n * (n - 1)) / 4;

		for (size_t m : {low_m, high_m} ) {
			for (LETDatasetType dt : {LETDatasetType::automotive_dt, LETDatasetType::harmonic_dt} ) {

				VERBOSE_INFO ("Run benchmark_age_latency with arguments " << sample_count << "," << iter_count << "," << n << "," << m << "," << dt << "," << seed );

				std::cout
				  << std::setw(5) << dt
				  << std::setw(5) << n
					  << std::setw(5) << m << std::flush;
					  AgeLatencyBenchmarkResult bench  = benchmark_age_latency ( original, sample_count, iter_count, n, m, dt, seed) ;
					  std::cout
					  << std::setw(10)  << std::setprecision(2)  << bench.sum_n
					  << std::setw(10) << std::setprecision(2)   << std::fixed << bench.time
					  << std::setw(10)  << std::setprecision(2)  << bench.iter
					  << std::setw(10) << bench.size
					  << std::setw(10) << bench.bound
					  << std::setw(10) << bench.g_ctime
					  << std::setw(10) << bench.p_ctime

					  << std::endl;
			}
		}
	}


}


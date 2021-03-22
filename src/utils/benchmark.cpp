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

ExpansionBenchmarkResult  benchmark_expansion   (GenerateExpansionFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m,  size_t seed) {
	  double sum_time = 0;
	  long sum_edge = 0;
	  long sum_vertex = 0;
	  Generator& g = Generator::getInstance();

	  VERBOSE_DEBUG("Start benchmark with n=" << n << " and " << " m=" << m << " seed=" << seed);
	  for (size_t i = 0 ; i < sample_count ; i ++ ) {
		  LETModel sample = g.generate(n,m, seed + i);

		  auto K = generate_random_periodicity_vector(sample, seed);

		  INTEGER_TIME_UNIT lcm = getLCM<INTEGER_TIME_UNIT>(sample);
		  VERBOSE_DEBUG("LCM=" << lcm);

		  double sub_sum_time = 0;
		  for (size_t i = 0 ; i < iter_count; i++) {
			  auto t1 = std::chrono::high_resolution_clock::now();
			  fun(sample, K);
			  auto t2 = std::chrono::high_resolution_clock::now();
			  auto duration = t2 - t1;
			  sub_sum_time += duration.count();
		  }

		  auto duration =  (sub_sum_time / n);
		  PartialConstraintGraph res = fun(sample, K);

		  auto original = generate_partial_constraint_graph(sample, K);
		  sum_vertex += original.getExecutions().size();
		  sum_edge += original.getConstraints().size();
		  if (res != original) {
				std::cout << "Failed with: K =" << K << std::endl<< sample << std::endl;
		  }

		  VERBOSE_ASSERT_EQUALS(res, original);

		  sum_time += duration / 1000000;
		  VERBOSE_DEBUG("    **** duration=" << duration);

	  }

	  return ExpansionBenchmarkResult( sample_count , (double)sum_time / (double)sample_count, sum_vertex, sum_edge);
}

AgeLatencyBenchmarkResult benchmark_age_latency (AgeLatencyFun fun, size_t sample_count, size_t iter_count, size_t n, size_t m, size_t seed) {

	  //double sum_time = 0;
	  //double sum_iter = 0;
	  //double sum_size = 0;

	  AgeLatencyBenchmarkResult bench_res (0,0,0,0,0,0);

	  VERBOSE_DEBUG("Start benchmark with n=" << n << " and " << " m=" << m);
	  for (size_t i = 0 ; i < sample_count ; i ++ ) {
			  LETModel sample = Generator::getInstance().generate(n,m, seed + i);
			  INTEGER_TIME_UNIT lcm = getLCM<INTEGER_TIME_UNIT>(sample);
			  VERBOSE_DEBUG("LCM=" << lcm);
			  INTEGER_TIME_UNIT sum_n = getSumN<INTEGER_TIME_UNIT> (sample);
			  auto duration = get_age_latency_execution_time (fun, sample, iter_count);
			  AgeLatencyResult fun_res = fun(sample, generate_partial_constraint_graph);
			  bench_res.time  += duration;
			  bench_res.iter  += fun_res.required_iterations;
			  bench_res.size  += (double) fun_res.expansion_size / (double) sum_n;
			  bench_res.bound += (double) fun_res.first_bound_error / (double) fun_res.age_latency;
			  bench_res.g_ctime += fun_res.graph_computation_time;
			  bench_res.p_ctime += fun_res.path_computation_time;
			  VERBOSE_DEBUG("    **** duration=" << duration << " iteration=" << fun_res.required_iterations
					  << " part=" <<  fun_res.expansion_size << " size=" << sum_n << " first_bound_error=" << fun_res.first_bound_error);

	  }

	  bench_res.time  /= (double) sample_count;
	  bench_res.iter  /= (double) sample_count;
	  bench_res.size  /= (double) sample_count;
	  bench_res.bound /= (double) sample_count;
	  bench_res.g_ctime  /= (double) sample_count;
	  bench_res.p_ctime /= (double) sample_count;

	  return bench_res;
}


void main_benchmark_expansion (size_t begin_n, size_t end_n, size_t step_n, size_t sample_count, size_t iter_count, size_t fseed) {

	size_t total = sample_count * (end_n - begin_n + step_n) / step_n;
	VERBOSE_INFO("Start benchmark of " << total << " runs.");

	  //boost::timer::progress_display show_progress( total );
	  std::cout << "##########################################################################################" << std::endl;
	  std::cout << "########## LET it Go Age Expansion Benchmarking                                    #######" << std::endl;
	  std::cout << "##########################################################################################" << std::endl;
	  std::cout << "#     begin_n = " << begin_n << "" << std::endl;
	  std::cout << "#     end_n = " << end_n << "" << std::endl;
	  std::cout << "#     step_n = " << step_n << "" << std::endl;
	  std::cout << "#     sample_count = " << sample_count << "" << std::endl;
	  std::cout << "#     iter_count = " << iter_count << "" << std::endl;
	  std::cout << "#     fseed = " << fseed << "" << std::endl;
	  std::cout << "##########################################################################################" << std::endl;


	std::cout << "n\tm\tV\tE\torig\tnew\topt\tratio" << std::endl;

	for (size_t n = begin_n ; n <= end_n ; n+= step_n) {
		size_t m = (n * (n - 1)) / 3;

		size_t seed = fseed + n;

		ExpansionBenchmarkResult bench_res1  = benchmark_expansion ( generate_partial_constraint_graph , sample_count, iter_count, n, m, seed) ;
		ExpansionBenchmarkResult bench_res2  = benchmark_expansion ( new_generate_partial_constraint_graph , sample_count, iter_count, n, m, seed) ;
		ExpansionBenchmarkResult bench_res3  = benchmark_expansion ( opt_new_generate_partial_constraint_graph , sample_count, iter_count, n, m, seed) ;

		std::cout << n << "\t" << m
				<< "\t"  << bench_res1.total_vertex_count / bench_res1.sample_count
				<< "\t"  << bench_res1.total_edge_count / bench_res1.sample_count
				<< "\t"  << std::setprecision(2) << std::fixed << bench_res1.average_time
			   	<< "\t"  << std::setprecision(2) << std::fixed << bench_res2.average_time
			   	<< "\t"  << std::setprecision(2) << std::fixed << bench_res3.average_time
			   	<< "\t"  << std::setprecision(2) << std::fixed << bench_res3.average_time /  bench_res1.average_time
				<< std::endl;

		 m = (n * (n - 1)) / 4;
		 bench_res1  = benchmark_expansion ( generate_partial_constraint_graph , sample_count, iter_count, n, m, seed) ;
		 bench_res2  = benchmark_expansion ( new_generate_partial_constraint_graph , sample_count, iter_count, n, m, seed) ;
		 bench_res3  = benchmark_expansion ( opt_new_generate_partial_constraint_graph , sample_count, iter_count, n, m, seed) ;

		 std::cout << n << "\t" << m
					<< "\t"  << bench_res1.total_vertex_count / bench_res1.sample_count
					<< "\t"  << bench_res1.total_edge_count / bench_res1.sample_count
						<< "\t"  << std::setprecision(2) << std::fixed << bench_res1.average_time
					   	<< "\t"  << std::setprecision(2) << std::fixed << bench_res2.average_time
					   	<< "\t"  << std::setprecision(2) << std::fixed << bench_res3.average_time
					   	<< "\t"  << std::setprecision(2) << std::fixed << bench_res3.average_time /  bench_res1.average_time
						<< std::endl;
	  }




}

void main_benchmark_age_latency (size_t begin_n, size_t end_n, size_t step_n, size_t sample_count, size_t iter_count, size_t fseed) {



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
	  std::cout << "     index     ltime      iter      size     bound     gtime     ptime     htime      iter      size     bound     gtime     ptime     " << std::endl;


	  for (auto n = begin_n ; n <= end_n ; n+= step_n) {
		  size_t seed = fseed + n;
		  size_t high_density_m = (n * (n - 1)) / 3;
		  size_t low_density_m = (n * (n - 1)) / 4;

		  AgeLatencyBenchmarkResult bench_low  = benchmark_age_latency ( ComputeAgeLatency, sample_count, iter_count, n, low_density_m, seed) ;
		  AgeLatencyBenchmarkResult bench_high = benchmark_age_latency ( ComputeAgeLatency, sample_count, iter_count, n, high_density_m, seed) ;

		  std::cout << std::setw(10) << n
				  << std::setw(10) << std::setprecision(2) << std::fixed << bench_low.time
				  << std::setw(10)  << std::setprecision(2)  << bench_low.iter
				  << std::setw(10) << bench_low.size
				  << std::setw(10) << bench_low.bound
				  << std::setw(10) << bench_low.g_ctime
				  << std::setw(10) << bench_low.p_ctime

				  << std::setw(10)  <<std::setprecision(2) << std::fixed << bench_high.time
				  << std::setw(10) << std::setprecision(2) << bench_high.iter
				  << std::setw(10) << bench_high.size
				  << std::setw(10) << bench_high.bound
				  << std::setw(10) << bench_high.g_ctime
				  << std::setw(10) << bench_high.p_ctime

				  << std::endl;

	  }


}


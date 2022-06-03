/*
 * age_latency.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: toky
 */

#include <model.h>
#include <partial_constraint_graph.h>
#include <repetition_vector.h>
#include <utils.h>
#include <age_latency.h>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stack>
#include <chrono>

// TODO: this ensure the graph generation is correct, and that the lower bound is correct.
#ifndef __RELEASE__
#define SUPERDBG
#endif


AgeLatencyResult ComputeAgeLatency(const LETModel &model, GenerateExpansionFun upper_fun, GenerateExpansionFun lower_fun) {

    auto start = std::chrono::high_resolution_clock::now();

    //letitgo::utils::set_verbose_custom_mode ("AGE_LATENCY", true);
	VERBOSE_INFO ("Run ComputeAgeLatency");
	AgeLatencyResult res;
	res.n = model.tasks().size();
	res.m = model.dependencies().size();

	res.sum_n = compute_sum_n(model);


	bool NeedsToContinue = true;
	PeriodicityVector K = generate_periodicity_vector(model);

	while (NeedsToContinue) {

		auto count = res.expansion_vertex_count.size();

        VERBOSE_INFO ("Iteration " << count<< " K = " << K);
        VERBOSE_INFO ("Iteration " << count<< " Graph Generation");

		auto s1 = std::chrono::high_resolution_clock::now();
		// Construct the PartialConstraintGraph and
		PartialConstraintGraph PKG = upper_fun(model, K);
		auto s2 = std::chrono::high_resolution_clock::now();
#ifdef SUPERDBG
		// TODO: Check against reference, Remove for real experiments
		VERBOSE_ASSERT_EQUALS(PKG, generate_partial_constraint_graph(model, K));
#endif

		VERBOSE_INFO ("Iteration " << count  << " Find Longest Path");

		// Find longest path and update the res
		auto s3 = std::chrono::high_resolution_clock::now();
		auto FLP = FindLongestPath(PKG);
		auto s4 = std::chrono::high_resolution_clock::now();

		auto P = FLP.first;


        VERBOSE_INFO ("Iteration " << count  << " Lower bound Graph Generation");
        // Compute the lower bound to check it is lower than the uppoer bound.
        auto pbgbis = lower_fun(model, K);
        VERBOSE_INFO ("Iteration " << count << " Lower bound Find Longest Path");
        auto lower_bound = FindLongestPath(pbgbis);
        VERBOSE_AGE_LATENCY(" * FindLongestPath(UPPER) = " << FLP);
        VERBOSE_AGE_LATENCY(" * FindLongestPath(LOWER) = " << lower_bound);
        VERBOSE_AGE_LATENCY(" * Check " << lower_bound.second << " <= " << FLP.second);
        VERBOSE_ASSERT(lower_bound.second <= FLP.second, "The lower bound function does not work");
        res.lower_bounds.push_back(lower_bound.second);


        auto s5 = std::chrono::high_resolution_clock::now();

        res.upper_bounds.push_back(FLP.second);
		res.expansion_vertex_count.push_back(PKG.getExecutions().size());
		res.expansion_edge_count.push_back(PKG.getConstraints().size());
		res.age_latency = FLP.second;

		res.graph_computation_time += (s2-s1).count() / 1000000;
        res.upper_computation_time += (s4-s3).count() / 1000000;
        res.lower_computation_time += (s5-s4).count() / 1000000;



		VERBOSE_INFO ("Iteration " << count  << " Conclude");

		INTEGER_TIME_UNIT T_P = 1;


		for (Execution e : P) {
			if (e.first == -1)
				continue;
			auto tid = e.getTaskId();
			auto task = model.getTaskById(tid);
			auto taskT = task.getT();
			VERBOSE_ASSERT(taskT > 0, "task.T Cannot be null");
			T_P = std::lcm(T_P, taskT);
		}

        VERBOSE_INFO ("Compute T_P = lcm (T in P) =" << T_P);
		VERBOSE_ASSERT(T_P > 0, "T_P Cannot be null");

		// Compute N[T]
		std::map<TASK_ID, INTEGER_TIME_UNIT> N;

		for (Execution e : P) {
			if (e.first == -1)
				continue;
			auto tid = e.getTaskId();
			auto task = model.getTaskById(tid);
			auto taskT = task.getT();
			N[tid] = (T_P / taskT);
		}

        VERBOSE_INFO ("Compute N[ti] = T_P / T =" << N);

		// we check the critical cycle is max
		NeedsToContinue = false;
		for (Execution e : P) {
			if (e.first == -1)
				continue;
			auto tid = e.getTaskId();
			if ((K[tid] % N[tid]) != 0) {
				NeedsToContinue = true;
				VERBOSE_AGE_LATENCY("NiP|Ki = " << N[tid] << "|" << K[tid]
																<< " needs to continue ..");
			} else {
				VERBOSE_AGE_LATENCY("NiP|Ki = " << N[tid] << "|" << K[tid]);
			}
		}



		if (!NeedsToContinue) {
            VERBOSE_INFO ("Iteration " << count  << " Is the last one.");
			continue;
		}

        VERBOSE_INFO ("Iteration " << count  << " Update K");

		for (Execution e : P) {
			if (e.first == -1)
				continue;
			auto tid = e.getTaskId();
			K[tid] = std::lcm(K[tid], N[tid]);
			VERBOSE_AGE_LATENCY("NiP|Ki = " << N[tid] << "|" << K[tid] << " will be "
					<< std::lcm(K[tid], N[tid]));
		}
	}

	VERBOSE_AGE_LATENCY("Final K = " << K);
	VERBOSE_DEBUG("Final Age latency = " << res.age_latency);

    res.total_time = (std::chrono::high_resolution_clock::now()-start).count() / 1000000;
	return res;
}


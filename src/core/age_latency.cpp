/*
 * age_latency.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: toky
 */

#include <model.h>
#include <partial_constraint_graph.h>
#include <utils.h>
#include <age_latency.h>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stack>
#include <chrono>

// TODO: this ensure the graph generation is correct, and that the lower bound is correct.
#define SUPERDBG



AgeLatencyResult ComputeAgeLatency(const LETModel &model, GenerateExpansionFun fun) {

	VERBOSE_INFO ("Run ComputeAgeLatency");
	AgeLatencyResult res;
	bool NeedsToContinue = true;
	PeriodicityVector K = generate_periodicity_vector(model);

	while (NeedsToContinue) {
		res.required_iterations++;
		VERBOSE_INFO ("Iteration" << res.required_iterations << " Graph Generation");

		auto s1 = std::chrono::high_resolution_clock::now();
		// Construct the PartialConstraintGraph and
		PartialConstraintGraph PKG = fun(model, K);
		auto s2 = std::chrono::high_resolution_clock::now();
#ifdef SUPERDBG
		// TODO: Check against reference, Remove for real experiments
		VERBOSE_ASSERT_EQUALS(PKG, generate_partial_constraint_graph(model, K));
#endif

		VERBOSE_INFO ("Iteration" << res.required_iterations << " Find Longest Path");

		// Find longest path and update the res
		auto s3 = std::chrono::high_resolution_clock::now();
		auto FLP = FindLongestPath(PKG);
		auto s4 = std::chrono::high_resolution_clock::now();

		auto P = FLP.first;
		res.expansion_size = PKG.getExecutions().size() - 2 ; // -2 for start and stop
		res.age_latency = FLP.second;

		res.graph_computation_time += (s2-s1).count() / 1000000;
		res.path_computation_time += (s4-s3).count() / 1000000;

#ifdef SUPERDBG
		VERBOSE_INFO ("Iteration" << res.required_iterations << " Lower bound Graph Generation");
		// Compute the lower bound to check it is lower than the uppoer bound.
		auto pbgbis = generate_partial_lowerbound_graph(model, K);
		VERBOSE_INFO ("Iteration" << res.required_iterations << " Lower bound Find Longest Path");
		auto lower_bound = FindLongestPath(pbgbis);
		VERBOSE_AGE_LATENCY(" * FindLongestPath(PKG) = " << FLP);



		VERBOSE_AGE_LATENCY(" * bound = " << lower_bound << " <= " << FLP);
		VERBOSE_ASSERT(lower_bound.second <= FLP.second, "The lower bound function does not work");
		if (res.required_iterations == 1) res.first_bound_error = FLP.second - lower_bound.second;
#endif

		VERBOSE_INFO ("Iteration" << res.required_iterations << " Conclude");

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

		VERBOSE_ASSERT(T_P > 0, "T_P Cannot be null");

		// COmpute N[T]
		std::map<TASK_ID, INTEGER_TIME_UNIT> N;
		for (Execution e : P) {
			if (e.first == -1)
				continue;
			auto tid = e.getTaskId();
			auto task = model.getTaskById(tid);
			auto taskT = task.getT();
			N[tid] = (T_P / taskT);
		}

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
			continue;
		}

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

	return res;
}


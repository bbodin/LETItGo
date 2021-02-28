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


#define VERBOSE_AGE_LATENCY(m) VERBOSE_CUSTOM_DEBUG("AGE_LATENCY", m)

AgeLatencyResult ComputeAgeLatency(const LETModel &model, GenerateExpansionFun fun) {
	AgeLatencyResult res;
	bool NeedsToContinue = true;
	PeriodicityVector K = generate_periodicity_vector(model);

	while (NeedsToContinue) {
		res.required_iterations++;
		VERBOSE_DEBUG ("Iteration" << res.required_iterations);
		PartialConstraintGraph PKG = fun(model, K);
		PartialConstraintGraph PKGGOLD = generate_partial_constraint_graph(model, K);
		VERBOSE_ASSERT_EQUALS(PKG, PKGGOLD);
		auto FLP = FindLongestPath(PKG);
		auto P = FLP.first;
		res.expansion_size = PKG.getExecutions().size();
		res.age_latency = FLP.second;
		VERBOSE_AGE_LATENCY("FindLongestPath(PKG) = " << FLP);
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

		std::map<TASK_ID, INTEGER_TIME_UNIT> N;
		for (Execution e : P) {
			if (e.first == -1)
				continue;
			auto tid = e.getTaskId();
			auto task = model.getTaskById(tid);
			auto taskT = task.getT();
			N[tid] = (T_P / taskT);
		}

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

		if (!NeedsToContinue)
			continue;

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


/*
 * age_latency.h
 *
 *  Created on: Dec 29, 2020
 *      Author: toky
 */

#ifndef SRC_INCLUDE_AGE_LATENCY_H_
#define SRC_INCLUDE_AGE_LATENCY_H_

#include <model.h>
#include <numeric>

#define VERBOSE_AGE_LATENCY(m) VERBOSE_CUSTOM_DEBUG("AGE_LATENCY", m)

struct AgeLatencyResult {
	size_t n = 0;
	size_t m = 0;
	size_t sum_n = 0;
	TIME_UNIT graph_computation_time = 0.0;
	TIME_UNIT path_computation_time  = 0.0;
	INTEGER_TIME_UNIT age_latency = 0;
	std::vector<INTEGER_TIME_UNIT> expansion_sizes;
	std::vector<INTEGER_TIME_UNIT> upper_bounds;
	std::vector<INTEGER_TIME_UNIT> lower_bounds;

	AgeLatencyResult () {}

	friend std::ostream &operator<<(std::ostream &stream, const AgeLatencyResult &obj) {
	    stream << "<AgeLatencyResult"
	    		<< " n=" << obj.n
	    		<< " m=" << obj.m
	    		<< " sumN=" << obj.sum_n
	    		<< " graph_computation_time=" << obj.graph_computation_time
	    		<< " path_computation_time=" << obj.path_computation_time
	    		<< " age_latency=" << obj.age_latency
	    		<< " iterations=" << obj.expansion_sizes.size()
	    		 << " size=" << obj.expansion_sizes.back()
	    		 << " first_bound_error=" << obj.lower_bounds.front()
	           << ">";
	    return stream;
	  }

};

typedef std::function<AgeLatencyResult(const LETModel &model, GenerateExpansionFun fun)> AgeLatencyFun;


AgeLatencyResult ComputeAgeLatency(const LETModel &model, GenerateExpansionFun fun = generate_partial_constraint_graph) ;





#endif /* SRC_INCLUDE_AGE_LATENCY_H_ */

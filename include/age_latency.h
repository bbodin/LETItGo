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

//#define VERBOSE_AGE_LATENCY(m) VERBOSE_CUSTOM_DEBUG("AGE_LATENCY", m)
#define VERBOSE_AGE_LATENCY(m) VERBOSE_DEBUG(m)

struct AgeLatencyResult {
	size_t n = 0;
	size_t m = 0;
	size_t sum_n = 0;
	TIME_UNIT graph_computation_time = 0.0;
    TIME_UNIT upper_computation_time  = 0.0;
    TIME_UNIT lower_computation_time  = 0.0;
    TIME_UNIT total_time  = 0.0;
	INTEGER_TIME_UNIT age_latency = 0;
	std::vector<INTEGER_TIME_UNIT> expansion_vertex_count;
	std::vector<INTEGER_TIME_UNIT> expansion_edge_count;
	std::vector<INTEGER_TIME_UNIT> upper_bounds;
	std::vector<INTEGER_TIME_UNIT> lower_bounds;

	AgeLatencyResult () {}

	friend std::ostream &operator<<(std::ostream &stream, const AgeLatencyResult &obj) {
	    stream << "<AgeLatencyResult"
	    		<< " n=" << obj.n
	    		<< " m=" << obj.m
	    		<< " sumN=" << obj.sum_n
	    		<< " graph_computation_time=" << obj.graph_computation_time
                << " upper_computation_time=" << obj.upper_computation_time
                << " lower_computation_time=" << obj.lower_computation_time
                << " total=" << obj.total_time
	    		<< " age_latency=" << obj.age_latency
	    		<< " iterations=" << obj.expansion_vertex_count.size()
	    	    << " ExVSize=" << obj.expansion_vertex_count.back()
	    		<< " ExESize=" << obj.expansion_edge_count.back()
	    		<< " upper_bounds=" << obj.upper_bounds
	    		<< " lower_bounds=" << obj.lower_bounds
	    		 << " first_bound_error=" << obj.lower_bounds.front()
	           << ">";
	    return stream;
	  }

};

typedef std::function<AgeLatencyResult(const LETModel &model, GenerateExpansionFun ufun, GenerateExpansionFun lfun)> AgeLatencyFun;


AgeLatencyResult compute_age_latency(const LETModel &model, GenerateExpansionFun upper_fun = generate_partial_upperbound_graph,
                                     GenerateExpansionFun lower_fun = generate_partial_lowerbound_graph) ;



AgeLatencyResult NewComputeAgeLatency(const LETModel &model, GenerateExpansionFun upper_fun = generate_partial_upperbound_graph,
                                     GenerateExpansionFun lower_fun = generate_partial_lowerbound_graph) ;



#endif /* SRC_INCLUDE_AGE_LATENCY_H_ */

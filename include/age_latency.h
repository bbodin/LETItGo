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
	TIME_UNIT graph_computation_time;
	TIME_UNIT path_computation_time;
	INTEGER_TIME_UNIT age_latency;
	size_t required_iterations;
	size_t expansion_size;
	size_t first_bound_error;

	AgeLatencyResult () :  graph_computation_time(0), path_computation_time(0), age_latency(0), required_iterations(0), expansion_size(0), first_bound_error(0) {}
	AgeLatencyResult (TIME_UNIT graph_computation_time, TIME_UNIT path_computation_time, INTEGER_TIME_UNIT age_latency, size_t required_iterations, size_t expansion_size, size_t first_bound_error) :
		graph_computation_time(graph_computation_time), path_computation_time(path_computation_time),
			age_latency(age_latency), required_iterations(required_iterations), expansion_size(expansion_size), first_bound_error(first_bound_error) {}

	  friend std::ostream &operator<<(std::ostream &stream, const AgeLatencyResult &obj) {
	    stream << "<AgeLatencyResult "
	    		<< "graph_computation_time=" << obj.graph_computation_time
	    		<< "path_computation_time=" << obj.path_computation_time
	    		<< "age_latency=" << obj.age_latency
	    		<< ", iterations=" << obj.required_iterations
	    		 << ", size=" << obj.expansion_size
	    		 << ", first_bound_error=" << obj.first_bound_error
	           << ">";
	    return stream;
	  }

};

typedef std::function<AgeLatencyResult(const LETModel &model, GenerateExpansionFun fun)> AgeLatencyFun;


AgeLatencyResult ComputeAgeLatency(const LETModel &model, GenerateExpansionFun fun = generate_partial_constraint_graph) ;





#endif /* SRC_INCLUDE_AGE_LATENCY_H_ */

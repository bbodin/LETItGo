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


struct AgeLatencyResult {
	INTEGER_TIME_UNIT age_latency;
	size_t required_iterations;
	size_t expansion_size;
	AgeLatencyResult () :  age_latency(0), required_iterations(0), expansion_size(0) {}

	  friend std::ostream &operator<<(std::ostream &stream, const AgeLatencyResult &obj) {
	    stream << "AgeLatencyResult(age_latency:" << obj.age_latency
	    		<< ", iter:" << obj.required_iterations
	    		 << ", size:" << obj.expansion_size
	           << ")";
	    return stream;
	  }

};

AgeLatencyResult ComputeAgeLatency(const LETModel &model, GenerateExpansionFun fun = generate_partial_constraint_graph) ;





#endif /* SRC_INCLUDE_AGE_LATENCY_H_ */

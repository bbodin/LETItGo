/*
 * letitgo.h
 *
 *  Created on: Dec 29, 2020
 *      Author: toky
 */

#ifndef SRC_INCLUDE_LETITGO_H_
#define SRC_INCLUDE_LETITGO_H_

#include <model.h>


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


AgeLatencyResult ComputeAgeLatency (const LETModel& model) ;
PartialConstraintGraph generate_partial_constraint_graph (const LETModel& model , const PeriodicityVector& K) ;
std::vector<Execution> topologicalOrder (PartialConstraintGraph PKG) ;
std::pair<std::vector<Execution> , INTEGER_TIME_UNIT>  FindLongestPath(PartialConstraintGraph PKG);

PeriodicityVector generate_unitary_periodicity_vector(const LETModel &model);

template <typename X> std::ostream & operator<<(std::ostream & stream, const std::vector<X> & obj) {
	for (auto x : obj) stream << x << " ";
	return stream;
}


template <typename X, typename Y> std::ostream & operator<<(std::ostream & stream, const  std::pair<X,Y> & obj) {
	stream << obj.first << " "  << obj.second;
	return stream;
}


template <typename X, typename Y> std::ostream & operator<<(std::ostream & stream, const  std::map<X,Y> & obj) {
	stream << "map (" << std::endl ;
	for (auto key : obj) {
		stream << key.first << " : "  <<  key.second << std::endl;

	}
	stream << ")" << std::endl ;
	return stream;
}

#endif /* SRC_INCLUDE_LETITGO_H_ */

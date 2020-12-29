/*
 * letitgo.h
 *
 *  Created on: Dec 29, 2020
 *      Author: toky
 */

#ifndef SRC_INCLUDE_LETITGO_H_
#define SRC_INCLUDE_LETITGO_H_

#include <model.h>


INTEGER_TIME_UNIT ComputeAgeLatency (const LETModel& model) ;

PartialConstraintGraph generate_partial_constraint_graph (const LETModel& model , const PeriodicityVector& K) ;


std::vector<Execution> topologicalOrder (PartialConstraintGraph PKG) ;
std::pair<std::vector<Execution> , INTEGER_TIME_UNIT>  FindLongestPath(PartialConstraintGraph PKG);

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

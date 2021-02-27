/*
 * periodicity_vector.h
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */

#ifndef SRC_INCLUDE_PERIODICITY_VECTOR_H_
#define SRC_INCLUDE_PERIODICITY_VECTOR_H_

#include <model.h>

typedef std::vector<EXECUTION_COUNT> PeriodicityVector;

PeriodicityVector generate_unitary_periodicity_vector(const LETModel &model);

inline PeriodicityVector generate_unitary_periodicity_vector(const LETModel &model) {
	return PeriodicityVector(model.getTaskCount(), 1);
}




#endif /* SRC_INCLUDE_PERIODICITY_VECTOR_H_ */

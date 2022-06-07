/*
 * periodicity_vector.h
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */

#ifndef SRC_INCLUDE_PERIODICITY_VECTOR_H_
#define SRC_INCLUDE_PERIODICITY_VECTOR_H_

#include <model.h>
#include <sstream>

typedef std::vector<EXECUTION_COUNT> PeriodicityVector;

inline PeriodicityVector generate_periodicity_vector(const LETModel &model, int k = 1) {
	return PeriodicityVector(model.getTaskCount(), k);
}
inline PeriodicityVector generate_unitary_periodicity_vector(const LETModel &model) {
	return generate_periodicity_vector(model,1);
}

inline EXECUTION_COUNT sumK(PeriodicityVector K) {
    EXECUTION_COUNT sum = 0;
    for (auto it: K) {
        sum += it;
    }
    return sum;
}

inline PeriodicityVector parse_periodicity_vector(std::string text) {
    std::stringstream ss(text);
    int n;
    PeriodicityVector tmp;
    while ( ss >> n )
        tmp.push_back(n);
    return tmp;
}


#endif /* SRC_INCLUDE_PERIODICITY_VECTOR_H_ */

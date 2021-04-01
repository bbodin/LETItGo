/*
 * repetition_vector.h
 *
 *  Created on: Apr 1, 2021
 *      Author: toky
 */

#ifndef INCLUDE_REPETITION_VECTOR_H_
#define INCLUDE_REPETITION_VECTOR_H_

#include <map>
#include <model.h>

std::map<TASK_ID, INTEGER_TIME_UNIT> compute_repetition_vector (const LETModel &model);



#endif /* INCLUDE_REPETITION_VECTOR_H_ */

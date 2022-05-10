/*
 * repetition_vector.cpp
 *
 *  Created on: Apr 1, 2021
 *      Author: toky
 */


#include <repetition_vector.h>
#include <numeric>

size_t compute_sum_n (const LETModel &model) {

	size_t sum = 0;
	INTEGER_TIME_UNIT T_P = 1;

	for (const Task t : model.tasks()) {
		auto taskT = t.getT();
		VERBOSE_ASSERT(taskT > 0, "task.T Cannot be null");
		T_P = std::lcm(T_P, taskT);
	}

	VERBOSE_ASSERT(T_P > 0, "T_P Cannot be null");


	for (const Task t : model.tasks()) {
        const auto tmp =  (T_P / t.getT());
        VERBOSE_DEBUG("N[" << t.getId() << "] = " << tmp);
        sum += tmp;
	}

	return sum;
}
std::map<TASK_ID, INTEGER_TIME_UNIT> compute_repetition_vector (const LETModel &model) {

	INTEGER_TIME_UNIT T_P = 1;

	for (const Task t : model.tasks()) {
		auto taskT = t.getT();
		VERBOSE_ASSERT(taskT > 0, "task.T Cannot be null");
		T_P = std::lcm(T_P, taskT);
	}

	VERBOSE_ASSERT(T_P > 0, "T_P Cannot be null");

	// COmpute N[T]
	std::map<TASK_ID, INTEGER_TIME_UNIT> N;

	for (const Task t : model.tasks()) {
		auto taskT = t.getT();
		N[t.getId()] = (T_P / taskT);
	}

	return N;

}


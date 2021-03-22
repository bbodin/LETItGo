/*
 * generator.cpp
 *
 *  Created on: Feb 12, 2021
 *      Author: toky
 */

#include <model.h>
#include <periodicity_vector.h>
#include <random>


PeriodicityVector generate_random_periodicity_vector(const LETModel &model, size_t seed = 0) {

	if (seed == 0) {
		std::random_device rd;
		seed = rd();
	}

	VERBOSE_DEBUG("generate_random_periodicity_vector Seed=" << seed);
	std::mt19937 gen(seed);


	PeriodicityVector K(model.getTaskCount(), 1);

	for (Task t : model.tasks()) {
		std::uniform_int_distribution<> K_distrib(1,10);
		K[t.getId()] = K_distrib(gen);
	}
	return K;
}

LETModel generate_LET (unsigned int n, unsigned int m, size_t seed = 0) {


	if (seed == 0) {
		std::random_device rd;
		seed = rd();
	}
	VERBOSE_DEBUG("N=" << n << " M=" << m << " Seed=" << seed);
	std::mt19937 gen(seed);



	LETModel sample ;

	  // Possible period to be taken uniformly
	  std::vector<TIME_UNIT> H = {1,2,5,10,20,50,100};

	  // Possible release time to be taken uniformly
	  std::vector<TIME_UNIT> R = {0,1,2,3,4,5};

	  std::uniform_int_distribution<> H_distrib(0, H.size() - 1);
	  std::uniform_int_distribution<> R_distrib(0, R.size() - 1);


	  // Di = Hi

	  for (unsigned int i = 0 ; i < n ; i++) {
		  const TIME_UNIT Ri = R[R_distrib(gen)];
		  const TIME_UNIT Ti = H[H_distrib(gen)];
		  const TIME_UNIT Di = Ti;
		  TASK_ID t = sample.addTask(Ri, Di, Ti);
		  VERBOSE_ASSERT(t == i, "addTask does not behave as expected");
	  }




	  // TODO this is so brutal ...
	  VERBOSE_ASSERT(m <= (n * (n - 1)  / 2) , "Impossible cannot generate this graph too many edges.");

	  std::vector<std::pair<TASK_ID,TASK_ID>> edgebag;

	  for (TASK_ID i = 0 ; i < n ; i++) {
		  for (TASK_ID j = i + 1 ; j < n ; j++) {
			  edgebag.push_back(std::pair<TASK_ID,TASK_ID>(i,j));
		  }
	  }

	  for (unsigned int j = 0 ; j < m ; j++) {
		  std::uniform_int_distribution<> edge_distrib(0, edgebag.size() -1);
		  size_t index = edge_distrib(gen);
		  std::pair<TASK_ID,TASK_ID> d = edgebag[index];
		  sample.addDependency(d.first, d.second);
		  edgebag.erase(edgebag.begin() + index);
	  }





	  return sample;


}


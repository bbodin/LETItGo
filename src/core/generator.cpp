/*
 * generator.cpp
 *
 *  Created on: Feb 12, 2021
 *      Author: toky
 */

#include <model.h>
#include <periodicity_vector.h>
#include <repetition_vector.h>
#include <generator.h>
#include <utils.h>
#include <random>


PeriodicityVector generate_random_periodicity_vector(const LETModel &model, size_t seed) {

	if (seed == 0) {
		std::random_device rd;
		seed = rd();
	}

	VERBOSE_DEBUG("generate_random_periodicity_vector Seed=" << seed);
	std::mt19937 gen(seed);

	std::map<TASK_ID, INTEGER_TIME_UNIT> N = compute_repetition_vector (model);

	PeriodicityVector K(model.getTaskCount(), 1);

	for (Task t : model.tasks()) {
		auto tid = t.getId();
		std::uniform_int_distribution<> K_distrib(1,N[tid]);
		auto val = K_distrib(gen);
		VERBOSE_ASSERT(val > 0, "Should never happen");
		K[t.getId()] = val;
	}
	return K;
}


PeriodicityVector generate_random_ni_periodicity_vector(const LETModel &model, size_t seed) {

	if (seed == 0) {
		std::random_device rd;
		seed = rd();
	}

	VERBOSE_DEBUG("generate_random_periodicity_vector Seed=" << seed);
	std::mt19937 gen(seed);


	PeriodicityVector K(model.getTaskCount(), 1);

	std::map<TASK_ID, INTEGER_TIME_UNIT> N = compute_repetition_vector (model);

	for (Task t : model.tasks()) {
		auto tid = t.getId();
		std::vector<EXECUTION_COUNT>  factors = get_factors(N[tid]);
		std::uniform_int_distribution<> K_distrib(0,factors.size() - 1);
		size_t index = K_distrib(gen);
		VERBOSE_ASSERT(index <  factors.size(), "Should never happen");
		auto val = factors[index];
		VERBOSE_ASSERT(val > 0, "Should never happen");
		K[t.getId()] = val;
	}
	return K;
}


/**
 *
 * Generate a bunch of `n` new tasks in the LET `sample` based on data found in automotive papers.
 *
 *  T = {1,2,5,10,20,50,100}
 *  R = {0,1,2,3,4,5}
 *
 */
void GenerateAutomotiveTasks(LETModel& sample, unsigned int n, std::mt19937& gen) {

	  // Possible period to be taken uniformly
	  std::vector<INTEGER_TIME_UNIT> T = {1,2,5,10,20,50,100};

	  // Possible release time to be taken uniformly
	  std::vector<TIME_UNIT> R = {0,1,2,3,4,5};

	  std::uniform_int_distribution<> T_distrib(0, T.size() - 1);
	  std::uniform_int_distribution<> R_distrib(0, R.size() - 1);
      std::uniform_real_distribution<> D_distrib(0, 1);


	  // Di = Hi

	  for (unsigned int i = 0 ; i < n ; i++) {
		  const TIME_UNIT Ri = R[R_distrib(gen)];
		  const INTEGER_TIME_UNIT Ti = T[T_distrib(gen)];
          const INTEGER_TIME_UNIT Di =  Ti - std::floor(D_distrib(gen) * Ti);
		  TASK_ID t = sample.addTask(Ri, Di, Ti);
		  VERBOSE_ASSERT(t == i, "addTask does not behave as expected");
	  }
}

/**
 *
 * Generate a bunch of `n` new tasks in the LET `sample` where every tasks' have harmonic periods
 *
 *  MinT = {1,2,3,4,5,6,7,8,9,10}
 *  FacT = {1,2,3,4,5,6,7,8,9,10}  maximum T is 10 * 10 = 100
 *  R = {0,1,2,3,4,5}
 *
 */
void GenerateHarmonicTasks(LETModel& sample, unsigned int n, std::mt19937& gen) {

	  // Possible period to be taken uniformly
	  std::uniform_int_distribution<> TValues_distrib(1, 6);

	  std::vector<INTEGER_TIME_UNIT> T = {};
	  INTEGER_TIME_UNIT prod = 1;
	  for (auto i = 0; i < 5; i++) {
		  auto new_value = TValues_distrib(gen);
		  T.push_back(new_value * prod);
		  prod = std::lcm ( prod, new_value * prod);
	  }


	  // Possible release time to be taken uniformly
	  std::vector<TIME_UNIT> R = {0,1,2,3,4,5};

	  std::uniform_int_distribution<> T_distrib(0, T.size() - 1);
	  std::uniform_int_distribution<> R_distrib(0, R.size() - 1);
      std::uniform_real_distribution<> D_distrib(0, 1);


	  // Di = Hi

	  for (unsigned int i = 0 ; i < n ; i++) {
		  const TIME_UNIT Ri = R[R_distrib(gen)];
		  const INTEGER_TIME_UNIT Ti = T[T_distrib(gen)];
          const INTEGER_TIME_UNIT Di =  Ti - std::floor(D_distrib(gen) * Ti);
		  TASK_ID t = sample.addTask(Ri, Di, Ti);
		  VERBOSE_ASSERT(t == i, "addTask does not behave as expected");
	  }
}

/*
 * Generate a bunch of `n` new tasks in the LET `sample` where every tasks' have generic periods
 *
 *  T = (0,100)
 *  R = {0,1,2,3,4,5}
 *
 */
void GenerateGenericTasks(LETModel& sample, unsigned int n, std::mt19937& gen) {

	  // Possible release time to be taken uniformly
	  std::vector<TIME_UNIT> R = {0,1,2,3,4,5};

	  std::uniform_int_distribution<> T_distrib(1, 10);
	  std::uniform_int_distribution<> R_distrib(0, R.size() - 1);
      std::uniform_real_distribution<> D_distrib(0, 1);


	  // Di = Hi

	  for (unsigned int i = 0 ; i < n ; i++) {
		  const TIME_UNIT Ri = R[R_distrib(gen)];
		  const INTEGER_TIME_UNIT Ti = T_distrib(gen);
          const INTEGER_TIME_UNIT Di =  Ti - std::floor(D_distrib(gen) * Ti);

		  TASK_ID t = sample.addTask(Ri, Di, Ti);
		  VERBOSE_ASSERT(t == i, "addTask does not behave as expected");
	  }
}

void GenerateUniformConnections(LETModel& sample, unsigned int m, std::mt19937& gen) {

	unsigned int n = sample.tasks().size();

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





}

LETModel generate_Automotive_LET (unsigned int n, unsigned int m, size_t seed) {
	if (seed == 0) {
		std::random_device rd;
		seed = rd();
	}
	VERBOSE_DEBUG("N=" << n << " M=" << m << " Seed=" << seed);
	std::mt19937 gen(seed);
	LETModel sample ;
	GenerateAutomotiveTasks(sample, n, gen) ;
	GenerateUniformConnections(sample, m, gen);
	return sample;
}
LETModel generate_Harmonic_LET (unsigned int n, unsigned int m, size_t seed) {
	if (seed == 0) {
		std::random_device rd;
		seed = rd();
	}
	VERBOSE_DEBUG("N=" << n << " M=" << m << " Seed=" << seed);
	std::mt19937 gen(seed);
	LETModel sample ;
	GenerateHarmonicTasks(sample, n, gen) ;
	GenerateUniformConnections(sample, m, gen);
	return sample;
}
LETModel generate_Generic_LET (unsigned int n, unsigned int m, size_t seed) {

	if (seed == 0) {
		std::random_device rd;
		seed = rd();
	}
	VERBOSE_DEBUG("N=" << n << " M=" << m << " Seed=" << seed);
	std::mt19937 gen(seed);
	LETModel sample ;
	GenerateGenericTasks(sample, n, gen) ;
	GenerateUniformConnections(sample, m, gen);
	return sample;

}


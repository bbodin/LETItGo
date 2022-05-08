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

//TODO: In order to merge the three generator function
////typedef std::function<std::vector<INTEGER_TIME_UNIT>()> PeriodGeneratorFunction;


/**
 *
 * Generate a bunch of `n` new tasks in the LET `sample` based on data found in automotive papers.
 *
 *  T = {1,2,5,10,20,50,100}
 *  R = {0,1,2,3,4,5}
 *
 */
void GenerateTasks(LETModel& sample,const std::vector<INTEGER_TIME_UNIT>& T,const std::vector<TIME_UNIT>& R, bool DiEqualTi, unsigned int n, std::mt19937& gen) {


    std::uniform_int_distribution<> T_distrib(0, T.size() - 1);
    std::uniform_int_distribution<> R_distrib(0, R.size() - 1);
    std::uniform_real_distribution<> D_distrib(0, 1);


    // Di = Hi
    for (unsigned int i = 0 ; i < n ; i++) {
        const TIME_UNIT Ri = R[R_distrib(gen)];
        const INTEGER_TIME_UNIT Ti = T[T_distrib(gen)];
        const INTEGER_TIME_UNIT Di =  DiEqualTi? Ti : Ti - std::floor((double) D_distrib(gen) * (double) Ti);
        TASK_ID t = sample.addTask(Ri, Di, Ti);
        VERBOSE_ASSERT(t == i, "addTask does not behave as expected");
    }
}


/**
 *
 * Generate a bunch of `n` new tasks in the LET `sample` based on data found in automotive papers.
 *
 *  T = {1,2,5,10,20,50,100}
 *  R = {0,1,2,3,4,5}
 *
 */
void GenerateAutomotiveTasks(LETModel& sample, unsigned int n, bool DiEqualTi, std::mt19937& gen) {

	// Possible period to be taken uniformly
	const std::vector<INTEGER_TIME_UNIT> T = {1,2,5,10,20,50,100};
      const std::vector<TIME_UNIT> R = {0,1,2,3,4,5};
      GenerateTasks(sample, T, R, DiEqualTi, n, gen);

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
void GenerateHarmonicTasks(LETModel& sample, unsigned int n, bool DiEqualTi, std::mt19937& gen) {

	  // Possible period to be taken uniformly
	  std::uniform_int_distribution<> TValues_distrib(1, 6);

	  std::vector<INTEGER_TIME_UNIT> T = {};
	  INTEGER_TIME_UNIT prod = 1;
	  for (auto i = 0; i < 5; i++) {
		  auto new_value = TValues_distrib(gen);
		  T.push_back(new_value * prod);
		  prod = std::lcm ( prod, new_value * prod);
	  }

    std::vector<TIME_UNIT> R = {0,1,2,3,4,5};
    GenerateTasks(sample, T, R, DiEqualTi, n, gen);
}

/*
 * Generate a bunch of `n` new tasks in the LET `sample` where every tasks' have generic periods
 *
 *  T = (0,100)
 *  R = {0,1,2,3,4,5}
 *
 */
void GenerateGenericTasks(LETModel& sample, unsigned int n, bool DiEqualTi, std::mt19937& gen) {

    std::vector<INTEGER_TIME_UNIT> T = {1,2,3,4,5,6,7,8,9,10};
    std::vector<TIME_UNIT> R = {0,1,2,3,4,5};
    GenerateTasks(sample, T, R, DiEqualTi, n, gen);
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

LETModel generate_LET (GeneratorRequest r) {
	auto seed = r.seed;
	if (seed == 0) {
        VERBOSE_WARNING("Experiment will not be reproducible");
		std::random_device rd;
		seed = rd();
	}

	auto n = r.n;
	auto m = r.m;
	auto kind = r.t;
    auto DiEqualTi = r.DiEqualTi;

	VERBOSE_DEBUG("N=" << n << " M=" << m << " Seed=" << seed);
	std::mt19937 gen(seed);
	LETModel sample ;

	switch (kind) {
		case unknown_dt:
			break;
		case generic_dt:
			GenerateGenericTasks(sample, n, DiEqualTi, gen) ;
			break;
		case harmonic_dt:
			GenerateHarmonicTasks(sample, n, DiEqualTi, gen) ;
			break;
		case automotive_dt:
			GenerateAutomotiveTasks(sample, n, DiEqualTi, gen) ;
			break;
	}

	GenerateUniformConnections(sample, m, gen);
	return sample;

}


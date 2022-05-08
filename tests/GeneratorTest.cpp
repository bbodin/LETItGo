/*
 * GeneratorTest.cpp
 *
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE GeneratorTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>
#include <benchmark.h>
#include <verbose.h>
#include <map>
#include <age_latency.h>

BOOST_AUTO_TEST_SUITE(test_generator)



BOOST_AUTO_TEST_CASE(test_GeneratorCacheEntry) {

	std::map<GeneratorRequest,int> cache;

    GeneratorRequest entry1 (2,3,4,LETDatasetType::automotive_dt);
    GeneratorRequest entry2 (2,3,4,LETDatasetType::automotive_dt);
    GeneratorRequest entry3 (3,3,4,LETDatasetType::automotive_dt);
    GeneratorRequest entry4 (3,3,4,LETDatasetType::generic_dt);

	BOOST_CHECK_EQUAL(entry1 , entry1);
	BOOST_CHECK_EQUAL(entry1 , entry2);

	BOOST_CHECK_NE(entry1 , entry3);
	BOOST_CHECK_NE(entry2 , entry3);
	BOOST_CHECK_NE(entry3 , entry4);

	BOOST_CHECK_LT(entry1 , entry3);
	BOOST_CHECK_LT(entry2 , entry3);

	cache[entry1] = 1;
	BOOST_CHECK_EQUAL(cache.size() , 1);
	BOOST_CHECK_EQUAL(cache[entry1] , 1);

	cache[entry2] = 2;
	BOOST_CHECK_EQUAL(cache.size() , 1);
	BOOST_CHECK_EQUAL(cache[entry1] , 2);
	BOOST_CHECK_EQUAL(cache[entry2] , 2);

	cache[entry3] = 3;
	BOOST_CHECK_EQUAL(cache.size() , 2);
	BOOST_CHECK_EQUAL(cache[entry1] , 2);
	BOOST_CHECK_EQUAL(cache[entry2] , 2);
	BOOST_CHECK_EQUAL(cache[entry3] , 3);


}






BOOST_AUTO_TEST_CASE(test_empty) {

	  LETModel sample_auto = generate_LET(GeneratorRequest(0,0,0,LETDatasetType::automotive_dt));

	  BOOST_CHECK_EQUAL(sample_auto.getTaskCount() , 0);
	  BOOST_CHECK_EQUAL(sample_auto.getDependencyCount() , 0);


	  LETModel sample_harmo = generate_LET(GeneratorRequest(0,0,0,LETDatasetType::harmonic_dt));

	  BOOST_CHECK_EQUAL(sample_harmo.getTaskCount() , 0);
	  BOOST_CHECK_EQUAL(sample_harmo.getDependencyCount() , 0);


	  LETModel sample_gen = generate_LET(GeneratorRequest(0,0,0,LETDatasetType::generic_dt));

	  BOOST_CHECK_EQUAL(sample_gen.getTaskCount() , 0);
	  BOOST_CHECK_EQUAL(sample_gen.getDependencyCount() , 0);

}

BOOST_AUTO_TEST_CASE(test_very_small) {

        LETModel sample = generate_LET(GeneratorRequest(1,0,0,LETDatasetType::automotive_dt));

  BOOST_CHECK_EQUAL(sample.getTaskCount() , 1);
  BOOST_CHECK_EQUAL(sample.getDependencyCount() , 0);

}


BOOST_AUTO_TEST_CASE(test_generate_random_periodicity_vector) {


        LETModel sample = generate_LET(GeneratorRequest(1,0,0,LETDatasetType::automotive_dt));

  BOOST_CHECK_EQUAL(sample.getTaskCount() , 1);
  BOOST_CHECK_EQUAL(sample.getDependencyCount() , 0);

  auto K = generate_random_periodicity_vector(sample);
  BOOST_CHECK_GT (K[0] , 0);

}



BOOST_AUTO_TEST_CASE(test_small) {

    LETModel sample = generate_LET(GeneratorRequest(3,2,0,LETDatasetType::automotive_dt));

  BOOST_CHECK_EQUAL(sample.getTaskCount() , 3);
  BOOST_CHECK_EQUAL(sample.getDependencyCount() , 2);

}

BOOST_AUTO_TEST_CASE(test_generator_small) {

  auto g = Generator::getInstance();
  LETModel sample = g.generate(GeneratorRequest(3,2, 123,automotive_dt));

  BOOST_CHECK_EQUAL(sample.getTaskCount() , 3);
  BOOST_CHECK_EQUAL(sample.getDependencyCount() , 2);

}
BOOST_AUTO_TEST_CASE(test_big) {

	int n = 300;
	int m = 200;

    LETModel sample = generate_LET(GeneratorRequest(n,m,0,LETDatasetType::automotive_dt));

	  BOOST_CHECK_EQUAL(sample.getTaskCount() , n);
	  BOOST_CHECK_EQUAL(sample.getDependencyCount() , m);

}


BOOST_AUTO_TEST_CASE(test_harmonic) {
	int n = 10;
	int m = 10;

        LETModel sample = generate_LET(GeneratorRequest(n,m,0,LETDatasetType::harmonic_dt));

	BOOST_CHECK_EQUAL(sample.getTaskCount() , n);
	BOOST_CHECK_EQUAL(sample.getDependencyCount() , m);

	for (Task ti : sample.tasks()) {
		for (Task tj : sample.tasks()) {

			if ( ti.getT() > tj.getT()) {
				BOOST_CHECK_EQUAL( ti.getT() % tj.getT()  , 0);
			}
		}
	}



}


BOOST_AUTO_TEST_CASE(test_generation) {

	int n = 3;
	int m = 2;
	int MAX_ITER = 400;

	std::map<INTEGER_TIME_UNIT,size_t> T_counts;
	std::map<Dependency, size_t> edge_counts;
	for (int i = 0 ; i < MAX_ITER ; i++) {
        LETModel sample = generate_LET(GeneratorRequest(n,m,0,LETDatasetType::automotive_dt));

		BOOST_CHECK_EQUAL(sample.getTaskCount() , n);
		BOOST_CHECK_EQUAL(sample.getDependencyCount() , m);

		VERBOSE_DEBUG(" ** SAMPLE ** ");
		for (Dependency d : sample.dependencies()) {
			VERBOSE_DEBUG(d);
			if (!edge_counts.count(d)) {
				edge_counts[d] = 0;
			}
			edge_counts[d]++;
		}

		for (Task t : sample.tasks()) {
			auto v = t.getT();
			if (!T_counts.count(v)) {
				T_counts[v] = 0;
			}
			T_counts[v]++;
		}

	}

	for (auto item : T_counts) {
		VERBOSE_DEBUG("H:" << item.first << " " << item.second);
	}

	VERBOSE_DEBUG(" ** RESULTS ** ");


	size_t total = MAX_ITER * m;
	size_t sum = 0 ;
	size_t quantity = 0 ;
	for (TASK_ID i = 0 ; i < n ; i++) {
		for (TASK_ID j = i + 1 ; j < n ; j++) {
			BOOST_CHECK_GT(edge_counts.count(Dependency(i,j)), 0);
			if (edge_counts.count(Dependency(i,j))) {
				sum += edge_counts[Dependency(i,j)];
				quantity ++;
				VERBOSE_DEBUG(Dependency(i,j) << " "<< (edge_counts[Dependency(i,j)]));
			}
		}
	}

	BOOST_CHECK_EQUAL(sum , total);
	BOOST_CHECK_EQUAL(quantity , n*(n - 1)/2);




}




BOOST_AUTO_TEST_CASE(test_fix1) {
	int n = 10;
	int m = 22;

        LETModel sample = generate_LET(GeneratorRequest(n,m,133,LETDatasetType::automotive_dt));
	PeriodicityVector K = generate_random_periodicity_vector(sample, 133);

	for (Task t : sample.tasks()) {
		auto tid = t.getId();
		BOOST_CHECK_GT(K[tid] , 0);
	}

}


BOOST_AUTO_TEST_CASE(test_fix2) {
	int n = 5;
	int m = 5;
	int seed = 130;

    LETModel sample = generate_LET(GeneratorRequest(n,m,seed,LETDatasetType::generic_dt));
	BOOST_CHECK_EQUAL(sample.tasks().size() , n);
	BOOST_CHECK_EQUAL(sample.dependencies().size() , m);
	std::cout << sample << std::endl;
	INTEGER_TIME_UNIT lcm = getLCM<INTEGER_TIME_UNIT>(sample);
	std::cout << "LCM=" << lcm << std::endl;

	ComputeAgeLatency(sample, generate_partial_constraint_graph);


}

    BOOST_AUTO_TEST_CASE(test_di_ti) {

        auto g = Generator::getInstance();
        LETModel sample = g.generate(GeneratorRequest(3,2, 123,automotive_dt,true));

        for (Task t : sample.tasks()) {
            BOOST_CHECK_EQUAL(t.getD(), t.getT());
        }

        LETModel samplebis = g.generate(GeneratorRequest(3,2, 123,automotive_dt,false));

        for (Task t : samplebis.tasks()) {
            BOOST_CHECK_NE(t.getD(), t.getT());
        }

        AgeLatencyResult res = ComputeAgeLatency(sample, generate_partial_constraint_graph);
        AgeLatencyResult resbis = ComputeAgeLatency(samplebis, generate_partial_constraint_graph);

        BOOST_CHECK_NE(res.age_latency, resbis.age_latency);
        std::cout << res << std::endl;
        std::cout << resbis << std::endl;

    }

BOOST_AUTO_TEST_SUITE_END()

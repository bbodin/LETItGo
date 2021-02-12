/*
 * GeneratorTest.cpp
 *
 */

#define BOOST_TEST_MODULE Figure1And2Test
#include <boost/test/included/unit_test.hpp>
#include <generator.h>
#include <map>

BOOST_AUTO_TEST_SUITE(test_generator)

BOOST_AUTO_TEST_CASE(test_empty) {

  LETModel sample = generate_LET(0,0);

  BOOST_CHECK_EQUAL(sample.getTaskCount() , 0);
  BOOST_CHECK_EQUAL(sample.getDependencyCount() , 0);

}

BOOST_AUTO_TEST_CASE(test_very_small) {

  LETModel sample = generate_LET(1,0);

  BOOST_CHECK_EQUAL(sample.getTaskCount() , 1);
  BOOST_CHECK_EQUAL(sample.getDependencyCount() , 0);

}

BOOST_AUTO_TEST_CASE(test_small) {

  LETModel sample = generate_LET(3,2);

  BOOST_CHECK_EQUAL(sample.getTaskCount() , 3);
  BOOST_CHECK_EQUAL(sample.getDependencyCount() , 2);

}

BOOST_AUTO_TEST_CASE(test_big) {

	int n = 300;
	int m = 200;

  LETModel sample = generate_LET(n,m);

  BOOST_CHECK_EQUAL(sample.getTaskCount() , n);
  BOOST_CHECK_EQUAL(sample.getDependencyCount() , m);

}




BOOST_AUTO_TEST_CASE(test_generation) {
	if (getenv("VERBOSE")) {
		commons::set_verbose_mode(atoi(getenv("VERBOSE")));
	}

	int n = 3;
	int m = 2;
	int MAX_ITER = 400;

	std::map<INTEGER_TIME_UNIT,size_t> T_counts;
	std::map<Dependency, size_t> edge_counts;
	for (int i = 0 ; i < MAX_ITER ; i++) {
		LETModel sample = generate_LET(n,m);

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

BOOST_AUTO_TEST_SUITE_END()

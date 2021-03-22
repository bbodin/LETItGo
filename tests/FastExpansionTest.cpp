/*
 * ExperimentalTest.cpp
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */





#define BOOST_TEST_MODULE FastExpansionTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>
#include <verbose.h>

BOOST_AUTO_TEST_SUITE(FastExpansionTest)



BOOST_AUTO_TEST_CASE(test_fast_graph_generation7) {

	/**
	 *
	 * This case has been problematic
     *
     *
     * Failed with: K =2 2 2 2 2
     * <LETModel>
     *  <Task id='0' r='5' C='100' D='100' T='100'>
     *  <Task id='1' r='1' C='50' D='50' T='50'>
     *  <Task id='2' r='0' C='5' D='5' T='5'>
     *  <Task id='3' r='0' C='10' D='10' T='10'>
     *  <Task id='4' r='1' C='5' D='5' T='5'>
     *  <Dependency from='1' to='4'>
     *  <Dependency from='0' to='4'>
     *  <Dependency from='2' to='4'>
     *  <Dependency from='2' to='3'>
     *  <Dependency from='1' to='2'>
     *  <Dependency from='0' to='3'>
     * </LETModel>
     *
	 *
	 */


	LETModel model;

	auto t0 = model.addTask(5,100);
	auto t1 = model.addTask(1,50);
	auto t2 = model.addTask(0,5);
	auto t3 = model.addTask(0,10);
	auto t4 = model.addTask(1,5);
	model.addDependency(t1, t4);
	model.addDependency(t0, t4);
	model.addDependency(t2, t4);
	model.addDependency(t2, t3);
	model.addDependency(t1, t2);
	model.addDependency(t0, t3);
	auto K = generate_periodicity_vector(model);
	auto verified = generate_partial_constraint_graph(model, K) ;
	auto new_version = new_generate_partial_constraint_graph(model, K) ;
	BOOST_REQUIRE_EQUAL(verified, new_version);

	auto SK = generate_periodicity_vector(model,2);
	utils::set_verbose_mode(10);
	utils::set_verbose_custom_mode("PCG", true);
	auto sverified = generate_partial_constraint_graph(model, SK) ;
	auto snew_version = new_generate_partial_constraint_graph(model, SK) ;
	utils::set_verbose_mode(0);
	BOOST_REQUIRE_EQUAL(sverified, snew_version);

}



BOOST_AUTO_TEST_CASE(test_fast_graph_generation6) {

	/**
	 *
	 * This case has been problematic
     *
     *    	<LETModel>
     *    	 <Task id='0' r='5' C='5' D='5' T='5'>
     *    	 <Task id='1' r='4' C='50' D='50' T='50'>
     *    	 <Dependency from='0' to='1'>
     *    </LETModel>
	 *
	 *
	 *
	 */


	LETModel model;

	auto t1 = model.addTask(5,5,5);
	auto t2 = model.addTask(4,50,50);
	model.addDependency(t1, t2);
	auto K = generate_periodicity_vector(model);
	auto verified = generate_partial_constraint_graph(model, K) ;
	auto new_version = new_generate_partial_constraint_graph(model, K) ;
	BOOST_REQUIRE_EQUAL(verified, new_version);

	auto SK = generate_periodicity_vector(model,9);
	utils::set_verbose_mode(10);
	utils::set_verbose_custom_mode("PCG", true);
	auto sverified = generate_partial_constraint_graph(model, SK) ;
	auto snew_version = new_generate_partial_constraint_graph(model, SK) ;
	utils::set_verbose_mode(0);
	BOOST_REQUIRE_EQUAL(sverified, snew_version);

}




BOOST_AUTO_TEST_CASE(test_fast_graph_generation5) {

	/**
	 *
	 * This case has been problematic
	 *
	 *<LETModel>
	 * <Task id='0' r='4' C='1' D='1' T='1'>
	 * <Task id='1' r='2' C='10' D='10' T='10'>
	 * <Dependency from='0' to='1'>
	 *</LETModel>

	 *
	 *
	 *
	 */


	LETModel model;

	auto t1 = model.addTask(4, 1, 1);
	auto t2 = model.addTask(2, 10, 10);
	model.addDependency(t1, t2);
	auto K = generate_periodicity_vector(model);
	auto verified = generate_partial_constraint_graph(model, K) ;
	auto new_version = new_generate_partial_constraint_graph(model, K) ;
	BOOST_REQUIRE_EQUAL(verified, new_version);

	auto K10 = generate_periodicity_vector(model,10);
	auto verified10 = generate_partial_constraint_graph(model, K10) ;
	auto new_version10 = new_generate_partial_constraint_graph(model, K10) ;
	BOOST_REQUIRE_EQUAL(verified10, new_version10);

}



BOOST_AUTO_TEST_CASE(test_fast_graph_generation4) {

	/**
	 *
	 * This case has been problematic with the fast generation, too many edges.
	 *
     * <LETModel>
     *  <Task id='0' r='4' C='20' D='20' T='20'>
     *  <Task id='1' r='1' C='5' D='5' T='5'>
     *  <Task id='2' r='1' C='20' D='20' T='20'>
     *  <Task id='3' r='3' C='50' D='50' T='50'>
     *  <Dependency from='1' to='3'>
     * </LETModel>
     *
	 *
	 *
	 *
	 */


	LETModel model = generate_LET(4,1, 123);
	auto K = generate_periodicity_vector(model);
	auto verified = generate_partial_constraint_graph(model, K) ;
	auto new_version = new_generate_partial_constraint_graph(model, K) ;
	BOOST_REQUIRE_EQUAL(verified, new_version);
}


BOOST_AUTO_TEST_CASE(test_fast_graph_generation3) {

	/**
	 *
	 * This case has been problematic with the fast generation, too many edges.
	 *
     * <LETModel>
     *  <Task id='0' r='1' C='5' D='5' T='5'>
     *  <Task id='1' r='3' C='50' D='50' T='50'>
     *  <Dependency from='0' to='1'>
     * </LETModel>
     *
	 *
	 *
	 *
	 */


	LETModel model;

	auto t1 = model.addTask(1, 5, 5);
	auto t2 = model.addTask(3, 50, 50);
	model.addDependency(t1, t2);
	auto K = generate_periodicity_vector(model);
	auto verified = generate_partial_constraint_graph(model, K) ;
	auto new_version = new_generate_partial_constraint_graph(model, K) ;
	BOOST_REQUIRE_EQUAL(verified, new_version);
}


BOOST_AUTO_TEST_CASE(test_fast_graph_generation2) {

	/**
	 *
	 * This case has been problematic with the fast generation, too many edges.
	 *
	 *  <LETModel>
	 *  	 <Task id='0' r='0' C='20' D='20' T='20'>
	 *  	 <Task id='1' r='4' C='100' D='100' T='100'>
	 *       <Dependency from='0' to='1'>
	 * 	</LETModel>
	 *
	 *
	 *
	 */


	LETModel model;

	auto t1 = model.addTask(0, 20, 20);
	auto t2 = model.addTask(4, 100, 100);
	model.addDependency(t1, t2);
	auto K = generate_periodicity_vector(model);
	auto verified = generate_partial_constraint_graph(model, K) ;
	auto new_version = new_generate_partial_constraint_graph(model, K) ;
	BOOST_REQUIRE_EQUAL(verified, new_version);

	auto K2 = generate_periodicity_vector(model,2);
	//utils::set_verbose_mode(10);
	//utils::set_verbose_custom_mode("PCG", true);
	auto verified2 = generate_partial_constraint_graph(model, K2) ;
	auto new_version2 = new_generate_partial_constraint_graph(model, K2) ;
	//utils::set_verbose_mode(0);
	BOOST_REQUIRE_EQUAL(verified2, new_version2);

}



BOOST_AUTO_TEST_CASE(test_fast_graph_generation1) {

	LETModel  model;
	TASK_ID t1 = model.addTask(0, 3, 4);
	TASK_ID t2 = model.addTask(1, 2, 3);
	model.addDependency(t1, t2);

	PeriodicityVector K = {1,1};
	auto verified = generate_partial_constraint_graph(model, K) ;

	auto new_version = new_generate_partial_constraint_graph(model, K) ;

	BOOST_REQUIRE_EQUAL(verified, new_version);

}




BOOST_AUTO_TEST_CASE(test_rosace_fast_graph) {


  auto rosace = new LETModel();

  TASK_ID t1 = rosace->addTask(0, 60, 60);
  TASK_ID t2 = rosace->addTask(0, 60, 60);
  TASK_ID t3 = rosace->addTask(0, 40, 40);
  TASK_ID t4 = rosace->addTask(0, 30, 30);
  TASK_ID t5 = rosace->addTask(0, 30, 30);
  TASK_ID t6 = rosace->addTask(0, 30, 30);

  rosace->addDependency(t1, t2);
  rosace->addDependency(t2, t3);
  rosace->addDependency(t3, t4);
  rosace->addDependency(t5, t3);
  rosace->addDependency(t6, t4);

  auto delay = ComputeAgeLatency(*rosace, new_generate_partial_constraint_graph);

  BOOST_REQUIRE_EQUAL(delay.age_latency , 240);

}


// Commented test are more complex


BOOST_AUTO_TEST_CASE(test_fast_graph_random) {
	return;
	size_t maxiter = 10000;
	size_t maxk = 10;
	for (size_t it = 0 ; it < maxiter ; it ++ ) {
		LETModel model = generate_LET(2,1, 123 + it);
		for (size_t k = 1; k <= maxk; k++) {
			auto K = generate_periodicity_vector(model,k);
			auto verified = generate_partial_constraint_graph(model, K) ;
			auto new_version = new_generate_partial_constraint_graph(model, K) ;
			if (verified != new_version) {
				std::cout << "Failed with: K =" << K << std::endl<< model << std::endl;
			}
			BOOST_REQUIRE_EQUAL(verified, new_version);
		}
	}
}





BOOST_AUTO_TEST_SUITE_END()

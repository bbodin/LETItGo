/*
 * ExperimentalTest.cpp
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */





#define BOOST_TEST_MODULE FastExpansionTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>

BOOST_AUTO_TEST_SUITE(FastExpansionTest)

BOOST_AUTO_TEST_CASE(test_fast_graph_generation) {

	commons::set_verbose_mode(0);


	LETModel  model;
	TASK_ID t1 = model.addTask(0, 3, 4);
	TASK_ID t2 = model.addTask(1, 2, 3);
	model.addDependency(t1, t2);

	PeriodicityVector K = {1,1};
	auto verified = generate_partial_constraint_graph(model, K) ;

	auto new_version = new_generate_partial_constraint_graph(model, K) ;

	BOOST_CHECK_EQUAL(verified, new_version);

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
	std::cout << model << std::endl;
	auto K = generate_unitary_periodicity_vector(model);
	auto verified = generate_partial_constraint_graph(model, K) ;
	commons::set_verbose_mode(10);
	auto new_version = new_generate_partial_constraint_graph(model, K) ;
	BOOST_CHECK_EQUAL(verified, new_version);
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
	std::cout << model << std::endl;
	auto K = generate_unitary_periodicity_vector(model);
	auto verified = generate_partial_constraint_graph(model, K) ;
	commons::set_verbose_mode(10);
	auto new_version = new_generate_partial_constraint_graph(model, K) ;
	BOOST_CHECK_EQUAL(verified, new_version);
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
	std::cout << model << std::endl;
	auto K = generate_unitary_periodicity_vector(model);
	auto verified = generate_partial_constraint_graph(model, K) ;
	commons::set_verbose_mode(10);
	auto new_version = new_generate_partial_constraint_graph(model, K) ;
	BOOST_CHECK_EQUAL(verified, new_version);
}





BOOST_AUTO_TEST_SUITE_END()

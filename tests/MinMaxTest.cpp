/*
 * MinMaxTest.cpp
 *
 *  Created on: Mar 4, 2021
 *      Author: toky
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MinMaxTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>
#include <verbose.h>

BOOST_AUTO_TEST_SUITE(test_figure1)

BOOST_AUTO_TEST_CASE(test_figure1_graph) {

	letitgo::utils::set_verbose_mode(10);
	letitgo::utils::set_verbose_custom_mode("PCG", true);
	VERBOSE_INFO("info works");
	VERBOSE_DEBUG("debug works");
	VERBOSE_PCG("debug works");


	LETModel figure1;
	TASK_ID t1 = figure1.addTask(0, 3, 4);
	TASK_ID t2 = figure1.addTask(1, 2, 3);
	figure1.addDependency(t1, t2);
	auto age_latency = compute_age_latency(figure1);
	VERBOSE_INFO ( " AGE LATENCY OF sample1 IS " << age_latency);


	auto K = generate_unitary_periodicity_vector(figure1);
	auto pcg = PartialConstraintGraph(figure1, K);
	auto ulp =  FindLongestPathUpper(pcg);
	auto llp =  FindLongestPathLower(pcg);
	VERBOSE_INFO("Upper/Lower bound graph" <<std::endl << pcg);
	VERBOSE_INFO("Upper bound path result" <<  ulp);
	VERBOSE_INFO("Lower bound path result" <<  llp);


	PeriodicityVector Kbis =  {3,4};
	auto pcgbis = PartialConstraintGraph(figure1, Kbis);
	auto ulpbis =  FindLongestPathUpper(pcgbis);
	auto llpbis =  FindLongestPathLower(pcgbis);
	VERBOSE_INFO("Upper/lower bound graph" <<std::endl << pcgbis);
	VERBOSE_INFO("Upper bound path result" <<  ulpbis);
	VERBOSE_INFO("Lower bound path result" <<  llpbis);

}

BOOST_AUTO_TEST_SUITE_END()

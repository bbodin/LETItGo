/*
 * ExperimentalTest.cpp
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */





#define BOOST_TEST_MODULE ExperimentalTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>

BOOST_AUTO_TEST_SUITE(ExperimentalTest)

BOOST_AUTO_TEST_CASE(test_FGG_performance) {

	commons::set_verbose_mode(0);

	size_t sample_count = 10;
	size_t min_n = 2, max_n = 20;

	for (size_t n = min_n ; n <= max_n ; n ++ ) {
		for (size_t m = 0 ; m <= (n * (n - 1)  / 2) ; m ++ ) {
			for (size_t i = 0 ; i < sample_count ; i ++ ) {



				LETModel model = generate_LET(n,m, 123 + i);
				auto K = generate_unitary_periodicity_vector(model);

				auto verified = generate_partial_constraint_graph(model, K) ;
				auto new_version = new_generate_partial_constraint_graph(model, K) ;

				std::cout << "generate_LET("<< n << ","<< m << ", "<< 123 + i << ");" << std::endl;
				std::cout << model << std::endl;
				BOOST_REQUIRE_EQUAL(verified, new_version);
			}


		}
	}
}



BOOST_AUTO_TEST_SUITE_END()

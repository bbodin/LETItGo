/*
 * LETModelTest.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: toky
 */


#define BOOST_TEST_MODULE LETModelTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>

BOOST_AUTO_TEST_SUITE(test_corner_cases)

BOOST_AUTO_TEST_CASE(test_equal) {
	 auto a1 = new LETModel();
	 a1->addTask(0, 3, 4);
	 a1->addTask(1, 2, 3);
	 a1->addDependency(0, 1);

	  auto a2 = new LETModel();
	  a2->addTask(0, 3, 4);
	  a2->addTask(1, 2, 3);
	  a2->addDependency(0, 1);

	  BOOST_CHECK_EQUAL(a1->tasks().size(), a2->tasks().size());
	  BOOST_CHECK_EQUAL(a1->dependencies().size(), a2->dependencies().size());


	  for (size_t i = 0 ; i < a1->tasks().size(); i++) {
		  BOOST_CHECK_EQUAL(a1->tasks()[i], a2->tasks()[i]);
	  }
	  for (size_t i = 0 ; i < a1->dependencies().size(); i++) {
		  BOOST_CHECK_EQUAL(a1->dependencies()[i], a2->dependencies()[i]);
	  }

	  BOOST_CHECK_EQUAL(*a1,*a2);

}
BOOST_AUTO_TEST_CASE(test_dep_from_unknow_task) {

  auto figure1 = new LETModel();
  figure1->addTask(0, 3, 4);
  figure1->addTask(1, 2, 3);

  bool error_occured = false;

  try {
	  figure1->addDependency(1, 2);
  } catch (...) {
	  error_occured = true;
  }

  BOOST_CHECK(error_occured);

}

BOOST_AUTO_TEST_SUITE_END()

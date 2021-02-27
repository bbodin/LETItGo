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

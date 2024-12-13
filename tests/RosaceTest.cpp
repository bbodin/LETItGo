/*
 * RosaceTest.cpp
 *
 */


#define BOOST_TEST_MODULE RosaceTest
#include <boost/test/unit_test.hpp>
#include <letitgo.h>
#include <benchmark.h>
#include <verbose.h>

BOOST_AUTO_TEST_SUITE(test_rosace)

BOOST_AUTO_TEST_CASE(test_rosace_delay) {

	letitgo::utils::set_verbose_mode(10);
	letitgo::utils::set_verbose_custom_mode("AGE_LATENCY", true);

	VERBOSE_INFO("INFO");
	VERBOSE_DEBUG("DEBUG");
	VERBOSE_AGE_LATENCY("AGE_LATENCY");

	auto rosace = new LETModel();

	TASK_ID t1 = rosace->addTask(0, 60, 60);
	TASK_ID t2 = rosace->addTask(0, 60, 60);
    TASK_ID t5 = rosace->addTask(0, 30, 30);
	TASK_ID t3 = rosace->addTask(0, 40, 40);
    TASK_ID t6 = rosace->addTask(0, 30, 30);
	TASK_ID t4 = rosace->addTask(0, 30, 30);

	rosace->addDependency(t1, t2);
	rosace->addDependency(t2, t3);
    rosace->addDependency(t5, t3);
	rosace->addDependency(t3, t4);
	rosace->addDependency(t6, t4);


	//std::cout << rosace->getXML() << std::endl;
	//std::cout << rosace->getDOT() << std::endl;
	std::string svg = rosace->getSVG();


	auto res = compute_age_latency(*rosace);
	//std::cout << res << std::endl;

	BOOST_CHECK_EQUAL(res.age_latency , 240);

    std::cout << rosace->getXML() << std::endl;

}

BOOST_AUTO_TEST_SUITE_END()

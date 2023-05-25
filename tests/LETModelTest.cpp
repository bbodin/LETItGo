/*
 * LETModelTest.cpp
 *
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LETModelTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>
#include <benchmark.h>
#include <verbose.h>

BOOST_AUTO_TEST_SUITE(test_letmodel)

    BOOST_AUTO_TEST_CASE(test_letmodel_basic) {

        letitgo::utils::set_verbose_mode(10);
        letitgo::utils::set_verbose_custom_mode("AGE_LATENCY", true);

        VERBOSE_INFO("INFO");
        VERBOSE_DEBUG("DEBUG");
        VERBOSE_AGE_LATENCY("AGE_LATENCY");

        auto a1 = new LETModel();
        a1->getSVG();
        TASK_ID t11 = a1->addTask(0, 3, 4);
        TASK_ID t12 = a1->addTask(1, 2, 3);
        a1->addDependency(t11, t12);

        auto a2 = new LETModel();
        TASK_ID t21 = a2->addTask(0, 3, 4);
        TASK_ID t22 = a2->addTask(1, 2, 3);
        a2->addDependency(t21, t22);

        BOOST_CHECK_EQUAL(a1->tasks().size(), a2->tasks().size());
        BOOST_CHECK_EQUAL(a1->dependencies().size(), a2->dependencies().size());


        for (size_t i = 0 ; i < a1->tasks().size(); i++) {
            BOOST_CHECK_EQUAL(a1->tasks()[i], a2->tasks()[i]);
        }
        for (size_t i = 0 ; i < a1->dependencies().size(); i++) {
            BOOST_CHECK_EQUAL(a1->dependencies()[i], a2->dependencies()[i]);
        }

        BOOST_CHECK_EQUAL(*a1,*a2);

        delete a1;
        delete a2;

        VERBOSE_INFO("ENd of the test.");

    }

BOOST_AUTO_TEST_SUITE_END()

/*
 * PCGTest.cpp
 *
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BenchmarkTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>
#include <map>
#include <chrono>
#include <benchmark.h>

BOOST_AUTO_TEST_SUITE(PCGTest)


BOOST_AUTO_TEST_CASE(test_constructor) {

    // LET Model
    LETModel model ;
    TASK_ID t11 = model.addTask(0, 3, 4);
    TASK_ID t12 = model.addTask(1, 2, 3);
    model.addDependency(t11, t12);

    // PeriodicityVector
    PeriodicityVector K = {1,1};

    // Generate PCG
    letitgo::utils::set_verbose_mode(10);
    PartialConstraintGraph pcg(model, K);
    BOOST_CHECK_EQUAL(4, pcg.getExecutions().size());
    BOOST_CHECK_EQUAL(3, pcg.getConstraints().size());
}

    BOOST_AUTO_TEST_CASE(test_string) {

        // LET Model
        LETModel model ;
        TASK_ID t11 = model.addTask(0, 3, 4);
        TASK_ID t12 = model.addTask(1, 2, 3);
        model.addDependency(t11, t12);

        // PeriodicityVector
        PeriodicityVector K = {1,1};

        // Generate PCG
        letitgo::utils::set_verbose_mode(10);
        PartialConstraintGraph pcg(model, K);
        std::cout << pcg << std::endl;
    }

    BOOST_AUTO_TEST_CASE(test_topological_order) {

        // LET Model
        LETModel model ;
        TASK_ID t11 = model.addTask(0, 3, 4);
        TASK_ID t12 = model.addTask(1, 2, 3);
        model.addDependency(t11, t12);

        // PeriodicityVector
        PeriodicityVector K = {1,1};

        // Generate PCG
        letitgo::utils::set_verbose_mode(10);
        PartialConstraintGraph pcg(model, K);
        std::cout << pcg << std::endl;

        auto order = pcg.getTopologicalOrder();
        std::cout << order << std::endl;


    }


    BOOST_AUTO_TEST_CASE(test_zero_task_case) {

        // LET Model
        LETModel model;

        // PeriodicityVector
        PeriodicityVector K;

        // Generate PCG
        letitgo::utils::set_verbose_mode(10);
        PartialConstraintGraph pcg(model, K);
        std::cout << pcg << std::endl;

        auto path = FindLongestPath(pcg, upper_wt);
        std::cout << path << std::endl;

    }



    BOOST_AUTO_TEST_CASE(test_output_dot) {

        // LET Model
        LETModel model ;
        TASK_ID t11 = model.addTask(0, 3, 4);
        TASK_ID t12 = model.addTask(1, 2, 3);
        model.addDependency(t11, t12);

        // PeriodicityVector
        PeriodicityVector K = {1,1};

        // Generate PCG
        letitgo::utils::set_verbose_mode(10);
        PartialConstraintGraph pcg(model, K);
        std::cout << pcg.getDOT(upper_wt) << std::endl;
    }

    BOOST_AUTO_TEST_CASE(test_output_svg) {

        // LET Model
        LETModel model ;
        TASK_ID t11 = model.addTask(0, 3, 4);
        TASK_ID t12 = model.addTask(1, 2, 3);
        model.addDependency(t11, t12);

        // PeriodicityVector
        PeriodicityVector K = {1,1};

        // Generate PCG
        letitgo::utils::set_verbose_mode(10);
        PartialConstraintGraph pcg(model, K);
        std::cout << pcg.getSVG(upper_wt) << std::endl;
    }





BOOST_AUTO_TEST_SUITE_END()

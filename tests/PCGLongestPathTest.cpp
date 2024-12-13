/*
 * PCGLongestPathTest.cpp
 *
 */

#define BOOST_TEST_MODULE BenchmarkTest
#include <boost/test/unit_test.hpp>
#include <letitgo.h>
#include <map>
#include <chrono>
#include <benchmark.h>

BOOST_AUTO_TEST_SUITE(PCGLongestPathTest)


    BOOST_AUTO_TEST_CASE(test_find_longest_path_upper) {

        // LET Model
        LETModel model ;
        TASK_ID t11 = model.addTask(0, 3, 4);
        TASK_ID t12 = model.addTask(1, 2, 3);
        model.addDependency(t11, t12);
        model.getSVG();
        // PeriodicityVector
        PeriodicityVector K = {1,1};

        // Generate PCG
        letitgo::utils::set_verbose_mode(10);
        PartialConstraintGraph pcg(model, K);
        std::cout << pcg << std::endl;

        auto path = FindLongestPath(pcg, upper_wt);
        std::cout << path << std::endl;

    }
    BOOST_AUTO_TEST_CASE(test_find_longest_path_lower) {

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

        auto path = FindLongestPath(pcg, lower_wt);
        std::cout << path << std::endl;

    }


    BOOST_AUTO_TEST_CASE(test_find_longest_path_lower_upper) {

        // LET Model
        LETModel model ;
        TASK_ID t0 = model.addTask(1, 9, 9);
        TASK_ID t1 = model.addTask(3, 9, 9);
        TASK_ID t2 = model.addTask(3, 3, 8);
        TASK_ID t3 = model.addTask(0, 1, 3);
        model.addDependency(t0, t2);
        model.addDependency(t1, t2);
        model.addDependency(t0, t3);

        // PeriodicityVector
        PeriodicityVector K = {1,1,1,1};

        // Generate PCG
        letitgo::utils::set_verbose_mode(10);
        PartialConstraintGraph pcg(model, K);
        std::cout << pcg << std::endl;

        auto path_low = FindLongestPath(pcg, lower_wt);
        auto path_up = FindLongestPath(pcg, upper_wt);
        std::cout << path_low << std::endl;
        std::cout << path_up << std::endl;
        BOOST_CHECK_LT(path_low.second, path_up.second);
    }




BOOST_AUTO_TEST_SUITE_END()

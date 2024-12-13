/*
 * StrangeTest.cpp
 *
 */

#define BOOST_TEST_MODULE StrangeTest
#include <boost/test/unit_test.hpp>
#include <letitgo.h>
#include <generator.h>
#include <verbose.h>
#include <partial_constraint_graph.h>

BOOST_AUTO_TEST_SUITE(test_strange)

    BOOST_AUTO_TEST_CASE(test_strange1) {

        GeneratorRequest r (
            4,3,812,generic_dt,false
                );
        auto sample = Generator::getInstance().generate(r);

        BOOST_CHECK_EQUAL(4, sample.getTaskCount());
        auto res = compute_age_latency(sample);
        std::cout << "Age latency = " << res.age_latency << std::endl;

        BOOST_CHECK_EQUAL(16, res.age_latency);

        std::cout << sample.getXML();
    }


    BOOST_AUTO_TEST_CASE(test_strange2) {

        GeneratorRequest r (
                4,3,3493,generic_dt,false
        );
        auto sample = Generator::getInstance().generate(r);

        BOOST_CHECK_EQUAL(4, sample.getTaskCount());
        auto res = compute_age_latency(sample);
        std::cout << "Age latency = " << res.age_latency << std::endl;

        BOOST_CHECK_EQUAL(15, res.age_latency);

        std::cout << sample.getXML();
    }


    BOOST_AUTO_TEST_CASE(test_strange3) {

        GeneratorRequest r (
                4,3,611,generic_dt,false
        );
        auto sample = Generator::getInstance().generate(r);

        BOOST_CHECK_EQUAL(4, sample.getTaskCount());
        auto res = compute_age_latency(sample);
        std::cout << "Age latency = " << res.age_latency << std::endl;

        BOOST_CHECK_EQUAL(20, res.age_latency);

        std::cout << sample.getXML();


	    PartialConstraintGraph pcg (sample, {1,1,1,1});

	    std::cout << pcg << std::endl;

	    // Bother orders are topological
	    
	    std::cout << pcg.getExecutions() << std::endl;
	    std::cout << pcg.getTopologicalOrder() << std::endl;
	
    }

BOOST_AUTO_TEST_SUITE_END()

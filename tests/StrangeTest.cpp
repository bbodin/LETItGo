/*
 * StrangeTest.cpp
 *
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE StrangeTest
#include <boost/test/included/unit_test.hpp>
#include <letitgo.h>
#include <generator.h>
#include <verbose.h>

BOOST_AUTO_TEST_SUITE(test_strange)

    BOOST_AUTO_TEST_CASE(test_strange1) {

        GeneratorRequest r (
            4,3,812,generic_dt,false
                );
        auto sample = Generator::getInstance().generate(r);

        BOOST_CHECK_EQUAL(4, sample.getTaskCount());
        auto res = ComputeAgeLatency(sample);
        std::cout << "Age latency = " << res.age_latency << std::endl;

        BOOST_CHECK_EQUAL(16, res.age_latency);

        std::cout << sample.getXML();
    }

BOOST_AUTO_TEST_SUITE_END()

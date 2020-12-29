/*
 * GeneratorTest.cpp
 *
 */

#define BOOST_TEST_MODULE Figure1And2Test
#include <boost/test/included/unit_test.hpp>
#include <model.h>
#include <letitgo.h>

BOOST_AUTO_TEST_SUITE (test_figure1)

BOOST_AUTO_TEST_CASE( test_figure1_graph )
{

	auto figure1 = new LETModel () ;
	TASK_ID t1 = figure1->addTask(0, 3, 4);
	TASK_ID t2 = figure1->addTask(1, 2, 3);
	figure1->addDependency(t1,t2);

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE (test_figure2)

BOOST_AUTO_TEST_CASE( test_figure2_graph )
{
	commons::set_verbose_mode(commons::DEBUG_LEVEL);

	LETModel * figure2 = new LETModel ();

	TASK_ID t1 = figure2->addTask(0, 1,   2);
	TASK_ID t2 = figure2->addTask(1, 0.5, 1);
	TASK_ID t3 = figure2->addTask(2, 4,   6);
	TASK_ID t4 = figure2->addTask(3, 3,   3);

	figure2->addDependency(t1,t2);
	figure2->addDependency(t2,t4);

	figure2->addDependency(t1,t3);
	figure2->addDependency(t3,t4);

	figure2->addDependency(t2,t3);

	PeriodicityVector K1 = generate_unitary_periodicity_vector(*figure2);
	PeriodicityVector K2 = {2,4,1,2};
	PartialConstraintGraph PG1 = generate_partial_constraint_graph (*figure2, K1);


	std::cout << PG1 << std::endl;


	PartialConstraintGraph PG2 = generate_partial_constraint_graph (*figure2, K2);
	std::cout << PG2 << std::endl;
	//assert (PG2.getConstraints().size() == 17);
	auto T = topologicalOrder(PG2);
	std::cout << "Topology: " << T  << std::endl;
	auto L = FindLongestPath(PG2);
	std::cout << "LongestPath: " << L << std::endl;


	std::cout << "Start ComputeAgeLatency"  << std::endl;
	auto delay = ComputeAgeLatency (*figure2) ;

	BOOST_TEST (delay == 12);

}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE (test_rosace)

BOOST_AUTO_TEST_CASE( test_rosace_delay )
{

	auto rosace = new LETModel () ;

	TASK_ID t1 = rosace->addTask(  0,  60,  60);
	TASK_ID t2 = rosace->addTask(  0,  60,  60);
	TASK_ID t3 = rosace->addTask(  0,  40,  40);
	TASK_ID t4 = rosace->addTask(  0,  30,  30);
	TASK_ID t5 = rosace->addTask(  0,  30,  30);
	TASK_ID t6 = rosace->addTask(  0,  30,  30);

	rosace->addDependency(t1,t2);
	rosace->addDependency(t2,t3);
	rosace->addDependency(t3,t4);
	rosace->addDependency(t5,t3);
	rosace->addDependency(t6,t4);

	auto delay = ComputeAgeLatency (*rosace) ;

	BOOST_TEST (delay == 240);
}

BOOST_AUTO_TEST_SUITE_END()



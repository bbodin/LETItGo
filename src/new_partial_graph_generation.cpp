/*
 * new_partial_graph_generation.cpp
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */


#include <partial_constraint_graph.h>
#include <utils.h>
#include <algorithm>
#include <numeric>
#include <cmath>


#define VERBOSE_PCG(m) VERBOSE_CUSTOM_DEBUG("PCG", m)


void take_this_ai_aj (const LETModel &model, const PeriodicityVector &K , const Dependency &d, EXECUTION_COUNT ai,  EXECUTION_COUNT aj, PartialConstraintGraph& graph) {
	TASK_ID ti_id = d.getFirst();
	TASK_ID tj_id = d.getSecond();

	Task ti = model.getTaskById(ti_id);
	Task tj = model.getTaskById(tj_id);

	INTEGER_TIME_UNIT Ti = ti.getT();
	INTEGER_TIME_UNIT Tj = tj.getT();

	auto Di = ti.getD();
	//auto Dj = tj.getD();

	auto ri = ti.getr();
	auto rj = tj.getr();

	EXECUTION_COUNT Ki = K[ti_id];
	EXECUTION_COUNT Kj = K[tj_id];

	INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);
	auto gcdeK = std::gcd(Ti * Ki, Tj * Kj);
	auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;



		VERBOSE_PCG("  "
				<< "from " << ai << " to " << aj);

		// recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

		INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
		INTEGER_TIME_UNIT pi_min =
				std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdeK);

		// From Theorem 6 (ECRTS2020)
		INTEGER_TIME_UNIT Lmax =
				rj - ri + Ti - Tj - (pi_min * gcdeK + alphae_ai_aj * gcdeT);

		Execution ei(ti_id, ai);
		Execution ej(tj_id, aj);
		Constraint cij(ei, ej, Lmax);
		graph.add(cij);


}


void take_this_ai (const LETModel &model, const PeriodicityVector &K , const Dependency &d, EXECUTION_COUNT ai, PartialConstraintGraph& graph) {
	TASK_ID ti_id = d.getFirst();
	TASK_ID tj_id = d.getSecond();

	Task ti = model.getTaskById(ti_id);
	Task tj = model.getTaskById(tj_id);

	INTEGER_TIME_UNIT Ti = ti.getT();
	INTEGER_TIME_UNIT Tj = tj.getT();

	auto Di = ti.getD();
	//auto Dj = tj.getD();

	auto ri = ti.getr();
	auto rj = tj.getr();

	EXECUTION_COUNT Ki = K[ti_id];
	EXECUTION_COUNT Kj = K[tj_id];

	INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);
	auto gcdeK = std::gcd(Ti * Ki, Tj * Kj);
	auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;


	for (auto aj = 1; aj <= Kj; aj++) {

		VERBOSE_PCG("  "
				<< "from " << ai << " to " << aj);

		// recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

		INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
		INTEGER_TIME_UNIT pi_min =
				std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdeK);

		// From Theorem 6 (ECRTS2020)
		INTEGER_TIME_UNIT Lmax =
				rj - ri + Ti - Tj - (pi_min * gcdeK + alphae_ai_aj * gcdeT);

		Execution ei(ti_id, ai);
		Execution ej(tj_id, aj);
		Constraint cij(ei, ej, Lmax);
		graph.add(cij);


	}
}


void take_this_aj (const LETModel &model, const PeriodicityVector &K , const Dependency &d, EXECUTION_COUNT aj, PartialConstraintGraph& graph) {

	TASK_ID ti_id = d.getFirst();
	TASK_ID tj_id = d.getSecond();

	Task ti = model.getTaskById(ti_id);
	Task tj = model.getTaskById(tj_id);

	INTEGER_TIME_UNIT Ti = ti.getT();
	INTEGER_TIME_UNIT Tj = tj.getT();

	auto Di = ti.getD();
	//auto Dj = tj.getD();

	auto ri = ti.getr();
	auto rj = tj.getr();

	EXECUTION_COUNT Ki = K[ti_id];
	EXECUTION_COUNT Kj = K[tj_id];

	INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);
	auto gcdeK = std::gcd(Ti * Ki, Tj * Kj);
	auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

	for (auto ai = 1; ai <= Ki; ai++) {

		VERBOSE_PCG("  "
				<< "from " << ai << " to " << aj);

		// recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

		INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
		INTEGER_TIME_UNIT pi_min =
				std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdeK);

		// From Theorem 6 (ECRTS2020)
		INTEGER_TIME_UNIT Lmax =
				rj - ri + Ti - Tj - (pi_min * gcdeK + alphae_ai_aj * gcdeT);

		Execution ei(ti_id, ai);
		Execution ej(tj_id, aj);
		Constraint cij(ei, ej, Lmax);
		graph.add(cij);

	}
}
void take_them_all (const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph) {

	TASK_ID ti_id = d.getFirst();
	TASK_ID tj_id = d.getSecond();

	Task ti = model.getTaskById(ti_id);
	Task tj = model.getTaskById(tj_id);

	INTEGER_TIME_UNIT Ti = ti.getT();
	INTEGER_TIME_UNIT Tj = tj.getT();

	auto Di = ti.getD();
	//auto Dj = tj.getD();

	auto ri = ti.getr();
	auto rj = tj.getr();

	EXECUTION_COUNT Ki = K[ti_id];
	EXECUTION_COUNT Kj = K[tj_id];

	INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);
	auto gcdeK = std::gcd(Ti * Ki, Tj * Kj);
	auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

	for (auto ai = 1; ai <= Ki; ai++) {
		for (auto aj = 1; aj <= Kj; aj++) {

			VERBOSE_PCG("  "
					<< "from " << ai << " to " << aj);

			// recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

			INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
			INTEGER_TIME_UNIT pi_min =
					std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdeK);

			// From Theorem 6 (ECRTS2020)
			INTEGER_TIME_UNIT Lmax =
					rj - ri + Ti - Tj - (pi_min * gcdeK + alphae_ai_aj * gcdeT);

			Execution ei(ti_id, ai);
			Execution ej(tj_id, aj);
			Constraint cij(ei, ej, Lmax);
			graph.add(cij);

		}
	}
}


#define VERBOSE_ALGO1(stream) VERBOSE_INFO("   Algorithm 1: " << stream)

void algorithm1(const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph, long x, double f0, double g0, long Tx,long Ty,long gcdk, long maxX , long maxY) {

	VERBOSE_ALGO1("Start algorithm 1 (x=" << x << ", f0=" << f0 << ", g0=" << g0 << ",  Tx=" << Tx << ", Ty=" << Ty << ", gcdk=" << gcdk << ", maxX=" << maxX << ",  maxY=" << maxY << ")");

	long g = std::gcd(gcdk,Ty);

	VERBOSE_ALGO1("g=" << g);

	double yg0 = (Tx * x - g0 * gcdk) / Ty;
	double yf0 = (Tx * x - f0 * gcdk) / Ty;
	//double Tyyg0 = (double) Ty * yg0;
	//double Tyyf0 = (double) Ty * yf0;

	double Tyyg0 = (Tx * x - g0 * gcdk);
	double Tyyf0 = (Tx * x - f0 * gcdk);

	long start = std::ceil ( Tyyg0 );
	long stop  = std::floor ( Tyyf0 );


	VERBOSE_ALGO1("yg0=" << yg0);
	VERBOSE_ALGO1("yf0=" << yf0);
	VERBOSE_ALGO1("Tyyg0=" << Tyyg0);
	VERBOSE_ALGO1("Tyyf0=" << Tyyf0);

	VERBOSE_ALGO1("Start loop from start=" << start << " to stop=" << stop);

	for (long theta = start ; theta <= stop; theta ++ ) {
		VERBOSE_ALGO1(" Iteration theta=" << theta);

		if (theta % g) continue;

		// theta divisible by g

		// Find (u0v0) solutions of uTy -vgcdk = tehta


		std::pair<long,long> res = extended_euclide ( Ty,  gcdk, theta);
		long u0 = res.first;
		long v0 = res.second;

		VERBOSE_ALGO1(" This theta is interesting run euclide (Ty=" << Ty << ",  gcdk=" << gcdk << ", theta=" << theta << ") = (u0,v0) = (" << u0 << ", " << v0 << ")");


		VERBOSE_ALGO1(" Start subloop from y=" << u0);
		for (long k = 0, y = u0 ; y <= maxY ; y = u0 + (++k * gcdk)/g) {
			VERBOSE_ALGO1("  Iteration k=" << k << " with x =" << x << " and y =" << y);
			if (y > 0 and y <= maxY) {
				VERBOSE_ALGO1("  Take this ai,aj ( ai=" << x << ", aj=" << y << " ) ");
			take_this_ai_aj (model, K , d, x, y,  graph);
			}
		}


	}

}



/**
 * Description of Algorithm 2:
 *
 *   g(x,y) = g(0,0) + (y*T_y - x*T_x)/gcd_K
 *   g(x,y) = f(0,0) + (y*T_y - x*T_x)/gcd_K
 *
 *   g(0,0) * gcd_K is INTEGER
 *   f(0,0) * gcd_K is INTEGER
 *
 *   g(0,0) >= f(0,0)
 *
 *   The algorithm returns the set S = {(x,y) in {1,..,K_x}*{1,..,K_y}, floor (g(x,y)) >= ceil (f(x,y))
 *
 *
 *   Now in the context of LET:
 *
 *   double f0, double g0,
 *   EXECUTION_COUNT Tx,EXECUTION_COUNT Ty,
 *   EXECUTION_COUNT gcdk,
 *   EXECUTION_COUNT maxX, EXECUTION_COUNT maxY
 *
 *
 *
 */

void algorithm2(const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph ) {

	VERBOSE_INFO("Algorithm 2 Starts");


	TASK_ID ti_id = d.getFirst();
	TASK_ID tj_id = d.getSecond();

	Task ti = model.getTaskById(ti_id);
	Task tj = model.getTaskById(tj_id);

	INTEGER_TIME_UNIT Ti = ti.getT();
	INTEGER_TIME_UNIT Tj = tj.getT();

	auto Di = ti.getD();
	//auto Dj = tj.getD();

	auto ri = ti.getr();
	auto rj = tj.getr();


	EXECUTION_COUNT Ki = K[ti_id];
	EXECUTION_COUNT Kj = K[tj_id];


	EXECUTION_COUNT maxX = Ki;
	EXECUTION_COUNT maxY = Kj;



	INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);
	auto gcdeK = std::gcd(Ti * Ki, Tj * Kj);
	auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;
	INTEGER_TIME_UNIT gcdk = gcdeT;

	//std::cout << "# dependence \"" <<  d << "\" between \"" <<  ti_id << "\" and \"" <<  tj_id << "\"" << std::endl;#
	//std::cout << "ai,aj,shift,fxy,gxy,pibmin,pibmax,w" << std::endl;



	// recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

	// recall: INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
	// recall: INTEGER_TIME_UNIT pi_min =				std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdeK);
	// recall: INTEGER_TIME_UNIT pi_max =				std::floor((-Me + Ti - alphae_ai_aj * gcdeT) / gcdeK);


	// By definition f00gcdz and g00gcdz are devisible by gcdz
	EXECUTION_COUNT f0gcdk = gcdeT - Me;
	EXECUTION_COUNT g0gcdk = Ti - Me ;

	EXECUTION_COUNT Tx = Ti;
	EXECUTION_COUNT Ty = Tj;

	double f0 =  (double) f0gcdk / (double) gcdeK;
	double g0 =  (double) g0gcdk / (double) gcdeK;

	VERBOSE_INFO("Tx=" << Tx << " Ty=" << Ty << "");

	VERBOSE_INFO("f0gcdk=" << f0gcdk << " g0gcdk=" << g0gcdk << " gcdeK=" << gcdeK << "");
	VERBOSE_INFO("f0=" << f0 << " g0=" << g0 << "");

	//VERBOSE_ASSERT ((f0gcdk % gcdk) == 0, "f0 Must be integer and here f0gcdk=" << f0gcdk << " with gcdk=" << gcdk << " that is (f0gcdk % gcdk)=" << f0gcdk % gcdk);
	//VERBOSE_ASSERT ((g0gcdk % gcdk) == 0, "g0 Must be integer and here g0gcdk=" << g0gcdk << " with gcdk=" << gcdk << " that is (g0gcdk % gcdk)=" << g0gcdk % gcdk);
	VERBOSE_ASSERT (f0 <= g0,   "f0 Must be less or equal to g0");


	// Algorithm 2
	if (g0 >= 1 + f0) {
		// Take them all
		VERBOSE_INFO (" Case 1 : Take them all");
		take_them_all (model, K , d, graph);
	} else if (Ty == gcdk) {

		VERBOSE_INFO (" Case 2 : Ty == gcdk");
		for (auto x = 1; x <= maxX ; x++ ) {
			VERBOSE_INFO ("  Test x =" << x);
			double shift = ((double) ( Tx * x ) /  (double) gcdk);

			double gx0 =  (double) g0 +  (double) shift;
			double fx0 =  (double) f0 +  (double) shift;

			long floorgx0 = (long) std::floor(gx0);
			long ceilfx0 = (long) std::ceil(fx0);

			if (floorgx0 == ceilfx0) {
				// Take (x,y) for every y
				VERBOSE_INFO ("   Take (x,y) for every y");
				take_this_ai (model, K , d, x, graph);
			} else {
				VERBOSE_INFO ("   Skip it");

			}
		}
	} else {

		VERBOSE_INFO (" Case 3 : algorithm 1");

		for (EXECUTION_COUNT x = 1; x <= maxX ; x++ ) {

			VERBOSE_INFO ("  Run algorithm 1 with x =" << x);

			// Algorithm 1
			algorithm1(model, K , d, graph, x, f0,  g0,  Tx, Ty, gcdk,  maxX,  maxY);

		}


	}

}



PartialConstraintGraph
new_generate_partial_constraint_graph(const LETModel &model,
		const PeriodicityVector &K) {

	PartialConstraintGraph graph;

	VERBOSE_DEBUG("1) Create constraints.");
	for (Dependency d : model.dependencies()) {
		VERBOSE_DEBUG(" Run Algorithm 2 for d=" << d);
		algorithm2(model, K , d, graph) ;
	}

	VERBOSE_DEBUG("2) Constraints done, add start and finish.");
	add_start_finish (model, K, graph) ;
	return graph;
}


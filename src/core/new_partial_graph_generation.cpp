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

#define VERBOSE_NPCG(m) VERBOSE_CUSTOM_DEBUG("NPCG", m)
#define VERBOSE_ALGO1(stream) VERBOSE_NPCG("   Algorithm 1: " << stream)

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




		// recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

		INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
		INTEGER_TIME_UNIT pi_min =
				std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdeK);

		// From Theorem 6 (ECRTS2020)
		INTEGER_TIME_UNIT Lmax =
				rj - ri + Ti - Tj - (pi_min * gcdeK + alphae_ai_aj * gcdeT);

		VERBOSE_NPCG ("    (" <<  ai <<  "," <<  aj <<  ") =" << Lmax);
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


		// recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

		INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
		INTEGER_TIME_UNIT pi_min =
				std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdeK);

		// From Theorem 6 (ECRTS2020)
		INTEGER_TIME_UNIT Lmax =
				rj - ri + Ti - Tj - (pi_min * gcdeK + alphae_ai_aj * gcdeT);

		VERBOSE_NPCG ("    (" <<  ai <<  "," <<  aj <<  ") =" << Lmax);
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

		// recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

		INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
		INTEGER_TIME_UNIT pi_min =
				std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdeK);

		// From Theorem 6 (ECRTS2020)
		INTEGER_TIME_UNIT Lmax =
				rj - ri + Ti - Tj - (pi_min * gcdeK + alphae_ai_aj * gcdeT);


		VERBOSE_NPCG ("    (" <<  ai <<  "," <<  aj <<  ") =" << Lmax);
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



			// recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

			INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
			INTEGER_TIME_UNIT pi_min =
					std::ceil( (-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdeK );

			// From Theorem 6 (ECRTS2020)
			INTEGER_TIME_UNIT Lmax =
					rj - ri + Ti - Tj - (pi_min * gcdeK + alphae_ai_aj * gcdeT);

			VERBOSE_NPCG ("    pi_min=" << pi_min);
			VERBOSE_NPCG ("    (" <<  ai <<  "," <<  aj <<  ") =" << Lmax);

			Execution ei(ti_id, ai);
			Execution ej(tj_id, aj);
			Constraint cij(ei, ej, Lmax);
			graph.add(cij);

		}
	}
}



void algorithm1(const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph, long x, double f0, double g0, long f0gcdK, long g0gcdK,  long Tx,long Ty,long gcdK, long maxX , long maxY) {

	VERBOSE_ALGO1("Start algorithm 1 (x=" << x << ", f0=" << f0 << ", g0=" << g0 << ",  Tx=" << Tx << ", Ty=" << Ty << ", gcdK=" << gcdK << ", maxX=" << maxX << ",  maxY=" << maxY << ")");

	long g = std::gcd(gcdK,Ty);

	VERBOSE_ALGO1("g=" << g);

	VERBOSE_ALGO1(" Tx:" << Tx
			<< " x:" << x
			<< " g0:" << g0
			<< " f0:" << f0
			<< " g0gcdK:" << g0gcdK
			<< " f0gcdK:" << f0gcdK
			<< " gcdK:" << gcdK
			<< " Ty:" << Ty);

	double yg0 = (Tx * x - g0gcdK) / Ty;
	double yf0 = (Tx * x - f0gcdK) / Ty;
	//double Tyyg0 = (double) Ty * yg0;
	//double Tyyf0 = (double) Ty * yf0;

	double Tyyg0 = (Tx * x - g0gcdK);
	double Tyyf0 = (Tx * x - f0gcdK);

	long start = std::ceil ( Tyyg0 );
	long stop  = std::floor ( Tyyf0 );


	VERBOSE_ALGO1("yg0=" << yg0);
	VERBOSE_ALGO1("yf0=" << yf0);
	VERBOSE_ALGO1("Tyyg0=" << Tyyg0);
	VERBOSE_ALGO1("Tyyf0=" << Tyyf0);
	VERBOSE_ALGO1("start=" << start);
	VERBOSE_ALGO1("stop=" << stop);

	VERBOSE_ALGO1("Start loop from start=" << start << " to stop=" << stop);

	for (long theta = start ; theta <= stop; theta ++ ) {
		VERBOSE_ALGO1(" Iteration theta=" << theta);

		if (theta % g) {
			VERBOSE_ALGO1(" Skip: theta is not divisble by g = " << g);
			continue;
		}

		// theta divisible by g

		// Find (u0v0) solutions of uTy -vgcdk = tehta


		std::pair<long,long> res = extended_euclide ( Ty,  gcdK, theta);
		long u0 = res.first;
		long v0 = res.second;

		VERBOSE_ALGO1(" Good Theta: euclide (Ty=" << Ty << " gcdK=" << gcdK << " theta=" << theta << ") = (u0,v0) = (" << u0 << "," << v0 << ")");


		long step = (gcdK)/g;
		long y0 = u0 - step * std::ceil(u0/step);
		VERBOSE_ALGO1(" Start subloop from y=" << y0 << " to " << maxY << " with increment of " << gcdK<< "/" <<g << "=" << (gcdK)/g );
		for (long y = y0 ; y <= maxY ; y += step ) {


			VERBOSE_ALGO1("  Iteration with x =" << x << " and y =" << y);



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

	VERBOSE_NPCG("Algorithm 2 Starts ");


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



//INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);
	auto gcdT = std::gcd(Ti, Tj);
	auto gcdK = std::gcd(Ti * Ki, Tj * Kj);
	auto Me = Tj + std::ceil((ri - rj + Di) / gcdT) * gcdT;


	// By definition f00gcdz and g00gcdz are divisible by gcdz
	EXECUTION_COUNT f0gcdk = gcdT - Me;
	EXECUTION_COUNT g0gcdk = Ti - Me ;

	EXECUTION_COUNT Tx = Ti;
	EXECUTION_COUNT Ty = Tj;

	double f0 =  (double) f0gcdk / (double) gcdK;
	double g0 =  (double) g0gcdk / (double) gcdK;

	VERBOSE_NPCG("Tx=" << Tx << " Ty=" << Ty << "");
	VERBOSE_NPCG("Ki=" << Ki << " Kj=" << Kj << "");
	VERBOSE_NPCG("gcdT=" << gcdT << " gcdK=" << gcdK << "");


	VERBOSE_NPCG("Me=" << Me << "");


	VERBOSE_NPCG("f0gcdk=" << f0gcdk << " g0gcdk=" << g0gcdk << "");
	VERBOSE_NPCG("f0=" << f0 << " g0=" << g0 << "");

	//VERBOSE_ASSERT ((f0gcdk % gcdk) == 0, "f0 Must be integer and here f0gcdk=" << f0gcdk << " with gcdk=" << gcdk << " that is (f0gcdk % gcdk)=" << f0gcdk % gcdk);
	//VERBOSE_ASSERT ((g0gcdk % gcdk) == 0, "g0 Must be integer and here g0gcdk=" << g0gcdk << " with gcdk=" << gcdk << " that is (g0gcdk % gcdk)=" << g0gcdk % gcdk);
	VERBOSE_ASSERT (f0 <= g0,   "f0 Must be less or equal to g0");


	// Algorithm 2
	if (g0 >= 1 + f0) {
		// Take them all
		VERBOSE_NPCG (" Case 1 : Take them all");
		take_them_all (model, K , d, graph);
	} else if (Ty == gcdK) {

		VERBOSE_NPCG (" Case 2 : Ty == gcdK");
		VERBOSE_NPCG ("  g0=" << g0 << " f0=" << f0 << " Tx=" << Tx << " gcdK=" << gcdK);
		for (auto x = 1; x <= maxX ; x++ ) {
			VERBOSE_NPCG ("  Test x =" << x);
			double shift = ((double) ( - Tx * x ) /  (double) gcdK);

			double gx0 =  (double) g0 +  (double) shift;
			double fx0 =  (double) f0 +  (double) shift;

			VERBOSE_NPCG ("   shift = " << shift << " gx0=" << gx0 << " fx0=" << fx0);

			long floorgx0 = (long) std::floor(gx0);
			long ceilfx0 = (long) std::ceil(fx0);

			VERBOSE_NPCG ("   floorgx0=" << floorgx0 << " ceilfx0=" << ceilfx0);


			if (floorgx0 == ceilfx0) {
				// Take (x,y) for every y
				VERBOSE_NPCG ("    Take (x,y) for every y");
				take_this_ai (model, K , d, x, graph);
			} else {
				VERBOSE_NPCG ("    Skip it");

			}
		}
	} else {

		VERBOSE_NPCG (" Case 3 : algorithm 1");

		for (EXECUTION_COUNT x = 1; x <= maxX ; x++ ) {

			VERBOSE_NPCG ("  Run algorithm 1 with x =" << x);

			// Algorithm 1
			algorithm1(model, K , d, graph, x, f0,  g0,  f0gcdk,  g0gcdk,  Tx, Ty, gcdK,  maxX,  maxY);

		}


	}

}



PartialConstraintGraph
new_generate_partial_constraint_graph(const LETModel &model,
		const PeriodicityVector &K) {

	PartialConstraintGraph graph;

	VERBOSE_NPCG("1) Create constraints.");
	for (Dependency d : model.dependencies()) {
		VERBOSE_NPCG(" Run Algorithm 2 for d=" << d);
		algorithm2(model, K , d, graph) ;
	}

	VERBOSE_NPCG("2) Constraints done, add start and finish.");
	add_start_finish (model, K, graph) ;
	return graph;
}


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

#ifdef ULTRA_DEBUG
#define VERBOSE_NPCG(m) VERBOSE_CUSTOM_DEBUG("NPCG", m)
#define VERBOSE_ALGO1(stream) VERBOSE_NPCG("   Algorithm 1: " << stream)
#else
#define VERBOSE_NPCG(m) {}
#define VERBOSE_ALGO1(stream) {}
#endif


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


template <typename entier>
entier opt_extended_euclide (entier _a, entier _b, entier _c) {

	// Kuṭṭaka, Aryabhata's algorithm for solving linear Diophantine equations in two unknowns
	const entier gcdab = std::gcd(_a,_b) ;

	const entier a = _a/ gcdab;
	const entier b = _b/ gcdab;
	const entier c = _c/ gcdab;

	std::pair<entier,entier> r (a,b);
	std::pair<entier,entier> s (1,0);

    while (r.second != 0) {
    	const entier quotient = r.first / r.second;
    	r = std::pair<entier,entier> ( r.second ,  r.first - quotient * r.second );
    	s = std::pair<entier,entier> ( s.second ,  s.first - quotient * s.second );
    }

    return c * s.first ;

}

void new_algorithm2(const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph ) {


	VERBOSE_NPCG("Algorithm 2 Starts ");

	const TASK_ID ti_id = d.getFirst();
	const TASK_ID tj_id = d.getSecond();

	const Task ti = model.getTaskById(ti_id);
	const Task tj = model.getTaskById(tj_id);

	const INTEGER_TIME_UNIT Ti = ti.getT();
	const INTEGER_TIME_UNIT Tj = tj.getT();

	const auto Di = ti.getD();
	const auto ri = ti.getr();
	const auto rj = tj.getr();

	const EXECUTION_COUNT Ki = K[ti_id];
	const EXECUTION_COUNT Kj = K[tj_id];

	const EXECUTION_COUNT maxX = Ki;
	const EXECUTION_COUNT maxY = Kj;

	const INTEGER_TIME_UNIT gcdT = std::gcd(Ti, Tj);
	const auto gcdK = std::gcd(Ti * Ki, Tj * Kj);
	const auto Me = Tj + std::ceil((ri - rj + Di) / gcdT) * gcdT;

	// By definition f00gcdz and g00gcdz are devisible by gcdz
	const EXECUTION_COUNT f0gcdk = gcdT - Me;
	const EXECUTION_COUNT g0gcdk = Ti - Me ;

	const EXECUTION_COUNT Tx = Ti;
	const EXECUTION_COUNT Ty = Tj;


	VERBOSE_NPCG("Tx=" << Tx << " Ty=" << Ty << "");
	VERBOSE_NPCG("Ki=" << Ki << " Kj=" << Kj << "");
	VERBOSE_NPCG("gcdT=" << gcdT << " gcdK=" << gcdK << "");


	VERBOSE_NPCG("Me=" << Me << "");


	VERBOSE_NPCG("f0gcdk=" << f0gcdk << " g0gcdk=" << g0gcdk << "");


	//const double f0 =  (double) f0gcdk / (double) gcdK;
	//const double g0 =  (double) g0gcdk / (double) gcdK;

	const EXECUTION_COUNT g = std::gcd(gcdK,Ty);

	const EXECUTION_COUNT rjmripTimTj = rj - ri + Ti - Tj;


	// Algorithm 2
	if (g0gcdk >= gcdK + f0gcdk) {
		// Take them all
		VERBOSE_NPCG (" Case 1 : Take them all");
		Algorithm2_statistics::getSingleton().total_case1++;


		for (auto ai = 1; ai <= Ki; ai++) {

			const Execution ei(ti_id, ai);

			for (auto aj = 1; aj <= Kj; aj++) {



				// recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

				INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdT;
				INTEGER_TIME_UNIT pi_min =
						std::ceil( (-Me + gcdT - alphae_ai_aj * gcdT) / gcdK );

				// From Theorem 6 (ECRTS2020)
				INTEGER_TIME_UNIT Lmax =
						rjmripTimTj - (pi_min * gcdK + alphae_ai_aj * gcdT);

				VERBOSE_NPCG ("    pi_min=" << pi_min);
				VERBOSE_NPCG ("    (" <<  ai <<  "," <<  aj <<  ") =" << Lmax);

				Execution ei(ti_id, ai);
				Execution ej(tj_id, aj);
				Constraint cij(ei, ej, Lmax);
				graph.add(cij);

			}
		}
	} else if (Ty == gcdK) {

		VERBOSE_NPCG (" Case 2 : Ty == gcdK");
		Algorithm2_statistics::getSingleton().total_case2++;


		VERBOSE_NPCG ("  g0=NA f0=NA Tx=" << Tx << " gcdK=" << gcdK);

		for (auto x = 1; x <= maxX ; x++ ) {
			VERBOSE_NPCG ("  Test x =" << x);
			const double shift = ((double) ( - Tx * x ) /  (double) gcdK);

			const double gx0 =  ((double) g0gcdk / (double) gcdK) +  (double) shift;
			const double fx0 =  ((double) f0gcdk / (double) gcdK) +  (double) shift;

			VERBOSE_NPCG ("   shift = " << shift << " gx0=" << gx0 << " fx0=" << fx0);
			const long floorgx0 = (long) std::floor(gx0);
			const long ceilfx0 = (long) std::ceil(fx0);
			VERBOSE_NPCG ("   floorgx0=" << floorgx0 << " ceilfx0=" << ceilfx0);

			if (floorgx0 == ceilfx0) {
				VERBOSE_NPCG ("    Take (x,y) for every y");
				// Take (x,y) for every y
				const EXECUTION_COUNT ai = x;

				const Execution ei(ti_id, ai);
				const EXECUTION_COUNT alphae_ai_ajgcdeTaiside = (Ti * ai);
				for (auto aj = 1; aj <= Kj; aj++) {

					const EXECUTION_COUNT alphae_ai_ajgcdeT = (alphae_ai_ajgcdeTaiside - Tj * aj);
					const INTEGER_TIME_UNIT pi_min =
							std::ceil((-Me + gcdT -alphae_ai_ajgcdeT) / gcdK);

					// From Theorem 6 (ECRTS2020)
					const INTEGER_TIME_UNIT Lmax =
							rjmripTimTj - (pi_min * gcdK + alphae_ai_ajgcdeT);


					VERBOSE_NPCG ("    (" <<  ai <<  "," <<  aj <<  ") =" << Lmax);

					const Execution ej(tj_id, aj);
					const Constraint cij(ei, ej, Lmax);
					graph.add(cij);


				}
			} else {

				VERBOSE_NPCG ("    Skip it");
			}
		}
	} else {

		VERBOSE_NPCG (" Case 3 : algorithm 1");
		Algorithm2_statistics::getSingleton().total_case3++;

		long step = (gcdK)/g;
		for (EXECUTION_COUNT x = 1; x <= maxX ; x++ ) {
			VERBOSE_NPCG ("  Run algorithm 1 with x =" << x);
			VERBOSE_ALGO1("Start algorithm 1 (x=" << x << ", f0=NA, g0=NA,  Tx=" << Tx << ", Ty=" << Ty << ", gcdK=" << gcdK << ", maxX=" << maxX << ",  maxY=" << maxY << ")");


			VERBOSE_ALGO1("g=" << g);

			VERBOSE_ALGO1(" Tx:" << Tx
					<< " x:" << x
					<< " g0:NA"
					<< " f0:NA"
					<< " g0gcdK:" << g0gcdk
					<< " f0gcdK:" << f0gcdk
					<< " gcdK:" << gcdK
					<< " Ty:" << Ty);

			const EXECUTION_COUNT ai = x;
			const Execution ei(ti_id, ai);
			const EXECUTION_COUNT alphae_ai_ajgcdeTaiside = (Ti * ai);

			const double Tyyg0 = (Tx * x - g0gcdk);
			const double Tyyf0 = (Tx * x - f0gcdk);

			const long start = std::ceil ( Tyyg0 );
			const long stop  = std::floor ( Tyyf0 );


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

				const long u0 = opt_extended_euclide ( Ty,  gcdK, theta);
				VERBOSE_ALGO1(" Good Theta: euclide (Ty=" << Ty << " gcdK=" << gcdK << " theta=" << theta << ") = (u0,v0) = (" << u0 << ",-)");

				long y0 = u0 - step * std::ceil(u0/step);
				VERBOSE_ALGO1(" Start subloop from y=" << y0 << " to " << maxY << " with increment of " << gcdK<< "/" <<g << "=" << (gcdK)/g );


				for (long y = y0 ; y <= maxY ; y += step ) {
					VERBOSE_ALGO1("  Iteration with x =" << x << " and y =" << y);

					if (y > 0 and y <= maxY) {
						VERBOSE_ALGO1("  Take this ai,aj ( ai=" << x << ", aj=" << y << " ) ");

						const EXECUTION_COUNT aj = y;

						const EXECUTION_COUNT alphae_ai_ajgcdeT = (alphae_ai_ajgcdeTaiside - Tj * aj);
						const INTEGER_TIME_UNIT pi_min =
									std::ceil((-Me + gcdT - alphae_ai_ajgcdeT) / gcdK);

							// From Theorem 6 (ECRTS2020)
						const INTEGER_TIME_UNIT Lmax =
									rjmripTimTj - (pi_min * gcdK + alphae_ai_ajgcdeT);

						VERBOSE_NPCG ("    (" <<  ai <<  "," <<  aj <<  ") =" << Lmax);
						const Execution ej(tj_id, aj);
						const Constraint cij(ei, ej, Lmax);
						graph.add(cij);

					}
				}


			}


		}


	}
}



PartialConstraintGraph
opt_new_generate_partial_constraint_graph(const LETModel &model,
		const PeriodicityVector &K) {

    long sumK;
    for(auto it : K) {
        sumK += it;
    }
    PartialConstraintGraph graph (2 + sumK);

	VERBOSE_NPCG("1) Create constraints.");
	for (Dependency d : model.dependencies()) {
		VERBOSE_NPCG(" Run Algorithm 2 for d=" << d);
		new_algorithm2(model, K , d, graph) ;
	}

	VERBOSE_NPCG("2) Constraints done, add start and finish.");
	add_start_finish (model, K, graph) ;
	return graph;
}


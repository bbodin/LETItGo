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

void new_algorithm2(const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph ) {

	VERBOSE_INFO("Algorithm 2 Starts");

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

	const INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);
	const auto gcdeK = std::gcd(Ti * Ki, Tj * Kj);
	const auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;
	const INTEGER_TIME_UNIT gcdk = gcdeT;

	// By definition f00gcdz and g00gcdz are devisible by gcdz
	const EXECUTION_COUNT f0gcdk = gcdeT - Me;
	const EXECUTION_COUNT g0gcdk = Ti - Me ;

	const EXECUTION_COUNT Tx = Ti;
	const EXECUTION_COUNT Ty = Tj;

	const double f0 =  (double) f0gcdk / (double) gcdeK;
	const double g0 =  (double) g0gcdk / (double) gcdeK;

	const EXECUTION_COUNT g = std::gcd(gcdk,Ty);

	const EXECUTION_COUNT gcdeTmMe = -Me + gcdeT ;
	const EXECUTION_COUNT rjmripTimTj = rj - ri + Ti - Tj;


	// Algorithm 2
	if (g0 >= 1 + f0) {
		// Take them all

		for (auto ai = 1; ai <= Ki; ai++) {
			const Execution ei(ti_id, ai);
			const EXECUTION_COUNT alphae_ai_ajgcdeTaiside = (Ti * ai);

			for (auto aj = 1; aj <= Kj; aj++) {

				const EXECUTION_COUNT alphae_ai_ajgcdeT = (alphae_ai_ajgcdeTaiside - Tj * aj);
				const INTEGER_TIME_UNIT pi_min =
						std::ceil((gcdeTmMe - alphae_ai_ajgcdeT) / gcdeK);

				// From Theorem 6 (ECRTS2020)
				const INTEGER_TIME_UNIT Lmax =
						rjmripTimTj - (pi_min * gcdeK + alphae_ai_ajgcdeT);

				const Execution ej(tj_id, aj);
				const Constraint cij(ei, ej, Lmax);
				graph.add(cij);

			}
		}
	} else if (Ty == gcdk) {

		for (auto x = 1; x <= maxX ; x++ ) {
			const double shift = ((double) ( Tx * x ) /  (double) gcdk);

			const double gx0 =  (double) g0 +  (double) shift;
			const double fx0 =  (double) f0 +  (double) shift;

			long floorgx0 = (long) std::floor(gx0);
			long ceilfx0 = (long) std::ceil(fx0);

			if (floorgx0 == ceilfx0) {
				// Take (x,y) for every y
				const EXECUTION_COUNT ai = x;

				const Execution ei(ti_id, ai);
				const EXECUTION_COUNT alphae_ai_ajgcdeTaiside = (Ti * ai);
				for (auto aj = 1; aj <= Kj; aj++) {

					EXECUTION_COUNT alphae_ai_ajgcdeT = (alphae_ai_ajgcdeTaiside - Tj * aj);
					INTEGER_TIME_UNIT pi_min =
							std::ceil((-Me + gcdeT -alphae_ai_ajgcdeT) / gcdeK);

					// From Theorem 6 (ECRTS2020)
					INTEGER_TIME_UNIT Lmax =
							rjmripTimTj - (pi_min * gcdeK + alphae_ai_ajgcdeT);

					Execution ej(tj_id, aj);
					Constraint cij(ei, ej, Lmax);
					graph.add(cij);


				}
			}
		}
	} else {


		for (EXECUTION_COUNT x = 1; x <= maxX ; x++ ) {


			EXECUTION_COUNT ai = x;
			const Execution ei(ti_id, ai);
			const EXECUTION_COUNT alphae_ai_ajgcdeTaiside = (Ti * ai);

			double Tyyg0 = (Tx * x - g0 * gcdk);
			double Tyyf0 = (Tx * x - f0 * gcdk);

			long start = std::ceil ( Tyyg0 );
			long stop  = std::floor ( Tyyf0 );

			for (long theta = start ; theta <= stop; theta ++ ) {

				if (theta % g) continue;

				std::pair<long,long> res = extended_euclide ( Ty,  gcdk, theta);
				long u0 = res.first;
				for (long k = 0, y = u0 ; y <= maxY ; y = u0 + (++k * gcdk)/g) {

					if (y > 0 and y <= maxY) {

					EXECUTION_COUNT aj = y;

							VERBOSE_PCG("  "
									<< "from " << ai << " to " << aj);

							EXECUTION_COUNT alphae_ai_ajgcdeT = (alphae_ai_ajgcdeTaiside - Tj * aj);
							INTEGER_TIME_UNIT pi_min =
									std::ceil((-Me + gcdeT - alphae_ai_ajgcdeT) / gcdeK);

							// From Theorem 6 (ECRTS2020)
							INTEGER_TIME_UNIT Lmax =
									rjmripTimTj - (pi_min * gcdeK + alphae_ai_ajgcdeT);

							Execution ej(tj_id, aj);
							Constraint cij(ei, ej, Lmax);
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

	PartialConstraintGraph graph;

	VERBOSE_DEBUG("1) Create constraints.");
	for (Dependency d : model.dependencies()) {
		VERBOSE_DEBUG(" Run Algorithm 2 for d=" << d);
		new_algorithm2(model, K , d, graph) ;
	}

	VERBOSE_DEBUG("2) Constraints done, add start and finish.");
	add_start_finish (model, K, graph) ;
	return graph;
}


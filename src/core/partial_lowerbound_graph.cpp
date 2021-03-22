/*
 * partial_lowerbound_graph.cpp
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */

#include <partial_constraint_graph.h>
#include <utils.h>
#include <algorithm>
#include <numeric>
#include <cmath>

#define VERBOSE_LWB(stream) VERBOSE_DEBUG(stream)


void add_lowerbounds (const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph) {

	TASK_ID ti_id = d.getFirst();
	TASK_ID tj_id = d.getSecond();

	Task ti = model.getTaskById(ti_id);
	Task tj = model.getTaskById(tj_id);

	INTEGER_TIME_UNIT Ti = ti.getT();
	INTEGER_TIME_UNIT Tj = tj.getT();
	INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);

	auto Di = ti.getD();
	//auto Dj = tj.getD();

	auto ri = ti.getr();
	auto rj = tj.getr();


	EXECUTION_COUNT Ki = K[ti_id];
	EXECUTION_COUNT Kj = K[tj_id];
	EXECUTION_COUNT TjKj = Tj * Kj;

	auto gcdK = std::gcd(Ti * Ki, Tj * Kj);


	EXECUTION_COUNT TjKj_gcdK = TjKj/gcdK;

	VERBOSE_LWB("   "
			<< " Ti= " << Ti
			<< " Tj= " << Tj
			<< " Ki= " << Ki
			<< " Kj= " << Kj
			<< " gcdK= " << gcdK
			<< " TjKj_gcdK= " << TjKj_gcdK);

	auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

	for (auto ai = 1; ai <= Ki; ai++) {
		for (auto aj = 1; aj <= Kj; aj++) {

			VERBOSE_LWB("  "
					<< "from " << ai << " to " << aj);

			// recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

			INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
			INTEGER_TIME_UNIT pi_min =
					std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdK);
			INTEGER_TIME_UNIT pi_max =
					std::floor((-Me + Ti - alphae_ai_aj * gcdeT) / gcdK);

			VERBOSE_LWB("   "
					<< "alphae_ai_aj= "
					<< "(" << Ti << "*" << ai << "-" << Tj << "*" << aj
					<< ") / " << gcdeT << " = " << alphae_ai_aj);
			VERBOSE_LWB("   "
					<< "pi_min= " << pi_min);
			VERBOSE_LWB("   "
					<< "pi_max= " << pi_max);



			std::pair<long,long> res = extended_euclide ( Ti * Ki,   Tj * Kj, gcdK);
			EXECUTION_COUNT x0 = res.first;
			VERBOSE_LWB("   "
					<< " x_0= " << x0);


			VERBOSE_LWB("   "
					<< " TjKj_gcdK = " << TjKj_gcdK);
			VERBOSE_LWB("   "
					<< " gcd(x0, TjKj_gcdK)= " << std::gcd(x0, TjKj_gcdK));

			VERBOSE_LWB("   "
					<< " x_0= " << x0);


			VERBOSE_LWB("   "
					<< " (pi_min + TjKj_gcdK = " << pi_min + TjKj_gcdK
					<< ") and (pi_max + 1 = " << pi_max + 1 << ")");


			if (x0 < 0) x0 += TjKj_gcdK;
			if (not ((x0 >= 0) and (x0 <= TjKj_gcdK))) {
				VERBOSE_DEBUG("x0 is not in the correct range, need to modulo, x0 = " << x0);
			}
			VERBOSE_ASSERT((x0 >= 0) and (x0 <= TjKj_gcdK), "Unsupported case yet, need to modula x0");

			if (    (std::gcd(x0, TjKj_gcdK)  == 1 ) and  (pi_min + TjKj_gcdK <= pi_max + 1) ) {


				// From Theorem 6 (ECRTS2020)
				INTEGER_TIME_UNIT Lmax =
						rj - ri + Ti - Tj - (pi_max * gcdK + alphae_ai_aj * gcdeT);
				VERBOSE_LWB("   "
						<< "Lmax= " << Lmax);

				Execution ei(ti_id, ai);
				Execution ej(tj_id, aj);
				Constraint cij(ei, ej, Lmax);
				graph.add(cij);
			}
		}
	}
}



PartialConstraintGraph
generate_partial_lowerbound_graph(const LETModel &model,	const PeriodicityVector &K) {

	PartialConstraintGraph graph;

	for (Dependency d : model.dependencies()) {
		VERBOSE_LWB(" " << d);
		add_lowerbounds (model, K , d, graph);
	}

	add_start_finish (model, K, graph) ;
	return graph;
}



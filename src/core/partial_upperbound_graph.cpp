/*
 * partial_constraint_graph.cpp
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */

#include <partial_constraint_graph.h>
#include <utils.h>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <graphviz/gvc.h>

void add_upperbounds_constraints (const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph) {

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
			INTEGER_TIME_UNIT pi_max =
					std::floor((-Me + Ti - alphae_ai_aj * gcdeT) / gcdeK);

			VERBOSE_PCG("   "
					<< "alphae_ai_aj= "
					<< "(" << Ti << "*" << ai << "-" << Tj << "*" << aj
					<< ") / " << gcdeT << " = " << alphae_ai_aj);
			VERBOSE_PCG("   "
					<< "pi_min= " << pi_min);
			VERBOSE_PCG("   "
					<< "pi_max= " << pi_max);

			if (pi_min <= pi_max) {
				// From Theorem 6 (ECRTS2020)
				INTEGER_TIME_UNIT Lmax =
						rj - ri + Ti - Tj - (pi_min * gcdeK + alphae_ai_aj * gcdeT);
				VERBOSE_PCG("   "
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
generate_partial_upperbound_graph(const LETModel &model,	const PeriodicityVector &K) {

	PartialConstraintGraph graph;

	for (Dependency d : model.dependencies()) {
		VERBOSE_PCG(" " << d);
        add_upperbounds_constraints (model, K , d, graph);
	}

	add_start_finish (model, K, graph) ;
	return graph;
}



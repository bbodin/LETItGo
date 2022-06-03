/*
 * combined_partial_expansion_graph.cpp
 *
 *  Created on: Jun 3, 2022
 *      Author: toky
 */

#include <partial_constraint_graph.h>
#include <utils.h>
#include <algorithm>
#include <numeric>
#include <cmath>


void add_upperlowerbounds (const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph) {

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



    auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

    for (auto ai = 1; ai <= Ki; ai++) {
        for (auto aj = 1; aj <= Kj; aj++) {



            // recall: auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

            INTEGER_TIME_UNIT alphae_ai_aj = (Ti * ai - Tj * aj) / gcdeT;
            INTEGER_TIME_UNIT pi_min =
                    std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdK);
            INTEGER_TIME_UNIT pi_max =
                    std::floor((-Me + Ti - alphae_ai_aj * gcdeT) / gcdK);





            std::pair<long,long> res = extended_euclide ( Ti * Ki,   Tj * Kj, gcdK);
            EXECUTION_COUNT x0 = res.first;



            if (x0 < 0) x0 += TjKj_gcdK;

            VERBOSE_ASSERT((x0 >= 0) and (x0 <= TjKj_gcdK), "Unsupported case yet, need to modula x0");

            if (pi_min <= pi_max) {
                // From Theorem 6 (ECRTS2020)
                INTEGER_TIME_UNIT Lmax =
                        rj - ri + Ti - Tj - (pi_min * gcdK + alphae_ai_aj * gcdeT);


                Execution ei(ti_id, ai);
                Execution ej(tj_id, aj);

                if (    (std::gcd(x0, TjKj_gcdK)  == 1 ) and  (pi_min + TjKj_gcdK <= pi_max + 1) ) {


                    // From Theorem 6 (ECRTS2020)
                    INTEGER_TIME_UNIT Lmin =
                            rj - ri + Ti - Tj - (pi_max * gcdK + alphae_ai_aj * gcdeT);

                    graph.add(Constraint (ei, ej, Lmax, Lmin));
                } else {
                    graph.add(Constraint (ei, ej, Lmax));

                }
            }
        }
    }
}



PartialConstraintGraph
generate_combined_partial_expansion_graph(const LETModel &model,	const PeriodicityVector &K) {

    PartialConstraintGraph graph;

    for (Dependency d : model.dependencies()) {

        add_upperlowerbounds (model, K , d, graph);
    }

    add_start_finish (model, K, graph) ;
    return graph;
}



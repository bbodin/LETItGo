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


PartialConstraintGraph::PartialConstraintGraph (const LETModel& model , const PeriodicityVector& K) : periodicity_vector(K) {

    VERBOSE_DEBUG("start PartialConstraintGraph Constructor");

    

    // Generate execution list  (In topological order assuming constraints are)
    this->addExecution(-1, 0);
    for (TASK_ID task_id = 0 ; task_id < (TASK_ID) K.size(); task_id++) {
        auto k = K[task_id];
        for (auto exec_index = 1 ; exec_index <= k; exec_index++) {
            this->addExecution (task_id, exec_index);
        }
    }
    this->addExecution(-1, 1);

    // Generate constraints list
    for (Dependency d : model.dependencies()) {
        add_dependency_constraints (model, K , d);
    }
    add_start_finish_constraints (model) ;

}


/**
 * This function is generic, used by upper and lower
 * @param model
 * @param K
 * @param graph
 */
void PartialConstraintGraph::add_start_finish_constraints (const LETModel &model) {

    Execution s = this->getExecution(-1, 0);
    Execution f = this->getExecution(-1, 1);

    for (Execution t : this->getExecutions()) {

        TASK_ID tid = t.getTaskId();
        if (tid == -1) continue;  // Skip fake tasks

        // We prepare the weight w1 and w2 depending if we find or not edges.
        auto Di = model.getTaskById(tid).getD();
        WEIGHT in_wlow = 0, in_wup = 0; // By default, the weight are set, if we find edges we unset
        WEIGHT out_wlow = Di, out_wup = Di; // By default, the weight are set, if we find edges we unset
        for (Constraint c : this->getInputs(t)) {
            if (c.hasWeight(upper_wt))  in_wup = NO_CONSTRAINT;
            if (c.hasWeight(lower_wt))  in_wlow = NO_CONSTRAINT;
        }
        for (Constraint c : this->getOutputs(t)) {
            if (c.hasWeight(upper_wt))  out_wup = NO_CONSTRAINT;
            if (c.hasWeight(lower_wt))  out_wlow = NO_CONSTRAINT;
        }

        if (!(in_wup == NO_CONSTRAINT && in_wlow == NO_CONSTRAINT))  this->addConstraint(s, t, in_wlow, in_wup);
        if (!(out_wup == NO_CONSTRAINT && out_wlow == NO_CONSTRAINT)) this->addConstraint(t, f, out_wlow, out_wup);

    }

}



void PartialConstraintGraph::add_dependency_constraints (const LETModel &model, const PeriodicityVector &K , const Dependency &d) {

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

    std::pair<long,long> res = extended_euclide ( Ti * Ki,   Tj * Kj, gcdK);

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

            EXECUTION_COUNT x0 = res.first;
            if (x0 < 0) x0 += TjKj_gcdK;

            VERBOSE_ASSERT((x0 >= 0) and (x0 <= TjKj_gcdK), "Unsupported case yet, need to modula x0");

            if (pi_min <= pi_max) {
                // From Theorem 6 (ECRTS2020)
                INTEGER_TIME_UNIT Lmax =
                        rj - ri + Ti - Tj - (pi_min * gcdK + alphae_ai_aj * gcdeT);


                Execution ei = this->getExecution(ti_id, ai);
                Execution ej = this->getExecution(tj_id, aj);

                if (    (std::gcd(x0, TjKj_gcdK)  == 1 ) and  (pi_min + TjKj_gcdK <= pi_max + 1) ) {


                    // From Theorem 6 (ECRTS2020)
                    INTEGER_TIME_UNIT Lmin =
                            rj - ri + Ti - Tj - (pi_max * gcdK + alphae_ai_aj * gcdeT);

                    this->addConstraint (ei, ej, Lmin, Lmax);
                } else {
                    this->addConstraint (ei, ej, NO_CONSTRAINT, Lmax);

                }
            }
        }
    }
}

/**
 * TODO: rewrite this one. too slow.
 * @return
 */
const std::vector<Execution>& PartialConstraintGraph::getTopologicalOrder()  const {

    // If this is not dirty, in the sense of no constraint violate (i,j) with i < j)
    // Then execution list remains valid topological order.
    if (not this->dirty) return this->executions;

    // Topological order needed.

    auto outbounds_copy = this->outbounds;
    auto inbounds_copy = this->inbounds;

    this->topologicalOrder.clear();
    std::vector<Execution> S = {this->getExecution(-1, 0)};

    while (S.size()) {
        Execution n = S[S.size() - 1];
        S.pop_back();
        this->topologicalOrder.push_back(n);

        for (Constraint e : outbounds_copy[n]) {
            Execution m = e.getDestination();
            // remove edge
            inbounds_copy[m].erase(e);

            if (inbounds_copy[m].size() == 0) {
                S.push_back(m);
            }
        }
        outbounds_copy[n].clear();
    }

    // assert all edges gone
    return this->topologicalOrder;
}

std::string  PartialConstraintGraph::getSVG(const WeightType wt) const {

    std::string res;
    GVC_t *gvc;
    Agraph_t *g;

    char* buffer;
    unsigned int buffer_size;

    std::string dot_version = this->getDOT(wt);
    gvc = gvContext();
    g = agmemread(dot_version.c_str());
    gvLayout(gvc, g, "dot");
    gvRenderData(gvc, g, "svg", &buffer, &buffer_size);
    res = buffer;
    gvFreeRenderData(buffer);
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
    return res;
}


std::string PartialConstraintGraph::getDOT(WeightType wt) const {

    auto upper_path = FindLongestPath(*this, upper_wt);
    auto lower_path = FindLongestPath(*this, lower_wt);

    std::stringstream ss;
    ss << "// Grahviz format using the DOT language \n";
    ss << "// ======================================\n";
    ss << "digraph {\n";
    for (Execution e : upper_path.first) {
        ss << "  \"" << e.getTaskId() << "," << e.getExecId() << "\"";
        ss  << "[penwidth=2.0]";
        ss << ";"<< std::endl;
    }

    for (Constraint c : this->getConstraints()) {
        if (!c.hasWeight(wt)) continue;
        auto es = c.getSource();
        auto ed = c.getDestination();
        ss << "  \"" << es.getTaskId() << "," << es.getExecId() << "\"" << " -> "
        << "\"" << ed.getTaskId() << "," << ed.getExecId() << "\""
        << "[label=\" " <<  c.getWeight(wt) << "\"]"
        << ";"<< std::endl;
    }

    ss << "}\n";
    return ss.str();
};



std::pair<std::vector<Execution>, INTEGER_TIME_UNIT>
FindLongestPath(const PartialConstraintGraph& PKG, const WeightType wt) {

    // Special case - no tasks.
    if (!PKG.getConstraintsCount()) {
        return std::pair<std::vector<Execution>, INTEGER_TIME_UNIT>(
                {}, 0);
    }


    std::map<Execution, WEIGHT> dist;
    std::map<Execution, Execution> prev;

    dist[PKG.getExecution(-1, 0)] = 0;

    const std::vector<Execution>& ordered_execution = PKG.getTopologicalOrder();

    for (Execution src : ordered_execution) {
        if (dist.count(src)) {
            for (Constraint c : PKG.getOutputs(src)) {
                auto weight = c.getWeight(wt);
                if (not c.hasWeight(wt)) continue;
                Execution dest = c.getDestination();
                if (dist.count(dest) == 0 || dist[dest] < dist[src] + weight) {
                    dist[dest] = dist[src] + weight;

                    auto const result = prev.insert(std::make_pair(dest, src));
                    if (not result.second) {
                        result.first->second = src;
                    }

                    VERBOSE_PCG(" Update " << dest << " by " << src);
                    VERBOSE_PCG(" prev =  " << prev);
                }
            }
        } else {
            VERBOSE_ERROR("Topological order failed");
        }
    }

    for (Execution e : ordered_execution) {
        VERBOSE_ASSERT(dist.count(e), "Could not find dist for execution" << e);
        VERBOSE_PCG(e << " distance is " << dist[e]);
    }

    VERBOSE_PCG(prev);

    std::vector<Execution> L;
    Execution e = PKG.getExecution(-1, 1);
    while (prev.count(e) == 1) {
        VERBOSE_PCG("Longest to " << e << " comes from " << prev.at(e));
        L.push_back(e);
        e = prev.at(e);
    }
    L.push_back(e);

    std::reverse(L.begin(), L.end());

    return std::pair<std::vector<Execution>, INTEGER_TIME_UNIT>(
            L, dist[PKG.getExecution(-1, 1)]);
}

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


std::string  PartialConstraintGraph::getSVG(){

    std::string res;
    GVC_t *gvc;
    Agraph_t *g;

    char* buffer;
    unsigned int buffer_size;

    std::string dot_version = this->getDOT();
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


std::string PartialConstraintGraph::getDOT() {

    auto FLP = FindLongestPath(*this);

    std::stringstream ss;
    ss << "// Grahviz format using the DOT language \n";
    ss << "// ======================================\n";
    ss << "digraph {\n";
    for(Execution e : this->getExecutions()) {
        ss << "  \"" << e.getTaskId() << "," << e.getExecId() << "\"";
        if (std::count(FLP.first.begin(), FLP.first.end(),e)) {
            ss  << "[penwidth=2.0]";
        }
        ss << ";"<< std::endl;
    }
    for (Constraint c : this->getConstraints()) {
        auto es = c.getSource();
        auto ed = c.getDestination();
        ss << "  \"" << es.getTaskId() << "," << es.getExecId() << "\"" << " -> "
        << "\"" << ed.getTaskId() << "," << ed.getExecId() << "\""
        << "[label=\" " <<  c.getWeight() << "\"]"
        << ";"<< std::endl;
    }

    ss << "}\n";
    return ss.str();
};


std::pair<std::vector<Execution>, INTEGER_TIME_UNIT>
FindLongestPathUpper(PartialConstraintGraph PKG) {
    return FindLongestPath(PKG);
}
std::pair<std::vector<Execution>, INTEGER_TIME_UNIT>
FindLongestPathLower(PartialConstraintGraph PKG) {

	std::map<Execution, WEIGHT> dist;
	std::map<Execution, Execution> prev;

	dist[Execution(-1, 0)] = 0;

	const std::vector<Execution>& ordered_execution = PKG.getTopologicalOrder();

	for (Execution src : ordered_execution) {
		if (dist.count(src)) {
			for (Constraint c : PKG.getOutputs(src)) {
                auto lowerWeight = c.getWeight2();
                if (not c.hasWeight2()) continue;
				Execution dest = c.getDestination();
				if (dist.count(dest) == 0 || dist[dest] < dist[src] + lowerWeight) {
					dist[dest] = dist[src] + lowerWeight;

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
	Execution e = Execution(-1, 1);
	while (prev.count(e) == 1) {
		VERBOSE_PCG("Longest to " << e << " comes from " << prev.at(e));
		L.push_back(e);
		e = prev.at(e);
	}
	L.push_back(e);

	std::reverse(L.begin(), L.end());

	return std::pair<std::vector<Execution>, INTEGER_TIME_UNIT>(
			L, dist[Execution(-1, 1)]);
}

std::pair<std::vector<Execution>, INTEGER_TIME_UNIT>
FindLongestPath(PartialConstraintGraph PKG) {

    std::map<Execution, WEIGHT> dist;
    std::map<Execution, Execution> prev;

    dist[Execution(-1, 0)] = 0;

    const std::vector<Execution>& ordered_execution = PKG.getTopologicalOrder();

    for (Execution src : ordered_execution) {
        if (dist.count(src)) {
            for (Constraint c : PKG.getOutputs(src)) {
                Execution dest = c.getDestination();
                if (dist.count(dest) == 0 || dist[dest] < dist[src] + c.getWeight()) {
                    dist[dest] = dist[src] + c.getWeight();

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
    Execution e = Execution(-1, 1);
    while (prev.count(e) == 1) {
        VERBOSE_PCG("Longest to " << e << " comes from " << prev.at(e));
        L.push_back(e);
        e = prev.at(e);
    }
    L.push_back(e);

    std::reverse(L.begin(), L.end());

    return std::pair<std::vector<Execution>, INTEGER_TIME_UNIT>(
            L, dist[Execution(-1, 1)]);
}




/**
 * This function is generic, used by upper and lower
 * @param model
 * @param K
 * @param graph
 */
void add_start_finish (const LETModel &model, const PeriodicityVector &K, PartialConstraintGraph& graph) {
	Execution s(-1, 0);
	Execution f(-1, 1);

	for (Task task : model.tasks()) {
		TASK_ID tid = model.getTaskIdByTask(task);
		auto Di = model.getTaskById(tid).getD();

		for (auto a = 1; a <= K[tid]; a++) {

			Execution t(tid, a);

			// for any t without pred add s-> t with weight 0
			//if (graph.getInputs(t).size() == 0)
            {
				Constraint cin(s, t, 0, 0);
				graph.add(cin);
			}

			// for any t without succ add t -> f with weight Di (i the task)
			//if (graph.getOutputs(t).size() == 0)
            {
				Constraint cout(t, f, Di, Di);
				graph.add(cout);
			}
		}
	}
}



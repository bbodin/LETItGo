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


PartialConstraintGraph::PartialConstraintGraph (const LETModel& model , const PeriodicityVector& K) : periodicity_vector(K), model(model) {

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

    Execution s = this->getSource();
    Execution f = this->getTarget();
    //TODO : This could be speed up by directly iterating through constraints vector.
    for (Execution t : this->getExecutions()) {

        TASK_ID tid = t.getTaskId();
        if (tid == -1) continue;  // Skip fake tasks

        // We prepare the weight w1 and w2 depending if we find or not edges.
        auto Di = model.getTaskById(tid).getD();

        // By default, the weight are set, if we find edges we unset
        WEIGHT in_wlow = 0, in_wup = 0;
        WEIGHT out_wlow = Di, out_wup = Di;

        for (Constraint c : this->getInputs(t)) {
            if (c.hasWeight(upper_wt))  {in_wup = NO_CONSTRAINT;}
            if (c.hasWeight(lower_wt))  {in_wlow = NO_CONSTRAINT;}
        }
        for (Constraint c : this->getOutputs(t)) {
            if (c.hasWeight(upper_wt))  {out_wup = NO_CONSTRAINT;}
            if (c.hasWeight(lower_wt))  {out_wlow = NO_CONSTRAINT;}
        }

        if ((in_wup != NO_CONSTRAINT || in_wlow != NO_CONSTRAINT))  this->addConstraint(s, t, in_wlow, in_wup);
        if ((out_wup != NO_CONSTRAINT || out_wlow != NO_CONSTRAINT)) this->addConstraint(t, f, out_wlow, out_wup);

    }

}



INTEGER_TIME_UNIT compute_alphae_ai_aj (const LETModel &model,const Dependency &d, EXECUTION_COUNT ai, EXECUTION_COUNT aj) {
    TASK_ID ti_id = d.getFirst();
    TASK_ID tj_id = d.getSecond();

    Task ti = model.getTaskById(ti_id);
    Task tj = model.getTaskById(tj_id);

    INTEGER_TIME_UNIT Ti = ti.getT();
    INTEGER_TIME_UNIT Tj = tj.getT();
    INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);

    return  (Ti * ai - Tj * aj) / gcdeT;
}
INTEGER_TIME_UNIT compute_pi_min (const LETModel &model,const Dependency &d, EXECUTION_COUNT ai, EXECUTION_COUNT aj, EXECUTION_COUNT Ki, EXECUTION_COUNT Kj) {
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

    auto gcdK = std::gcd(Ti * Ki, Tj * Kj);
    auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;
    INTEGER_TIME_UNIT alphae_ai_aj = compute_alphae_ai_aj (model,d, ai, aj);
    return std::ceil((-Me + gcdeT - alphae_ai_aj * gcdeT) / gcdK);
}
INTEGER_TIME_UNIT compute_pi_max (const LETModel &model,const Dependency &d, EXECUTION_COUNT ai, EXECUTION_COUNT aj, EXECUTION_COUNT Ki, EXECUTION_COUNT Kj) {
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

    auto gcdK = std::gcd(Ti * Ki, Tj * Kj);
    auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;
    INTEGER_TIME_UNIT alphae_ai_aj = compute_alphae_ai_aj (model,d, ai, aj);
    return std::floor((-Me + Ti - alphae_ai_aj * gcdeT) / gcdK);
}

std::string PartialConstraintGraph::getAlphasAsLatex(DEPENDENCY_ID edgeId) const {

    auto edge = this->model.getDependencyById(edgeId);
    EXECUTION_COUNT Ki = this->get_periodicity_vector().at(edge.getFirst());
    EXECUTION_COUNT Kj = this->get_periodicity_vector().at(edge.getSecond());

        std::ostringstream latex;

        // Retrieve dependency
        Dependency dep = model.dependencies().at(edgeId);
        TASK_ID i = dep.getFirst();  // Source task ID
        TASK_ID j = dep.getSecond(); // Destination task ID

        // Define ranges for a_i and a_j based on Ki and Kj
        std::vector<EXECUTION_COUNT> a_i(Ki);
        std::vector<EXECUTION_COUNT> a_j(Kj);

        // Fill the vectors with values 1 through Ki and 1 through Kj
        for (EXECUTION_COUNT k = 1; k <= Ki; ++k) {
            a_i[k - 1] = k;
        }
        for (EXECUTION_COUNT k = 1; k <= Kj; ++k) {
            a_j[k - 1] = k;
        }


        // Start the description
        latex << "For example, let us consider again the LET communication $e=(t_{" << i << "},t_{" << j << "})$ "
              << "from the instance presented by Figure \\ref{fig:example_dag}.\n";
        latex << "Let us also suppose here that $K_{" << i << "}=" << Ki << "$ and $K_{" << j << "}=" << Kj << "$.\n";
        latex << "Figure \\ref{fig:alphaetpiminmax} presents the associated functions "
              << "$\\alpha_e(a_{" << i << "},a_{" << j << "})$, $\\pi^{\\text{max}}_e(a_{" << i << "},a_{" << j << "})$, "
              << "and $\\pi^{\\text{min}}_e(a_{" << i << "},a_{" << j << "})$ for "
              << "$(a_{" << i << "},a_{" << j << "})\\in\\{1,\\ldots," << Ki << "\\}\\times\\{1,\\ldots," << Kj << "\\}$.\n";



        // Begin LaTeX figure
        latex << "\\begin{figure}\n"
              << "\\centering\n";

        // Generate table for alpha_e(a_i, a_j)
        latex << "\\begin{center}\n"
              << "\\begin{tabular}{|c";
        for (size_t col = 0; col < a_j.size(); ++col) latex << "|c";
        latex << "|}\n\\hline\n"
              << "\\diagbox{$a_{" << i << "}$}{$a_{" << j << "}$}";
        for (EXECUTION_COUNT aj : a_j) {
            latex << " & $" << aj << "$";
        }
        latex << " \\\\\n\\hline\n";
        for (EXECUTION_COUNT ai : a_i) {
            latex << "$" << ai << "$";
            for (EXECUTION_COUNT aj : a_j) {
                INTEGER_TIME_UNIT alpha = compute_alphae_ai_aj(model, dep, ai, aj);
                latex << " & $" << alpha << "$";
            }
            latex << " \\\\\n\\hline\n";
        }
        latex << "\\end{tabular}\n"
              << "\\vskip0.5cm\n"
              << "$\\alpha_e(a_{" << i << "},a_{" << j << "})$\n"
              << "\\end{center}\n";
        latex << "\\begin{multicols}{2}\n";
        // Generate table for pi^max_e(a_i, a_j)
        latex << "\\begin{center}\n"
              << "\\begin{tabular}{|c";
        for (size_t col = 0; col < a_j.size(); ++col) latex << "|c";
        latex << "|}\n\\hline\n"
              << "\\diagbox{$a_{" << i << "}$}{$a_{" << j << "}$}";
        for (EXECUTION_COUNT aj : a_j) {
            latex << " & $" << aj << "$";
        }
        latex << " \\\\\n\\hline\n";
        for (EXECUTION_COUNT ai : a_i) {
            latex << "$" << ai << "$";
            for (EXECUTION_COUNT aj : a_j) {
                INTEGER_TIME_UNIT piMax = compute_pi_max(model, dep, ai, aj, Ki, Kj);
                latex << " & $" << piMax << "$";
            }
            latex << " \\\\\n\\hline\n";
        }
        latex << "\\end{tabular}\n"
              << "\\vskip0.5cm\n"
              << "$\\pi^{\\text{max}}_e(a_{" << i << "},a_{" << j << "})$\n"
              << "\\end{center}\n"
              << "\\columnbreak\n";

        // Generate table for pi^min_e(a_i, a_j)
        latex << "\\begin{center}\n"
              << "\\begin{tabular}{|c";
        for (size_t col = 0; col < a_j.size(); ++col) latex << "|c";
        latex << "|}\n\\hline\n"
              << "\\diagbox{$a_{" << i << "}$}{$a_{" << j << "}$}";
        for (EXECUTION_COUNT aj : a_j) {
            latex << " & $" << aj << "$";
        }
        latex << " \\\\\n\\hline\n";
        for (EXECUTION_COUNT ai : a_i) {
            latex << "$" << ai << "$";
            for (EXECUTION_COUNT aj : a_j) {
                INTEGER_TIME_UNIT piMin = compute_pi_min(model, dep, ai, aj, Ki, Kj);
                latex << " & $" << piMin << "$";
            }
            latex << " \\\\\n\\hline\n";
        }
        latex << "\\end{tabular}\n"
              << "\\vskip0.5cm\n"
              << "$\\pi^{\\text{min}}_e(a_{" << i << "},a_{" << j << "})$\n"
              << "\\end{center}\n";

        // End LaTeX figure
        latex << "\\end{multicols}\n"
              << "\\caption{Functions $\\alpha_e(a_{" << i << "},a_{" << j << "})$, "
              << "$\\pi^{\\text{max}}_e(a_{" << i << "},a_{" << j << "})$, and "
              << "$\\pi^{\\text{min}}_e(a_{" << i << "},a_{" << j << "})$ expressed over "
              << "$(a_{" << i << "},a_{" << j << "})\\in\\{1,\\ldots," << Ki << "\\}\\times\\{1,\\ldots," << Kj << "\\}$,"
              << " for the LET communication $e = (t_" << i << ",t_" << j << ")$, with $K_{"<< i << "} = "<< Ki << "$ and $K_{"<< j << "} = "<< Kj << "$"
              << ".}\n"
              << "\\label{fig:alphaetpiminmax}\n"
              << "\\end{figure}";


        return latex.str();

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
    VERBOSE_DEBUG("Me = " << Me);
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
    VERBOSE_FAILURE();
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
std::map<Execution, std::pair<double, double>> placeNodes(const PartialConstraintGraph& graph) {

    int width  = 8;
    int height = 4;

    const Execution& source = graph.getSource();
    const Execution& target = graph.getTarget();

    std::map<Execution, std::pair<double, double>> positions;

    // Fix source and target positions
    positions[source] = {0, 0};
    positions[target] = {width, height};

    // Initialize positions for all other nodes randomly
    for (const Constraint& c : graph.getConstraints()) {
        auto es = c.getSource();
        auto ed = c.getDestination();

        if (positions.find(es) == positions.end()) {
            positions[es] = {static_cast<double>(rand() % width) , static_cast<double>(rand() % height)};
        }
        if (positions.find(ed) == positions.end()) {
            positions[ed] = {static_cast<double>(rand() % width) , static_cast<double>(rand() % height)};
        }
    }

    // Spring algorithm parameters
    const double springLength = std::min(width, height) / 2.0;
    const double springStrength = 0.1;
    const double repulsionStrength = std::min(width, height) / 2.0;
    const double dampingFactor = 0.9; // Scale down displacements to prevent overshooting
    const int iterations = 100;

    // Perform spring algorithm iterations
    for (int iter = 0; iter < iterations; ++iter) {
        std::map<Execution, std::pair<double, double>> displacements;

        // Initialize displacements
        for (const auto& [node, pos] : positions) {
            displacements[node] = {0.0, 0.0};
        }

        // Calculate repulsive forces
        for (const auto& [nodeA, posA] : positions) {
            for (const auto& [nodeB, posB] : positions) {
                if (nodeA == nodeB) continue;

                double dx = posA.first - posB.first;
                double dy = posA.second - posB.second;
                double distance = std::sqrt(dx * dx + dy * dy) + 1e-6; // Avoid division by zero
                double force = repulsionStrength / (distance * distance);

                displacements[nodeA].first += force * dx / distance;
                displacements[nodeA].second += force * dy / distance;
            }
        }

        // Calculate spring forces
        for (const Constraint& c : graph.getConstraints()) {
            auto es = c.getSource();
            auto ed = c.getDestination();

            auto posA = positions[es];
            auto posB = positions[ed];

            double dx = posB.first - posA.first;
            double dy = posB.second - posA.second;
            double distance = std::sqrt(dx * dx + dy * dy) + 1e-6;
            double force = springStrength * (distance - springLength);

            displacements[es].first += force * dx / distance;
            displacements[es].second += force * dy / distance;

            displacements[ed].first -= force * dx / distance;
            displacements[ed].second -= force * dy / distance;
        }

        // Update positions with damping and boundary constraints
        for (auto& [node, pos] : positions) {
            if (node == source || node == target) continue; // Skip fixed nodes

            pos.first += dampingFactor * displacements[node].first;
            pos.second += dampingFactor * displacements[node].second;

            // Apply boundary constraints
            pos.first = std::max(0.0, std::min((double)width, pos.first));
            pos.second = std::max(0.0, std::min((double)height, pos.second));
        }
    }

    return positions;
}



std::string PartialConstraintGraph::getTikZ() const {

    std::stringstream ss;
    ss << "\\begin{tikzpicture}[" << std::endl;
    ss << "  node/.style={circle, draw, fill=white, minimum size=1cm, font=\\small}," << std::endl;
    ss << "  edge/.style={->, thick}," << std::endl;
    ss << "  label/.style={font=\\small, midway, sloped, above}"
       << std::endl;
    ss << "]\n" << std::endl;

    // Add nodes
    //std::map<std::string, std::string> nodePositions; // Maps each node to its position


    std::map<Execution , std::pair<double, double>> nodePositions = placeNodes(*this);


    for (Execution e : this->getExecutions()) {
        std::string nodeName = "n" + std::to_string(e.getTaskId()) + "_" + std::to_string(e.getExecId());
        auto x = nodePositions[e].first;
        auto y = nodePositions[e].second;
        ss << "\\node[node] (" << nodeName << ") at " << "(" + std::to_string(x) + ", " + std::to_string(y) + ")" << " {$"
           << e.getTaskId() << "," << e.getExecId() << "$};\n";
    }

    // Add edges with labels
    for (Constraint c : this->getConstraints()) {
        if (!c.hasWeight(lower_wt) && !c.hasWeight(upper_wt)) continue;

        auto es = c.getSource();
        auto ed = c.getDestination();

        std::string sourceNode = "n" + std::to_string(es.getTaskId()) + "_" + std::to_string(es.getExecId());
        std::string destNode = "n" + std::to_string(ed.getTaskId()) + "_" + std::to_string(ed.getExecId());

        ss << "\\draw[edge] (" << sourceNode << ") -- (" << destNode << ") node[label] {$ ";

        // Add label with upper and lower bounds
        ss << (c.hasWeight(lower_wt) ? std::to_string(c.getWeight(lower_wt)) :  "-") ;
        ss << " / ";
        ss << (c.hasWeight(upper_wt) ? std::to_string(c.getWeight(upper_wt)) :  "-") ;
        ss << " $};\n";
    }

    // Add legend (optional)
    ss << "\\matrix [draw,  below right=1em of n-1_1, nodes={draw, anchor=west}] {\n";
    ss << "  \\node {Edge Label: Upper Bound / Lower Bound}; \\\\ \n";
    ss << "};\n";

    ss << "\\end{tikzpicture}\n";

    return ss.str();
}

std::string PartialConstraintGraph::getDOT(WeightType wt) const {

    auto path = FindLongestPath(*this, wt);

    std::stringstream ss;
    ss << "// Grahviz format using the DOT language \n";
    ss << "// ======================================\n";
    ss << "digraph {\n";
    for (Execution e : path.first) {
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
        << "[label=\" " <<  c.getWeight(lower_wt) << "/" <<  c.getWeight(upper_wt) << "\"]"
        << ";"<< std::endl;
    }

    ss << "}\n";
    return ss.str();
}

const LETModel &PartialConstraintGraph::getModel() const {
    return this->model;
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

    dist[PKG.getSource()] = 0;

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
    Execution e = PKG.getTarget();
    while (prev.count(e) == 1) {
        VERBOSE_PCG("Longest to " << e << " comes from " << prev.at(e));
        L.push_back(e);
        e = prev.at(e);
    }
    L.push_back(e);

    std::reverse(L.begin(), L.end());

    return std::pair<std::vector<Execution>, INTEGER_TIME_UNIT>(
            L, dist[PKG.getTarget()]);
}

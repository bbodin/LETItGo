/*
 * LETModel.cpp
 *
 *  Created on: 11 Aug 2021
 *      Author: toky
 */

#include <model.h>
#include <sstream>
#include <graphviz/gvc.h>
#include <numeric>
#include <cmath>

bool LETModel::check_dependency  (Dependency d, long Vi, long Vj)  const {
    TASK_ID ti_id = d.getFirst();
    TASK_ID tj_id = d.getSecond();

    Task ti = this->getTaskById(ti_id);
    Task tj = this->getTaskById(tj_id);

    INTEGER_TIME_UNIT Ti = ti.getT();
    INTEGER_TIME_UNIT Tj = tj.getT();

    auto Di = ti.getD();
    //auto Dj = tj.getD();

    auto ri = ti.getr();
    auto rj = tj.getr();
    INTEGER_TIME_UNIT gcdeT = std::gcd(Ti, Tj);
    auto Me = Tj + std::ceil((ri - rj + Di) / gcdeT) * gcdeT;

    auto center = Me + Ti * Vi - Tj * Vj;

    VERBOSE_DEBUG("check_dependency between " << ti_id << "," << Vi << " and " << tj_id << "," << Vj);
    if ((Ti >= center) and (center > 0)) {
        VERBOSE_DEBUG("VERIFIED!");
        VERBOSE_DEBUG("Ti = " << Ti);
        VERBOSE_DEBUG("Tj = " << Tj);

        VERBOSE_DEBUG("ri = " << ri);
        VERBOSE_DEBUG("rj = " << rj);

        VERBOSE_DEBUG("Di = " << Di);

        VERBOSE_DEBUG("gcdeT = " << gcdeT);
        VERBOSE_DEBUG("Me = " << Me);
        VERBOSE_DEBUG("center = " << center);
        VERBOSE_DEBUG("Ti = " << Ti);
    }
    return (Ti >= center) and (center > 0);


}

std::string LETModel::getTikzDAG() const {
    std::ostringstream latex;
    latex << "\\begin{tikzpicture}[scale=0.90]\n";

    // Add nodes
    for (const auto &task : this->tasks()) {
        latex << "\\node[circle,draw] (" << task.getId()  << ") at ("
              << task.getId() * 2 << ","
              << ((task.getId() % 2 == 0) ? 1 : -1) << ") {$t_" << task.getId()  << "$};\n";
    }

    // Add edges
    for (const auto &dependency : this->dependencies()) {
        latex << "\\draw[->]  (" << dependency.getFirst()  << ") -- (" << dependency.getSecond()  << ");\n";
    }

    latex << "\\end{tikzpicture}\n";

    return latex.str();
}
std::string LETModel::getTabular() const {
    std::stringstream latex;
    latex << "\\begin{tabular}{|c|c|c|c|c|}\n"
          << "\\hline\n"
          << "$t_i$ &";

    // Task IDs
    for (const auto &task : this->tasks()) {
        latex << " $t_" << task.getId()  << "$ &";
    }
    latex.seekp(-1, std::ios_base::end);
    latex << "\\\\\\hline\n$r_i$ &";

    // Release times
    for (const auto &task : this->tasks()) {
        latex << " $" << task.getr() << "$ &";
    }
    latex.seekp(-1, std::ios_base::end);
    latex << "\\\\\\hline\n$D_i$ &";

    // Deadlines
    for (const auto &task : this->tasks()) {
        latex << " $" << task.getD() << "$ &";
    }
    latex.seekp(-1, std::ios_base::end);
    latex << "\\\\\\hline\n$T_i$ &";

    // Periods
    for (const auto &task : this->tasks()) {
        latex << " $" << task.getT() << "$ &";
    }
    latex.seekp(-1, std::ios_base::end);
    latex << "\\\\\\hline\n"
          << "\\end{tabular}\n";
    return latex.str();
}

std::string LETModel::getTikzSchedule(int duration) const {
    std::stringstream res;
    res << "" << std::endl;
    res << "\\begin{LET}{" << this->getTaskCount() << "}{" << duration << "}" << std::endl;

    for (auto t : this->tasks()) {
        res << "\\LETTask{"<< t.getId() <<"}"
            << "{" << t.getr() << "}"
            << "{" << t.getC() << "}"
            << "{" << t.getD() << "}"
            << "{" << t.getT() << "}"
            << std::endl;
    }


    for (auto d : this->dependencies()) {
        auto ti = this->getTaskById(d.getFirst());
        auto tj = this->getTaskById(d.getSecond());
        int max_i = std::ceil((duration - ti.getr()) / ti.getT());
        int max_j = std::ceil((duration - tj.getr()) / tj.getT());
        VERBOSE_DEBUG("Print dependency " << d);
        for (auto i = 1 ; i < max_i; i++) {
            for (auto j = 1 ; j < max_j; j++) {

                // VERBOSE_DEBUG("Check dependency " << d << " " << i << "," << j);

                if (this->check_dependency(d, i, j)) {
                    res << "\\LETExecutionDependency{"<< d.getFirst() <<"}"
                        << "{" << i << "}"
                        << "{" << d.getSecond() << "}"
                        << "{" << j << "}"
                        << std::endl;
                }
            }
        }
    }

    res << "\\end{LET}" << std::endl;
    res << "" << std::endl;
    return res.str();

}

std::string LETModel::getXML() {
    std::stringstream res;
    res << *this << std::endl;
    return res.str();
}
std::string LETModel::getDOT() {
	std::stringstream res;
	res << "digraph structs {" << std::endl;
    for (auto t : this->tasks()) {
        res << " " << t.getId()
        << "[label=\" Task " << t.getId() << "\n"
        << " r=" << t.getr()
        << " T=" << t.getT()
        << " D=" << t.getD()<< "\"];"
        << std::endl;
    }
	for (auto d : this->dependencies()) {
		res << " " << d.getFirst() << " -> " << d.getSecond() << ";" << std::endl;
	}
	res << "}" << std::endl;

	return res.str();
}

std::string  LETModel::getSVG(){

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

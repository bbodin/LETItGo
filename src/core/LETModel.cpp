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

std::string LETModel::getTikz(int duration) {
    std::stringstream res;
    res << "" << std::endl;
    res << "\\begin{tikzpicture}[transform shape,scale=0.5]" << std::endl;
    res << "\\LETSchedule{" << this->getTaskCount() << "}{" << duration << "}" << std::endl;

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

    res << "\\end{tikzpicture}" << std::endl;
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

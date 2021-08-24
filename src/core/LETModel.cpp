/*
 * LETModel.cpp
 *
 *  Created on: 11 Aug 2021
 *      Author: toky
 */

#include <model.h>
#include <sstream>
#include <graphviz/gvc.h>



std::string LETModel::getXML() {
	std::stringstream res;
	res << *this << std::endl;
	return res.str();
}
std::string LETModel::getDOT() {
	std::stringstream res;
	res << "digraph structs {" << std::endl;
	for (auto d : this->dependencies()) {
		res << d.getFirst() << "-" << d.getSecond() << std::endl;
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

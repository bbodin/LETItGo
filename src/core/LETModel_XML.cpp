
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <string.h>
#include <model.h>

#define _LETModel_XML_NODE_NAME_ "LETModel"
#define _Task_XML_NODE_NAME_ "Task"
#define _Dependency_XML_NODE_NAME_ "Dependency"
#define _Task_XML_ID_ATTRIBUT_ "id"
#define _Task_XML_RELEASE_DATE_ATTRIBUT_ "r"


#define _Task_XML_WCET_ATTRIBUT_  "C"
#define _Task_XML_RELATIVE_DEADLINE_ATTRIBUT_  "D"
#define _Task_XML_PERIOD_ATTRIBUT_ "T"

#define _Dependency_XML_FROM_ATTRIBUT_ "from"
#define _Dependency_XML_TO_ATTRIBUT_ "to"

// <LETModel>
//  <Task id='0' r='0' C='60' D='60' T='60'>
//  <Task id='1' r='0' C='60' D='60' T='60'>
//  <Task id='2' r='0' C='40' D='40' T='40'>
//  <Task id='3' r='0' C='30' D='30' T='30'>
//  <Task id='4' r='0' C='30' D='30' T='30'>
//  <Task id='5' r='0' C='30' D='30' T='30'>
//  <Dependency from='0' to='1'>
//  <Dependency from='1' to='2'>
//  <Dependency from='2' to='3'>
//  <Dependency from='4' to='2'>
//  <Dependency from='5' to='3'>
// </LETModel>
//


Task parseTaskNode (xmlNodePtr cur_node) {

	TASK_ID tid = 0;
	TIME_UNIT r = 0;
	INTEGER_TIME_UNIT C= 0, D= 0, T= 0;

	for (xmlAttrPtr cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next) {

		// Task Id
		if (strcmp((const char*)cur_attr->name,_Task_XML_ID_ATTRIBUT_) == 0) {
			const char * id_str = (const char*)cur_attr->children->content;
			tid = std::stoi( id_str );

		}

		// Release date (r)
		if (strcmp((const char*)cur_attr->name,_Task_XML_RELEASE_DATE_ATTRIBUT_) == 0) {
			const char * id_str = (const char*)cur_attr->children->content;
			r = std::stold( id_str );

		}

		//  worst-case execution time (C)
		if (strcmp((const char*)cur_attr->name,_Task_XML_WCET_ATTRIBUT_) == 0) {
			const char * id_str = (const char*)cur_attr->children->content;
			C = std::stoll( id_str );

		}

		// relative deadline (D)
		if (strcmp((const char*)cur_attr->name,_Task_XML_RELATIVE_DEADLINE_ATTRIBUT_) == 0) {
			const char * id_str = (const char*)cur_attr->children->content;
			D = std::stoll( id_str );

		}
		// period (T)
		if (strcmp((const char*)cur_attr->name,_Task_XML_PERIOD_ATTRIBUT_) == 0) {
			const char * id_str = (const char*)cur_attr->children->content;
			T = std::stoll( id_str );

		}

	}
	return Task (tid, r, C, D, T);
}


Dependency parseDependencyNode (xmlNodePtr cur_node) {

	TASK_ID from = 0, to = 0;

	for (xmlAttrPtr cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next) {

		// From Task Id
		if (strcmp((const char*)cur_attr->name,_Dependency_XML_FROM_ATTRIBUT_) == 0) {
			const char * id_str = (const char*)cur_attr->children->content;
			from = std::stoi( id_str );

		}

		// To Task Id
		if (strcmp((const char*)cur_attr->name,_Dependency_XML_TO_ATTRIBUT_) == 0) {
			const char * id_str = (const char*)cur_attr->children->content;
			to = std::stoi( id_str );

		}

	}
	return Dependency (from,to);
}


LETModel::LETModel(std::string xmldata)  {

	xmlDocPtr doc =  xmlReadMemory(xmldata.c_str(), (int) xmldata.size(), NULL, NULL,XML_PARSE_NOERROR|XML_PARSE_NOWARNING|XML_PARSE_RECOVER);

	if (doc == NULL) {
			VERBOSE_ERROR("Could not read the XML data");
	}



	xmlNodePtr root = xmlDocGetRootElement(doc);

	if (root == NULL) {
			FAILED("Document XML invalide");
		}

		if (std::string((const char*)root->name) != _LETModel_XML_NODE_NAME_) {
			FAILED("Document XML invalide");
		}


		// Look for ApplicationGraph Node
		//--------------------------------

		for (xmlNodePtr cur_node = root->children; cur_node; cur_node = cur_node->next) {
			if (cur_node->type == XML_ELEMENT_NODE) {
				if (std::string((const char*)cur_node->name) == std::string(_Task_XML_NODE_NAME_))  {

					Task t = parseTaskNode (cur_node);
					this->addTask(t);
				} else 	if (std::string((const char*)cur_node->name) == std::string(_Dependency_XML_NODE_NAME_))  {

					Dependency d = parseDependencyNode (cur_node);
					this->addDependency(d);
				}
			}
		}




	xmlFreeDoc(doc);

}

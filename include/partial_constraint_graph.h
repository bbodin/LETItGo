/*
 * partial_constraint_graph.h
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */

#ifndef SRC_INCLUDE_PARTIAL_CONSTRAINT_GRAPH_H_
#define SRC_INCLUDE_PARTIAL_CONSTRAINT_GRAPH_H_

#include <model.h>
#include <periodicity_vector.h>
#include <verbose.h>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <utils.h>

#ifdef ULTRA_DEBUG
#define VERBOSE_PCG(m) VERBOSE_CUSTOM_DEBUG("PCG", m)
#else
#define VERBOSE_PCG(m) {}
#endif


class PartialConstraintGraph {

private: // cache
    mutable bool dirty = false; // set to true if a constraint mess with the topological order
    mutable std::vector<Execution> topologicalOrder;

private: // data
    std::vector<Execution> executions;
    std::vector<Constraint> constraints;
    std::map<Execution, std::set<Constraint>> inbounds;
    std::map<Execution, std::set<Constraint>> outbounds;

    inline void add (Constraint c) {
        // TODO FIXME URGENT Need to add a check for dirty
        //executions.insert(c.getSource());
        //executions.insert(c.getDestination());
        constraints.push_back(c);
        inbounds[c.getDestination()].insert(c);
        outbounds[c.getSource()].insert(c);
    };

private: // protection against catastrophe
    PartialConstraintGraph() {};

public: // Constructor

    /**
     * This is the constructor of PCG, for a given model and periodicity vector,
     * will generate the associated graph using a standard implementation.
     * @param model
     * @param K
     */
    PartialConstraintGraph(const LETModel& model , const PeriodicityVector& K) ;

private: // Consturctor helpers

    void add_start_finish_constraints (const LETModel &model, const PeriodicityVector &K) ;
    void add_dependency_constraints (const LETModel &model, const PeriodicityVector &K , const Dependency &d) ;

public: // Getters

  inline const std::set<Constraint> getInputs(Execution e) const {
    if (inbounds.count(e))
      return inbounds.at(e);
    return std::set<Constraint>();
  }

  inline const std::set<Constraint> getOutputs(Execution e) const {
    if (outbounds.count(e))
      return outbounds.at(e);
    return std::set<Constraint>();
  }


    inline const std::vector<Constraint>& getConstraints() const { return constraints; }
    inline const std::vector<Execution>& getExecutions() const { return executions; }

    inline size_t getConstraintsCount() const { return constraints.size(); }
    inline size_t getExecutionsCount() const { return executions.size(); }



  friend std::ostream &operator<<(std::ostream &stream,
                           const PartialConstraintGraph &obj) {
    stream << "PartialConstraintGraph(" << std::endl;
      for (auto e : obj.getExecutions()) {
          stream << "  " << e ;
          stream << std::endl;
      }
     for (auto c : obj.getConstraints()) {
    	 stream << "  " << c ;
    	 stream << std::endl;
     }
    stream << ")" << std::endl;
    return stream;
  }

  friend bool operator ==(const PartialConstraintGraph & a1, const PartialConstraintGraph & a2) {


      bool constraints_identical = a1.constraints == a2.constraints;
      if (!constraints_identical) return false;

      bool executions_identical = a1.executions == a2.executions;
      if (!executions_identical) return false;

      return true;

  }

  friend bool operator !=(const PartialConstraintGraph & a1, const PartialConstraintGraph & a2) {
	  return not (a1 == a2);
  }

    std::string getDOT() const ;
    std::string getSVG() const;


    const std::vector<Execution>& getTopologicalOrder () const;


};



struct Algorithm2_statistics {

	size_t total_case1 = 0;
	size_t total_case2 = 0;
	size_t total_case3 = 0;

	static Algorithm2_statistics & getSingleton() {
		static Algorithm2_statistics current;
		return current;
	}
	void clear () {
		this->total_case1 = 0;
		this->total_case2 = 0;
		this->total_case3 = 0;
	}

	friend Algorithm2_statistics operator+(const Algorithm2_statistics&l, const Algorithm2_statistics& r) {
		Algorithm2_statistics res;
		res.clear();
		res.total_case1 = l.total_case1 + r.total_case1;
		res.total_case2 = l.total_case2 + r.total_case2;
		res.total_case3 = l.total_case3 + r.total_case3;
		return res;
	}
};


typedef std::function<PartialConstraintGraph(const LETModel &model, const PeriodicityVector& K)> GenerateExpansionFun;

std::pair<std::vector<Execution> , INTEGER_TIME_UNIT>  FindLongestPathLower(PartialConstraintGraph PKG);
std::pair<std::vector<Execution> , INTEGER_TIME_UNIT>  FindLongestPathUpper(PartialConstraintGraph PKG);




#endif /* SRC_INCLUDE_PARTIAL_CONSTRAINT_GRAPH_H_ */

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

#ifdef ULTRA_DEBUG
#define VERBOSE_PCG(m) VERBOSE_CUSTOM_DEBUG("PCG", m)
#else
#define VERBOSE_PCG(m) {}
#endif


class PartialConstraintGraph {

private:
    long task_count;
    std::set<Execution> executions;
    std::set<Constraint> constraints;
    bool dirty = false; // set to false everytime the graph change
    PartialConstraintGraph() {};
    std::map<Execution, std::set<Constraint>> inbounds;
    std::map<Execution, std::set<Constraint>> outbounds;

public:

    PartialConstraintGraph(long task_count) : task_count(task_count){
    }

  inline void add(Constraint c) {
    dirty = false;
	executions.insert(c.getSource());
    executions.insert(c.getDestination());
    constraints.insert(c);
    inbounds[c.getDestination()].insert(c);
    outbounds[c.getSource()].insert(c);
  };

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


    inline const std::set<Constraint>& getConstraints() const { return constraints; }
    //inline const std::set<Execution>& getExecutions() const { return executions; }

    inline size_t getConstraintsCount() const { return constraints.size(); }
    inline size_t getExecutionsCount() const { return executions.size(); }



  friend std::ostream &operator<<(std::ostream &stream,
                           const PartialConstraintGraph &obj) {
    stream << "PartialConstraintGraph(" << std::endl;
    for (auto c : obj.getConstraints()) {
    	 stream << "  " << c ;
    	 stream << std::endl;
    }
    stream << ")" << std::endl;
    return stream;
  }

  friend bool operator ==(const PartialConstraintGraph & a1, const PartialConstraintGraph & a2) {

      bool executions_identical = a1.executions == a2.executions;
      if (!executions_identical) return false;

      bool constraints_identical = a1.constraints == a2.constraints;
      if (!constraints_identical) return false;

      return true;

  }

  friend bool operator !=(const PartialConstraintGraph & a1, const PartialConstraintGraph & a2) {
	  return not (a1 == a2);
  }

    std::string getDOT();
    std::string getSVG();


    std::vector<Execution> topologicalOrder;
    const std::vector<Execution>& getTopologicalOrder () ;

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

void add_constraints (const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph);
void add_start_finish (const LETModel &model, const PeriodicityVector &K, PartialConstraintGraph& graph);

PartialConstraintGraph new_generate_partial_constraint_graph(const LETModel &model, const PeriodicityVector &K) ;
PartialConstraintGraph opt_new_generate_partial_constraint_graph(const LETModel &model, const PeriodicityVector &K) ;


std::pair<std::vector<Execution> , INTEGER_TIME_UNIT>  FindLongestPath(PartialConstraintGraph PKG);
std::pair<std::vector<Execution> , INTEGER_TIME_UNIT>  FindLongestPathLower(PartialConstraintGraph PKG);
std::pair<std::vector<Execution> , INTEGER_TIME_UNIT>  FindLongestPathUpper(PartialConstraintGraph PKG);

void add_upperbounds_constraints (const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph);
void add_lowerbounds_constraints (const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph);
void add_lowerupperbounds_constraints (const LETModel &model, const PeriodicityVector &K , const Dependency &d, PartialConstraintGraph& graph);
PartialConstraintGraph generate_partial_lowerbound_graph (const LETModel& model , const PeriodicityVector& K) ;
PartialConstraintGraph generate_partial_upperbound_graph (const LETModel& model , const PeriodicityVector& K) ;
PartialConstraintGraph generate_combined_partial_expansion_graph (const LETModel& model , const PeriodicityVector& K) ;


#endif /* SRC_INCLUDE_PARTIAL_CONSTRAINT_GRAPH_H_ */

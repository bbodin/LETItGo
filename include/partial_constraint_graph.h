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

#define NO_CONSTRAINT -1
#define FAKE_TASK_ID  -1



class Execution {
private:
private:
    EXECUTION_ID execId;
    TASK_ID taskId;
    EXECUTION_COUNT executionIndex;
public:
    Execution(EXECUTION_ID execId, TASK_ID taskId, EXECUTION_COUNT taskExecutionIndex)
            : execId(execId), taskId(taskId), executionIndex(taskExecutionIndex) {}

public:
    inline EXECUTION_COUNT getId() const {return execId;}
    inline TASK_ID getTaskId() const { return this->taskId; }
    inline TASK_ID getExecId() const { return this->executionIndex; }

    friend std::ostream &operator<<(std::ostream &stream, const Execution &obj) {
        stream << "Execution(id:" << obj.taskId << ", index:" << obj.executionIndex << ")";
        return stream;
    }
    inline friend bool operator<(const Execution &l, const Execution &r) {
        return std::tie(l.taskId, l.executionIndex) < std::tie(r.taskId, r.executionIndex);
    }

    inline friend bool operator>(const Execution &l, const Execution &r) {
        return std::tie(l.taskId, l.executionIndex) > std::tie(r.taskId, r.executionIndex);
    }

    inline friend bool operator ==(const Execution & a1, const Execution & a2) {
        return (a1.taskId == a2.taskId) and (a1.executionIndex == a2.executionIndex) ;
    }

};

enum WeightType {
    lower_wt, upper_wt
};

class Constraint {
    size_t index;
    Execution ei, ej;
    WEIGHT wLow, wUp;
    bool hasLow, hasUp;

public:
    Constraint(size_t index, Execution e1, Execution e2, WEIGHT wLow, WEIGHT wUp) : index(index), ei(e1), ej(e2), wLow(wLow) , wUp(wUp) , hasLow(wLow>=0), hasUp(wUp>=0) {}
    inline const Execution getSource() const { return ei; }
    inline const Execution getDestination() const { return ej; }
    [[nodiscard]] inline constexpr WEIGHT getWeight(const WeightType wt) const { if (wt == lower_wt) return wLow; else return wUp; }
    [[nodiscard]] inline constexpr bool   hasWeight(const WeightType wt) const { if (wt == lower_wt) return hasLow; else return hasUp; }

    inline friend bool operator<(const Constraint &l, const Constraint &r) {
        return std::tie(l.ei, l.ej, l.wLow, l.wUp) < std::tie(r.ei, r.ej, r.wLow, r.wUp);
    }

    inline friend bool operator>(const Constraint &l, const Constraint &r) {
        return std::tie(l.ei, l.ej, l.wLow, l.wUp) > std::tie(r.ei, r.ej, r.wLow, r.wUp);
    }

    inline friend std::ostream &operator<<(std::ostream &stream, const Constraint &obj) {
        stream << "Constraint(from:" << obj.getSource()
               << ", to:" << obj.getDestination() << ", lower:" << obj.getWeight(lower_wt) << ", upper:" << obj.getWeight(upper_wt)
               << ")";
        return stream;
    }
    inline friend bool operator ==(const Constraint & a1, const Constraint & a2) {
        return (a1.ei == a2.ei) and (a1.ej == a2.ej) and (a1.wLow == a2.wLow) and (a1.wUp == a2.wUp) ;
    }

};


class PartialConstraintGraph {

private: // cache
    mutable bool dirty = false; // set to true if a constraint mess with the topological order
    mutable std::vector<Execution> topologicalOrder;
    std::map<Execution, std::vector<Constraint>> inbounds;
    std::map<Execution, std::vector<Constraint>> outbounds;

private: // data

    std::map<Execution, size_t> executions2index;
    std::vector<Execution> executions;
    std::vector<Constraint> constraints;

    const PeriodicityVector periodicity_vector;
    const LETModel model;



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

private: // Constructor helpers

    inline void addExecution (TASK_ID tid, EXECUTION_COUNT executionCount) {
        Execution e (executions.size(), tid, executionCount);
        executions2index[e] = executions.size();
        executions.push_back(e);
    }

    inline void addConstraint (Execution e1, Execution e2, WEIGHT wLow, WEIGHT wUp) {
        if (executions2index[e1] > executions2index[e2]) {
            this->dirty = true;
            VERBOSE_FAILURE();
        }
        Constraint c (constraints.size(), e1, e2, wLow, wUp);
        constraints.push_back(c);
        inbounds[c.getDestination()].push_back(c);
        outbounds[c.getSource()].push_back(c);
    };

    void add_start_finish_constraints (const LETModel &model) ;
    void add_dependency_constraints (const LETModel &model, const PeriodicityVector &K , const Dependency &d) ;
    void add_dependency_constraints_seq (const LETModel &model, const PeriodicityVector &K , const Dependency &d) ;
    void add_dependency_constraints_par (const LETModel &model, const PeriodicityVector &K , const Dependency &d) ;

public: // Getters

    inline Execution getSource () const {
        auto index = executions2index.at(Execution (-1,-1,0));
        return executions[index];
    }
    inline Execution getTarget () const {
        auto index = executions2index.at(Execution (-1,-1,1));
        return executions[index];
    }

    inline Execution getExecution (EXECUTION_ID idx) const {
        return this->executions[idx];
    }

    inline Execution getExecution (TASK_ID tid, EXECUTION_COUNT exec) const {
        Execution tmp = Execution (-1,tid,exec);
        size_t idx = this->executions2index.at(tmp);
        return this->executions[idx];
    }

    const PeriodicityVector& get_periodicity_vector() const {
        return this->periodicity_vector;
    }

  inline const std::vector<Constraint>& getInputs(const Execution& e) const {
        static const std::vector<Constraint> empty_set = std::vector<Constraint>();
    if (inbounds.count(e))
      return inbounds.at(e);
    return empty_set;
  }

  inline const std::vector<Constraint>& getOutputs(const Execution& e) const {
      static const std::vector<Constraint> empty_set = std::vector<Constraint>();
    if (outbounds.count(e))
      return outbounds.at(e);
    return empty_set;
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

    std::string getDOT(WeightType wt) const ;
    std::string getSVG(WeightType wt) const;
    std::string getTikZ() const;

    std::string getAlphasAsLatex(DEPENDENCY_ID edgeId)const ;


    const std::vector<Execution>& getTopologicalOrder () const;


    const LETModel &getModel() const;

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

std::pair<std::vector<Execution> , INTEGER_TIME_UNIT>  FindLongestPath(const PartialConstraintGraph &PKG, const WeightType wt);




#endif /* SRC_INCLUDE_PARTIAL_CONSTRAINT_GRAPH_H_ */

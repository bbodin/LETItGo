/*
 * partial_constraint_graph.h
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */

#ifndef SRC_INCLUDE_PARTIAL_CONSTRAINT_GRAPH_H_
#define SRC_INCLUDE_PARTIAL_CONSTRAINT_GRAPH_H_

#include <ConstraintIterator.h>
#include <ExecutionIterator.h>
#include <InOutboundIterator.h>
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

#define FAKE_TASK_ID  -1




class PartialConstraintGraph {

private: // data

    std::vector<Constraint> constraints_matrix; // (ti -> tj) -> (ai -> aj) -> constraint
    const PeriodicityVector periodicity_vector;
    const LETModel model;
    ExecutionIndexCache execution_cache;



private: // protection against catastrophe
    PartialConstraintGraph() : execution_cache({}) {};

public: // Constructor

    /**
     * This is the constructor of PCG, for a given model and periodicity vector,
     * will generate the associated graph using a standard implementation.
     * @param model
     * @param K
     */
    PartialConstraintGraph(const LETModel& model , const PeriodicityVector& K) ;

private: // Constructor helpers

    inline void addConstraint (Execution e1, Execution e2, WEIGHT wLow, WEIGHT wUp) {

        if ((wLow == NO_CONSTRAINT) and (wUp == NO_CONSTRAINT)) return;

        constraintCounter ++ ;

        auto e1Idx = execution_cache.getExecutionIndex(e1);
        auto e2Indx = execution_cache.getExecutionIndex(e2);
        auto execCount = execution_cache.getExecutionCount();
        auto constraintIndex = e1Idx * execCount + e2Indx;

        Constraint c (constraintIndex, e1, e2, wLow, wUp);
        constraints_matrix.at(constraintIndex) = c;
    };

    void add_start_finish_constraints (const LETModel &model) ;
    void add_dependency_constraints (const LETModel &model, const PeriodicityVector &K , const Dependency &d) ;
    void add_dependency_constraints_seq (const LETModel &model, const PeriodicityVector &K , const Dependency &d) ;
    void add_dependency_constraints_par (const PeriodicityVector &K , const Dependency &d) ;

public: // Getters

    inline Execution getSource () const {
        return getExecution(-1,0);
    }
    inline Execution getTarget () const {
        return getExecution(-1,1);
    }

    inline EXECUTION_ID getExecutionId (Execution e) const {
        return this->execution_cache.getExecutionIndex(e);
    }

    inline Execution getExecution (EXECUTION_ID idx) const {
        return this->execution_cache.getExecution(idx);
    }

    inline Execution getExecution (TASK_ID tid, EXECUTION_COUNT exec) const {
        Execution tmp = Execution (-1,tid,exec);
        size_t idx = getExecutionId(tmp);
        Execution res = getExecution(idx);
        VERBOSE_ASSERT_EQUALS(res.getTaskId(), tid);
        VERBOSE_ASSERT_EQUALS(res.getExecId(), exec);
        return res;
    }

    const PeriodicityVector& get_periodicity_vector() const {
        return this->periodicity_vector;
    }

  inline InOutboundIterable getInputs(const Execution& e) const {
        return  InOutboundIterable(constraints_matrix, e, false);
  }

  inline InOutboundIterable getOutputs(const Execution& e) const {
      return  InOutboundIterable(constraints_matrix, e, true);
  }


    inline ConstraintIterable getConstraints() const { return ConstraintIterable(periodicity_vector, constraints_matrix);; }
    inline ExecutionIterable getExecutions() const { return  ExecutionIterable(periodicity_vector); }

    inline size_t getConstraintsCount() const { return constraintCounter; }
    inline size_t getExecutionsCount() const { return execution_cache.getExecutionCount(); }



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


    std::string getDOT(WeightType wt) const ;
    std::string getSVG(WeightType wt) const;
    std::string getTikZ() const;

    std::string getAlphasAsLatex(DEPENDENCY_ID edgeId)const ;


    ExecutionIterable getTopologicalOrder () const;


    const LETModel &getModel() const;

    int constraintCounter = 0;
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

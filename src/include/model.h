/*
 * model.h
 *
 *  Created on: Dec 29, 2020
 *      Author: toky
 */

#ifndef SRC_INCLUDE_MODEL_H_
#define SRC_INCLUDE_MODEL_H_

#include <set>
#include <vector>
#include <map>
#include <ostream>
#include <verbose.h>

typedef double    TIME_UNIT;
typedef long long INTEGER_TIME_UNIT;
typedef long long TASK_ID;

typedef long long EXECUTION_COUNT;
typedef long long COUNT_T;
typedef std::vector<EXECUTION_COUNT> PeriodicityVector;
typedef long long WEIGHT;



class Task {
private:
	TASK_ID           id;
	TIME_UNIT         r; // 	is the release date (the offset) of the first execution
	TIME_UNIT         C; //     is the worst-case execution time
	TIME_UNIT         D; //     is the relative deadline
	INTEGER_TIME_UNIT T; //     is the period

public:
	Task (TASK_ID id, TIME_UNIT r, TIME_UNIT C, TIME_UNIT D, INTEGER_TIME_UNIT T) : id(id), r(r),  C(C),  D(D),  T(T) {

	};

	INTEGER_TIME_UNIT getT() const { return T;}
	TIME_UNIT getC() const { return C;}
	TIME_UNIT getD() const { return D;}
	TIME_UNIT getr() const { return r;}

	friend bool operator<(const Task& l, const Task& r) {
	        return l.id < r.id;
	    }

};



class Dependency : private std::pair<TASK_ID,TASK_ID> {
public:
	Dependency (TASK_ID t1, TASK_ID t2) : std::pair<TASK_ID,TASK_ID> (t1,t2){}
	TASK_ID getFirst () const { return this->first; }
	TASK_ID getSecond () const { return this->second; }

	friend std::ostream & operator<<(std::ostream & stream, const Dependency & obj) {
		stream << "Dependency(from:" << obj.getFirst() << ", to:" << obj.getSecond() << ")";
		return stream;
	}

};


class LETModel  {

private :
	std::vector<Task>       TaskIdToTask             ;
	std::map<Task,TASK_ID>  TaskToTaskId             ;


	std::vector<Dependency> DependencyIdToDependency;
public :

	LETModel () : TaskIdToTask(), TaskToTaskId(), DependencyIdToDependency() {

	}

	TASK_ID addTask (TIME_UNIT r,TIME_UNIT D,TIME_UNIT T) {
		const TASK_ID id = TaskIdToTask.size();
		const Task t (id,r,D,D,T) ;
		TaskIdToTask.push_back(t);
		TaskToTaskId[t] = id;
		return id;
	}

	const std::vector<Task>& tasks () const {
		return TaskIdToTask;
	}


	const Task getTaskById (TASK_ID id) const {
		return TaskIdToTask[id];
	}

	const TASK_ID getTaskIdByTask (Task t) const {
		return TaskToTaskId.at(t);
	}

	void addDependency (TASK_ID t1, TASK_ID t2) {
		Dependency d (t1,t2);
		DependencyIdToDependency.push_back(d);
	}

	size_t getTaskCount () const {
		return TaskIdToTask.size();
	}

	const std::vector<Dependency>& dependencies () const {
		return DependencyIdToDependency;
	}

};


class Execution : public std::pair <TASK_ID, EXECUTION_COUNT> {
public :
	Execution (TASK_ID id, EXECUTION_COUNT e) :  std::pair <TASK_ID, EXECUTION_COUNT> (id,e) {}
public:
	TASK_ID getTaskId () const { return this->first;}



};

std::ostream & operator<<(std::ostream & stream, const Execution & obj) {
		stream << "Execution(id:" << obj.first << ", index:" << obj.second << ")";
		return stream;
}

class Constraint {
	Execution ei,ej;
	WEIGHT    w;
public:
	Constraint (Execution e1, Execution e2, WEIGHT w) : ei(e1), ej(e2), w(w) {}
	const Execution getSource() const {return ei;}
	const Execution getDestination() const {return ej;}
	const WEIGHT    getWeight() const {return w;}

	friend bool operator<(const Constraint& l, const Constraint& r) {
	        return std::tie(l.ei,l.ej,l.w) < std::tie(r.ei,r.ej,r.w);
	    }
};

std::ostream & operator<<(std::ostream & stream, const Constraint & obj) {
	stream << "Constraint(from:" << obj.getSource() << ", to:" << obj.getDestination() << ", weight:"<< obj.getWeight() << ")";
	return stream;
}

class PartialConstraintGraph {
	std::set<Execution> executions;
	std::vector<Constraint> constraints;
public:
	std::map<Execution, std::set<Constraint>> inbounds;
	std::map<Execution, std::set<Constraint>> outbounds;
public:
	inline void add (Constraint c) {
		executions.insert(c.getSource());
		executions.insert(c.getDestination());
		constraints.push_back(c);
		inbounds[c.getDestination()].insert(c);
		outbounds[c.getSource()].insert(c);
	};

	inline const std::set<Constraint> getInputs (Execution e) const {
		if (inbounds.count(e)) return inbounds.at(e);
		return std::set<Constraint>();
	}

	inline const std::set<Constraint> getOutputs (Execution e) const {
		if (outbounds.count(e)) return outbounds.at(e);
		return std::set<Constraint>();
	}

	inline const std::vector<Constraint> getConstraints () const {
		return constraints;
	}
	inline const std::set<Execution> getExecutions () const {
		return executions;
	}



};

std::ostream & operator<<(std::ostream & stream, const PartialConstraintGraph & obj) {
	stream << "PartialConstraintGraph(" << std::endl;
		for (auto c : obj.getConstraints()) {
			stream << "  " << c << std::endl;
		}
		stream << ")" << std::endl;
		return stream;
}



PeriodicityVector generate_unitary_periodicity_vector (const LETModel& model) {
	return PeriodicityVector(model.getTaskCount(), 1);
}

#endif /* SRC_INCLUDE_MODEL_H_ */

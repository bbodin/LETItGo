/*
 * model.h
 *
 *  Created on: Dec 29, 2020
 *      Author: toky
 */

#ifndef SRC_INCLUDE_MODEL_H_
#define SRC_INCLUDE_MODEL_H_

#include <verbose.h>

#include <map>
#include <ostream>
#include <set>
#include <vector>

typedef double TIME_UNIT;
typedef long INTEGER_TIME_UNIT;
typedef long TASK_ID; // TODO: Signed because -1 is a valid task id as in compute_age_latency, but should not.

typedef long EXECUTION_COUNT;
typedef long COUNT_T;
typedef long WEIGHT;

class ModelException : std::exception {
public :
  ModelException(std::string) {
    
  }
};

class Task {
private:
	TASK_ID id;
	TIME_UNIT r; // 	is the release date (the offset) of the first execution
	TIME_UNIT C; //     is the worst-case execution time
	TIME_UNIT D; //     is the relative deadline
	INTEGER_TIME_UNIT T; //     is the period

public:
	Task(TASK_ID id, TIME_UNIT r, TIME_UNIT C, TIME_UNIT D, INTEGER_TIME_UNIT T)
: id(id), r(r), C(C), D(D), T(T){

	};

	TASK_ID getId() const { return id; }
	INTEGER_TIME_UNIT getT() const { return T; }
	TIME_UNIT getC() const { return C; }
	TIME_UNIT getD() const { return D; }
	TIME_UNIT getr() const { return r; }

	friend bool operator<(const Task &l, const Task &r) { return l.id < r.id; }

	friend std::ostream &operator<<(std::ostream &stream, const Task &obj) {
		stream << "<Task id='" << obj.id << "' r='" << obj.r << "' C='" << obj.C << "' D='" << obj.D << "' T='" << obj.T << "'/>";
		return stream;
	}


	 friend bool operator ==(const Task & a1, const Task & a2) {
		 if (a1.id != a2.id) return false;
		 VERBOSE_ASSERT(a1.id == a2.id, "Comparing Task from two different LET.");
		 VERBOSE_ASSERT(a1.r == a2.r, "Comparing Task from two different LET.");
		 VERBOSE_ASSERT(a1.C == a2.C, "Comparing Task from two different LET.");
		 VERBOSE_ASSERT(a1.D == a2.D, "Comparing Task from two different LET.");
		 VERBOSE_ASSERT(a1.T == a2.T, "Comparing Task from two different LET.");
		 return true;
	 }


	 friend bool operator !=(const Task & a1, const Task & a2) {
		 return not (a1 == a2);
	 }
};






class Dependency : private std::pair<TASK_ID, TASK_ID> {
public:
	Dependency(TASK_ID t1, TASK_ID t2) : std::pair<TASK_ID, TASK_ID>(t1, t2) {}
	TASK_ID getFirst() const { return this->first; }
	TASK_ID getSecond() const { return this->second; }

	friend bool operator<(const Dependency &l, const Dependency &r) { return std::pair<TASK_ID, TASK_ID>(l) < std::pair<TASK_ID, TASK_ID>(r); }

	friend std::ostream &operator<<(std::ostream &stream, const Dependency &obj) {
		stream << "<Dependency from='" << obj.getFirst() << "' to='" << obj.getSecond()  << "'/>";
		return stream;
	}

	 friend bool operator ==(const Dependency & a1, const Dependency & a2) {
		 if (a1.first != a2.first) return false;
		 if (a1.second != a2.second) return false;
		 return true;
	 }

	 friend bool operator !=(const Dependency & a1, const Dependency & a2) {
		 return not (a1 == a2);
	 }
};







/**
 * TODO: Task id inside the Task, could remove the map
 */
class LETModel {

private:
	std::vector<Task> TaskIdToTask;
	std::map<Task, TASK_ID> TaskToTaskId;

	std::vector<Dependency> DependencyIdToDependency;

public:
	LETModel() : TaskIdToTask(), TaskToTaskId(), DependencyIdToDependency() {}
	LETModel(std::string xmldata) ;

    bool check_dependency  (Dependency d, long Vi, long Vj) const ;
    std::string getTikz(int duration = 25);
    std::string getXML();
	std::string getDOT();
	std::string getSVG();

	TASK_ID addTask(Task t) {
		const TASK_ID id = TaskIdToTask.size();
		const Task nt(id, t.getr(), t.getD(), t.getD(), t.getT());
		if (t.getId() != id) {
			VERBOSE_WARNING("The task with id " << t.getId() << " got a new id " << id);
		}
		TaskIdToTask.push_back(nt);
		TaskToTaskId[nt] = id;
		return id;
	}
	TASK_ID addTask(TIME_UNIT r, INTEGER_TIME_UNIT DandT) {
		return this->addTask(r, DandT, DandT);
	}
	TASK_ID addTask(TIME_UNIT r, INTEGER_TIME_UNIT D, INTEGER_TIME_UNIT T) {
		const TASK_ID id = TaskIdToTask.size();
		const Task t(id, r, D, D, T);
		TaskIdToTask.push_back(t);
		TaskToTaskId[t] = id;
		return id;
	}
	TASK_ID addTask(TIME_UNIT r, INTEGER_TIME_UNIT C,  INTEGER_TIME_UNIT D, INTEGER_TIME_UNIT T) {
		VERBOSE_ASSERT(C==D, "Unsupported case");
		const TASK_ID id = TaskIdToTask.size();
		const Task t(id, r, C, D, T);
		TaskIdToTask.push_back(t);
		TaskToTaskId[t] = id;
		return id;
	}

	const std::vector<Task> &tasks() const { return TaskIdToTask; }

	const Task getTaskById(TASK_ID id) const { return TaskIdToTask[id]; }

	TASK_ID getTaskIdByTask(Task t) const { return TaskToTaskId.at(t); }

	void addDependency(Dependency d) {
		this->addDependency(d.getFirst(), d.getSecond());
	}
	void addDependency(TASK_ID t1, TASK_ID t2) {

		if (TaskIdToTask.size() <= (size_t) std::max(t1,t2)) {
			throw ModelException("Task not found");
		}

		VERBOSE_ASSERT(TaskIdToTask.size() > (size_t) t1, "Task not found");
		VERBOSE_ASSERT(TaskIdToTask.size() > (size_t) t2, "Task not found");
		Dependency d(t1, t2);
		DependencyIdToDependency.push_back(d);
	}

	size_t getTaskCount() const { return TaskIdToTask.size(); }
	size_t getDependencyCount() const { return DependencyIdToDependency.size(); }

	const std::vector<Dependency> &dependencies() const {
		return DependencyIdToDependency;
	}

	friend std::ostream &operator<<(std::ostream &stream, const LETModel &obj) {
		stream << "<LETModel>" << std::endl;
		for (Task task : obj.TaskIdToTask) {
			stream << " " << task << std::endl;
		}
		for (Dependency deps : obj.DependencyIdToDependency) {
			stream << " " << deps << std::endl;
		}
		stream << "</LETModel>" << std::endl;
		return stream;
	}

	 friend bool operator ==(const LETModel & a1, const LETModel & a2) {

		 if (a1.TaskIdToTask.size() != a2.TaskIdToTask.size()) {
			 VERBOSE_ERROR("Different Size");
			 return false;

		 }
		 if (a1.DependencyIdToDependency.size() != a2.DependencyIdToDependency.size()) {return false;}

		 for (size_t i = 0 ; i < a1.TaskIdToTask.size(); i++) {
			 if (a1.TaskIdToTask[i] != a2.TaskIdToTask[i]) {
				 return false;
			 }
		 }
		 for (size_t i = 0 ; i < a1.DependencyIdToDependency.size(); i++) {
				 if (a1.DependencyIdToDependency[i] != a2.DependencyIdToDependency[i]) {
					 return false;
				 }
			 }

		 return true;

	  }

	  friend bool operator !=(const LETModel & a1, const LETModel & a2) {
		  return not (a1 == a2);
	  }
};






class Execution : public std::pair<TASK_ID, EXECUTION_COUNT> {
public:
	Execution(TASK_ID id, EXECUTION_COUNT e)
: std::pair<TASK_ID, EXECUTION_COUNT>(id, e) {}

public:
    TASK_ID getTaskId() const { return this->first; }
    TASK_ID getExecId() const { return this->second; }

	friend std::ostream &operator<<(std::ostream &stream, const Execution &obj) {
		stream << "Execution(id:" << obj.first << ", index:" << obj.second << ")";
		return stream;
	}

};








class Constraint {
	Execution ei, ej;
	WEIGHT w1,w2;
    bool hasw2;

public:
    Constraint(Execution e1, Execution e2, WEIGHT w) : ei(e1), ej(e2), w1(w) , w2(w), hasw2(false) {}
    Constraint(Execution e1, Execution e2, WEIGHT w1, WEIGHT w2) : ei(e1), ej(e2), w1(w1) , w2(w2) , hasw2(true) {}
	inline const Execution getSource() const { return ei; }
	inline const Execution getDestination() const { return ej; }
    inline WEIGHT getWeight() const { return w1; }
    inline WEIGHT getWeight1() const { return w1; }
    inline WEIGHT getWeight2() const { return w2; }
    inline WEIGHT hasWeight1() const { return true; }
    inline WEIGHT hasWeight2() const { return hasw2; }

	inline friend bool operator<(const Constraint &l, const Constraint &r) {
		return std::tie(l.ei, l.ej, l.w1, l.w2) < std::tie(r.ei, r.ej, r.w1, r.w2);
	}

	inline friend std::ostream &operator<<(std::ostream &stream, const Constraint &obj) {
		stream << "Constraint(from:" << obj.getSource()
        		   << ", to:" << obj.getDestination() << ", weight:" << obj.getWeight()
				   << ")";
		return stream;
	}
	inline friend bool operator ==(const Constraint & a1, const Constraint & a2) {
		return (a1.ei == a2.ei) and (a1.ej == a2.ej) and (a1.w1 == a2.w1) and (a1.w2 == a2.w2) ;
	}

};








#endif /* SRC_INCLUDE_MODEL_H_ */

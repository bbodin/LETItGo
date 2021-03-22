/*
 * model.h
 *
 *  Created on: Dec 29, 2020
 *      Author: toky
 */

#ifndef SRC_INCLUDE_MODEL_H_
#define SRC_INCLUDE_MODEL_H_

#include <map>
#include <ostream>
#include <set>
#include <vector>
#include <verbose.h>

typedef double TIME_UNIT;
typedef long INTEGER_TIME_UNIT;
typedef long TASK_ID; // TODO: Signed because -1 is a valid task id as in ComputeAgeLatency, but should not.

typedef long EXECUTION_COUNT;
typedef long COUNT_T;
typedef long WEIGHT;

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
		stream << "<Task id='" << obj.id << "' r='" << obj.r << "' C='" << obj.C << "' D='" << obj.D << "' T='" << obj.T << "'>";
		return stream;
	}
};






class Dependency : private std::pair<TASK_ID, TASK_ID> {
public:
	Dependency(TASK_ID t1, TASK_ID t2) : std::pair<TASK_ID, TASK_ID>(t1, t2) {}
	TASK_ID getFirst() const { return this->first; }
	TASK_ID getSecond() const { return this->second; }

	friend bool operator<(const Dependency &l, const Dependency &r) { return std::pair<TASK_ID, TASK_ID>(l) < std::pair<TASK_ID, TASK_ID>(r); }

	friend std::ostream &operator<<(std::ostream &stream, const Dependency &obj) {
		stream << "<Dependency from='" << obj.getFirst() << "' to='" << obj.getSecond()  << "'>";
		return stream;
	}
};








class LETModel {

private:
	std::vector<Task> TaskIdToTask;
	std::map<Task, TASK_ID> TaskToTaskId;

	std::vector<Dependency> DependencyIdToDependency;

public:
	LETModel() : TaskIdToTask(), TaskToTaskId(), DependencyIdToDependency() {}

	TASK_ID addTask(TIME_UNIT r, TIME_UNIT DandT) {
		return this->addTask(r, DandT, DandT);
	}
	TASK_ID addTask(TIME_UNIT r, TIME_UNIT D, TIME_UNIT T) {
		const TASK_ID id = TaskIdToTask.size();
		const Task t(id, r, D, D, T);
		TaskIdToTask.push_back(t);
		TaskToTaskId[t] = id;
		return id;
	}

	const std::vector<Task> &tasks() const { return TaskIdToTask; }

	const Task getTaskById(TASK_ID id) const { return TaskIdToTask[id]; }

	TASK_ID getTaskIdByTask(Task t) const { return TaskToTaskId.at(t); }

	void addDependency(TASK_ID t1, TASK_ID t2) {

		if (TaskIdToTask.size() <= (size_t) std::max(t1,t2)) {
			throw std::runtime_error("Task not found");
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
		for (auto task : obj.TaskIdToTask) {
			stream << " " << task << std::endl;
		}
		for (auto deps : obj.DependencyIdToDependency) {
			stream << " " << deps << std::endl;
		}
		stream << "</LETModel>" << std::endl;
		return stream;
	}
};






class Execution : public std::pair<TASK_ID, EXECUTION_COUNT> {
public:
	Execution(TASK_ID id, EXECUTION_COUNT e)
: std::pair<TASK_ID, EXECUTION_COUNT>(id, e) {}

public:
	TASK_ID getTaskId() const { return this->first; }

	friend std::ostream &operator<<(std::ostream &stream, const Execution &obj) {
		stream << "Execution(id:" << obj.first << ", index:" << obj.second << ")";
		return stream;
	}

};








class Constraint {
	Execution ei, ej;
	WEIGHT w;

public:
	Constraint(Execution e1, Execution e2, WEIGHT w) : ei(e1), ej(e2), w(w) {}
	const Execution getSource() const { return ei; }
	const Execution getDestination() const { return ej; }
	WEIGHT getWeight() const { return w; }

	friend bool operator<(const Constraint &l, const Constraint &r) {
		return std::tie(l.ei, l.ej, l.w) < std::tie(r.ei, r.ej, r.w);
	}

	friend std::ostream &operator<<(std::ostream &stream, const Constraint &obj) {
		stream << "Constraint(from:" << obj.getSource()
        		   << ", to:" << obj.getDestination() << ", weight:" << obj.getWeight()
				   << ")";
		return stream;
	}
	friend bool operator ==(const Constraint & a1, const Constraint & a2) {
		return (a1.ei == a2.ei) and (a1.ej == a2.ej) and (a1.w == a2.w) ;
	}

};








#endif /* SRC_INCLUDE_MODEL_H_ */

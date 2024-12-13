//
// Created by toky on 13/12/24.
//

#ifndef LETITGO_EXECUTION_H
#define LETITGO_EXECUTION_H

#include <model.h>
#include <periodicity_vector.h>
#include <verbose.h>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <utils.h>


class Execution {
private:
private:
    EXECUTION_ID execId;
    TASK_ID taskId;
    EXECUTION_COUNT executionIndex;
public:
    Execution() : execId(-1), taskId(-1), executionIndex(-1)  {}
    Execution(EXECUTION_ID execId, TASK_ID taskId, EXECUTION_COUNT taskExecutionIndex)
            : execId(execId), taskId(taskId), executionIndex(taskExecutionIndex) {}

public:
    inline EXECUTION_COUNT getId() const {return execId;}
    inline TASK_ID getTaskId() const { return this->taskId; }
    inline EXECUTION_COUNT getExecId() const { return this->executionIndex; }

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

#endif //LETITGO_EXECUTION_H

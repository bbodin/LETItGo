//
// Created by toky on 13/12/24.
//

#ifndef LETITGO_EXECUTIONITERATOR_H
#define LETITGO_EXECUTIONITERATOR_H


#include <Execution.h>
#include <iterator>
#include <vector>
#include <cassert>
class ExecutionIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Execution;
    using difference_type = std::ptrdiff_t;
    using pointer = const Execution*;
    using reference = const Execution&;

    ExecutionIterator(const std::vector<EXECUTION_COUNT>& K, EXECUTION_ID id, TASK_ID task_id, EXECUTION_COUNT exec_index)
            : K(K), id(id), task_id(task_id), exec_index(exec_index) {}

    ExecutionIterator& operator++() {
        if (task_id == -1 && exec_index == 0) {
            id = 1;
            task_id = 0; // Move to the first real task
            exec_index = 1;
        } else if (task_id == -1 && exec_index == 1) {
            id = -1;
            task_id = -1; // Move to the first real task
            exec_index = 2;
        } else {
            // Move to the next execution
            exec_index++;
            while (task_id < (TASK_ID)K.size() && exec_index > K[task_id]) {
                task_id++;
                exec_index = 1;
            }
            id++;
        }

        if (task_id == (TASK_ID)K.size()) {
            task_id = -1; // Move to the last special task
            exec_index = 1;
        }
        return *this;
    }

    Execution operator*() const {
        return Execution(id, task_id, exec_index);
    }

    bool operator==(const ExecutionIterator& other) const {
        return id == other.id && task_id == other.task_id && exec_index == other.exec_index ;
    }

    bool operator!=(const ExecutionIterator& other) const {
        return !(*this == other);
    }

private:
    const std::vector<EXECUTION_COUNT>& K; // Reference to the periodicity vector
    EXECUTION_ID id;
    TASK_ID task_id;
    EXECUTION_COUNT exec_index;
};

class ExecutionIterable {
public:
    ExecutionIterable(const std::vector<EXECUTION_COUNT>& K) : K(K) {}

    ExecutionIterator begin() const {
        return ExecutionIterator(K, 0, -1, 0); // Start from Execution(-1, 0)
    }

    ExecutionIterator end() const {
        return ExecutionIterator(K, -1, -1, 2); // End iterator, Execution(-1, 1) as the last
    }

    std::size_t size() const {
        // Total size: 1 for Execution(-1, 0), 1 for Execution(-1, 1), and sum of K
        return 2 + std::accumulate(K.begin(), K.end(), 0);
    }

private:
    const std::vector<EXECUTION_COUNT>& K; // Reference to the periodicity vector
};


class ExecutionIndexCache {
public:
    ExecutionIndexCache(const PeriodicityVector& K) : cumulative_sizes(K.size() + 1, 0) {
        // Compute the cumulative sizes of K
        for (std::size_t i = 0; i < K.size(); ++i) {
            cumulative_sizes[i + 1] = cumulative_sizes[i] + K[i];
        }
    }
    Execution getExecution(std::size_t index) const {
        // Special cases for Execution(-1, 0) and Execution(-1, 1)
        if (index == 0) {
            return Execution(index, -1, 0);
        }

        auto last_index = cumulative_sizes.back() + 1;
        if (index == last_index) {
            return Execution(index, -1, 1);
        }


        // Find the task_id using the cumulative_sizes
        auto it = std::upper_bound(cumulative_sizes.begin(), cumulative_sizes.end(), index - 1);
        if (it == cumulative_sizes.end()) {
            throw std::out_of_range("Index out of range");
        }

        TASK_ID task_id = static_cast<TASK_ID>(std::distance(cumulative_sizes.begin(), it) - 1);
        int exec_index = static_cast<int>(index - cumulative_sizes[task_id]);

        return Execution(index, task_id, exec_index);
    }

    std::size_t getExecutionCount() const {
        // Total count is 2 for the special cases (-1, 0) and (-1, 1) plus the sum of K
        return 2 + cumulative_sizes.back();
    }

    std::size_t getExecutionIndex(const Execution& exec) const {
        // Special cases for Execution(-1, 0) and Execution(-1, 1)
        if (exec.getTaskId() == -1) {
            if (exec.getExecId() == 0) {
                return 0; // Execution(-1, 0) is the first element
            }
            if (exec.getExecId() == 1) {
                return getExecutionCount() - 1; // Execution(-1, 1) is the last element
            }
            throw std::invalid_argument("Invalid Execution with task_id -1 and exec_index > 1");
        }

        // General case for task_id >= 0
        TASK_ID task_id = exec.getTaskId();
        EXECUTION_COUNT exec_index = exec.getExecId();

        // Validate task_id and exec_index
        if (task_id < 0 || task_id >= (TASK_ID)cumulative_sizes.size() - 1) {
            throw std::out_of_range("Invalid task_id in Execution");
        }
        if (exec_index <= 0 || exec_index > (EXECUTION_COUNT)(cumulative_sizes[task_id + 1] - cumulative_sizes[task_id])) {
            throw std::out_of_range("Invalid exec_index in Execution");
        }

        // Calculate the index:
        // - Add the first two special executions (-1, 0) and (-1, 1)
        // - Use the cumulative sizes to get the starting index for the task
        std::size_t index = cumulative_sizes[task_id] + exec_index ;
        return index;
    }

private:
    std::vector<std::size_t> cumulative_sizes; // Cumulative sizes of K
};


#endif //LETITGO_EXECUTIONITERATOR_H

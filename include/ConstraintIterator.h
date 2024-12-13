//
// Created by toky on 13/12/24.
//

#ifndef LETITGO_CONSTRAINTITERATOR_H
#define LETITGO_CONSTRAINTITERATOR_H


#include <Constraint.h>
#include <iterator>
#include <vector>
#include <cassert>

class ConstraintIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Constraint;
    using difference_type = std::ptrdiff_t;
    using pointer = const Constraint*;
    using reference = const Constraint&;

    ConstraintIterator(const std::vector<Constraint>& constraints, size_t index)
            : constraints(constraints) , index(index) {}

    ConstraintIterator& operator++() {

        index++;
        while(index < constraints.size() && !constraints.at(index).hasWeight(upper_wt)) {
            index++;
        }

        return *this;
    }

    Constraint operator*() const {
        return constraints.at(index);
    }

    bool operator==(const ConstraintIterator& other) const {
        return index == other.index;
    }

    bool operator!=(const ConstraintIterator& other) const {
        return !(*this == other);
    }

private:
    const std::vector<Constraint>&      constraints;
    size_t index;
};

class ConstraintIterable {
public:
    ConstraintIterable(const std::vector<EXECUTION_COUNT>& K,
                       const std::vector<Constraint>&      constraints) : K(K) , constraints (constraints) {}

    ConstraintIterator begin() const {
        size_t index = 0;
        while((index < constraints.size()) && !constraints.at(index).hasWeight(upper_wt)) {
            index++;
        }
        return ConstraintIterator(constraints, index);
    }

    ConstraintIterator end() const {
        return ConstraintIterator(constraints, constraints.size());
    }

private:
    const std::vector<EXECUTION_COUNT>& K;
    const std::vector<Constraint>&      constraints;
};



#endif //LETITGO_CONSTRAINTITERATOR_H

#ifndef LETITGO_INOUTBOUNDITERATOR_H
#define LETITGO_INOUTBOUNDITERATOR_H

#include <Constraint.h>
#include <vector>
#include <iterator>

class InOutboundIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Constraint;
    using difference_type = std::ptrdiff_t;
    using pointer = const Constraint*;
    using reference = const Constraint&;

    // Constructor
    InOutboundIterator(const std::vector<Constraint>& constraints, size_t index, const Execution& execution, bool isSource)
            : constraints(constraints), index(index), execution(execution), isSource(isSource) {
        advanceToNextValid();
    }

    // Increment operator
    InOutboundIterator& operator++() {
        index++;
        advanceToNextValid();
        return *this;
    }

    // Dereference operator
    Constraint operator*() const {
        return constraints.at(index);
    }

    // Equality operator
    bool operator==(const InOutboundIterator& other) const {
        return index == other.index;
    }

    // Inequality operator
    bool operator!=(const InOutboundIterator& other) const {
        return !(*this == other);
    }

private:
    const std::vector<Constraint>& constraints;
    size_t index;
    Execution execution;
    bool isSource;

    // Move index to the next valid constraint
    void advanceToNextValid() {
        while (index < constraints.size()) {
            const Constraint& constraint = constraints.at(index);
            if ((isSource && constraint.getSource() == execution) ||
                (!isSource && constraint.getDestination() == execution)) {
                break;
            }
            index++;
        }
    }
};

class InOutboundIterable {
public:
    InOutboundIterable(const std::vector<Constraint>& constraints, const Execution& execution, bool isSource)
            : constraints(constraints), execution(execution), isSource(isSource) {}

    InOutboundIterator begin() const {
        return InOutboundIterator(constraints, 0, execution, isSource);
    }

    InOutboundIterator end() const {
        return InOutboundIterator(constraints, constraints.size(), execution, isSource);
    }

private:
    const std::vector<Constraint>& constraints;
    Execution execution;
    bool isSource;
};

#endif // LETITGO_INOUTBOUNDITERATOR_H

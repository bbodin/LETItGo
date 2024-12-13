//
// Created by toky on 13/12/24.
//

#ifndef LETITGO_CONSTRAINT_H
#define LETITGO_CONSTRAINT_H

#include <ExecutionIterator.h>
#include <model.h>
#include <periodicity_vector.h>
#include <verbose.h>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <utils.h>


#define NO_CONSTRAINT -1

enum WeightType {
    lower_wt, upper_wt
};


class Constraint {
    size_t index;
    Execution ei, ej;
    WEIGHT wLow, wUp;
    bool hasLow, hasUp;

public:
    Constraint() :  index(0), ei(), ej(), wLow(NO_CONSTRAINT) , wUp(NO_CONSTRAINT) , hasLow(false), hasUp(false)  {}
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

#endif //LETITGO_CONSTRAINT_H

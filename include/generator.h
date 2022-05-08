/*
 * generator.h
 *
 *  Created on: Feb 12, 2021
 *      Author: toky
 */

#ifndef SRC_INCLUDE_GENERATOR_H_
#define SRC_INCLUDE_GENERATOR_H_

#include <model.h>
#include <periodicity_vector.h>
#include <functional>


enum LETDatasetType { unknown_dt, generic_dt, harmonic_dt, automotive_dt};

struct GeneratorRequest {
    size_t n;
    size_t m;
    size_t seed;
    LETDatasetType t;
    bool DiEqualTi;
    GeneratorRequest (size_t n,size_t m, size_t s, LETDatasetType t , bool DiEqualTi = false) : n(n), m(m), seed(s),  t(t), DiEqualTi(DiEqualTi) {}
    friend bool operator<(const GeneratorRequest& l,const GeneratorRequest& r) {
        return     (l.n  < r.n)
                   or (l.n == r.n and l.m  < r.m)
                   or (l.n == r.n and l.m == r.m and l.seed  < r.seed)
                   or (l.n == r.n and l.m == r.m and l.seed == r.seed and l.t < r.t)
                   or (l.n == r.n and l.m == r.m and l.seed == r.seed and l.t == r.t and l.DiEqualTi < r.DiEqualTi);
    }

    friend bool operator==(const GeneratorRequest& l,const GeneratorRequest& r) {
        return (l.n == r.n and l.m == r.m and l.seed == r.seed and l.t == r.t and l.DiEqualTi == r.DiEqualTi);
    }
    friend bool operator!=(const GeneratorRequest& l,const GeneratorRequest& r) {
        return  not (l == r);
    }
    friend std::ostream &operator<<(std::ostream &stream, const GeneratorRequest &obj) {
        stream << "<GeneratorCacheEntry t='" << obj.t << "' DEqTi='" << obj.DiEqualTi << "' n='" << obj.n << "' m='" << obj.m << "' s='" << obj.seed << "'>";
        return stream;
    }
};

typedef std::function<PeriodicityVector(LETModel &m, size_t seed)> PeriodicityVectorGenerationFunction;


inline LETDatasetType str2kind(std::string str) {
	if (str == "automotive") return LETDatasetType::automotive_dt;
	if (str == "harmonic") return LETDatasetType::harmonic_dt;
	if (str == "generic") return LETDatasetType::generic_dt;
	return LETDatasetType::unknown_dt;
};


LETModel generate_LET (GeneratorRequest r) ;

PeriodicityVector generate_random_periodicity_vector(const LETModel &model, size_t seed = 0);
PeriodicityVector generate_random_ni_periodicity_vector(const LETModel &model, size_t seed = 0);




class Generator {

	std::map <GeneratorRequest, LETModel> cache;

public:

	LETModel generate (GeneratorRequest r) {
		VERBOSE_ASSERT(r.seed > 0 , "The generator need positive seed");

		if (cache.count(r) == 0) {
			LETModel model = generate_LET (r);
			cache[r] = model;
		}

		return cache[r];
	}

	static Generator& getInstance () {
		static Generator singleton;
		return singleton;
	}

};




#endif /* SRC_INCLUDE_GENERATOR_H_ */

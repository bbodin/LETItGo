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
    GeneratorRequest (size_t n,size_t m, size_t s, LETDatasetType t ) : n(n), m(m), seed(s),  t(t) {}
    friend bool operator<(const GeneratorRequest& l,const GeneratorRequest& r) {
        return     (l.n  < r.n)
                   or (l.n == r.n and l.m  < r.m)
                   or (l.n == r.n and l.m == r.m and l.seed  < r.seed)
                   or (l.n == r.n and l.m == r.m and l.seed == r.seed and l.t < r.t);
    }

    friend bool operator==(const GeneratorRequest& l,const GeneratorRequest& r) {
        return (l.n == r.n and l.m == r.m and l.seed == r.seed and l.t == r.t);
    }
    friend bool operator!=(const GeneratorRequest& l,const GeneratorRequest& r) {
        return  not (l == r);
    }
    friend std::ostream &operator<<(std::ostream &stream, const GeneratorRequest &obj) {
        stream << "<GeneratorCacheEntry t='" << obj.t << "' n='" << obj.n << "' m='" << obj.m << "' s='" << obj.seed << "'>";
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
	LETModel generateAutomotive (size_t n, size_t m, size_t seed) {
        GeneratorRequest r (n, m , seed, LETDatasetType::automotive_dt);
		return this->generate(r);
	}
	LETModel generateHarmonic (size_t n, size_t m, size_t seed) {
        GeneratorRequest r (n, m , seed, LETDatasetType::harmonic_dt);
        return this->generate(r);
	}
	LETModel generateGeneric (size_t n, size_t m, size_t seed) {
        GeneratorRequest r (n, m , seed, LETDatasetType::generic_dt);
        return this->generate(r);
	}
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

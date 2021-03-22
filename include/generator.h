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


LETModel generate_LET (unsigned int n, unsigned int m, size_t seed = 0);
PeriodicityVector generate_random_periodicity_vector(const LETModel &model, size_t seed = 0);


struct GeneratorCacheEntry {
	size_t n;
	size_t m;
	size_t s;
	GeneratorCacheEntry (size_t n,size_t m,size_t s) : n(n),m(m),s(s) {}
	friend bool operator<(const GeneratorCacheEntry& l,const GeneratorCacheEntry& r) {
		return l.n < r.n or (l.n == r.n and l.m < r.m) or  (l.n == r.n and l.m == r.m and l.s < r.s);
	}
};

class Generator {

	std::map <GeneratorCacheEntry, LETModel> cache;

public:
	LETModel generate (size_t n, size_t m, size_t seed) {
		VERBOSE_ASSERT(seed > 0 , "The generator need positive seed");
		GeneratorCacheEntry entry(n,m,seed);
		if (!cache.count(entry)) {
			cache[entry] = generate_LET (n, m, seed);
		}

		return cache[entry];
	}

	static Generator& getInstance () {
		static Generator singleton;
		return singleton;
	}

};




#endif /* SRC_INCLUDE_GENERATOR_H_ */

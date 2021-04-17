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

typedef std::function<PeriodicityVector(LETModel &m, size_t seed)> PeriodicityVectorGenerationFunction;
typedef std::function<LETModel(unsigned int n, unsigned int m, size_t seed)> LETGenerationFunction;

enum LETDatasetType { unknown_dt, generic_dt, harmonic_dt, automotive_dt};
enum PeriodicityFactorType { unknown_pf, generic_pf, harmonic_pf };

inline LETDatasetType str2kind(std::string str) {
	if (str == "automotive") return LETDatasetType::automotive_dt;
	if (str == "harmonic") return LETDatasetType::harmonic_dt;
	if (str == "generic") return LETDatasetType::generic_dt;
	return LETDatasetType::unknown_dt;
};
//void GenerateAutomotiveTasks(LETModel& sample, unsigned int n, std::mt19937& gen);
//void GenerateHarmonicTasks(LETModel& sample, unsigned int n, std::mt19937& gen);
//void GenerateGenericTasks(LETModel& sample, unsigned int n, std::mt19937& gen) ;
//void GenerateUniformConnections(LETModel& sample, unsigned int m, std::mt19937& gen);

LETModel generate_Automotive_LET (unsigned int n, unsigned int m, size_t seed = 0);
LETModel generate_Harmonic_LET (unsigned int n, unsigned int m, size_t seed = 0);
LETModel generate_Generic_LET (unsigned int n, unsigned int m, size_t seed = 0);

PeriodicityVector generate_random_periodicity_vector(const LETModel &model, size_t seed = 0);
PeriodicityVector generate_random_ni_periodicity_vector(const LETModel &model, size_t seed = 0);


struct GeneratorCacheEntry {
	LETDatasetType t;
	size_t n;
	size_t m;
	size_t s;
	GeneratorCacheEntry (LETDatasetType t, size_t n,size_t m,size_t s) : t(t), n(n),m(m),s(s) {}
	friend bool operator<(const GeneratorCacheEntry& l,const GeneratorCacheEntry& r) {
		return     (l.n  < r.n)
				or (l.n == r.n and l.m  < r.m)
		        or (l.n == r.n and l.m == r.m and l.s  < r.s)
				or (l.n == r.n and l.m == r.m and l.s == r.s and l.t < r.t);
	}

	friend bool operator==(const GeneratorCacheEntry& l,const GeneratorCacheEntry& r) {
		return (l.n == r.n and l.m == r.m and l.s == r.s and l.t == r.t);
	}
	friend bool operator!=(const GeneratorCacheEntry& l,const GeneratorCacheEntry& r) {
		return  not (l == r);
	}
	friend std::ostream &operator<<(std::ostream &stream, const GeneratorCacheEntry &obj) {
		stream << "<GeneratorCacheEntry t='" << obj.t << "' n='" << obj.n << "' m='" << obj.m << "' s='" << obj.s << "'>";
		return stream;
	}
};

class Generator {

	std::map <GeneratorCacheEntry, LETModel> cache;

public:
	LETModel generateAutomotive (size_t n, size_t m, size_t seed) {
		return this->generate(LETDatasetType::automotive_dt, n , m , seed);
	}
	LETModel generateHarmonic (size_t n, size_t m, size_t seed) {
		return this->generate(LETDatasetType::harmonic_dt, n , m , seed);
	}
	LETModel generateGeneric (size_t n, size_t m, size_t seed) {
		return this->generate(LETDatasetType::generic_dt, n , m , seed);
	}
	LETModel generate (LETDatasetType t, size_t n, size_t m, size_t seed) {
		VERBOSE_ASSERT(seed > 0 , "The generator need positive seed");
		GeneratorCacheEntry entry(t, n, m, seed);

		if (cache.count(entry) == 0) {
			LETGenerationFunction fun = generate_Automotive_LET;
			switch (t) {
				case LETDatasetType::generic_dt : fun = generate_Generic_LET; break;
				case LETDatasetType::harmonic_dt : fun = generate_Harmonic_LET; break;
				case LETDatasetType::automotive_dt : fun = generate_Automotive_LET; break;
				default : fun = generate_Automotive_LET;
			}
			LETModel model = fun (n, m, seed);
			cache[entry] = model;
		}

		return cache[entry];
	}

	static Generator& getInstance () {
		static Generator singleton;
		return singleton;
	}

};




#endif /* SRC_INCLUDE_GENERATOR_H_ */

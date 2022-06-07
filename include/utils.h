/*
 * utils.h
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */

#ifndef SRC_INCLUDE_UTILS_H_
#define SRC_INCLUDE_UTILS_H_

#include <verbose.h>

#include <iostream>
#include <numeric>
#include <vector>
#include <map>


template <typename X> std::ostream & operator<<(std::ostream & stream, const std::vector<X> & obj) {
	bool first = true;
	stream << "[";
	for (auto x : obj) {if (!first) {stream << ",";}  stream << x; first = false;}
	stream << "]";
	return stream;
}


template <typename X, typename Y> std::ostream & operator<<(std::ostream & stream, const  std::pair<X,Y> & obj) {
	stream << obj.first << " "  << obj.second;
	return stream;
}


template <typename X, typename Y> std::ostream & operator<<(std::ostream & stream, const  std::map<X,Y> & obj) {
	stream << "map (" << std::endl ;
	for (auto key : obj) {
		stream << key.first << " : "  <<  key.second << std::endl;

	}
	stream << ")" << std::endl ;
	return stream;
}


template<typename entier>
std::vector<entier> get_factors(entier n) {
	std::vector<entier> factors = {1};
	entier z = 2;
    while (z * z <= n) {
        if (n % z == 0) {
        	factors.push_back(z);
            n /= z;
        } else {
            z++;
        }
    }
    if (n > 1) {
    	factors.push_back(n);
    }
    return factors;
}



template <typename entier>
std::pair<entier,entier> extended_euclide (entier _a, entier _b, entier _c) {

	// Kuṭṭaka, Aryabhata's algorithm for solving linear Diophantine equations in two unknowns
	const entier gcdab = std::gcd(_a,_b) ;

	VERBOSE_ASSERT (_c % gcdab == 0, "No solution");

	const entier a = _a/ gcdab;
	const entier b = _b/ gcdab;
	const entier c = _c/ gcdab;

	std::pair<entier,entier> r (a,b);
	std::pair<entier,entier> s (1,0);
	std::pair<entier,entier> t (0,1);

    while (r.second != 0) {
    	const entier quotient = r.first / r.second;
    	r = std::pair<entier,entier> ( r.second ,  r.first - quotient * r.second );
    	s = std::pair<entier,entier> ( s.second ,  s.first - quotient * s.second );
    	t = std::pair<entier,entier> ( t.second ,  t.first - quotient * t.second );
    }

    return std::pair<entier,entier>  ( (c * s.first), (c * t.first));

}



#endif /* SRC_INCLUDE_UTILS_H_ */

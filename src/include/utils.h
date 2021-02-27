/*
 * utils.h
 *
 *  Created on: Feb 18, 2021
 *      Author: toky
 */

#ifndef SRC_INCLUDE_UTILS_H_
#define SRC_INCLUDE_UTILS_H_


#include <iostream>
#include <numeric>
#include <vector>
#include <map>


template <typename X> std::ostream & operator<<(std::ostream & stream, const std::vector<X> & obj) {
	for (auto x : obj) stream << x << " ";
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


template <typename entier>
std::pair<entier,entier> extended_euclide (entier _a, entier _b, entier _c) {

	// Kuṭṭaka, Aryabhata's algorithm for solving linear Diophantine equations in two unknowns


	entier gcdab = std::gcd(_a,_b) ;

	VERBOSE_ASSERT (_c % gcdab == 0, "No solution");

	entier a = _a/ gcdab;
	entier b = _b/ gcdab;
	entier c = _c/ gcdab;

	std::pair<entier,entier> r (a,b);
	std::pair<entier,entier> s (1,0);
	std::pair<entier,entier> t (0,1);

    while (r.second != 0) {
    	entier quotient = r.first / r.second;
    	r = std::pair<entier,entier> ( r.second ,  r.first - quotient * r.second );
    	s = std::pair<entier,entier> ( s.second ,  s.first - quotient * s.second );
    	t = std::pair<entier,entier> ( t.second ,  t.first - quotient * t.second );
    }

    VERBOSE_INFO ( "A:" <<  _a << ", B:" <<  _b << ", C:" <<  _c );
    VERBOSE_INFO ( "gcdab:" <<  gcdab );
    VERBOSE_INFO ( "A:" <<  a << ", B:" <<  b << ", C:" <<  c );
    VERBOSE_INFO ( "Bézout coefficients:" <<  s.first << "," <<  t.first );
    VERBOSE_INFO ( "greatest common divisor:" <<  r.first);
    VERBOSE_INFO ( "quotients by the gcd:" <<  t.second<< "," <<   s.second);

    return std::pair<entier,entier>  ( (c * s.first), (c * t.first));

}




#endif /* SRC_INCLUDE_UTILS_H_ */

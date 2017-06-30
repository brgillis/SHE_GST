/*
 * common.h
 *
 *  Created on: 17 Apr 2015
 *      Author: brg
 */

#ifndef BRG_LENSING_COMMON_H_
#define BRG_LENSING_COMMON_H_

#include <complex>
#include <vector>

namespace IceBRG {

typedef double flt_t;
typedef long double long_flt_t;

typedef int int_t;
typedef long int long_int_t;
typedef short int short_int_t;

typedef unsigned int unsigned_int_t;
typedef long unsigned int long_unsigned_int_t;
typedef short unsigned int short_unsigned_int_t;

typedef std::complex<flt_t> complex_t;
typedef std::complex<long_flt_t> long_complex_t;

template<typename T>
using vector_t = std::vector<T>;

typedef vector_t<int_t> int_vector_t;
typedef vector_t<long_int_t> long_int_vector_t;
typedef vector_t<short_int_t> short_int_vector_t;

typedef vector_t<unsigned_int_t> unsigned_int_vector_t;
typedef vector_t<long_unsigned_int_t> long_unsigned_int_vector_t;
typedef vector_t<short_unsigned_int_t> short_unsigned_int_vector_t;

typedef vector_t<flt_t> flt_vector_t;
typedef vector_t<long_flt_t> long_flt_vector_t;

typedef vector_t<complex_t> complex_vector_t;
typedef vector_t<long_complex_t> long_complex_vector_t;

// Typedef for the signed version of size_t
typedef std::make_signed<size_t>::type ssize_t;
typedef ssize_t ssize_t;

typedef std::string str_t;

} // namespace IceBRG

// Global compiler directives
// Alter these by switching between #define and #undef
#if(1)

//#define _BRG_WARN_FOR_SAFE_FUNCTIONS_TRIGGERED_ // Warns if a function like "safe_d" prevents an error
// This may be expected or not an issue in some cases though,
// so just undef this for release builds if you're satisfied
// there's no actual problem.

#if(__cplusplus==201103L)
#define _BRG_USE_CPP_11_STD_
#else
#undef _BRG_USE_CPP_11_STD_
#endif

// Magic values
#if(1)

#ifndef MAX_STACK_DEPTH
#define MAX_STACK_DEPTH 100
#endif

// Defining pi to keep it short
constexpr IceBRG::flt_t pi = M_PI;

#ifndef MIN_DIVISOR
#define MIN_DIVISOR 1e-99
#endif

#ifndef SMALL_FACTOR
#define SMALL_FACTOR 1e-9
#endif

#ifndef FLT_ROUNDING_EPSILON
#define FLT_ROUNDING_EPSILON 10*FLT_EPSILON
#endif

#ifndef ROUNDING_EPSILON
#define ROUNDING_EPSILON 10*DBL_EPSILON
#endif

#ifndef DBL_MAX_PRECISION
#define DBL_MAX_PRECISION 14
#endif

#endif // Magic values

// Conditional defines
#if(1)

#ifdef _BRG_USE_CPP_11_STD_
#define BRG_UNIQUE_PTR std::unique_ptr
#define BRG_SHARED_PTR std::shared_ptr
#else
#define BRG_UNIQUE_PTR std::auto_ptr
#define BRG_SHARED_PTR boost::shared_ptr
#endif // #ifdef _BRG_USE_CPP_11_STD_

#ifndef NULL
#ifdef _BRG_USE_CPP_11_STD_
#define NULL nullptr
#else
#define NULL (void *)0
#endif // _BRG_USE_CPP_11_STD_
#endif // #ifndef NULL

#endif // Conditional defines

#endif

#endif /* BRG_LENSING_COMMON_H_ */

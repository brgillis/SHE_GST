/*
 * two_halo_term.hpp
 *
 *  Created on: 17 Apr 2015
 *      Author: brg
 */

#ifndef BRG_LENSING_TWO_HALO_TERM_FUNCTIONS_HPP_
#define BRG_LENSING_TWO_HALO_TERM_FUNCTIONS_HPP_

#include <cmath>

#include "SHE_GST_IceBRG_main/common.hpp"

namespace IceBRG { namespace two_halo_calcs {

// General functions

constexpr flt_t delta_c = 1.686;
constexpr flt_t A = 1.; // TODO Actually needs to be normalized, see Seljak 00
constexpr flt_t a = 0.707;
constexpr flt_t p = 0.3;

inline flt_t nu_prime(flt_t nu)
{
	return nu *= a;
}

inline flt_t nu_f_nu(flt_t nu)
{

	flt_t nu_p = nu_prime(std::move(nu));

	return A*(1+std::pow(nu_p,-p))*std::sqrt(nu_p)*std::exp(-nu_p/2);
}

inline flt_t b_of_nu(const flt_t & nu)
{
	return 1 + (nu-1)/delta_c + 2*p/(delta_c*(1+std::pow(nu_prime(nu),p)));
}

inline flt_t _W_R_k(const flt_t & R, const flt_t & k)
{
	return 4*M_PI/cube(k) * (std::sin(k*R) - k*R*std::cos(k*R));
}

} }

#endif /* BRG_LENSING_TWO_HALO_TERM_FUNCTIONS_HPP_ */

/**********************************************************************\
 @file constants.hpp
 ------------------

 This file stores the definitions of various physical and
 astrophysical constants, stored as variables. Key points:

 -H_0 is set to 70 km/s/Mpc. If this is not changed, all results should
 be assumed to be in h_70 units.
 -Cosmological values are based on best-fit WMAP + priors values
 from Hinshaw et al. 2012.

 **********************************************************************

 Copyright (C) 2016 brg

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

\**********************************************************************/

#ifndef ICEBRG_PHYSICS_CONSTANTS_HPP_
#define ICEBRG_PHYSICS_CONSTANTS_HPP_

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/math/misc_math.hpp"

namespace IceBRG
{

// Physical/mathematical constants with units

#ifdef _BRG_USE_UNITS_
const custom_unit_type<3,-2,-1,0,0> Gc = 6.67384e-11 * cube(m) / square(s) / kg; // In m^3 s^-2 kg^-1
const velocity_type c = IceBRG::unitconv::ctomps*mps;
const time_type t_present_day = 13.616*unitconv::Gyrtos*s;     // WMAP9 + priors
const inverse_time_type H_0 = 70*unitconv::kmtom/unitconv::stos/unitconv::Mpctom / IceBRG::second; // So all results will implicitly be in h_70 units
const flt_t h_0 = H_0 / (100*unitconv::kmtom/unitconv::stos/unitconv::Mpctom / IceBRG::second);
#else
constexpr flt_t Gc = 6.67384e-11; // In m^3 s^-2 kg^-1
constexpr flt_t c = IceBRG::unitconv::ctomps;
constexpr flt_t t_present_day = 13.616*unitconv::Gyrtos;     // WMAP9 + priors
constexpr flt_t H_0 = 70*unitconv::kmtom/unitconv::stos/unitconv::Mpctom / IceBRG::second; // So all results will implicitly be in h_70 units
constexpr flt_t h_0 = H_0 / (100*unitconv::kmtom/unitconv::stos/unitconv::Mpctom / IceBRG::second);
#endif

// Cosmological constants

constexpr flt_t Omega_m = 0.288; // WMAP9 + priors
constexpr flt_t Omega_r = 0.000086; // WMAP9 + priors
constexpr flt_t Omega_k = 0; // Assuming flat space
constexpr flt_t Omega_l = 1 - Omega_k - Omega_m - Omega_r;
constexpr flt_t Omega_b = 0.0472; // WMAP9 + priors
constexpr flt_t sigma_8 = 0.830; // WMAP9 + priors
constexpr flt_t n_s = 0.971;     // WMAP9 + priors

// Default values

constexpr flt_t default_c = 6.; // To help prevent crashes. Warning will be output
constexpr flt_t default_tau_factor = 2.;

} // namespace IceBRG



#endif // ICEBRG_PHYSICS_CONSTANTS_HPP_

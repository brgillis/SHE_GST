/**********************************************************************\
  @file unit_conversions.hpp

 **********************************************************************

 Copyright (C) 2012-2020 Euclid Science Ground Segment      

 This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General    
 Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)    
 any later version.    

 This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied    
 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more    
 details.    

 You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to    
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

\**********************************************************************/

#ifndef _BRG_UNIT_CONVERSIONS_HPP_INCLUDED_
#define _BRG_UNIT_CONVERSIONS_HPP_INCLUDED_

#include "SHE_GST_IceBRG_main/common.hpp"

namespace IceBRG{ namespace unitconv {
// All unit conversions are exact (to the limit of flt_t variables) unless noted

// Distance
// Default unit: meter (m)
constexpr flt_t mtom = 1;
constexpr flt_t mtomm = 1e3;
constexpr flt_t mmtom = 1 / mtomm;
constexpr flt_t mtocm = 1e2;
constexpr flt_t cmtom = 1 / mtocm;
constexpr flt_t mtoum = 1e6;
constexpr flt_t umtom = 1 / mtoum;
constexpr flt_t mtonm = 1e9;
constexpr flt_t nmtom = 1 / mtonm;
constexpr flt_t mtoangstrom = 1e10;
constexpr flt_t angstromtom = 1 / mtoangstrom;
constexpr flt_t mtokm = 1e-3;
constexpr flt_t kmtom = 1 / mtokm;
constexpr flt_t ltyrtom = 9460730472580800;
constexpr flt_t mtoltyr = 1 / ltyrtom;
constexpr flt_t AUtom = 149597870700;
constexpr flt_t mtoAU = 1 / AUtom;
constexpr flt_t pctom = AUtom * 648000 / pi;
constexpr flt_t mtopc = 1 / pctom;
constexpr flt_t kpctom = 1000 * pctom;
constexpr flt_t mtokpc = 1 / kpctom;
constexpr flt_t Mpctom = 1000000 * pctom;
constexpr flt_t mtoMpc = 1 / Mpctom;
constexpr flt_t mitom = 1609.344;
constexpr flt_t mtomi = 1 / mitom;
constexpr flt_t Mmitom = 1e6 * mitom;
constexpr flt_t mtoMmi = 1 / Mmitom;
constexpr flt_t fttom = 0.3048;
constexpr flt_t mtoft = 1 / fttom;
constexpr flt_t intom = .0254;
constexpr flt_t mtoin = 1 / intom;
constexpr flt_t ydtom = 0.9144;
constexpr flt_t mtoyd = 1 / ydtom;

// Time
// Default unit: second (s)
constexpr flt_t stos = 1;
constexpr flt_t stocs = 1e2;
constexpr flt_t cstos = 1 / stocs;
constexpr flt_t stoms = 1e3;
constexpr flt_t mstos = 1 / stoms;
constexpr flt_t stous = 1e6;
constexpr flt_t ustos = 1 / stous;
constexpr flt_t stons = 1e9;
constexpr flt_t nstos = 1 / stons;
constexpr flt_t mintos = 60;
constexpr flt_t stomin = 1 / mintos;
constexpr flt_t hrtos = mintos * 60;
constexpr flt_t stohr = 1 / hrtos;
constexpr flt_t daytos = hrtos * 24;
constexpr flt_t stoday = 1 / daytos;
constexpr flt_t weektos = daytos * 7;
constexpr flt_t stoweek = 1 / weektos;
constexpr flt_t yrtos = daytos * 365.25; // Julian year - Astronomic definition (exact)
constexpr flt_t stoyr = 1 / yrtos;
constexpr flt_t monthtos = yrtos / 12; // Mean month length for Julian year
constexpr flt_t stomonth = 1 / monthtos;
constexpr flt_t kyrtos = yrtos * 1e3;
constexpr flt_t stokyr = 1 / kyrtos;
constexpr flt_t Myrtos = yrtos * 1e6;
constexpr flt_t stoMyr = 1 / Myrtos;
constexpr flt_t Gyrtos = yrtos * 1e9;
constexpr flt_t stoGyr = 1 / Gyrtos;

// Velocity
// Default units: meters per second (mps)
constexpr flt_t mpstomps = 1;
constexpr flt_t mpstokmps = 1e-3;
constexpr flt_t kmpstomps = 1 / mpstokmps;
constexpr flt_t ctomps = 299792458;
constexpr flt_t mpstoc = 1 / ctomps;
constexpr flt_t mpstomiphr = mtomi / stohr;
constexpr flt_t miphr = 1 / mpstomiphr;

// Mass
// Default unit: kilogram (kg)
constexpr flt_t kgtokg = 1;
constexpr flt_t kgtogm = 1e3;
constexpr flt_t gmtokg = 1 / kgtogm;
constexpr flt_t Mearthtokg = 5.9736e24; // Approximate
constexpr flt_t kgtoMearth = 1 / Mearthtokg; // Approximate
constexpr flt_t Msuntokg = 1.9891e30; // Approximate
constexpr flt_t kgtoMsun = 1 / Msuntokg; // Approximate
constexpr flt_t kgtottMsun = kgtoMsun * 1e-10; // Approximate
constexpr flt_t ttMsuntokg = 1 / kgtottMsun; // Approximate

// Temperature
// Default unit: Kelvin (K)
constexpr flt_t KtoK = 1;
constexpr flt_t KtodegF = 1.8;
constexpr flt_t degCtoK = KtoK;
constexpr flt_t KtodegC = 1 / degCtoK;
constexpr flt_t degFtoK = 1 / KtodegF;
constexpr flt_t degCtodegF = KtodegF;
constexpr flt_t degFtodegC = degFtoK;
constexpr flt_t KtodegR = KtodegF;
constexpr flt_t degRtoK = degFtoK;
constexpr flt_t degCtodegR = KtodegF;
constexpr flt_t degRtodegC = degFtoK;

// Angle
// Default unit: radian (rad)
constexpr flt_t radtorad = 1;
constexpr flt_t degtorad = pi / 180;
constexpr flt_t radtodeg = 1 / degtorad;
constexpr flt_t degtoamin = 60;
constexpr flt_t amintodeg = 1 / degtoamin;
constexpr flt_t amintoasec = 60;
constexpr flt_t asectoamin = 1 / amintoasec;
constexpr flt_t asectodeg = asectoamin * amintodeg;
constexpr flt_t degtoasec = 1 / asectodeg;
constexpr flt_t amintorad = amintodeg * degtorad;
constexpr flt_t radtoamin = 1 / amintorad;
constexpr flt_t asectorad = asectodeg * degtorad;
constexpr flt_t radtoasec = 1 / asectorad;

// Charge
// Default unit: Coulomb (C)
constexpr flt_t CtoC = 1;
constexpr flt_t Ctoesu = 6.241509324e18; // Approximate
constexpr flt_t esutoC = 1 / Ctoesu; // Approximate

} } // namespace IceBRG::unitconv



#endif /* _BRG_UNIT_CONVERSIONS_HPP_INCLUDED_ */

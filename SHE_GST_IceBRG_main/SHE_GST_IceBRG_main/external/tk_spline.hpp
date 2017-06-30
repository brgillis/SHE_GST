/**
 * @file tk_spline.h
 *
 * simple cubic spline interpolation library without external
 * dependencies
 *
 * Split into .h and .cpp by Bryan Gillis July 16, 2014
 *
 * ---------------------------------------------------------------------
 * Copyright (C) 2011, 2014 Tino Kluge (ttk448 at gmail.com)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------
 *
 */

// body file: brg/math/interpolator/tk_spline.cpp

#ifndef _tk_spline_h
#define _tk_spline_h

#include <vector>

#include "SHE_GST_IceBRG_main/utility.hpp"

namespace tk {

// band matrix solver
class band_matrix {
private:
   std::vector< ::IceBRG::flt_vector_t > m_upper;  // upper band
   std::vector< ::IceBRG::flt_vector_t > m_lower;  // lower band
public:
   band_matrix() {};                             // constructor
   band_matrix(::IceBRG::int_t dim, ::IceBRG::int_t n_u, ::IceBRG::int_t n_l);       // constructor
   ~band_matrix() {};                            // destructor
   void resize(::IceBRG::int_t dim, ::IceBRG::int_t n_u, ::IceBRG::int_t n_l);      // init with dim,n_u,n_l
   ::IceBRG::int_t dim() const;                             // matrix dimension
   ::IceBRG::int_t num_upper() const {
      return m_upper.size()-1;
   }
   ::IceBRG::int_t num_lower() const {
      return m_lower.size()-1;
   }
   // access operator
   ::IceBRG::flt_t & operator () (::IceBRG::int_t i, ::IceBRG::int_t j);            // write
   ::IceBRG::flt_t   operator () (::IceBRG::int_t i, ::IceBRG::int_t j) const;      // read
   // we can store an additional diogonal (in m_lower)
   ::IceBRG::flt_t& saved_diag(::IceBRG::int_t i);
   ::IceBRG::flt_t  saved_diag(::IceBRG::int_t i) const;
   void lu_decompose();
   ::IceBRG::flt_vector_t r_solve(const ::IceBRG::flt_vector_t & b) const;
   ::IceBRG::flt_vector_t l_solve(const ::IceBRG::flt_vector_t & b) const;
   ::IceBRG::flt_vector_t lu_solve(const ::IceBRG::flt_vector_t & b,
                                bool is_lu_decomposed=false);

};


// spline interpolation
class spline {
private:
   ::IceBRG::flt_vector_t m_x,m_y;           // x,y coordinates of points
   // interpolation parameters
   // f(x) = a*(x-x_i)^3 + b*(x-x_i)^2 + c*(x-x_i) + y_i
   ::IceBRG::flt_vector_t m_a,m_b,m_c,m_d;
public:
   void set_points(const ::IceBRG::flt_vector_t & x,
                   const ::IceBRG::flt_vector_t & y, bool cubic_spline=true);
   ::IceBRG::flt_t operator() (::IceBRG::flt_t x) const;
};

} // namespace tk

#endif /* _tk_spline_h */

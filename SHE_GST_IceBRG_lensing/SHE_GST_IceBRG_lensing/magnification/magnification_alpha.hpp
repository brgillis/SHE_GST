/**********************************************************************\
 @file magnification_alpha.h
 ------------------

 File containing a function for "alpha" used in magnification. That is,
 the local slope of the number count.

 **********************************************************************

 Copyright (C) 2014  Bryan R. Gillis

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


// body file: magnification_alpha.cpp


#ifndef _BRG_MAGNIFICATION_ALPHA_H_INCLUDED_
#define _BRG_MAGNIFICATION_ALPHA_H_INCLUDED_

#include "SHE_GST_IceBRG_main/common.hpp"

namespace IceBRG {

flt_t magnification_alpha(const flt_t & m, const flt_t & z);

} // namespace IceBRG



#endif // _BRG_MAGNIFICATION_ALPHA_H_INCLUDED_

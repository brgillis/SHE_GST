/**********************************************************************\
 @file galaxy_morphology.cpp
 ------------------

 TODO <Insert file description here>

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cassert>


#include "IceBRG_main/Eigen.hpp"
#include "IceBRG_main/logging.hpp"
#include "IceBRG_main/math/random/random_functions.hpp"

#include "SHE_SIM/common.hpp"

#include "SHE_SIM/default_values.hpp"
#include "SHE_SIM/dependency_functions/morphology.hpp"

namespace SHE_SIM {

using namespace IceBRG;

// Static initialisation
#if(1)

const array_t<flt_t,num_bulge_classes,1> sersic_index_data::sm_coeffs = load_morphology_sm_coeffs();
const array_t<flt_t,num_bulge_classes,1> sersic_index_data::z_coeffs = load_morphology_z_coeffs();
const array_t<flt_t,num_bulge_classes,1> sersic_index_data::y_intercepts =
		load_morphology_y_intercepts();

const array_t<flt_t,num_bulge_classes,1> sersic_index_data::bulge_sersic_means =
		load_bulge_sersic_means();
const array_t<flt_t,num_bulge_classes,1> sersic_index_data::bulge_sersic_stds =
		load_bulge_sersic_stds();

#endif // End static initialisation

// Loading functions for static initialisation
#if(1)

array_t<flt_t,num_bulge_classes,1> load_morphology_sm_coeffs()
{
	array_t<flt_t,num_bulge_classes,1> res;

	res <<  1.49695144,
            0.58817801,
           -0.40911011,
           -0.57818587,
            0.30539142;

	return res;
}

array_t<flt_t,num_bulge_classes,1> load_morphology_z_coeffs()
{
	array_t<flt_t,num_bulge_classes,1> res;

	res << -1.90822080,
           -0.87745313,
           -0.36022461,
            0.78895037,
            0.71603493;

	return res;
}

array_t<flt_t,num_bulge_classes,1> load_morphology_y_intercepts()
{
	array_t<flt_t,num_bulge_classes,1> res;

	res << -16.49570234,
			-6.52302501,
             3.32521522,
			 4.11680201,
			-5.24419872;

	return res;
}

array_t<flt_t,num_bulge_classes,1> load_bulge_sersic_means()
{
	array_t<flt_t,num_bulge_classes,1> res;

	res << 3.29313486,
           1.90371314,
           1.03420396,
           0.64977613,
           2.31558550;

	return res;
}

array_t<flt_t,num_bulge_classes,1> load_bulge_sersic_stds()
{
	array_t<flt_t,num_bulge_classes,1> res;

	res << 1.23736668,
           0.85641805,
           0.41139250,
           0.25539489,
           1.84302273;

	return res;
}

#endif // End loading functions for static initialisation


flt_t generate_bulge_class( flt_t const & stellar_mass, flt_t const & redshift, gen_t & rng )
{
	flt_t l10_sm = std::log10(stellar_mass);

	// Calculate t values for each bulge classification, where t is monotonic
	// increasing with probability
	auto ts =
			l10_sm * sersic_index_data::sm_coeffs +
			redshift * sersic_index_data::z_coeffs
			+ sersic_index_data::y_intercepts;

	flt_t (*pf)(flt_t) = &std::exp;

	array_t<flt_t,num_bulge_classes,1> probs = 1. / (1 + (-ts).unaryExpr(pf));

	probs /= probs.sum();

	flt_t p = drand(rng);

	// Check if it's the unknown type first
	if( p > 1-probs[num_bulge_classes-1] )
	{
		return num_bulge_classes-1.;
	}
	else
	{
		flt_t res = 0.;
		int i;
		for(i=0; i<num_bulge_classes-1; ++i)
		{
			if(p < probs[i])
			{
				res = i + p/probs[i] - 0.5;
				break;
			}
			else
			{
				p -= probs[i];
			}
		}
		assert(i<num_bulge_classes-1);
		return res;
	}
}

flt_t generate_sersic_index_from_apparent_mag_vis( flt_t const & , gen_t &  )
{
  ICEBRG_WARN_TRIVIAL() << "Dummy function generate_sersic_index_from_apparent_mag_vis used.";

	return dv::sersic_index;
}

flt_t generate_sersic_index_from_bulge_class( flt_t const & bulge_class, gen_t & rng )
{
	flt_t mean;
	flt_t stddev;

	if(bulge_class>num_bulge_classes-1.5)
	{
		mean = sersic_index_data::bulge_sersic_means[num_bulge_classes-1];
		stddev = sersic_index_data::bulge_sersic_stds[num_bulge_classes-1];
	}
	else
	{
		int_t il = static_cast<int_t>(bulge_class+0.5);
		if(il>=num_bulge_classes-3) il = num_bulge_classes-3;
		flt_t d = bulge_class+0.5-il;

		mean = sersic_index_data::bulge_sersic_means[il]*(1-d);
		stddev = sersic_index_data::bulge_sersic_stds[il]*(1-d);
		mean += sersic_index_data::bulge_sersic_means[il+1]*d;
		stddev += sersic_index_data::bulge_sersic_stds[il+1]*d;
	}

	flt_t sersic_index = trunc_Gaus_rand( mean, stddev, dv::sersic_index_min,
			dv::sersic_index_max, rng);

	return sersic_index;
}

flt_t generate_bulge_fraction( flt_t const & , flt_t const & , gen_t &  )
{
	ICEBRG_WARN_TRIVIAL() << "Dummy function generate_bulge_fraction used.";

	return dv::bulge_fraction;
}

flt_t get_bulge_class_from_fraction( flt_t const & bulge_frac )
{
	flt_t bulge_class = 3.5 - 4.*bulge_frac;

	return bulge_class;
}

flt_t get_bulge_fraction_from_class( flt_t const & bulge_class )
{
	if(bulge_class>=num_bulge_classes-1.5)
	{
		return dv::bulge_fraction;
	}
	flt_t bulge_frac = 3.5/4. - bulge_class/4.;

	// Enforce bounds
	if( bulge_frac > 1. )
	{
		bulge_frac = 1;
	}
	else if( bulge_frac < 0. )
	{
		bulge_frac = 0;
	}

	return bulge_frac;
}

flt_t get_bulge_axis_ratio( flt_t const & sersic_index )
{
	if( sersic_index > dv::bulge_axis_ratio_n_cutoff )
	{
		return dv::bulge_axis_ratio_high_n;
	}
	else
	{
		return dv::bulge_axis_ratio_low_n;
	}
}

flt_t get_bulge_ellipticity( flt_t const & bulge_axis_ratio, flt_t const & tilt )
{
	flt_t int_r_square = IceBRG::square(bulge_axis_ratio);

	flt_t sin2_tilt = IceBRG::square(std::sin(tilt*IceBRG::unitconv::degtorad));
	flt_t cos2_tilt = 1-sin2_tilt;

	flt_t r = std::sqrt(int_r_square*sin2_tilt + cos2_tilt);

	flt_t bulge_ellipticity = (1-r)/(1+r);

	return bulge_ellipticity;
}


} // namespace SHE_SIM

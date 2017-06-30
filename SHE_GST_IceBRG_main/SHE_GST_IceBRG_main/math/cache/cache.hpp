/**********************************************************************\
  @file cache.hpp

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

#ifndef _BRG_CACHE_HPP_INCLUDED_
#define _BRG_CACHE_HPP_INCLUDED_

#ifndef BRG_CACHE_ND_NAME_SIZE
#define BRG_CACHE_ND_NAME_SIZE 9 // Needs an end character, so will only actually allow 8 chars
#endif

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/Eigen.hpp"
#include "SHE_GST_IceBRG_main/error_handling.hpp"
#include "SHE_GST_IceBRG_main/file_access/ascii_table.hpp"
#include "SHE_GST_IceBRG_main/file_access/open_file.hpp"
#include "SHE_GST_IceBRG_main/file_access/trim_comments.hpp"
#include "SHE_GST_IceBRG_main/file_system.hpp"
#include "SHE_GST_IceBRG_main/globals.hpp"
#include "SHE_GST_IceBRG_main/math/misc_math.hpp"
#include "SHE_GST_IceBRG_main/math/safe_math.hpp"

#include "SHE_GST_IceBRG_main/units/units.hpp"

// Macro definitions

// SPCP: "Static Polymorphic Const Pointer"
// The constness isn't actually enforced, but this is for the reader's understanding
#define SPCP(name) static_cast<const name*>(this)

// SPP: "Static Polymorphic Pointer"
#define SPP(name) static_cast<name*>(this)

#define ICEBRG_STRINGIFY(a) #a

#define DECLARE_BRG_CACHE(class_name,name_base,Tin,Tout) \
class class_name : public IceBRG::brg_cache<class_name,Tin,Tout> \
{ \
private: \
 \
    static Tin _min_1_, _max_1_, _step_1_; \
	static IceBRG::ssize_t _resolution_1_; \
	static IceBRG::array_t<Tout> _results_; \
 \
	static IceBRG::short_int_t _is_monotonic_; \
 \
	static IceBRG::str_t _file_name_; \
	static IceBRG::int_t _version_number_; \
 \
    static IceBRG::short_int_t _num_alive_; \
 \
	static bool _loaded_, _initialised_; \
 \
	friend class IceBRG::brg_cache<class_name,Tin,Tout>; \
 \
protected: \
 \
	IceBRG::str_t _name_base() const \
	{ \
		char c_str_name_base[BRG_CACHE_ND_NAME_SIZE] = #name_base; \
		return c_str_name_base; \
	} \
 \
	Tout _calculate( Tin const & in_param ) const; \
 \
	void _load_cache_dependencies() const; \
 \
	void _unload_cache_dependencies() const; \
 \
 	bool _critical_load() const; \
}; \

#define DEFINE_BRG_CACHE(class_name,Tin,Tout, \
		init_min,init_max,init_step, \
		calc_method, \
		dependency_loading, \
		dependency_unloading) \
	 \
	Tin class_name::_min_1_ = init_min; \
	Tin class_name::_max_1_ = init_max; \
	Tin class_name::_step_1_ = init_step; \
	IceBRG::ssize_t class_name::_resolution_1_ = 0; \
	 \
	IceBRG::short_int_t class_name::_is_monotonic_ = 0; \
	 \
	bool class_name::_loaded_ = false; \
	bool class_name::_initialised_ = false; \
	 \
	IceBRG::str_t class_name::_file_name_ = ""; \
	IceBRG::int_t class_name::_version_number_ = 3; \
	 \
	IceBRG::short_int_t class_name::_num_alive_ = 0; \
	 \
	IceBRG::array_t<Tout> class_name::_results_; \
	 \
	Tout class_name::_calculate( Tin const & in_param ) const \
	{ \
		calc_method \
	} \
	void class_name::_load_cache_dependencies() const \
	{ \
		dependency_loading \
	} \
	void class_name::_unload_cache_dependencies() const \
	{ \
		dependency_unloading \
	} \
	bool class_name::_critical_load() const \
	{ \
		if(static_cast<const class_name*>(this)->_loaded_) return false; \
 \
		static_cast<const class_name*>(this)->_load_cache_dependencies(); \
 \
		bool bad_result = false; \
		_Pragma( ICEBRG_STRINGIFY(omp critical( load_brg_cache_##class_name  ) ) ) \
		{ \
			try \
			{ \
				static_cast<const class_name*>(this)->_load(); \
			} \
			catch(const std::exception &e) \
			{ \
				handle_error_message(e.what()); \
				bad_result = true; \
			} \
		} \
		return bad_result; \
	}

namespace IceBRG
{

template<typename name, typename Tin=flt_t, typename Tout=flt_t>
class brg_cache
{
private:

	// Private variables
#if (1)

    static Tin _min_1_, _max_1_, _step_1_;
	static IceBRG::ssize_t _resolution_1_;
	static IceBRG::array_t<Tout> _results_;

	static IceBRG::short_int_t _is_monotonic_;

	static IceBRG::str_t _file_name_;
	static IceBRG::int_t _version_number_;

	static bool _loaded_, _initialised_;

#endif // Private variables

	// Private methods
#if (1)
	void _init() const
	{
		// We check for initialisation twice due to the critical section here.
		// It's expensive to enter, and we don't want to do anything inside it more than once,
		// so we check whether we need to both once outside it and once inside it.
		if(SPCP(name)->_initialised_) return;

		#ifdef _OPENMP
		#pragma omp critical(init_brg_cache)
		#endif
		if(!SPCP(name)->_initialised_)
		{
			SPCP(name)->_resolution_1_ = (ssize_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) / safe_d(SPCP(name)->_step_1_)) + 1, 2);
			SPCP(name)->_file_name_ = SPCP(name)->_name_base() + "_cache.bin";
			SPCP(name)->_version_number_ = 3; // This should be changed when there are changes to this code
			SPCP(name)->_num_alive_ = 0;

			SPCP(name)->_initialised_ = true;
		}
	}
	bool _critical_load() const
	{
		return true;
	}
	str_t _current_file_name() const
	{
		return join_path(globals::workdir,SPCP(name)->_file_name_);
	}
	void _unload() const
	{
		SPCP(name)->_loaded_ = false;
		set_zero(SPCP(name)->_results_);
	}
	void _calc_if_necessary() const
	{
		if(SPCP(name)->_loaded_) return;

		// Load any caches we depend, in case this might get calculated there
		SPCP(name)->_load_cache_dependencies();

		if(!SPCP(name)->_loaded_) SPCP(name)->_calc();
		return;
	}
	void _calc() const
	{

		// Test that range is sane
		if ( ( SPCP(name)->_max_1_ <= SPCP(name)->_min_1_ ) || ( value_of(SPCP(name)->_step_1_) <= 0 ) )
		{
			throw std::runtime_error("ERROR: Bad range passed to brg_cache::_calc() for " +
					SPCP(name)->_name_base() + "\n");
		}

		// Print a message that we're generating the cache
		handle_notification("Generating " + SPCP(name)->_current_file_name() + ". This may take some time.");

		// Set up data
		SPCP(name)->_resolution_1_ = (ssize_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) /
				safe_d(SPCP(name)->_step_1_)) + 1, 2);
		SPCP(name)->_results_.resize(SPCP(name)->_resolution_1_ );

		// Calculate data
		bool bad_result = false;

		#ifdef _OPENMP
		#pragma omp parallel for schedule(dynamic)
		#endif
		for ( ssize_t i = 0; i < SPCP(name)->_resolution_1_; i++ )
		{
			Tout result = 0;
			Tin x = SPCP(name)->_min_1_ + static_cast<flt_t>(i)*SPCP(name)->_step_1_;
			try
			{
				result = SPCP(name)->_calculate(x);
			}
			catch(const std::exception &e)
			{
				handle_error_message(e.what());
				bad_result = true;
			}
			SPCP(name)->_results_[i] = result;
		}

		if(bad_result) throw std::runtime_error("One or more calculations failed in generating cache " +
				SPCP(name)->_name_base());

		// Check if it's monotonic increasing or decreasing

		if(std::is_sorted(begin(SPCP(name)->_results_),end(SPCP(name)->_results_),std::less<Tout>()))
			SPCP(name)->_is_monotonic_ = 1;
		else if(std::is_sorted(begin(SPCP(name)->_results_),end(SPCP(name)->_results_),std::greater<Tout>()))
			SPCP(name)->_is_monotonic_ = -1;
		else
			SPCP(name)->_is_monotonic_ = 0;

		SPCP(name)->_loaded_ = true;

		// Unload cache dependencies now
		SPCP(name)->_unload_cache_dependencies();

		// Print a message that we've finished generating the cache
		handle_notification("Finished generating " + SPCP(name)->_current_file_name() + "!");
	}
	void _output() const
	{
		std::ofstream out_file;
		str_t file_data;

		if ( !SPCP(name)->_loaded_ )
		{
			SPCP(name)->_calc_if_necessary();
		}

		open_bin_file_output( out_file, SPCP(name)->_current_file_name() );

		// Output name and version

		str_t file_name = SPCP(name)->_name_base();
		int_t file_version = SPCP(name)->_version_number_;

		out_file.write(file_name.c_str(),BRG_CACHE_ND_NAME_SIZE);
		out_file.write((char *)&file_version,sizeof(file_version));

		// Output range parameters

		decltype(value_of(SPCP(name)->_min_1_)) temp_in_1;
		const std::streamsize in1_size = sizeof(temp_in_1);

		temp_in_1 = value_of(SPCP(name)->_min_1_);
		out_file.write((char *)&temp_in_1,in1_size);
		temp_in_1 = value_of(SPCP(name)->_max_1_);
		out_file.write((char *)&temp_in_1,in1_size);
		temp_in_1 = value_of(SPCP(name)->_step_1_);
		out_file.write((char *)&temp_in_1,in1_size);

		// Output data

		decltype(value_of(SPCP(name)->_results_[0])) temp_out;
		const std::streamsize out_size = sizeof(temp_out);

		ssize_t i_1=0;

		while ( i_1 < SPCP(name)->_resolution_1_ )
		{
			temp_out = value_of(SPCP(name)->_results_[i_1]);
			out_file.write((char *)&temp_out,out_size);

			++i_1;
		}

		out_file.close();
		out_file.clear();
	}
#endif // Private methods

protected:

	// Protected methods
	// These are made protected instead of private so base classes can overload them
#if (1)

	/// Long calculation function, which is used to generate the cache; must be overloaded by each
	/// child.
	Tout _calculate(Tin const & x) const;

	/// The default name (without extension) for the cache file; should be unique for each cache.
	str_t _name_base() const;

	/// This function should be overloaded to call each cache of the same dimensionality as
	/// this cache, which this depends upon in calculation. This is necessary in order to avoid critical
	/// sections of the same name being called recursively.
	void _load_cache_dependencies() const
	{
	}


	void _load() const
	{
		std::ifstream in_file;
		str_t file_data;
		bool need_to_calc = false;
		int_t loop_counter = 0;

		if ( SPCP(name)->_loaded_ )
			return;

		do
		{
			if ( loop_counter >= 2 )
			{
				throw std::runtime_error("Infinite loop detected trying to load " + SPCP(name)->_current_file_name() + " in IceBRG::brg_cache_2d.\n");
			}
			else
			{
				loop_counter++;
			}
			need_to_calc = false;

			try
			{
				open_bin_file_input( in_file, SPCP(name)->_current_file_name() );
			}
			catch(const std::exception &e)
			{
				need_to_calc = true;
			}
			if(need_to_calc)
			{
				SPCP(name)->_calc_if_necessary();
				SPCP(name)->_output();
				SPCP(name)->_unload();
				continue;
			}

			// Check that it has the right name and version

			char file_name[BRG_CACHE_ND_NAME_SIZE];
			int_t file_version = std::numeric_limits<int_t>::max();

			in_file.read(file_name,BRG_CACHE_ND_NAME_SIZE);
			in_file.read((char *)&file_version,sizeof(file_version));

			if( (!in_file) || (((str_t)file_name) != SPCP(name)->_name_base()) ||
					(file_version != SPCP(name)->_version_number_) )
			{
				need_to_calc = true;
				SPCP(name)->_calc_if_necessary();
				SPCP(name)->_output();
				SPCP(name)->_unload();
				continue;
			}
			// Load range parameters;

			decltype(value_of(SPCP(name)->_min_1_)) temp_in_1;
			const std::streamsize in1_size = sizeof(temp_in_1);

			in_file.read((char *)&temp_in_1,in1_size);
			SPCP(name)->_min_1_ = units_cast<Tin>(temp_in_1);
			in_file.read((char *)&temp_in_1,in1_size);
			SPCP(name)->_max_1_ = units_cast<Tin>(temp_in_1);
			in_file.read((char *)&temp_in_1,in1_size);
			SPCP(name)->_step_1_ = units_cast<Tin>(temp_in_1);

			// Set up data
			SPCP(name)->_resolution_1_ = (ssize_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) / safe_d(SPCP(name)->_step_1_)) + 1, 2);
			make_vector_default( SPCP(name)->_results_, SPCP(name)->_resolution_1_ );

			// Read in data

			// Initialise
			decltype(value_of(SPCP(name)->_results_[0])) temp_out;
			const std::streamsize out_size = sizeof(temp_out);
			ssize_t i_1=0;

			while ( ( !in_file.eof() ) && (in_file) && (i_1<SPCP(name)->_resolution_1_) )
			{
				in_file.read((char *)&temp_out,out_size);
				SPCP(name)->_results_[i_1] = units_cast<Tout>(temp_out);

				++i_1;
			}

			// Check that it was all read properly
			if ( (i_1 != SPCP(name)->_resolution_1_) || (!in_file) )
			{
				need_to_calc = true;
				SPCP(name)->_calc_if_necessary();
				SPCP(name)->_output();
				SPCP(name)->_unload();
				continue;
			}

		} while ( need_to_calc );

		// Finish up
		in_file.close();
		in_file.clear();

		// Check if it's monotonic
		if(std::is_sorted(begin(SPCP(name)->_results_),end(SPCP(name)->_results_),std::less<Tout>()))
			SPCP(name)->_is_monotonic_ = 1;
		else if(std::is_sorted(begin(SPCP(name)->_results_),end(SPCP(name)->_results_),std::greater<Tout>()))
			SPCP(name)->_is_monotonic_ = -1;
		else
			SPCP(name)->_is_monotonic_ = 0;

		SPCP(name)->_loaded_ = true;
	}

#endif // Protected methods

public:

	// Public methods
#if (1)

	/**
	 * Set the name of the cache file to use.
	 *
	 * @param new_name The name of the cache file to use
	 */
	void set_file_name( str_t const & new_name )
	{
		SPP(name)->_file_name_ = new_name;
		if ( SPCP(name)->_loaded_ )
		{
			SPCP(name)->_unload();
		}
	} // set_file_name()

	/**
	 * Set the range of the independent parameter for wish you want values to be
	 * cached.
	 *
	 * @param new_min The new minimum value
	 * @param new_max The new maximum value
	 * @param new_step The number of points at which to cache the results
	 */
	void set_range( Tin const & new_min, Tin const & new_max,
			Tin const & new_step)
	{
		SPP(name)->_min_1_ = new_min;
		SPP(name)->_max_1_ = new_max;
		SPP(name)->_step_1_ = new_step;

		SPCP(name)->_unload();
		SPCP(name)->_calc_if_necessary();
		SPCP(name)->_output();
	} // void set_range()

	/**
	 * Print the cached input and output values to an output stream.
	 *
	 * @param out The output stream you wish to print the cached values to.
	 */
	template<typename otype>
	void print( otype & out ) const
	{
		// Load if necessary
		if ( !SPCP(name)->_loaded_ )
		{
			SPCP(name)->_critical_load(SPCP(name)->_min_1_);
		}

		// Fill up header
		vector_t< str_t > header(3);
		header[0] = "#";
		header[1] = "x_1";
		header[2] = "y";

		// Fill up data
		vector_t< vector_t<str_t> > data(3);
		std::stringstream ss;
		for(ssize_t i_1=0; i_1<SPCP(name)->_resolution_1_; ++i_1)
		{
			data[0].push_back("");
			ss.str("");
			ss << value_of(SPCP(name)->_min_1_ + i_1*SPCP(name)->_step_1_);
			data[1].push_back(ss.str());
			ss.str("");
			ss << value_of(SPCP(name)->_results_[i_1]);
			data[2].push_back(ss.str());
		}

		print_table(out,data,header);
	}

	/**
	 * Get the result of the cached function for a given value.
	 *
	 * @param init_x The value for which you desired the cached result.
	 * @return The cached result for the input value.
	 */
	Tout get( const Tin & x ) const
	{
		Tin xlo, xhi;
		ssize_t x_i; // Lower nearby array point
		Tout result;

		// Load if necessary
		if ( !SPCP(name)->_loaded_ )
		{
			if ( SPCP(name)->_critical_load() )
			{
				throw std::runtime_error("ERROR: Could neither load " + SPCP(name)->_current_file_name() + " nor calculate in brg_cache::get()\n");
			}
		}

		x_i = (ssize_t)bound(0,
				( ( x - SPCP(name)->_min_1_ ) / SPCP(name)->_step_1_ ),
				SPCP(name)->_resolution_1_ - 2 );

		xlo = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * static_cast<flt_t>(x_i);
		xhi = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * static_cast<flt_t>( x_i + 1 );

		result = ( ( x - xlo ) * SPCP(name)->_results_[x_i + 1] + ( xhi - x ) * SPCP(name)->_results_[x_i] )
				/ SPCP(name)->_step_1_;

		return result;

	} // get()

	/**
	 * Get the independent parameter "x" from the corresponding dependent parameter "y". This only
	 * works if the function is monotonic increasing or monotonic decreasing.
	 *
	 * @param y The independent parameter of the cached function
	 * @return The corresponding independent parameter of the cached function
	 */
	Tin inverse_get( Tout const & y ) const
	{
		if ( !SPCP(name)->_loaded_ )
		{
			SPCP(name)->_critical_load();
		}

		// Check if it's possible to do an inverse get
		if((SPCP(name)->_is_monotonic_!=1)&&((SPCP(name)->_is_monotonic_!=-1)))
		{
			// Not a monotonic function. Inverse get isn't possible
			str_t err = "ERROR: Attempt to use inverse_get in cache for " + SPCP(name)->_current_file_name() +
					" for function which isn't monotonic.\n";
			throw std::runtime_error(err);
		}

		auto res_begin = begin(SPCP(name)->_results_);
		auto res_end = end(SPCP(name)->_results_);

		array_t<Tout> opp_results;

		flt_t res_sign;

		if(SPCP(name)->_is_monotonic_==1)
		{
			res_sign = 1;
		}
		else
		{
			// Use the opposite for lower_bound
			opp_results = -SPCP(name)->_results_;

			res_begin = begin(opp_results);
			res_end = end(opp_results);
			res_sign = -1;
		} // _is_monotonic == -1

		auto p_yhi = std::lower_bound(res_begin,res_end,res_sign*y,std::less<Tout>());

		if(p_yhi==res_begin)
			++p_yhi;
		if(p_yhi==res_end)
			--p_yhi;
		auto p_ylo = p_yhi - 1;

		Tout ylo = *p_ylo;
		Tout yhi = *p_yhi;

		ssize_t x_i = p_yhi - res_begin;

		Tin xlo = SPCP(name)->_min_1_ + (x_i-1)*SPCP(name)->_step_1_;
		Tin xhi = SPCP(name)->_min_1_ + x_i*SPCP(name)->_step_1_;

		Tin res = (xlo + ( xhi - xlo ) * ( res_sign*y - ylo ) / safe_d( yhi - ylo ));

		return res;

	}

	/// Load the cache, calculating if necessary
	void load() const
	{
		SPCP(name)->_critical_load();
	}

	/// Reload the cache, calculating if necessary.
	void reload() const
	{
		SPCP(name)->unload();
		SPCP(name)->_critical_load();
	}

	/// Unload the cache
	void unload() const
	{
		// Only safe to unload if this is the only one alive - another might be calculating
		if(SPCP(name)->_num_alive_!=1) return;

		#ifdef _OPENMP
		#pragma omp critical(unload_brg_cache_1d)
		#endif
		{
			if(SPCP(name)->_num_alive_==1)
				SPCP(name)->_unload();
		}
	}

	/// Recalculate function. Call if you want to overwrite a cache when something's changed in the code
	/// (for instance, the _calculate() function has been altered)
	void recalc() const
	{
		SPCP(name)->unload();
		SPCP(name)->_output();
	}

	// Constructor
	brg_cache()
	{
		SPP(name)->_init();

		#ifdef _OPENMP
		#pragma omp atomic
		#endif
		++(SPCP(name)->_num_alive_);
	}

	// Deconstructor
	virtual ~brg_cache()
	{
		#ifdef _OPENMP
		#pragma omp atomic
		#endif
		--(SPCP(name)->_num_alive_);
	}

#endif // Public methods

}; // class brg_cache

} // namespace IceBRG

// Undef macros
#undef SPP
#undef SPCP

#endif // __BRG_CACHE_HPP_INCLUDED__

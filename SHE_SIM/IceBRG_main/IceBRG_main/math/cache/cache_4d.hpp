/**********************************************************************\
  @file cache_4d.hpp

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

#ifndef _BRG_CACHE_4D_HPP_INCLUDED_
#define _BRG_CACHE_4D_HPP_INCLUDED_

#ifndef BRG_CACHE_ND_NAME_SIZE
#define BRG_CACHE_ND_NAME_SIZE 9 // Needs an end character, so will only actually allow 8 chars
#endif

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "IceBRG_main/common.hpp"

#include "../../error_handling.hpp"
#include "IceBRG_main/file_access/ascii_table.hpp"
#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/safe_math.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/utility.hpp"

// Macro definitions

// SPCP: "Static Polymorphic Const Pointer"
// The constness isn't actually enforced, but this is for the reader's understanding
#define SPCP(name) static_cast<const name*>(this)

// SPP: "Static Polymorphic Pointer"
#define SPP(name) static_cast<name*>(this)

#define ICEBRG_STRINGIFY(a) #a

#define DECLARE_BRG_CACHE_4D(class_name,name_base,Tin1,Tin2,Tin3,Tin4,Tout) \
class class_name : public IceBRG::brg_cache_4d<class_name,Tin1,Tin2,Tin3,Tin4,Tout> \
{ \
private: \
 \
    static Tin1 _min_1_, _max_1_, _step_1_; \
	static IceBRG::ssize_t _resolution_1_; \
    static Tin2 _min_2_, _max_2_, _step_2_; \
	static IceBRG::ssize_t _resolution_2_; \
    static Tin3 _min_3_, _max_3_, _step_3_; \
	static IceBRG::ssize_t _resolution_3_; \
    static Tin4 _min_4_, _max_4_, _step_4_; \
	static IceBRG::ssize_t _resolution_4_; \
	static IceBRG::array_t<IceBRG::array_t<IceBRG::array_t<IceBRG::array_t<Tout>>>> _results_; \
 \
	static IceBRG::str_t _file_name_; \
	static IceBRG::int_t _version_number_; \
 \
    static IceBRG::short_int_t _num_alive_; \
 \
	static bool _loaded_, _initialised_; \
 \
	friend class IceBRG::brg_cache_4d<class_name,Tin1,Tin2,Tin3,Tin4,Tout>; \
 \
protected: \
 \
	IceBRG::str_t _name_base() const \
	{ \
		char c_str_name_base[BRG_CACHE_ND_NAME_SIZE] = #name_base; \
		return c_str_name_base; \
	} \
 \
	Tout _calculate( Tin1 const & in_param_1, Tin2 const & in_param_2, \
			Tin3 const & in_param_3, Tin4 const & in_param_4 ) const; \
 \
	void _load_cache_dependencies() const; \
 \
	void _unload_cache_dependencies() const; \
 \
 	bool _critical_load() const; \
}; \

#define DEFINE_BRG_CACHE_4D(class_name,Tin1,Tin2,Tin3,Tin4,Tout, \
						    init_min_1,init_max_1,init_step_1, \
						    init_min_2,init_max_2,init_step_2, \
						    init_min_3,init_max_3,init_step_3, \
						    init_min_4,init_max_4,init_step_4, \
						    calc_method, \
                            dependency_loading, \
                            dependency_unloading) \
	Tin1 class_name::_min_1_ = init_min_1; \
	Tin1 class_name::_max_1_ = init_max_1; \
	Tin1 class_name::_step_1_ = init_step_1; \
	IceBRG::ssize_t class_name::_resolution_1_ = 0; \
	 \
	Tin2 class_name::_min_2_ = init_min_2; \
	Tin2 class_name::_max_2_ = init_max_2; \
	Tin2 class_name::_step_2_ = init_step_2; \
	IceBRG::ssize_t class_name::_resolution_2_ = 0; \
	 \
	Tin3 class_name::_min_3_ = init_min_3; \
	Tin3 class_name::_max_3_ = init_max_3; \
	Tin3 class_name::_step_3_ = init_step_3; \
	IceBRG::ssize_t class_name::_resolution_3_ = 0; \
	 \
	Tin3 class_name::_min_4_ = init_min_4; \
	Tin3 class_name::_max_4_ = init_max_4; \
	Tin3 class_name::_step_4_ = init_step_4; \
	IceBRG::ssize_t class_name::_resolution_4_ = 0; \
	 \
	bool class_name::_loaded_ = false; \
	bool class_name::_initialised_ = false; \
	 \
	IceBRG::str_t class_name::_file_name_ = ""; \
	IceBRG::int_t IceBRG::class_name::_version_number_ = 3; \
	 \
	IceBRG::short_int_t IceBRG::class_name::_num_alive_ = 0; \
	 \
	IceBRG::array_t<IceBRG::array_t<IceBRG::array_t<IceBRG::array_t<Tout>>>> class_name::_results_; \
	 \
	Tout class_name::_calculate( Tin1 const & in_param_1, Tin2 const & in_param_2, \
			Tin3 const & in_param_3, Tin4 const & in_param_4 ) const \
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

template<typename name, typename Tin1=flt_t, typename Tin2=flt_t, typename Tin3=flt_t,
		typename Tin4=flt_t, typename Tout=flt_t>
class brg_cache_4d
{
private:

	// Private variables
#if (1)

    static Tin1 _min_1_, _max_1_, _step_1_;
	static IceBRG::ssize_t _resolution_1_;
    static Tin2 _min_2_, _max_2_, _step_2_;
	static IceBRG::ssize_t _resolution_2_;
    static Tin3 _min_3_, _max_3_, _step_3_;
	static IceBRG::ssize_t _resolution_3_;
    static Tin3 _min_4_, _max_4_, _step_4_;
	static IceBRG::ssize_t _resolution_4_;
	static IceBRG::array_t<IceBRG::array_t<IceBRG::array_t<IceBRG::array_t<Tout>>>> _results_;

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
		#pragma omp critical(init_brg_cache_4d)
		#endif
		if(!SPCP(name)->_initialised_)
		{
			SPCP(name)->_resolution_1_ = (ssize_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) / safe_d(SPCP(name)->_step_1_)) + 1, 2);
			SPCP(name)->_resolution_2_ = (ssize_t) max( ( ( SPCP(name)->_max_2_ - SPCP(name)->_min_2_ ) / safe_d(SPCP(name)->_step_2_)) + 1, 2);
			SPCP(name)->_resolution_3_ = (ssize_t) max( ( ( SPCP(name)->_max_3_ - SPCP(name)->_min_3_ ) / safe_d(SPCP(name)->_step_3_)) + 1, 2);
			SPCP(name)->_resolution_4_ = (ssize_t) max( ( ( SPCP(name)->_max_4_ - SPCP(name)->_min_4_ ) / safe_d(SPCP(name)->_step_4_)) + 1, 2);
			SPCP(name)->_file_name_ = SPCP(name)->_name_base() + "_cache.bin";
			SPCP(name)->_version_number_ = 3; // This should be changed when there are changes to this code
			SPCP(name)->_num_alive_ = 0;

			SPCP(name)->_initialised_ = true;
		}

		#ifdef _OPENMP
		#pragma omp atomic
		#endif
		++(SPCP(name)->_num_alive_);
	}
	bool _critical_load() const
	{
		return true;
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
		if ( ( SPCP(name)->_max_1_ <= SPCP(name)->_min_1_ ) || ( SPCP(name)->_step_1_ <= 0 ) ||
				 ( SPCP(name)->_max_2_ <= SPCP(name)->_min_2_ ) || ( SPCP(name)->_step_2_ <= 0 ) ||
				 ( SPCP(name)->_max_3_ <= SPCP(name)->_min_3_ ) || ( SPCP(name)->_step_3_ <= 0 ) ||
				 ( SPCP(name)->_max_4_ <= SPCP(name)->_min_4_ ) || ( SPCP(name)->_step_4_ <= 0 ) )
		{
			throw std::runtime_error("Bad range passed to brg_cache_4d::_calc() for " + SPCP(name)->_name_base() + "\n");
		}

		// Load any caches we depend upon before the critical section
		SPCP(name)->_load_cache_dependencies();

		// Print a message that we're generating the cache
		handle_notification("Generating " + SPCP(name)->_file_name_ + ". This may take some time.");

		// Set up data
		SPCP(name)->_resolution_1_ = (ssize_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) / safe_d(SPCP(name)->_step_1_)) + 1, 2);
		SPCP(name)->_resolution_2_ = (ssize_t) max( ( ( SPCP(name)->_max_2_ - SPCP(name)->_min_2_ ) / safe_d(SPCP(name)->_step_2_)) + 1, 2);
		SPCP(name)->_resolution_3_ = (ssize_t) max( ( ( SPCP(name)->_max_3_ - SPCP(name)->_min_3_ ) / safe_d(SPCP(name)->_step_3_)) + 1, 2);
		SPCP(name)->_resolution_4_ = (ssize_t) max( ( ( SPCP(name)->_max_4_ - SPCP(name)->_min_4_ ) / safe_d(SPCP(name)->_step_4_)) + 1, 2);
		make_vector_default( SPCP(name)->_results_, SPCP(name)->_resolution_1_, SPCP(name)->_resolution_2_,
				SPCP(name)->_resolution_3_, SPCP(name)->_resolution_4_ );

		// Calculate data
		bool bad_result = false;

		#ifdef _OPENMP
		#pragma omp parallel for schedule(dynamic)
		#endif
		for ( ssize_t i_1 = 0; i_1 < SPCP(name)->_resolution_1_; ++i_1 )
		{
			Tin1 x_1 = SPCP(name)->_min_1_ + i_1*SPCP(name)->_step_1_;
			for( ssize_t i_2 = 0; i_2 < SPCP(name)->_resolution_2_; ++i_2)
			{
				Tin2 x_2 = SPCP(name)->_min_2_ + i_2*SPCP(name)->_step_2_;
				for( ssize_t i_3 = 0; i_3 < SPCP(name)->_resolution_3_; ++i_3)
				{
					Tin3 x_3 = SPCP(name)->_min_3_ + i_3*SPCP(name)->_step_3_;
					for( ssize_t i_4 = 0; i_4 < SPCP(name)->_resolution_4_; ++i_4)
					{
						Tin4 x_4 = SPCP(name)->_min_4_ + i_4*SPCP(name)->_step_4_;
						Tout result(0);
						try
						{
							result = SPCP(name)->_calculate(x_1, x_2, x_3, x_4);
						}
						catch(const std::exception &e)
						{
							bad_result = true;
						}
						SPCP(name)->_results_[i_1][i_2][i_3][i_4] = result;
					}
				}
			}
		}

		if(bad_result) throw std::runtime_error("One or more calculations in generating cache " + SPCP(name)->_file_name_ + " failed.");
		SPCP(name)->_loaded_ = true;

		// Unload cache dependencies now
		SPCP(name)->_unload_cache_dependencies();

		// Print a message that we've finished generating the cache
		handle_notification(str_t("Finished generating ") + SPCP(name)->_file_name_ + "!");
	}
	void _output() const
	{

		std::ofstream out_file;
		str_t file_data;

		if ( !SPCP(name)->_loaded_ )
		{
			SPCP(name)->_calc_if_necessary();
		}

		open_bin_file_output( out_file, SPCP(name)->_file_name_ );

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

		decltype(value_of(SPCP(name)->_min_2_)) temp_in_2;
		const std::streamsize in2_size = sizeof(temp_in_2);

		temp_in_2 = value_of(SPCP(name)->_min_2_);
		out_file.write((char *)&temp_in_2,in2_size);
		temp_in_2 = value_of(SPCP(name)->_max_2_);
		out_file.write((char *)&temp_in_2,in2_size);
		temp_in_2 = value_of(SPCP(name)->_step_2_);
		out_file.write((char *)&temp_in_2,in2_size);

		decltype(value_of(SPCP(name)->_min_3_)) temp_in_3;
		const std::streamsize in3_size = sizeof(temp_in_3);

		temp_in_3 = value_of(SPCP(name)->_min_3_);
		out_file.write((char *)&temp_in_3,in3_size);
		temp_in_3 = value_of(SPCP(name)->_max_3_);
		out_file.write((char *)&temp_in_3,in3_size);
		temp_in_3 = value_of(SPCP(name)->_step_3_);
		out_file.write((char *)&temp_in_3,in3_size);

		decltype(value_of(SPCP(name)->_min_4_)) temp_in_4;
		const std::streamsize in4_size = sizeof(temp_in_4);

		temp_in_4 = value_of(SPCP(name)->_min_4_);
		out_file.write((char *)&temp_in_4,in4_size);
		temp_in_4 = value_of(SPCP(name)->_max_4_);
		out_file.write((char *)&temp_in_4,in4_size);
		temp_in_4 = value_of(SPCP(name)->_step_4_);
		out_file.write((char *)&temp_in_4,in4_size);

		// Output data

		// Initialize

		decltype(value_of(SPCP(name)->_results_[0][0][0][0])) temp_out;
		const std::streamsize out_size = sizeof(temp_out);

		ssize_t i_1=0, i_2=0, i_3=0, i_4=0;

		while ( i_4<SPCP(name)->_resolution_4_ )
		{
			temp_out = value_of(SPCP(name)->_results_[i_1][i_2][i_3][i_4]);
			out_file.write((char *)&temp_out,out_size);

			++i_1;
			if(i_1==(SPCP(name)->_resolution_1_))
			{
				++i_2;
				if(i_2==(SPCP(name)->_resolution_2_))
				{
					++i_3;
					if(i_3==(SPCP(name)->_resolution_3_))
					{
						++i_4;
						i_3 = 0;
					}
					i_2 = 0;
				}
				i_1 = 0;
			}
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
	flt_t _calculate(const Tin1 & x_1, const Tin2 & x_2, const Tin3 & x_3,
			const Tin4 & x_4) const;

	/// The default name (without extension) for the cache file; should be unique for each cache.
	const str_t _name_base() const;

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
				throw std::runtime_error("ERROR: infinite loop detected trying to load " + SPCP(name)->_file_name_ + " in IceBRG::brg_cache_2.\n");
			}
			else
			{
				loop_counter++;
			}
			need_to_calc = false;
			try
			{
				open_bin_file_input( in_file, SPCP(name)->_file_name_ );
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
				continue;
			}
			// Load range parameters;

			decltype(value_of(SPCP(name)->_min_1_)) temp_in_1;
			const std::streamsize in1_size = sizeof(temp_in_1);

			in_file.read((char *)&temp_in_1,in1_size);
			SPCP(name)->_min_1_ = units_cast<Tin1>(temp_in_1);
			in_file.read((char *)&temp_in_1,in1_size);
			SPCP(name)->_max_1_ = units_cast<Tin1>(temp_in_1);
			in_file.read((char *)&temp_in_1,in1_size);
			SPCP(name)->_step_1_ = units_cast<Tin1>(temp_in_1);

			decltype(value_of(SPCP(name)->_min_2_)) temp_in_2;
			const std::streamsize in2_size = sizeof(temp_in_2);

			in_file.read((char *)&temp_in_2,in2_size);
			SPCP(name)->_min_2_ = units_cast<Tin2>(temp_in_2);
			in_file.read((char *)&temp_in_2,in2_size);
			SPCP(name)->_max_2_ = units_cast<Tin2>(temp_in_2);
			in_file.read((char *)&temp_in_2,in2_size);
			SPCP(name)->_step_2_ = units_cast<Tin2>(temp_in_2);

			decltype(value_of(SPCP(name)->_min_3_)) temp_in_3;
			const std::streamsize in3_size = sizeof(temp_in_3);

			in_file.read((char *)&temp_in_3,in3_size);
			SPCP(name)->_min_3_ = units_cast<Tin3>(temp_in_3);
			in_file.read((char *)&temp_in_3,in3_size);
			SPCP(name)->_max_3_ = units_cast<Tin3>(temp_in_3);
			in_file.read((char *)&temp_in_3,in3_size);
			SPCP(name)->_step_3_ = units_cast<Tin3>(temp_in_3);

			decltype(value_of(SPCP(name)->_min_4_)) temp_in_4;
			const std::streamsize in4_size = sizeof(temp_in_4);

			in_file.read((char *)&temp_in_4,in4_size);
			SPCP(name)->_min_4_ = units_cast<Tin4>(temp_in_4);
			in_file.read((char *)&temp_in_4,in4_size);
			SPCP(name)->_max_4_ = units_cast<Tin4>(temp_in_4);
			in_file.read((char *)&temp_in_4,in4_size);
			SPCP(name)->_step_4_ = units_cast<Tin4>(temp_in_4);

			// Set up data
			SPCP(name)->_resolution_1_ = (ssize_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) / safe_d(SPCP(name)->_step_1_)) + 1, 2);
			SPCP(name)->_resolution_2_ = (ssize_t) max( ( ( SPCP(name)->_max_2_ - SPCP(name)->_min_2_ ) / safe_d(SPCP(name)->_step_2_)) + 1, 2);
			SPCP(name)->_resolution_3_ = (ssize_t) max( ( ( SPCP(name)->_max_3_ - SPCP(name)->_min_3_ ) / safe_d(SPCP(name)->_step_3_)) + 1, 2);
			SPCP(name)->_resolution_4_ = (ssize_t) max( ( ( SPCP(name)->_max_4_ - SPCP(name)->_min_4_ ) / safe_d(SPCP(name)->_step_4_)) + 1, 2);
			make_vector_default( SPCP(name)->_results_, SPCP(name)->_resolution_1_, SPCP(name)->_resolution_2_,
					SPCP(name)->_resolution_3_, SPCP(name)->_resolution_4_ );

			// Read in data

			// Initialise
			decltype(value_of(SPCP(name)->_results_[0][0][0][0])) temp_out;
			const std::streamsize out_size = sizeof(temp_out);
			ssize_t i_1=0, i_2=0, i_3=0, i_4=0;

			while ( ( !in_file.eof() ) && ( i_4 < SPCP(name)->_resolution_4_ )
					&& (in_file) )
			{
				in_file.read((char *)&temp_out,out_size);
				SPCP(name)->_results_[i_1][i_2][i_3][i_4] = units_cast<Tout>(temp_out);

				++i_1;
				if(i_1==SPCP(name)->_resolution_1_)
				{
					++i_2;
					if(i_2==SPCP(name)->_resolution_2_)
					{
						++i_3;
						if(i_3==SPCP(name)->_resolution_3_)
						{
							++i_4;
							i_3=0;
						}
						i_2=0;
					}
					i_1=0;
				}
			}

			// Check that it was all read properly
			if ( (i_4 != SPCP(name)->_resolution_4_) || (i_3 != 0) || (i_2 != 0) || (i_1 != 0)
					|| (!in_file) )
			{
				need_to_calc = true;
				SPCP(name)->_calc_if_necessary();
				SPCP(name)->_output();
				continue;
			}

		} while ( need_to_calc );

		// Finish up
		in_file.close();
		in_file.clear();
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
	void set_file_name( const str_t new_name )
	{
		SPP(name)->_file_name_ = new_name;
		if ( SPCP(name)->_loaded_ )
		{
			SPCP(name)->_unload();
		}
	} // void set_file_name()

	/**
	 * Set the range of the independent parameter for wish you want values to be
	 * cached.
	 *
	 * @param new_min_1 The new minimum value, dimension 1.
	 * @param new_max_1 The new maximum value, dimension 1.
	 * @param new_step_1 The number of points at which to cache the results, dimension 1.
	 * @param new_min_2 The new minimum value, dimension 2.
	 * @param new_max_2 The new maximum value, dimension 2.
	 * @param new_step_2 The number of points at which to cache the results, dimension 2.
	 * @param new_min_3 The new minimum value, dimension 3.
	 * @param new_max_3 The new maximum value, dimension 3.
	 * @param new_step_3 The number of points at which to cache the results, dimension 3.
	 * @param new_min_4 The new minimum value, dimension 4.
	 * @param new_max_4 The new maximum value, dimension 4.
	 * @param new_step_4 The number of points at which to cache the results, dimension 4.
	 */
	void set_range( const Tin1 & new_min_1, const Tin1 & new_max_1, const Tin1 & new_step_1,
	         const Tin2 & new_min_2, const Tin2 & new_max_2, const Tin2 & new_step_2,
 	         const Tin3 & new_min_3, const Tin3 & new_max_3, const Tin3 & new_step_3,
 	         const Tin4 & new_min_4, const Tin4 & new_max_4, const Tin4 & new_step_4)
	{
		SPP(name)->_min_1_ = new_min_1;
		SPP(name)->_max_1_ = new_max_1;
		SPP(name)->_step_1_ = new_step_1;
		SPP(name)->_min_2_ = new_min_2;
		SPP(name)->_max_2_ = new_max_2;
		SPP(name)->_step_2_ = new_step_2;
		SPP(name)->_min_3_ = new_min_3;
		SPP(name)->_max_3_ = new_max_3;
		SPP(name)->_step_3_ = new_step_3;
		SPP(name)->_min_4_ = new_min_4;
		SPP(name)->_max_4_ = new_max_4;
		SPP(name)->_step_4_ = new_step_4;

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
	void print( otype & out) const
	{

		// Load if necessary
		if ( !SPCP(name)->_loaded_ )
		{
			SPCP(name)->_critical_load();
		}

		// Fill up header
		std::vector< str_t > header(6);
		header[0] = "#";
		header[1] = "x_1";
		header[2] = "x_2";
		header[3] = "x_3";
		header[4] = "x_4";
		header[5] = "y";

		// Fill up data
		std::vector< std::vector<str_t> > data(6);
		std::stringstream ss;
		for(ssize_t i_1=0; i_1<SPCP(name)->_resolution_1_; ++i_1)
		{
			for(ssize_t i_2=0; i_2<SPCP(name)->_resolution_2_; ++i_2)
			{
				for(ssize_t i_3=0; i_3<SPCP(name)->_resolution_3_; ++i_3)
				{
					for(ssize_t i_4=0; i_4<SPCP(name)->_resolution_4_; ++i_4)
					{
						data[0].push_back("");
						ss.str("");
						ss << SPCP(name)->_min_1_ + i_1*SPCP(name)->_step_1_;
						data[1].push_back(ss.str());
						ss.str("");
						ss << SPCP(name)->_min_2_ + i_2*SPCP(name)->_step_2_;
						data[2].push_back(ss.str());
						ss.str("");
						ss << SPCP(name)->_min_3_ + i_3*SPCP(name)->_step_3_;
						data[3].push_back(ss.str());
						ss.str("");
						ss << SPCP(name)->_min_4_ + i_4*SPCP(name)->_step_4_;
						data[4].push_back(ss.str());
						ss.str("");
						ss << SPCP(name)->_results_[i_1][i_2][i_3][i_4];
						data[5].push_back(ss.str());
					}
				}
			}
		}

		print_table(out,data,header);
	}

	/**
	 * Get the result of the cached function for a given value.
	 *
	 * @param x_1 The value for which you desired the cached result, dimension 1.
	 * @param x_2 The value for which you desired the cached result, dimension 2.
	 * @param x_3 The value for which you desired the cached result, dimension 3.
	 * @param x_4 The value for which you desired the cached result, dimension 4.
	 * @return The cached result for the input value.
	 */
	Tout get( const Tin1 & x_1, const Tin2 & x_2,
			const Tin3 & x_3, const Tin4 & x_4) const
	{

		Tin1 xlo_1, xhi_1;
		ssize_t xi_1; // Lower nearby array point
		Tin2 xlo_2, xhi_2;
		ssize_t xi_2; // Lower nearby array point
		Tin3 xlo_3, xhi_3;
		ssize_t xi_3; // Lower nearby array point
		Tin4 xlo_4, xhi_4;
		ssize_t xi_4; // Lower nearby array point
		Tout result;
		decltype(x_1*x_2*x_3*x_4) total_weight;
		decltype(result*total_weight) weighted_result;

		// Load if necessary
		if ( !SPCP(name)->_loaded_ )
		{
			if ( SPCP(name)->_critical_load() )
			{
				throw std::runtime_error("ERROR: Could neither load " + SPCP(name)->_file_name_ + " nor calculate in brg_cache_4d::get()\n");
			}
		}

		xi_1 = (ssize_t)bound(0,
				( ( x_1 - SPCP(name)->_min_1_ ) / SPCP(name)->_step_1_ ),
				SPCP(name)->_resolution_1_ - 2 );
		xi_2 = (ssize_t)bound(0,
				( ( x_2 - SPCP(name)->_min_2_ ) / SPCP(name)->_step_2_ ),
				SPCP(name)->_resolution_2_ - 2 );
		xi_3 = (ssize_t)bound(0,
				( ( x_3 - SPCP(name)->_min_3_ ) / SPCP(name)->_step_3_ ),
				SPCP(name)->_resolution_3_ - 2 );
		xi_4 = (ssize_t)bound(0,
				( ( x_4 - SPCP(name)->_min_4_ ) / SPCP(name)->_step_4_ ),
				SPCP(name)->_resolution_4_ - 2 );

		xlo_1 = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * xi_1;
		xhi_1 = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * ( xi_1 + 1 );
		xlo_2 = SPCP(name)->_min_2_ + SPCP(name)->_step_2_ * xi_2;
		xhi_2 = SPCP(name)->_min_2_ + SPCP(name)->_step_2_ * ( xi_2 + 1 );
		xlo_3 = SPCP(name)->_min_3_ + SPCP(name)->_step_3_ * xi_3;
		xhi_3 = SPCP(name)->_min_3_ + SPCP(name)->_step_3_ * ( xi_3 + 1 );
		xlo_4 = SPCP(name)->_min_4_ + SPCP(name)->_step_4_ * xi_4;
		xhi_4 = SPCP(name)->_min_4_ + SPCP(name)->_step_4_ * ( xi_4 + 1 );

		total_weight = (xhi_1-xlo_1)*(xhi_2-xlo_2)*(xhi_3-xlo_3)*(xhi_4-xlo_4);

		weighted_result = SPCP(name)->_results_[xi_1][xi_2][xi_3][xi_4] *
				(xhi_1-x_1)*(xhi_2-x_2)*(xhi_3-x_3)*(xhi_4-x_4);
		weighted_result += SPCP(name)->_results_[xi_1+1][xi_2][xi_3][xi_4] *
				(x_1-xlo_1)*(xhi_2-x_2)*(xhi_3-x_3)*(xhi_4-x_4);

		weighted_result += SPCP(name)->_results_[xi_1][xi_2+1][xi_3][xi_4] *
				(xhi_1-x_1)*(x_2-xlo_2)*(xhi_3-x_3)*(xhi_4-x_4);
		weighted_result += SPCP(name)->_results_[xi_1+1][xi_2+1][xi_3][xi_4] *
				(x_1-xlo_1)*(x_2-xlo_2)*(xhi_3-x_3)*(xhi_4-x_4);


		weighted_result += SPCP(name)->_results_[xi_1][xi_2][xi_3+1][xi_4] *
				(xhi_1-x_1)*(xhi_2-x_2)*(x_3-xlo_3)*(xhi_4-x_4);
		weighted_result += SPCP(name)->_results_[xi_1+1][xi_2][xi_3+1][xi_4] *
				(x_1-xlo_1)*(xhi_2-x_2)*(x_3-xlo_3)*(xhi_4-x_4);

		weighted_result += SPCP(name)->_results_[xi_1][xi_2+1][xi_3+1][xi_4] *
				(xhi_1-x_1)*(x_2-xlo_2)*(x_3-xlo_3)*(xhi_4-x_4);
		weighted_result += SPCP(name)->_results_[xi_1+1][xi_2+1][xi_3+1][xi_4] *
				(x_1-xlo_1)*(x_2-xlo_2)*(x_3-xlo_3)*(xhi_4-x_4);




		weighted_result += SPCP(name)->_results_[xi_1][xi_2][xi_3][xi_4+1] *
				(xhi_1-x_1)*(xhi_2-x_2)*(xhi_3-x_3)*(x_4-xlo_4);
		weighted_result += SPCP(name)->_results_[xi_1+1][xi_2][xi_3][xi_4+1] *
				(x_1-xlo_1)*(xhi_2-x_2)*(xhi_3-x_3)*(x_4-xlo_4);

		weighted_result += SPCP(name)->_results_[xi_1][xi_2+1][xi_3][xi_4+1] *
				(xhi_1-x_1)*(x_2-xlo_2)*(xhi_3-x_3)*(x_4-xlo_4);
		weighted_result += SPCP(name)->_results_[xi_1+1][xi_2+1][xi_3][xi_4+1] *
				(x_1-xlo_1)*(x_2-xlo_2)*(xhi_3-x_3)*(x_4-xlo_4);


		weighted_result += SPCP(name)->_results_[xi_1][xi_2][xi_3+1][xi_4+1] *
				(xhi_1-x_1)*(xhi_2-x_2)*(x_3-xlo_3)*(x_4-xlo_4);
		weighted_result += SPCP(name)->_results_[xi_1+1][xi_2][xi_3+1][xi_4+1] *
				(x_1-xlo_1)*(xhi_2-x_2)*(x_3-xlo_3)*(x_4-xlo_4);

		weighted_result += SPCP(name)->_results_[xi_1][xi_2+1][xi_3+1][xi_4+1] *
				(xhi_1-x_1)*(x_2-xlo_2)*(x_3-xlo_3)*(x_4-xlo_4);
		weighted_result += SPCP(name)->_results_[xi_1+1][xi_2+1][xi_3+1][xi_4+1] *
				(x_1-xlo_1)*(x_2-xlo_2)*(x_3-xlo_3)*(x_4-xlo_4);

		result = weighted_result / safe_d(total_weight);

		return result;

	} // get()

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
		#pragma omp critical(unload_brg_cache_4d)
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
		SPCP(name)->_calc_if_necessary();
		SPCP(name)->_output();
	}

	// Constructor
	brg_cache_4d()
	{
		SPP(name)->_init();

		#ifdef _OPENMP
		#pragma omp atomic
		#endif
		++(SPCP(name)->_num_alive_);
	}

	// Deconstructor
	virtual ~brg_cache_4d()
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

#endif // __BRG_CACHE_2D_HPP_INCLUDED__

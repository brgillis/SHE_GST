/**********************************************************************\
 @file open_file.hpp
 ------------------

 TODO <Insert file description here>

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


#ifndef _BRG_OPEN_FILE_H_INCLUDED_
#define _BRG_OPEN_FILE_H_INCLUDED_

#include <fstream>
#include <stdexcept>
#include <string>

#include "SHE_GST_IceBRG_main/common.hpp"

namespace IceBRG {

// Functions to open a file and check that it's been opened successfully. An exception will be thrown
// if the file isn't opened successfully.

template<typename stream_type>
void open_file( stream_type & stream, const std::string & name )
{
	stream.close();
	stream.clear();
	stream.open( name.c_str() );
	if ( !stream )
	{
		throw std::runtime_error("Could not open file " + name + ".");
	}
}
template<typename stream_type>
void open_file_input( stream_type & stream, const std::string & name )
{
	stream.close();
	stream.clear();
	stream.open( name.c_str(), std::ios::in );
	if ( !stream )
	{
		throw std::runtime_error("Could not open file " + name + ".");
	}
}
template<typename stream_type>
void open_file_output( stream_type & stream, const std::string & name )
{
	stream.close();
	stream.clear();
	stream.open( name.c_str(), std::ios::out );
	if ( !stream )
	{
		throw std::runtime_error("Could not open file " + name + ".");
	}
}
template<typename stream_type>
void open_file_io( stream_type & stream, const std::string & name )
{
	stream.close();
	stream.clear();
	stream.open( name.c_str(), std::ios::in | std::ios::out );
	if ( !stream )
	{
		throw std::runtime_error("Could not open file " + name + ".");
	}
}
template<typename stream_type>
void open_file_append( stream_type & stream, const std::string & name )
{
	stream.close();
	stream.clear();
	stream.open( name.c_str(), std::ios::app );
	if ( !stream )
	{
		throw std::runtime_error("Could not open file " + name + ".");
	}
}

template<typename stream_type>
void open_bin_file( stream_type & stream, const std::string & name )
{
	stream.close();
	stream.clear();
	stream.open( name.c_str(), std::ios::binary  );
	if ( !stream )
	{
		throw std::runtime_error("Could not open file " + name + ".");
	}
}
inline void open_bin_file( std::fstream & stream, const std::string & name )
{
	stream.close();
	stream.clear();
	stream.open( name.c_str(), std::ios::out | std::ios::in | std::ios::binary  );
	if ( !stream )
	{
		throw std::runtime_error("Could not open file " + name + ".");
	}
}
template<typename stream_type>
void open_bin_file_input( stream_type & stream, const std::string & name )
{
	stream.close();
	stream.clear();
	stream.open( name.c_str(), std::ios::in | std::ios::binary );
	if ( !stream )
	{
		throw std::runtime_error("Could not open file " + name + ".");
	}
}
template<typename stream_type>
void open_bin_file_io( stream_type & stream, const std::string & name )
{
	stream.close();
	stream.clear();
	stream.open( name.c_str(), std::ios::out | std::ios::out | std::ios::binary );
	if ( !stream )
	{
		throw std::runtime_error("Could not open file " + name + ".");
	}
}
template<typename stream_type>
void open_bin_file_output( stream_type & stream, const std::string & name )
{
	stream.close();
	stream.clear();
	stream.open( name.c_str(), std::ios::out | std::ios::binary );
	if ( !stream )
	{
		throw std::runtime_error("Could not open file " + name + ".");
	}
}
template<typename stream_type>
void open_bin_file_append( stream_type & stream, const std::string & name )
{
	stream.close();
	stream.clear();
	stream.open( name.c_str(), std::ios::app | std::ios::binary );
	if ( !stream )
	{
		throw std::runtime_error("Could not open file " + name + ".");
	}
}


} // namespace IceBRG


#endif // _BRG_OPEN_FILE_H_INCLUDED_

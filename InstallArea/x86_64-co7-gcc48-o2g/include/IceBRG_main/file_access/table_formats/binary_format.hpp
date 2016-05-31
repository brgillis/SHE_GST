/**********************************************************************\
 @file binary_format.hpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2015 brg

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

#ifndef ICEBRG_MAIN_FILE_ACCESS_TABLE_FORMATS_BINARY_FORMAT_HPP_
#define ICEBRG_MAIN_FILE_ACCESS_TABLE_FORMATS_BINARY_FORMAT_HPP_

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "IceBRG_main/common.hpp"

namespace IceBRG {

template<typename T_table>
class binary_format
{

public:

	typedef T_table table_type;
	typedef typename table_type::value_type value_type;

	/// Destructor (default virtual)
	virtual ~binary_format() noexcept {}

	/// Name for this format (all in lower case)
	static str_t name()
	{
		return "binary";
	}

	/// Read method
	static table_type read(std::istream & fi)
	{
		table_type table;
	    boost::archive::binary_iarchive ia(fi);

    	ia & table;

    	return table;
	}

	/// Write method
	static void write(const table_type & table, std::ostream & fo)
	{
	    boost::archive::binary_oarchive oa(fo);

    	oa & table;
	}

};

} // namespace IceBRG



#endif // ICEBRG_MAIN_FILE_ACCESS_TABLE_FORMATS_BINARY_FORMAT_HPP_

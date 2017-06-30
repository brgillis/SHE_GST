/**********************************************************************\
 @file ascii_format.hpp
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

#ifndef ICEBRG_MAIN_FILE_ACCESS_TABLE_FORMATS_ASCII_FORMAT_HPP_
#define ICEBRG_MAIN_FILE_ACCESS_TABLE_FORMATS_ASCII_FORMAT_HPP_

#include "SHE_GST_IceBRG_main/common.hpp"
#include "SHE_GST_IceBRG_main/file_access/ascii_table.hpp"

namespace IceBRG {

template<typename T_table>
class ascii_format
{

public:

	typedef T_table table_type;
	typedef typename table_type::value_type value_type;

	/// Destructor (default virtual)
	virtual ~ascii_format() noexcept {}

	/// Name for this format (all in lower case)
	static str_t name()
	{
		return "ascii";
	}

	/// Read method
	static table_type read(std::istream & fi)
	{
		table_type table;
    	table.set_labels(load_header(fi));
    	table.set_rows(load_table<value_type>(fi,Eigen::RowMajor,
    			value_type(),
    			table.num_cols()));
    	return table;
	}

	/// Write method
	static void write(const table_type & table, std::ostream & fo)
	{
    	header_t header = table.get_labels();

		// First, the header
		fo << "# ";
		for(const auto & label : header)
		{
			fo << label << "\t";
		}
		fo << std::endl;

		// And now the data table
		fo << table.data_table() << std::endl;

    	return;
	}

};

} // namespace IceBRG



#endif // ICEBRG_MAIN_FILE_ACCESS_TABLE_FORMATS_ASCII_FORMAT_HPP_

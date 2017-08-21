/**********************************************************************\
 @file file_system.hpp
 ------------------

 Functions relating to the file system, including getting the path of a
 file, joining paths, and getting the filename without an extension.

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

#ifndef FILE_SYSTEM_HPP_
#define FILE_SYSTEM_HPP_

#include <boost/algorithm/string/replace.hpp>

#include "SHE_GST_IceBRG_main/common.hpp"

namespace IceBRG {

/**
 * Replace all double and triple slashes in a string with single slashes.
 * @param s
 */
inline void reduce_slashes(std::string & s)
{
	boost::replace_all(s,"///","/"); // Cut down triple slashes to single
	boost::replace_all(s,"//","/"); // Cut down double slashes to single
}

/**
 * Concatenates two strings.
 * @param s1
 * @param s2
 * @return
 */
inline std::string join_strings(const std::string & s1, const std::string & s2)
{
	return std::string(s1 + s2);
}

/**
 * Concatenates three or more strings
 * @param s1
 * @param s2
 * @param s3
 * @param so
 * @return
 */
template<typename... To>
inline std::string join_strings(const std::string & s1, const std::string & s2,
		const std::string & s3, To... so)
{
	return join_strings(join_strings(s1,s2),s3,so...);
}

/**
 * Joins two strings together with exactly one slash between them, regardless of whether
 * s1 begins with a slash and/or s2 ends with one.
 * @param s1
 * @param s2
 * @return
 */
inline std::string join_path(const std::string & s1, const std::string & s2)
{
	char s1_end = *(s1.data()+s1.size()-1);
	char s2_begin = *(s2.data());

	bool s1_end_slash = ( s1_end=='/' );
	bool s2_begin_slash = ( s2_begin=='/' );

	if( s1_end_slash xor s2_begin_slash )
	{
	  return s1 + s2;
	}
	else if( s1_end_slash and s2_begin_slash)
	{
	  return s1 + s2.substr(1);
	}
	else
	{
	  return s1 + "/" + s2;
	}
}

/**
 * Joins multiple strings together with exactly one slash between each pair, regardless of whether
 * they begin or end with slashes.
 *
 * @param s1
 * @param s2
 * @param s3
 * @param so
 * @return
 */
template<typename... To>
inline std::string join_path(const std::string & s1, const std::string & s2,
		const std::string & s3, To... so)
{
	const std::string s12(join_path(s1,s2));

	return join_path(s12,s3,so...); // Recurse down
}

/**
 * Gets the path to a file (not including the final slash).
 *
 * @param filename
 * @return
 */
inline std::string get_path(const std::string & filename)
{
    size_t pos = filename.rfind("/");
    if(pos == std::string::npos)  // No slashes found
        return filename;

    return filename.substr(0, pos);
}

/**
 * Gets the base filename, without the last extension. Beware of cases like "file.tar.gz" which
 * will become "file.tar".
 *
 * @param filename
 * @return
 */
inline std::string get_base_filename(const std::string & filename)
{
    size_t pos = filename.rfind(".");
    if(pos == std::string::npos)  // No periods found
        return filename;

    if(pos == 0)    // It's at the front, so no extension
        return filename;

    return filename.substr(0, pos);
}

} // namespace IceBRG

#endif // FILE_SYSTEM_HPP_

/*
 * helpers.hpp
 *
 *  Created on: 13.03.2015
 *      Author: neunertm
 */

#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include <string>
#include <cassert>

namespace matlab {
namespace helpers {

void assertValidVariableName(const std::string& name)
{
	// Maybe better use regular expressions here?
	assert(name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_") == std::string::npos && "Illegal character in Variable name");
	assert(!std::isdigit(name[0]) && "First character of variable name must not be a number");
	assert(name != "i" && name!="j" && name!="mode" && name!="char" && name!="size" && name!="path" && "Illegal name, would overlay Matlab built-in function/type name");
}

} // namespace matlab
} // namespace helpers


#endif /* HELPERS_HPP_ */

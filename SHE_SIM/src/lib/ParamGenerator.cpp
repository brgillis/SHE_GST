/**********************************************************************\
 @file ParamGenerator.cpp
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

#include "SHE_SIM/ParamGenerator.hpp"

#include <limits>
#include <stdexcept>

#include "IceBRG_main/logging.hpp"

#include "SHE_SIM/common.hpp"
#include "SHE_SIM/default_param_params.hpp"
#include "SHE_SIM/ParamHierarchyLevel.hpp"
#include "SHE_SIM/ParamParam.hpp"

#define UNCACHED_VALUE std::numeric_limits<flt_t>::infinity()

// Toggle debug-level logging with a define, so we can completely disable it for efficiency later
#define DEBUGGING false
#define DEBUG_LOG() if(DEBUGGING) logger.info()

namespace SHE_SIM
{

static auto logger = ICEBRG_GET_LOGGER(logger_name);

// Protected methods

flt_t ParamGenerator::_request_param_value(name_t const & param_name)
{
    DEBUG_LOG() << "Entering/exiting " << name() << "<ParamGenerator>::_request_param_value(\"" << param_name << "\") method.";
	if(!_p_owner) throw std::logic_error("Cannot request another param value from a default ParamGenerator.");
	return _p_owner->_request_param_value(param_name, name());
}

ParamGenerator * ParamGenerator::_request_param(name_t const & param_name)
{
    DEBUG_LOG() << "Entering/exiting " << name() << "<ParamGenerator>::_request_param(\"" << param_name << "\") method.";
	if(!_p_owner) return nullptr;
	return _p_owner->_request_param(param_name, name());
}

void ParamGenerator::_generate()
{
    DEBUG_LOG() << "Entering " << name() << "<ParamGenerator>::_generate method.";
	if(_p_params->get_mode()==ParamParam::INDEPENDENT)
	{
	    _cache_value(_p_params->get_independently(get_rng()));
	}
	else
	{
		throw bad_mode_error(_p_params->get_mode_name());
	}
    DEBUG_LOG() << "Exiting " << name() << "<ParamGenerator>::_generate method.";
}

// Private methods

bool ParamGenerator::_is_cached() const
{
	return _cached_value != UNCACHED_VALUE;
}

void ParamGenerator::_cache_value(flt_t const & new_value)
{
    _cached_value = new_value;
}

void ParamGenerator::_cache_provisional_value(flt_t const & new_value)
{
    _cache_value(new_value);
    if(!_generated_at_this_level())
    {
        _p_parent_version()->_cache_provisional_value(new_value);
    }
    else
    {
        // Uncache for any children
        for( auto const & child : _p_owner->_children )
        {
            child->_clear_param_cache(name());
        }
    }
}

void ParamGenerator::_decache()
{
	_cached_value = UNCACHED_VALUE;
}

void ParamGenerator::_clear_cache()
{
    DEBUG_LOG() << "Entering " << name() << "<ParamGenerator>::_clear_cache method.";
	_decache();

	if(_p_owner)
	{
		// Uncache for any children
		for( auto const & child : _p_owner->_children )
		{
			child->_clear_param_cache(name());
		}

		// Uncache any dependants as well
		for( auto const & dependant_name : _dependant_names )
		{
			_p_owner->_clear_param_cache(dependant_name);
		}
	}

	_dependant_names.clear();
    DEBUG_LOG() << "Exiting " << name() << "<ParamGenerator>::_clear_cache method.";
}

void ParamGenerator::_add_dependant(name_t const & dependant_name)
{
	_dependant_names.insert(dependant_name);
}

bool ParamGenerator::_generated_at_this_level() const
{
	if(!_p_owner) return true;
	return _p_owner->get_hierarchy_level() <= level_generated_at();
}

bool ParamGenerator::_provisionally_generated_at_this_level() const
{
    // If we really generate here, it isn't provisional
    if(_generated_at_this_level()) return false;

    // Provisional if parent version doesn't exist, or parent is at too shallow a level
    auto const & _p_parent = _p_parent_version();
    if(!_p_parent) return false;
    bool res = _p_parent->_p_owner->get_hierarchy_level() < level_generated_at();

    if(res)
    {
        DEBUG_LOG() << "Parameter " << name() << " found to be generated provisionally.";
    }
    else
    {
        DEBUG_LOG() << "Parameter " << name() << " found not to be generated provisionally.";
    }

    return res;
}

void ParamGenerator::_determine_value()
{
    DEBUG_LOG() << "Entering " << name() << "<ParamGenerator>::_determine_value method.";
	if(_generated_at_this_level())
	{
		_generate();
	}
	else if(_provisionally_generated_at_this_level())
	{
        // Generated provisionally, so generate here
        _generate();
	}
    else
    {
        // Generated at parent's level or higher
        _cache_value(_parent_version().get());
    }
    DEBUG_LOG() << "Exiting " << name() << "<ParamGenerator>::_determine_value method.";
}

void ParamGenerator::_determine_new_value()
{
    DEBUG_LOG() << "Entering " << name() << "<ParamGenerator>::_determine_new_value method.";
	_clear_cache();
	this->_determine_value();
    DEBUG_LOG() << "Exiting " << name() << "<ParamGenerator>::_determine_new_value method.";
}

ParamGenerator * ParamGenerator::_p_parent_version()
{
    DEBUG_LOG() << "Entering/exiting " << name() << "<ParamGenerator>::_p_parent_version method.";
	if(!_p_owner) return nullptr;
	auto p_parent = _p_owner->get_parent();
	if(!p_parent) return nullptr;
	return p_parent->get_param(name());
}

ParamGenerator const * ParamGenerator::_p_parent_version() const
{
    DEBUG_LOG() << "Entering/exiting " << name() << "<ParamGenerator>::_p_parent_version method.";
	if(!_p_owner) return nullptr;
	auto p_parent = _p_owner->get_parent();
	if(!p_parent) return nullptr;
	return p_parent->get_param(name());
}

ParamGenerator & ParamGenerator::_parent_version()
{
	assert(_p_parent_version());
	return *_p_parent_version();
}

ParamGenerator const & ParamGenerator::_parent_version() const
{
	assert(_p_parent_version());
	return *_p_parent_version();
}

ParamGenerator::ParamGenerator( owner_t * const & p_owner )
: _cached_value(UNCACHED_VALUE),
  _p_owner(p_owner),
  _p_params(nullptr),
  _p_generation_level(nullptr)
{
}


ParamGenerator::owner_t * ParamGenerator::get_p_owner()
{
	return _p_owner;
}
ParamGenerator::owner_t const * ParamGenerator::get_p_owner() const
{
	return _p_owner;
}
ParamGenerator::owner_t & ParamGenerator::get_owner()
{
	if(!_p_owner) throw std::logic_error("Owner of ParamGenerator requested for default generator.");
	return *_p_owner;
}
ParamGenerator::owner_t const & ParamGenerator::get_owner() const
{
	if(!_p_owner) throw std::logic_error("Owner of ParamGenerator requested for default generator.");
	return *_p_owner;
}
void ParamGenerator::set_p_owner(ParamGenerator::owner_t * const & p_owner)
{
	_p_owner = p_owner;
}
void ParamGenerator::set_owner(ParamGenerator::owner_t & owner)
{
	_p_owner = &owner;
}

gen_t * ParamGenerator::get_p_rng()
{
	if(!get_p_owner()) return nullptr;
	return &(get_p_owner()->_rng);
}
gen_t const * ParamGenerator::get_p_rng() const
{
	if(!get_p_owner()) return nullptr;
	return &(get_p_owner()->_rng);
}
gen_t & ParamGenerator::get_rng()
{
	if(!get_p_owner()) throw std::logic_error("RNG of ParamGenerator requested for default generator.");
	return get_p_owner()->_rng;
}
gen_t const & ParamGenerator::get_rng() const
{
	if(!get_p_owner()) throw std::logic_error("RNG of ParamGenerator requested for default generator.");
	return get_p_owner()->_rng;
}

void ParamGenerator::set_p_params(ParamParam const * const & p)
{
	_clear_cache();
	_p_params = p;
}

ParamParam const & ParamGenerator::get_params() const
{
	return *_p_params;
}

ParamParam const * const & ParamGenerator::get_p_params() const noexcept
{
	return _p_params;
}

level_t const & ParamGenerator::get_generation_level() const
{
	return *_p_generation_level;
}

level_t const * const & ParamGenerator::get_p_generation_level() const
{
	return _p_generation_level;
}

void ParamGenerator::set_generation_level( level_t const & level )
{
    DEBUG_LOG() << "Entering " << name() << "<ParamGenerator>::set_generation_level method.";
	_p_owner->set_generation_level(name(),level);
    DEBUG_LOG() << "Exiting " << name() << "<ParamGenerator>::set_generation_level method.";
}

void ParamGenerator::set_p_generation_level( level_t const * const & p_level )
{
	_clear_cache();
	_p_generation_level = p_level;
}

flt_t const & ParamGenerator::get()
{
	if(!_is_cached())
	{
		_determine_value();
	}
	return _cached_value;
}

flt_t const & ParamGenerator::get_new()
{
	_determine_new_value();
	return _cached_value;
}

ParamGenerator * ParamGenerator::request(name_t const & requester_name)
{
    DEBUG_LOG() << "Entering " << name() << "<ParamGenerator>::request(\"" << requester_name << "\") method.";
	_add_dependant(requester_name);
	if(!_is_cached())
	{
		_determine_value();
	}
    DEBUG_LOG() << "Exiting " << name() << "<ParamGenerator>::request(\"" << requester_name << "\") method.";
	return this;
}

ParamGenerator * ParamGenerator::request_new(name_t const & requester_name)
{
    DEBUG_LOG() << "Entering " << name() << "<ParamGenerator>::request_new(\"" << requester_name << "\") method.";
	_add_dependant(requester_name);
	_determine_new_value();
    DEBUG_LOG() << "Exiting " << name() << "<ParamGenerator>::request_new(\"" << requester_name << "\") method.";
	return this;
}

flt_t const & ParamGenerator::request_value(name_t const & requester_name)
{
    DEBUG_LOG() << "Entering " << name() << "<ParamGenerator>::request_value(\"" << requester_name << "\") method.";

	_add_dependant(requester_name);
	flt_t const & res = get();

    DEBUG_LOG() << "Exiting " << name() << "<ParamGenerator>::request_value(\"" << requester_name << "\") method.";
	return res;
}

flt_t const & ParamGenerator::request_new_value(name_t const & requester_name)
{
    DEBUG_LOG() << "Entering " << name() << "<ParamGenerator>::request_new_value(\"" << requester_name << "\") method.";

	_add_dependant(requester_name);
	flt_t const & res =get_new();

    DEBUG_LOG() << "Exiting " << name() << "<ParamGenerator>::request_new_value(\"" << requester_name << "\") method.";
	return res;
}

const level_t & ParamGenerator::level_generated_at() const
{
	return _p_owner->get_generation_level(name());
}

} // namespace SHE_SIM

/**********************************************************************\
 @file ParamHierarchyLevel.cpp
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


#include "SHE_GST_PhysicalModel/ParamHierarchyLevel.hpp"

#include <ctime>
#include <memory>
#include <random>
#include <utility>

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_PhysicalModel/ParamGenerator.hpp"
#include "SHE_GST_PhysicalModel/ParamParam.hpp"
#include "SHE_GST_PhysicalModel/levels/Cluster.hpp"
#include "SHE_GST_PhysicalModel/levels/ClusterGroup.hpp"
#include "SHE_GST_PhysicalModel/levels/Field.hpp"
#include "SHE_GST_PhysicalModel/levels/FieldGroup.hpp"
#include "SHE_GST_PhysicalModel/levels/Galaxy.hpp"
#include "SHE_GST_PhysicalModel/levels/GalaxyGroup.hpp"
#include "SHE_GST_PhysicalModel/levels/GalaxyPair.hpp"
#include "SHE_GST_PhysicalModel/levels/Image.hpp"
#include "SHE_GST_PhysicalModel/levels/ImageGroup.hpp"
#include "SHE_GST_PhysicalModel/levels/Survey.hpp"
#include "SHE_GST_PhysicalModel/params_list.hpp"
#include "SHE_GST_IceBRG_main/logging.hpp"


// Toggle debug-level logging with a define, so we can completely disable it for efficiency later
#define DEBUGGING false
#define DEBUG_LOG() if(DEBUGGING) logger.info()

namespace SHE_GST_PhysicalModel
{

static auto logger = ICEBRG_GET_LOGGER(logger_name);

// Private methods
void ParamHierarchyLevel::_update_parent(parent_ptr_t const & new_p_parent)
{
    _p_parent = new_p_parent;
}
void ParamHierarchyLevel::_update_child(child_t * const & old_p_child, child_t * const & new_p_child,
    bool release )
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::_update_child method.";
    // Find this child in the children list

    // Start by trying to index by the local ID
    int_t old_child_ID = old_p_child->get_local_ID();
    if( old_child_ID < int_t(_children.size()) )
    {
        auto & test_child = _children.at(old_child_ID);
        if( test_child.get() == old_p_child )
        {
            if(release) test_child.release();
            test_child.reset(new_p_child);
            DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::_update_child method successfully.";
            return;
        }
    }

    // Can't find by local ID, so we'll have to search
    for( auto & test_child : _children )
    {
        if(test_child.get()==old_p_child)
        {
            if(release) test_child.release();
            test_child.reset(new_p_child);
            DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::_update_child method successfully.";
            return;
        }
    }

    // Not in children
    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::_update_child method unsuccessfully.";
    throw std::runtime_error("Cannot update child - can't find it in children list.");
}

flt_t const & ParamHierarchyLevel::_request_param_value(name_t const & name, name_t const & requester_name)
{
    return _params.at(name)->request_value(requester_name);
}

ParamGenerator * ParamHierarchyLevel::_request_param(name_t const & name, name_t const & requester_name)
{
    return _params.at(name)->request(requester_name);
}

void ParamHierarchyLevel::_drop_local_param_param(name_t const & name)
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::_drop_local_param_param(\"" << name << "\") method.";
    if ( _local_param_params.find(name) == _local_param_params.end() )
    {
        // Key isn't in the map
        return;
    }
    else
    {
        if(_local_param_params.at(name).get() == get_p_param_params(name))
        {
            // Don't drop if we're using it
            return;
        }
        else
        {
            // Unused, so drop it
            _local_param_params.erase(name);
        }
    }
    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::_drop_local_param_param(\"" << name << "\") method successfully.";
}

void ParamHierarchyLevel::_drop_local_generation_level(name_t const & name)
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::_drop_local_generation_level(\"" << name << "\") method.";
    if ( _local_generation_levels.find(name) == _local_generation_levels.end() )
    {
        // Key isn't in the map
        return;
    }
    else
    {
        if(_local_generation_levels.at(name).get() == get_p_generation_level(name))
        {
            // Don't drop if we're using it
            return;
        }
        else
        {
            // Unused, so drop it
            _local_generation_levels.erase(name);
        }
    }
    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::_drop_local_generation_level(\"" << name << "\") method successfully.";
}

void ParamHierarchyLevel::_clear_param_cache(name_t const & name)
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::_clear_param_cache(\"" << name << "\") method.";
    // Clear for this
    _clear_own_param_cache(name);

    // Clear for all children
    for( auto & child : _children )
    {
        child->_clear_param_cache(name);
    }
    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::_clear_param_cache(\"" << name << "\") method successfully.";
}

void ParamHierarchyLevel::_clear_own_param_cache(name_t const & name)
{
    _params.at(name)->_clear_cache();
}

void ParamHierarchyLevel::_clear_param_cache()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::_clear_param_cache method.";
    // Clear for this
    _clear_own_param_cache();

    // Clear for all children
    for( auto & child : _children )
    {
        child->_clear_param_cache();
    }
    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::_clear_param_cache method successfully.";
}

void ParamHierarchyLevel::_clear_own_param_cache()
{
    for( auto & param : _params )
    {
        param.second->_clear_cache();
    }
}

// Public methods

ParamHierarchyLevel::ParamHierarchyLevel(parent_ptr_t const & p_parent)
: _p_parent(p_parent)
{
    DEBUG_LOG() << "Entering ParamHierarchyLevel::ParamHierarchyLevel method.";
    // Inherit parameters and generation_levels from parent if it exists
    if(p_parent)
    {
        // Get ID from the parent's number of children
        _local_ID = p_parent->num_children();
        set_seed(p_parent->get_seed());

        // Deep copy parameters map from parent
        for( auto const & param_name_and_ptr : p_parent->_params )
        {
            param_ptr_t new_param_ptr(param_name_and_ptr.second->clone());
            new_param_ptr->set_p_owner(this);
            auto new_param_name_and_ptr = std::make_pair( param_name_and_ptr.first ,
                    std::move(new_param_ptr) );
            _params.insert( std::move(new_param_name_and_ptr) );
        }
    }
    else
    {
        // Set ID to zero
        _local_ID = 0;

        // Use default seed
        set_seed();

        // Use default parameters map
        _params = get_full_params_map(*this);
    }

    DEBUG_LOG() << "Exiting ParamHierarchyLevel::ParamHierarchyLevel method successfully.";
}

ParamHierarchyLevel::ParamHierarchyLevel(const ParamHierarchyLevel & other)
: _p_parent(other._p_parent),
  _local_ID(other._local_ID),
  _seed(other._seed),
  _seed_vec(other._seed_vec),
  _rng(other._rng)
{
    DEBUG_LOG() << "Entering ParamHierarchyLevel::ParamHierarchyLevel method.";

    // Deep-copy maps

    for( auto const & child_ptr : other._children )
    {
        _children.push_back( child_ptr_t( child_ptr->clone() ) );
        _children.back()->_update_parent(this);
    }

    for( auto const & param_name_and_ptr : other._params )
    {
        _params.insert( std::make_pair( param_name_and_ptr.first , param_ptr_t( param_name_and_ptr.second->clone() ) ) );
    }

    for( auto const & param_param_name_and_ptr : other._local_param_params )
    {
        _local_param_params.insert( std::make_pair( param_param_name_and_ptr.first,
                param_param_ptr_t( param_param_name_and_ptr.second->clone() ) ) );
    }

    for( auto const & gen_level_name_and_ptr : other._local_generation_levels )
    {
        _local_generation_levels.insert( std::make_pair( gen_level_name_and_ptr.first,
                level_ptr_t( new level_t( *(gen_level_name_and_ptr.second.get()) ) ) ) );
    }

    DEBUG_LOG() << "Exiting ParamHierarchyLevel::ParamHierarchyLevel method successfully.";
}


/**
 * Move constructor.
 *
 * @param other
 */
ParamHierarchyLevel::ParamHierarchyLevel(ParamHierarchyLevel && other)
: _p_parent(std::move(other._p_parent)),
  _children(std::move(other._children)),
  _local_param_params(std::move(other._local_param_params)),
  _local_generation_levels(std::move(other._local_generation_levels)),
  _local_ID(std::move(other._local_ID)),
  _seed(std::move(other._seed)),
  _seed_vec(std::move(other._seed_vec)),
  _rng(std::move(other._rng)),
  _params(std::move(other._params))
{
    DEBUG_LOG() << "Entering ParamHierarchyLevel::ParamHierarchyLevel method.";

    // Update parent's pointer to this
    if(_p_parent)
    {
        _p_parent->_update_child(&other,this);
    }

    // Update children's pointers to this
    for( auto & child : _children )
    {
        child->_update_parent(this);
    }
    DEBUG_LOG() << "Exiting ParamHierarchyLevel::ParamHierarchyLevel method successfully.";
}

ParamHierarchyLevel & ParamHierarchyLevel::operator=(const ParamHierarchyLevel & other)
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::operator= method.";

    _p_parent = other._p_parent;
    _local_ID = other._local_ID;
    _seed = other._seed;
    _seed_vec = other._seed_vec;
    _rng = other._rng;

    // Deep-copy maps

    _children.clear();
    for( auto const & child_ptr : other._children )
    {
        _children.push_back( child_ptr_t( child_ptr->clone() ) );
        _children.back()->_update_parent(this);
    }

    _params.clear();
    for( auto const & param_name_and_ptr : other._params )
    {
        param_ptr_t new_param_ptr(param_name_and_ptr.second->clone());
        new_param_ptr->set_p_owner(this);
        auto new_param_name_and_ptr = std::make_pair( param_name_and_ptr.first ,
                std::move(new_param_ptr) );
        _params.insert( std::move(new_param_name_and_ptr) );
    }

    _local_param_params.clear();
    for( auto const & param_param_name_and_ptr : other._local_param_params )
    {
        _local_param_params.insert( std::make_pair( param_param_name_and_ptr.first,
                param_param_ptr_t( param_param_name_and_ptr.second->clone() ) ) );
    }

    _local_generation_levels.clear();
    for( auto const & gen_level_name_and_ptr : other._local_generation_levels )
    {
        _local_generation_levels.insert( std::make_pair( gen_level_name_and_ptr.first,
                level_ptr_t( new level_t( *(gen_level_name_and_ptr.second.get()) ) ) ) );
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::operator= method successfully.";
    return *this;
}

ParamHierarchyLevel & ParamHierarchyLevel::operator=(ParamHierarchyLevel && other)
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::_drop_local_param_param method.";

    _p_parent = std::move(other._p_parent);
    _params = std::move(other._params);
    _children = std::move(other._children);
    _local_param_params = std::move(other._local_param_params);
    _local_generation_levels = std::move(other._local_generation_levels);
    _local_ID = std::move(other._local_ID);
    _seed = std::move(other._seed);
    _seed_vec = std::move(other._seed_vec);
    _rng = std::move(other._rng);

    // Update parent's pointer to this
    if(_p_parent)
    {
        _p_parent->_update_child(&other,this);
    }

    // Update children's pointers to this
    for( auto & child : _children )
    {
        child->_update_parent(this);
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::operator= method successfully.";
    return *this;
}

// Public methods


std::vector<ParamGenerator *> ParamHierarchyLevel::emancipate()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::emancipate method.";

    std::vector<ParamGenerator *> provisional_params;

    // Use the parent's method to orphan this

    // If there is no parent, silently return
    if(!_p_parent)
    {
        DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::emancipate method successfully.";
        return provisional_params;
    }

    // Fill up the result list of which params are/were provisionally generated here
    for( auto & param_name_and_uptr : _params )
    {
        ParamGenerator * p_param = param_name_and_uptr.second.get();
        if(p_param->_provisionally_generated_at_this_level())
        {
            provisional_params.push_back(p_param);
        }
    }

    _p_parent->orphan_child(this);

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::emancipate method successfully.";
    return provisional_params;
}

std::vector<ParamHierarchyLevel::child_t *> ParamHierarchyLevel::get_children( name_t const & type_name )
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_children(\"" << type_name << "\") method.";

    std::vector<ParamHierarchyLevel::child_t *> res;

    for( auto & child : _children )
    {
        if( ( child->get_name()==type_name ) or ( type_name == "" ) ) res.push_back( child.get() );
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_children method(\"" << type_name << "\") successfully.";
    return res;
}

std::vector<const ParamHierarchyLevel::child_t *> ParamHierarchyLevel::get_children( name_t const & type_name ) const
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_children(\"" << type_name << "\") method.";

    std::vector<const ParamHierarchyLevel::child_t *> res;

    for( const auto & child : _children )
    {
      if( !child.get() ) continue;

      if( ( child->get_name()==type_name ) or ( type_name == "" ) ) res.push_back( child.get() );
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_children(\"" << type_name << "\") method successfully.";
    return res;
}

std::vector<ParamHierarchyLevel::child_t *> ParamHierarchyLevel::get_descendants( name_t const & type_name )
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_descendants(\"" << type_name << "\") method.";

    std::vector<child_t *> res;

    for( auto & child : _children )
    {
    if( !child.get() ) continue;

        if( ( child->get_name()==type_name ) or ( type_name == "" ) )
            res.push_back( child.get() );

        // Check if this one has any descendants of the desired type
        std::vector<child_t *> childs_descendants = child->get_descendants(type_name);
        for( auto & descendant : childs_descendants )
        {
            res.push_back(descendant);
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_descendants(\"" << type_name << "\") method successfully.";
    return res;
}

std::vector<const ParamHierarchyLevel::child_t *> ParamHierarchyLevel::get_descendants( name_t const & type_name ) const
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_descendants(\"" << type_name << "\") method.";

    std::vector<const child_t *> res;

    for( const auto & child : _children )
    {
    if( !child.get() ) continue;

        if( ( child->get_name()==type_name ) or ( type_name == "" ) )
            res.push_back( child.get() );

        // Check if this one has any descendants of the desired type
        std::vector<child_t *> childs_descendants = child->get_descendants(type_name);
        for( auto & descendant : childs_descendants )
        {
            res.push_back(descendant);
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_descendants(\"" << type_name << "\") method successfully.";
    return res;
}

ParamHierarchyLevel::child_t * ParamHierarchyLevel::get_child(const int & i)
{
    return _children.at(i).get();
}

ParamHierarchyLevel::child_t const * ParamHierarchyLevel::get_child(const int & i) const
{
    return _children.at(i).get();
}

ParamHierarchyLevel::child_t * ParamHierarchyLevel::orphan_child(const int & i)
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::orphan_child method.";

    child_t * p_child = _children.at(i).release();

    p_child->_update_parent(nullptr);

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::orphan_child method successfully.";
    return p_child;
}

ParamHierarchyLevel::child_t * ParamHierarchyLevel::orphan_child(child_t * const & p_child)
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::orphan_child method.";

    _update_child(p_child,nullptr,true);

    p_child->_update_parent(nullptr);

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::orphan_child method successfully.";
    return p_child;
}

void ParamHierarchyLevel::adopt_child(child_t * const & p_child, std::vector<ParamGenerator *> provisional_params)
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::adopt_child method.";

    // Check that the child is orphaned first
    if( !p_child->is_orphan() )
    {
        DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::adopt_child method unsuccessfully.";
        throw std::runtime_error("Only orphaned children can be adopted.");
    }
    // Make sure the child is of a deeper hierarchy level
    if( p_child->get_hierarchy_level() <= get_hierarchy_level() )
    {
        DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::adopt_child method sunuccessfully.";
        throw std::runtime_error("Children must be of a deeper hierarchy level to be adopted.");
    }

    _children.push_back( child_ptr_t(p_child) );
    p_child->_update_parent(this);

    // Handle provisional parameters of the child now

    // If it's the first child, we'll propagate anything it's already generated upward
    if(_children.size()==1)
    {
        DEBUG_LOG() << "In " << get_name() << "<ParamHierarchyLevel>::adopt_child, "
                << "this is first child, so updating provisional parameters.";

        std::vector<flt_t> provisional_param_values;

        for( auto p_param : provisional_params )
        {
            provisional_param_values.push_back(p_param->get());
        }
        for( size_t i=0; i<provisional_params.size(); ++i )
        {
            auto p_param = provisional_params[i];
            flt_t cached_value = provisional_param_values[i];

            auto p_parent_version = p_param->_p_parent_version();
            p_parent_version->_cache_provisional_value(cached_value);
        }
    }
    // If it's not the first child, clear the caches of all provisional params
    else
    {
        DEBUG_LOG() << "In " << get_name() << "<ParamHierarchyLevel>::adopt_child, "
                << "this is not first child, so clearing provisional parameters.";
        for( auto p_param : provisional_params )
        {
            if(p_param->_is_cached())
            {
                p_param->_clear_cache();
            }
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::adopt_child method successfully.";
}

void ParamHierarchyLevel::abduct_child(child_t * const & p_child)
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::abduct_child method.";

    // Make sure the child is of a deeper hierarchy level
    if( p_child->get_hierarchy_level() <= get_hierarchy_level() )
    {
        DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::abduct_child method unsuccessfully.";
        throw std::runtime_error("Children must be of a deeper hierarchy level to be abducted.");
    }
    std::vector<ParamGenerator *> provisional_params = p_child->emancipate();

    adopt_child(p_child,provisional_params);

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::abduct_child method successfully.";
}

void ParamHierarchyLevel::autofill_children()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::autofill_children method.";

    clear_children();
    fill_children();
    for( auto & child : _children )
    {
    if( !child.get() ) continue;

        child->autofill_children();
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::autofill_children method successfully.";
}

// Methods to get children of specific types
#if(1)

std::vector<ImageGroup *> ParamHierarchyLevel::get_image_groups()
{
    return get_children<ImageGroup>();
}
std::vector<Image *> ParamHierarchyLevel::get_images()
{
    return get_children<Image>();
}
std::vector<ClusterGroup *> ParamHierarchyLevel::get_cluster_groups()
{
    return get_children<ClusterGroup>();
}
std::vector<Cluster *> ParamHierarchyLevel::get_clusters()
{
    return get_children<Cluster>();
}
std::vector<FieldGroup *> ParamHierarchyLevel::get_field_groups()
{
    return get_children<FieldGroup>();
}
std::vector<Field *> ParamHierarchyLevel::get_fields()
{
    return get_children<Field>();
}
std::vector<GalaxyGroup *> ParamHierarchyLevel::ParamHierarchyLevel::get_galaxy_groups()
{
    return get_children<GalaxyGroup>();
}
std::vector<GalaxyPair *> ParamHierarchyLevel::ParamHierarchyLevel::get_galaxy_pairs()
{
    return get_children<GalaxyPair>();
}
std::vector<Galaxy *> ParamHierarchyLevel::get_galaxies()
{
    return get_children<Galaxy>();
}
std::vector<Galaxy *> ParamHierarchyLevel::get_background_galaxies()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_background_galaxies method.";

    std::vector<Galaxy *> res;

    for( auto & child : get_children() )
    {
        Galaxy * casted_child = dynamic_cast<Galaxy *>(child.get());
        if( casted_child != nullptr )
        {
            if( casted_child->is_background_galaxy())
                res.push_back(casted_child);
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_background_galaxies method successfully.";
    return res;
}
std::vector<Galaxy *> ParamHierarchyLevel::get_foreground_galaxies()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_foreground_galaxies method successfully.";

    std::vector<Galaxy *> res;

    for( auto & child : get_children() )
    {
        Galaxy * casted_child = dynamic_cast<Galaxy *>(child.get());
        if( casted_child != nullptr )
        {
            if( casted_child->is_foreground_galaxy())
                res.push_back(casted_child);
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_foreground_galaxies method successfully.";
    return res;
}
Galaxy * ParamHierarchyLevel::get_central_galaxy()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_central_galaxy method.";

    for( auto & child : get_children() )
    {
        Galaxy * casted_child = dynamic_cast<Galaxy *>(child.get());
        if( casted_child != nullptr )
        {
            if( casted_child->is_central_galaxy())
            {
                DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_central_galaxy method successfully.";
                return casted_child;
            }
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_central_galaxy method unsuccessfully.";
    return nullptr;
}
std::vector<Galaxy *> ParamHierarchyLevel::get_field_galaxies()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_field_galaxies method.";

    std::vector<Galaxy *> res;

    for( auto & child : get_children() )
    {
        Galaxy * casted_child = dynamic_cast<Galaxy *>(child.get());
        if( casted_child != nullptr )
        {
            if( casted_child->is_field_galaxy())
                res.push_back(casted_child);
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_field_galaxies method successfully.";
    return res;
}
std::vector<Galaxy *> ParamHierarchyLevel::get_satellite_galaxies()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_satellite_galaxies method.";

    std::vector<Galaxy *> res;

    for( auto & child : get_children() )
    {
        Galaxy * casted_child = dynamic_cast<Galaxy *>(child.get());
        if( casted_child != nullptr )
        {
            if( casted_child->is_satellite_galaxy())
                res.push_back(casted_child);
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_satellite_galaxies method successfully.";
    return res;
}

#endif // Methods to get children of specific types

// Methods to get descendants of specific types
#if(1)

std::vector<ImageGroup *> ParamHierarchyLevel::get_image_group_descendants()
{
    return get_descendants<ImageGroup>();
}
std::vector<Image *> ParamHierarchyLevel::get_image_descendants()
{
    return get_descendants<Image>();
}
std::vector<ClusterGroup *> ParamHierarchyLevel::get_cluster_group_descendants()
{
    return get_descendants<ClusterGroup>();
}
std::vector<Cluster *> ParamHierarchyLevel::get_cluster_descendants()
{
    return get_descendants<Cluster>();
}
std::vector<FieldGroup *> ParamHierarchyLevel::get_field_group_descendants()
{
    return get_descendants<FieldGroup>();
}
std::vector<Field *> ParamHierarchyLevel::get_field_descendants()
{
    return get_descendants<Field>();
}
std::vector<GalaxyGroup *> ParamHierarchyLevel::ParamHierarchyLevel::get_galaxy_group_descendants()
{
    return get_descendants<GalaxyGroup>();
}
std::vector<GalaxyPair *> ParamHierarchyLevel::ParamHierarchyLevel::get_galaxy_pair_descendants()
{
    return get_descendants<GalaxyPair>();
}
std::vector<Galaxy *> ParamHierarchyLevel::get_galaxy_descendants()
{
    return get_descendants<Galaxy>();
}
std::vector<Galaxy *> ParamHierarchyLevel::get_background_galaxy_descendants()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_background_galaxy_descendants method.";

    std::vector<Galaxy *> res;

    for( auto & child : get_children() )
    {
        Galaxy * casted_child = dynamic_cast<Galaxy *>(child.get());
        if( casted_child != nullptr )
        {
            if( casted_child->is_background_galaxy())
                res.push_back(casted_child);
        }
        else
        {
            // Otherwise check if this one has any descendants of the desired type
            std::vector<Galaxy *> childs_descendants = child->get_descendants<Galaxy>();
            for( auto & descendant : childs_descendants )
            {
                res.push_back(descendant);
            }
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_background_galaxy_descendants method successfully.";
    return res;
}
std::vector<Galaxy *> ParamHierarchyLevel::get_foreground_galaxy_descendants()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_foreground_galaxy_descendants method.";

    std::vector<Galaxy *> res;

    for( auto & child : get_children() )
    {
        Galaxy * casted_child = dynamic_cast<Galaxy *>(child.get());
        if( casted_child != nullptr )
        {
            if( casted_child->is_foreground_galaxy())
                res.push_back(casted_child);
        }
        else
        {
            // Otherwise check if this one has any descendants of the desired type
            std::vector<Galaxy *> childs_descendants = child->get_descendants<Galaxy>();
            for( auto & descendant : childs_descendants )
            {
                res.push_back(descendant);
            }
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_foreground_galaxy_descendants method successfully.";
    return res;
}
std::vector<Galaxy *> ParamHierarchyLevel::get_central_galaxy_descendants()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_central_galaxy_descendants method.";

    std::vector<Galaxy *> res;

    for( auto & child : get_children() )
    {
        Galaxy * casted_child = dynamic_cast<Galaxy *>(child.get());
        if( casted_child != nullptr )
        {
            if( casted_child->is_central_galaxy())
                res.push_back(casted_child);
        }
        else
        {
            // Otherwise check if this one has any descendants of the desired type
            std::vector<Galaxy *> childs_descendants = child->get_descendants<Galaxy>();
            for( auto & descendant : childs_descendants )
            {
                res.push_back(descendant);
            }
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_central_galaxy_descendants method successfully.";
    return res;
}
std::vector<Galaxy *> ParamHierarchyLevel::get_field_galaxy_descendants()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_central_galaxy_descendants method.";

    std::vector<Galaxy *> res;

    for( auto & child : get_children() )
    {
        Galaxy * casted_child = dynamic_cast<Galaxy *>(child.get());
        if( casted_child != nullptr )
        {
            if( casted_child->is_field_galaxy())
                res.push_back(casted_child);
        }
        else
        {
            // Otherwise check if this one has any descendants of the desired type
            std::vector<Galaxy *> childs_descendants = child->get_descendants<Galaxy>();
            for( auto & descendant : childs_descendants )
            {
                res.push_back(descendant);
            }
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_central_galaxy_descendants method successfully.";
    return res;
}
std::vector<Galaxy *> ParamHierarchyLevel::get_satellite_galaxy_descendants()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_satellite_galaxy_descendants method.";

    std::vector<Galaxy *> res;

    for( auto & child : get_children() )
    {
        Galaxy * casted_child = dynamic_cast<Galaxy *>(child.get());
        if( casted_child != nullptr )
        {
            if( casted_child->is_satellite_galaxy())
                res.push_back(casted_child);
        }
        else
        {
            // Otherwise check if this one has any descendants of the desired type
            std::vector<Galaxy *> childs_descendants = child->get_descendants<Galaxy>();
            for( auto & descendant : childs_descendants )
            {
                res.push_back(descendant);
            }
        }
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_satellite_galaxy_descendants method successfully.";
    return res;
}

#endif // Methods to get descendants of specific types

param_t * ParamHierarchyLevel::get_param( name_t const & name )
{
    return _params.at(name).get();
}

const param_t * ParamHierarchyLevel::get_param( name_t const & name) const
{
    return _params.at(name).get();
}

flt_t const & ParamHierarchyLevel::get_param_value( name_t name )
{
    boost::algorithm::to_lower(name);
    return _params.at(name).get()->get();
}

level_t const & ParamHierarchyLevel::get_generation_level( name_t name ) const
{
    boost::algorithm::to_lower(name);
    return get_param(name)->get_generation_level();
}

level_t const * const & ParamHierarchyLevel::get_p_generation_level( name_t const & name ) const
{
    return get_param(name)->get_p_generation_level();
}

void ParamHierarchyLevel::set_p_generation_level( name_t const & name, level_t const * const & p_level )
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::set_p_generation_level(\"" << name << "\") method.";

    get_param(name)->set_p_generation_level( p_level );

    // Pass this along to all children
    for( auto & child : _children )
    {
        child->set_p_generation_level( name, p_level );
    }

    _drop_local_generation_level(name);

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::set_p_generation_level(\"" << name << "\") method successfully.";
}

void ParamHierarchyLevel::set_generation_level( name_t name, level_t const & level )
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::set_generation_level(\"" << name << "\") method.";

    boost::algorithm::to_lower(name);
    _local_generation_levels[name] = level_ptr_t( new level_t(level) );
    set_p_generation_level( name, _local_generation_levels.at(name).get() );

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::set_generation_level(\"" << name << "\") method successfully.";
}

ParamParam const * const & ParamHierarchyLevel::get_p_param_params( name_t const & name ) const
{
    return get_param(name)->get_p_params();
}

void ParamHierarchyLevel::set_p_param_params( name_t const & name, ParamParam const * const & params )
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_p_param_params(\"" << name << "\") method.";

    get_param(name)->set_p_params(params);

    // Pass this along to all children
    for( auto & child : _children )
    {
        child->set_p_param_params(name,params);
    }

    _drop_local_param_param(name);

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_p_param_params(\"" << name << "\") method successfully.";
}

void ParamHierarchyLevel::generate_parameters()
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::generate_parameters method.";

    // Get all parameters at this level
    for( auto & param_name_and_ptr : _params )
    {
        param_name_and_ptr.second->get();
    }

    // Generate for all children as well
    for( auto & child : _children )
    {
        child->generate_parameters();
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::generate_parameters method successfully.";
}

long_int_t ParamHierarchyLevel::get_full_ID() const
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_full_ID method.";

    long_int_t ID = get_local_ID();

    // Add the proper parent ID
    if(_p_parent)
    {
    long_int_t parent_ID = _p_parent->get_full_ID();

        // Multiply the parent ID by 256^(num levels above this)
        for( int i = _p_parent->get_hierarchy_level(); i<get_hierarchy_level(); ++i)
        {
            parent_ID *= 256;
        }

    ID += parent_ID;
    }

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_full_ID method successfully.";
    return ID;
}

std::vector<int_t> ParamHierarchyLevel::get_ID_seq() const
{
    DEBUG_LOG() << "Entering ParamHierarchyLevel::get_ID_seq method.";

    // Append this to the parent's sequence if the parent exists
    if(_p_parent)
    {
        auto res = _p_parent->get_ID_seq();

        res.push_back(get_local_ID());

        DEBUG_LOG() << "Exiting ParamHierarchyLevel::get_ID_seq method successfully.";
        return res;
    }
    else // Just use this one's ID
    {
        std::vector<int_t> res({get_local_ID()});

        DEBUG_LOG() << "Exiting ParamHierarchyLevel::get_ID_seq method successfully.";
        return res;
    }
}

long_int_t ParamHierarchyLevel::get_full_seed() const
{
    DEBUG_LOG() << "Entering " << get_name() << "<ParamHierarchyLevel>::get_full_seed method.";

    // Start with the actual seed value
    long_int_t seed = get_seed();

    // Multiply it by 256^(depth of this level)
    for( int i = 0; i<get_hierarchy_level(); ++i)
    {
        seed *= 256;
    }

    // Add the full ID, so that each object will get a unique value for this function
    seed += get_full_ID();

    DEBUG_LOG() << "Exiting " << get_name() << "<ParamHierarchyLevel>::get_full_seed method successfully.";
    return seed;
}

void ParamHierarchyLevel::set_seed()
{
    set_seed(time(nullptr));
}


void ParamHierarchyLevel::set_seed( int_t const & seed )
{
    DEBUG_LOG() << "Entering ParamHierarchyLevel::set_seed method.";
    // Clear the cache
    _clear_own_param_cache();

    // Get a seed sequence
    _seed_vec = get_ID_seq();
    _seed_vec.push_back(seed);

    _seed = seed;

    std::seed_seq seed_seq(_seed_vec.begin(),_seed_vec.end());

    // Seed the generator
    _rng.seed(seed_seq);

    // Seed all children with this
    for( auto & child : _children )
    {
        child->set_seed(seed);
    }
    DEBUG_LOG() << "Exiting ParamHierarchyLevel::set_seed method successfully.";
}

} // namespace SHE_GST_PhysicalModel

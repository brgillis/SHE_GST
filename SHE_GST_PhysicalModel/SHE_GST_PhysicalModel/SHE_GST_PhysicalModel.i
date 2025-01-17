/**********************************************************************\
 @file SHE_GST_PhysicalModel.i
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

// SWIG includes
%include "typemaps.i"
%include "std_string.i"
%include "std_vector.i"

%module SHE_GST_PhysicalModel

%{
	 
	/* Include the headers in the wrapper code */
	#include "SHE_GST_PhysicalModel/common.hpp"
	#include "SHE_GST_PhysicalModel/default_values.hpp"
	#include "SHE_GST_PhysicalModel/ParamHierarchyLevel.hpp"
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
	
	// Include headers containing other useful functions
	#include "SHE_GST_PhysicalModel/dependency_functions/cosmology.hpp"
	 
%}
 
// Parse the header files to generate wrappers
%include "SHE_GST_PhysicalModel/common.hpp"
%include "SHE_GST_PhysicalModel/default_values.hpp"
%include "SHE_GST_PhysicalModel/ParamHierarchyLevel.hpp"
%include "SHE_GST_PhysicalModel/ParamGenerator.hpp"
%include "SHE_GST_PhysicalModel/ParamParam.hpp"
%include "SHE_GST_PhysicalModel/levels/Cluster.hpp"
%include "SHE_GST_PhysicalModel/levels/ClusterGroup.hpp"
%include "SHE_GST_PhysicalModel/levels/Field.hpp"
%include "SHE_GST_PhysicalModel/levels/FieldGroup.hpp"
%include "SHE_GST_PhysicalModel/levels/Galaxy.hpp"
%include "SHE_GST_PhysicalModel/levels/GalaxyGroup.hpp"
%include "SHE_GST_PhysicalModel/levels/GalaxyPair.hpp"
%include "SHE_GST_PhysicalModel/levels/Image.hpp"
%include "SHE_GST_PhysicalModel/levels/ImageGroup.hpp"
%include "SHE_GST_PhysicalModel/levels/Survey.hpp"

%include "SHE_GST_PhysicalModel/dependency_functions/cosmology.hpp"

%apply int& {SHE_GST_PhysicalModel::level_t&};

namespace SHE_GST_PhysicalModel {

%extend ParamHierarchyLevel { 
	void set_param_params(name_t name, name_t param_type )
	{
		return $self->set_param_params( name, param_type );
	}
	void set_param_params(name_t name, name_t param_type, flt_t arg1 )
	{
		return $self->set_param_params( name, param_type, arg1 );
	}
	void set_param_params(name_t name, name_t param_type, flt_t arg1, flt_t arg2 )
	{
		return $self->set_param_params( name, param_type, arg1, arg2 );
	}
	void set_param_params(name_t name, name_t param_type, flt_t arg1, flt_t arg2, flt_t arg3 )
	{
		return $self->set_param_params( name, param_type, arg1, arg2, arg3 );
	}
	void set_param_params(name_t name, name_t param_type, flt_t arg1, flt_t arg2, flt_t arg3, flt_t arg4 )
	{
		return $self->set_param_params( name, param_type, arg1, arg2, arg3, arg4 );
	}
	void set_param_params(name_t name, name_t param_type, flt_t arg1, flt_t arg2, flt_t arg3, flt_t arg4, flt_t arg5 )
	{
		return $self->set_param_params( name, param_type, arg1, arg2, arg3, arg4, arg5 );
	}
	void set_param_params(name_t name, name_t param_type, flt_t arg1, flt_t arg2, flt_t arg3, flt_t arg4, flt_t arg5, flt_t arg6 )
	{
		return $self->set_param_params( name, param_type, arg1, arg2, arg3, arg4, arg5, arg6 );
	}
}

}

// Tell SWIG to implement vectors of the PHL types as lists
namespace std {

%template(PHLVector) vector<SHE_GST_PhysicalModel::ParamHierarchyLevel *>;

%template(ClusterVector) vector<SHE_GST_PhysicalModel::Cluster *>;
%template(ClusterGroupVector) vector<SHE_GST_PhysicalModel::ClusterGroup *>;
%template(FieldVector) vector<SHE_GST_PhysicalModel::Field *>;
%template(FieldGroupVector) vector<SHE_GST_PhysicalModel::FieldGroup *>;
%template(GalaxyVector) vector<SHE_GST_PhysicalModel::Galaxy *>;
%template(GalaxyGroupVector) vector<SHE_GST_PhysicalModel::GalaxyGroup *>;
%template(GalaxyPairVector) vector<SHE_GST_PhysicalModel::GalaxyPair *>;
%template(ImageVector) vector<SHE_GST_PhysicalModel::Image *>;
%template(ImageGroupVector) vector<SHE_GST_PhysicalModel::ImageGroup *>;

}
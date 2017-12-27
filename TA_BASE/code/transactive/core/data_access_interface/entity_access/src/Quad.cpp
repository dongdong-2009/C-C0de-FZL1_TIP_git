/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/Quad.cpp $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Quad is a concrete implementation of IQuad, which is in turn an implementation of
  * IEntityData. It provides access to Quad entity data and a specific framework
  * for Quad entity parameters.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <sstream>

#include "core/data_access_interface/entity_access/src/Quad.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    // 
    // The entity type is fixed for all instances of this class.
    // 

    const std::string Quad::ENTITY_TYPE = "Quad";

    // 
    // Entity parameters
    //


    //
    // Quad
    //
    Quad::Quad()
        : VideoInput( ENTITY_TYPE )
    {
    }


    //
    // Quad
    //
    Quad::Quad( unsigned long key ) 
        : VideoInput( key, ENTITY_TYPE )
    {
    }


    ///////////////////////////////////////////////////////////////////
    // 
    // Supported Features
    //
    ///////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string Quad::getType()
    {
        return getStaticType();
    }


    //
    // getStaticType
    //
    std::string Quad::getStaticType()
    {
        return ENTITY_TYPE;
    }


    //
    // clone
    //
    IEntityData* Quad::clone( unsigned long key )
    {
        return new Quad( key );        
    }

	std::string Quad::getIncludedCameras()
	{
		// Get the parameters for this entity
		return getHelper()->getParameter("IncludedCameras");
	}

} // TA_Base_Core

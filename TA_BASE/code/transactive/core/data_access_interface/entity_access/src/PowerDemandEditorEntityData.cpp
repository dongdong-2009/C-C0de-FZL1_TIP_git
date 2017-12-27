/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/PowerDemandEditorEntityData.cpp $
  * @author:  Wen Ching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  */

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/entity_access/src/PowerDemandEditorEntityData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    const std::string PowerDemandEditorEntityData::ENTITY_TYPE = "PowerDemandEditor";

	PowerDemandEditorEntityData::PowerDemandEditorEntityData()
	{

	}

	PowerDemandEditorEntityData::PowerDemandEditorEntityData( unsigned long key ) :
		EntityData( key, getStaticType() )
    {
    }

	PowerDemandEditorEntityData::~PowerDemandEditorEntityData()
	{

	}

	void PowerDemandEditorEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

    std::string PowerDemandEditorEntityData::getType()
    {
        return getStaticType();
    }

	std::string PowerDemandEditorEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    TA_Base_Core::IEntityData* PowerDemandEditorEntityData::clone(unsigned long key)
    {
        return( new PowerDemandEditorEntityData(key));
    }

}

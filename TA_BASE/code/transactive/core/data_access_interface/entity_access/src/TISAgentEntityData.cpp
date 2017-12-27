/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TISAgentEntityData.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * TISAgentEntityData is a concrete implementation of ITISAgentEntityData.  
  * It proves read/write access to TISAgent data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string TISAgentEntityData::ENTITY_TYPE                   = "TisAgent";
	const std::string TISAgentEntityData::CURRENT_TIME_SCHEDULE_VERSION = "CurrentTimeScheduleVersion";
	const std::string TISAgentEntityData::STATUS_POLL_INTERVAL = "StatusPollInterval";


	const std::string TISAgentEntityData::PARAMETER_TRUE   = "1";
    const std::string TISAgentEntityData::PARAMETER_FALSE  = "0";

	const std::string TISAgentEntityData::ASSET_NAME = "AssetName";

    TISAgentEntityData::TISAgentEntityData(unsigned long key)
		: EntityData(key, getType())
    {
    }

    TISAgentEntityData::~TISAgentEntityData()
    {
    }

    //
    // setParameter (for boolean parameters)
    //
	void TISAgentEntityData::setParameter( const std::string& name, bool value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;
        if ( value )
        {
            paramMap[ name ] = PARAMETER_TRUE;
        }
        else
        {
            paramMap[ name ] = PARAMETER_FALSE;
        }
        getHelper()->writeParameters( paramMap );
    }

    //
    // setParameter (for string parameters)
    //
    void TISAgentEntityData::setParameter( const std::string& name, 
        const std::string& value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;
        paramMap[ name ] = value;
        getHelper()->writeParameters( paramMap );
    }

 	std::string TISAgentEntityData::getCurrentTimeScheduleVersion()
	{
		return getHelper()->getParameter(CURRENT_TIME_SCHEDULE_VERSION);
	}

	int TISAgentEntityData::getStatusPollInterval()
	{
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(STATUS_POLL_INTERVAL);
		int statusPollInterval = 0;
		if (!value.empty())
		{
            statusPollInterval = EntityHelper::getIntegerData(value);
		}

        return statusPollInterval;
	}

 	std::string TISAgentEntityData::getAssetName()
	{
		return getHelper()->getParameter(ASSET_NAME);
	}
	
    std::string TISAgentEntityData::getType()
    {
        return getStaticType();
    }

	std::string TISAgentEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}
    
    IEntityData* TISAgentEntityData::clone(unsigned long key)
    {
        return new TISAgentEntityData(key);        
    }

    void TISAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core


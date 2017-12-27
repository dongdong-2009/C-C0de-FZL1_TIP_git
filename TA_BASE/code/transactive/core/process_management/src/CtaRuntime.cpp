/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/process_management/src/CtaRuntime.cpp $
  * @author:  K. Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Implementation of ItaRuntime
  */

#include <sstream>

#include "CtaRuntime.h"

#include "core/data_access_interface/entity_access/src/DefaultEntity.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/CtaRuntimeEntityParameterNotFoundException.h"

namespace TA_Base_Core
{
    CtaRuntime::CtaRuntime(IEntityDataPtr agentConfiguration)
    : 
	m_entityDatabaseObject(agentConfiguration)
    {
        m_entityName = agentConfiguration->getName();
        m_entityKey  = agentConfiguration->getKey();
        m_entityTypeKey = agentConfiguration->getTypeKey();
        m_entityType = agentConfiguration->getType();
        m_entityLocation = agentConfiguration->getLocation();
        m_entitySubsystem = agentConfiguration->getSubsystem();
    }


    CtaRuntime::~CtaRuntime()
    {
        // Empty
    }


    std::string CtaRuntime::getConfigParameterValue(const std::string& name)
    {
        DefaultEntityPtr convertedEntity = 
			boost::dynamic_pointer_cast<DefaultEntity>(m_entityDatabaseObject);

        TA_ASSERT(convertedEntity.get() != NULL, "You can only call this method if your agent entity is a default entity.");

        
        try
        {
            return convertedEntity->getParameterValue(name);
        }
        catch( ... ) // Data and Database Exceptions will be caught here normally
        {
            std::stringstream error;
            error << "A value could not be found for the entity parameter " << name;
            TA_THROW( CtaRuntimeEntityParameterNotFoundException(error.str()) );
        }

        return ""; // Should never reach this line of code
    }


    std::string CtaRuntime::getMySessionId() const
    {
        return RunParams::getInstance().get(RPARAM_SESSIONID);
    }


    unsigned long CtaRuntime::getMyProcessLocationKey() const
    {
        // Retrieve the location as a string
        std::string locationString = RunParams::getInstance().get(RPARAM_LOCATIONKEY);
        
        // Now set up a stream with the location string in it and extra out an unsigned long
        unsigned long locationKey = 0;
        
        std::istringstream locationStream;
        locationStream.str(locationString);

        locationStream >> locationKey;

        return locationKey;
    }
}


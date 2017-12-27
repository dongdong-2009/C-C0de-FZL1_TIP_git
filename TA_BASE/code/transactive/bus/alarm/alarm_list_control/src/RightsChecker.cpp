/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/RightsChecker.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class is used to determine if a user has access rights for a certain item. This could be modified
  * later to be used in the config_plugin_helper by making it print out nice error messages to the user
  * (as well as logging) before returning the result
  */

#include <string>

#include "bus/alarm/alarm_list_control/src/stdafx.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"


#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"

#include "core/exceptions/src/RightsException.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    RightsChecker::RightsChecker() : m_rightsLibrary(NULL)
    {
        FUNCTION_ENTRY("Constructor");

        TA_Base_Bus::RightsLibraryFactory rightsLibraryFactory;

        m_rightsLibrary = rightsLibraryFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
        
        //TEST: This is a test line and should be used in place of the above line to remove all rights checking
        //m_rightsLibrary = rightsLibraryFactory.buildRightsLibrary(TA_Base_Core::ALWAYS_TRUE_RL);

        FUNCTION_EXIT;
    }


    RightsChecker::~RightsChecker()
    {
        try
        {
            if (m_rightsLibrary != NULL)
            {
                delete m_rightsLibrary;
                m_rightsLibrary = NULL;
            }
        }
        catch ( ... )
        {
        }
    }


    bool RightsChecker::canPerformAction(unsigned long entityKey, unsigned long action)
    {
        FUNCTION_ENTRY("canPerformAction");

        TA_ASSERT(m_rightsLibrary != NULL, "The rights library was not constructed correctly so we cannot determine rights");

        // First get the resource for the specified entitykey
        unsigned long resourceID = 0;

        std::map<unsigned long, unsigned long>::iterator matching = m_resourceIds.find(entityKey);
        if (matching != m_resourceIds.end())
        {
            resourceID = matching->second;
        }
        else
        {
		    TA_Base_Core::IResource* resource = NULL;
            try
            {
                resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(entityKey);
                if (resource == NULL)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError,"Could not find resource for the entity key %d", entityKey);
                    return false;
                }

                resourceID = resource->getKey();
                m_resourceIds.insert(std::map<unsigned long, unsigned long>::value_type(entityKey, resourceID));

                // Delete the resource pointer
                if (NULL != resource)
                {
                    delete resource;
                    resource = NULL;
                }
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not retrieve resource");

                // Delete the resource pointer first
                if (NULL != resource)
                {
                    delete resource;
                    resource = NULL;
                }

                return false;
            }
        }


        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        try
        {
            bool canPerformAction = m_rightsLibrary->isActionPermittedOnResource( 
                                                                 RunParams::getInstance().get(RPARAM_SESSIONID),
                                                                 resourceID,
                                                                 action,
                                                                 reason,
                                                                 decisionModule );
            if (!canPerformAction)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Cannot perform action because %s", reason.c_str());
            }

            return canPerformAction;
        }
        catch( const TA_Base_Core::RightsException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "RightsException", "Could not determine rights for this action. This alarm action will not be available.");
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "RightsException", "Could not determine rights for this action. This alarm action will not be available.");
        }

        FUNCTION_EXIT;
        return false;
    }

}



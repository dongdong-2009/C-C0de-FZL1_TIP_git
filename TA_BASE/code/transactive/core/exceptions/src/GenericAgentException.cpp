/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/GenericAgentException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#include "GenericAgentException.h"

namespace TA_Base_Core
{

    //
    // Constructor
    //
    GenericAgentException::GenericAgentException(const EGenericAgentFailType type) throw()
    : m_type(type), TransactiveException(getErrorStr(type).c_str())
    {
    }


    //
    // Constructor
    //
    GenericAgentException::GenericAgentException(const EGenericAgentFailType type, const char* msg) throw()
    : m_type(type), TransactiveException(msg)
    {
    }


    //
    // Constructor
    //
    GenericAgentException::GenericAgentException(const char* msg) throw()
    : m_type(AGENT_ERROR), TransactiveException(msg)
    {
    }
    

    //
    // getType
    //
    GenericAgentException::EGenericAgentFailType GenericAgentException::getType()
    {
        return m_type;
    }


    std::string GenericAgentException::getErrorStr(EGenericAgentFailType type)
    {
        std::string str;

        switch (type)
        {
        case AGENT_ERROR:
            str = "";
            break;
        case CONFIGURATION_ERROR:
            str = "Generic Agent failed to initialise because it was configured incorrectly";
            break;
        case INITIALISATION_ERROR:
            str = "Generic Agent failed to initialise";
            break;
        case ENTITY_CONFIG_FAILURE:
            str = "One of the agents entities was configured incorrectly";
            break;
        case CANNOT_CONNECT_TO_DB:
            str = "Cannot connect to the database";
            break;
        case INVALID_ENTITY_TYPE:
            str = "One of the agents entities was configured with an invalid type";
            break;
        case ENTITY_NOT_CONFIGURED:
            str = "The agent was configured with an entity name that does not exist in the database";
            break;
        case ENTITY_NAME_NOT_UNIQUE:
            str = "The agent was configured with a non-unique entity name";
            break;
        case AGENT_IN_MONITOR_MODE:
            str = "The agent was unable to complete the request because it is currently operating in Monitor Mode";
            break;
        }

        return str;
    }
}

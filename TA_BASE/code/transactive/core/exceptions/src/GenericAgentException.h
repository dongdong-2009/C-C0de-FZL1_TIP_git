/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/GenericAgentException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#if !defined(GENERICAGENTEXCEPTION)
#define GENERICAGENTEXCEPTION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#    pragma warning( disable : 4786 )
#endif

#include "TransactiveException.h"
#include <string>
#include <map>

namespace TA_Base_Core
{
	class GenericAgentException : public TA_Base_Core::TransactiveException  
	{
    public:

        // If you are adding to this, remember to update the initialiseMessageMap method
        enum EGenericAgentFailType
        {
            AGENT_ERROR            = 0,
            CONFIGURATION_ERROR    = 1,
            INITIALISATION_ERROR   = 2,
            ENTITY_CONFIG_FAILURE  = 3,
            CANNOT_CONNECT_TO_DB   = 4,
            INVALID_ENTITY_TYPE    = 5,
            ENTITY_NOT_CONFIGURED  = 6,
            ENTITY_NAME_NOT_UNIQUE = 7,
            AGENT_IN_MONITOR_MODE  = 8
        };

        GenericAgentException(const EGenericAgentFailType type) throw();
        GenericAgentException(const EGenericAgentFailType type, const char* msg) throw();
        GenericAgentException(const char* msg) throw();
        EGenericAgentFailType getType();
        virtual ~GenericAgentException() throw(){}

    private:

        EGenericAgentFailType m_type;
        std::string getErrorStr(EGenericAgentFailType type);

        GenericAgentException(){}
    };    
}

#endif // !defined(GENERICAGENTEXCEPTION)

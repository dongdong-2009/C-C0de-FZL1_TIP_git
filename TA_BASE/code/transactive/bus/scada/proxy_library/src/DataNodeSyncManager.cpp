/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeSyncManager.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is responsible for managing the DataNode
  * named objects, local objects and subscriptions.
  *
  */

///////////////////////////////////////////////////////////
//  DataNodeSyncManager.cpp
//  Implementation of the Class DataNodeSyncManager
//  Created on:      28-Jun-2004 09:40:02 AM
///////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

#include "bus/scada/proxy_library/src/DataNodeSyncManager.h"
#include "bus/scada/proxy_library/src/DataNodeBulkPollingSynchronizer.h"
#include "bus/scada/proxy_library/src/DataNodeLocalSynchronizer.h"
#include "bus/generic_agent/src/IEntity.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#define DEFAULT_DN_CATCH_HANDLER(logMessage)\
        catch (const TA_Base_Core::NotAvailableException& e)\
        {\
            std::ostringstream ostr;\
            ostr << "DataNode not available Exception: " << e._name();\
            \
            LOG_EXCEPTION_CATCH(SourceInfo, ostr.str(), logMessage);\
            TA_THROW(TA_Base_Core::ScadaProxyException(logMessage));\
        }\
        catch (const TA_Base_Core::BadParameterException& e)\
        {\
            std::ostringstream ostr;\
            ostr << "Bad Parameter Exception: " << e._name();\
            \
            LOG_EXCEPTION_CATCH(SourceInfo, ostr.str(), logMessage);\
            TA_THROW(TA_Base_Core::ScadaProxyException(logMessage));\
        }\
        catch (const TA_Base_Core::AccessDeniedException& e)\
        {\
            std::ostringstream ostr;\
            ostr << "Access Denied Exception: " << e._name();\
            \
            LOG_EXCEPTION_CATCH(SourceInfo, ostr.str(), logMessage);\
            TA_THROW(TA_Base_Core::ScadaProxyException(logMessage));\
        }\
        catch (const TA_Base_Core::OperationModeException& e)\
        {\
            std::ostringstream ostr;\
            ostr << "Operation Mode Exception: " << e._name();\
            \
            LOG_EXCEPTION_CATCH(SourceInfo, ostr.str(), logMessage);\
            TA_THROW(TA_Base_Core::ScadaProxyException(logMessage));\
        }\
        catch (const CORBA::Exception& e)\
        {\
            std::ostringstream ostr;\
            ostr << "CORBA Exception: " << e._name();\
            \
            LOG_EXCEPTION_CATCH(SourceInfo, ostr.str(), logMessage);\
            TA_THROW(TA_Base_Core::ScadaProxyException(logMessage));\
        }\
        catch (const TA_Base_Core::ObjectResolutionException& e)\
        {\
            std::ostringstream ostr;\
            ostr << "Object Resolution Exception: " << e.what();\
            \
            LOG_EXCEPTION_CATCH(SourceInfo, ostr.str(), logMessage);\
            TA_THROW(TA_Base_Core::ScadaProxyException(logMessage));\
        }\
        catch (...)\
        {\
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", logMessage);\
            TA_THROW(TA_Base_Core::ScadaProxyException(logMessage));\
        }


#define CHECK_OBJACCESS_THROW(objAccess, logMessage, entityKey)\
        {\
            if (NULL == objAccess.get())\
            {\
				std::ostringstream ss; \
				ss << logMessage << " entity key " << entityKey;\
                TA_THROW(TA_Base_Core::ScadaProxyException(ss.str().c_str()));\
            }\
        }


namespace TA_Base_Bus
{
    DataNodeSyncManager::DataNodeSyncManager()
		: m_callback (NULL)
    {
        FUNCTION_ENTRY("Constructor");
        FUNCTION_EXIT;
    }


    DataNodeSyncManager::~DataNodeSyncManager()
    {
        FUNCTION_ENTRY("Destructor");

		removeAllAgentInterfaces();

        FUNCTION_EXIT;
    }


    void DataNodeSyncManager::setCallback( DataNodeProxyManager& updateCallback )
	{
		FUNCTION_ENTRY("setCallback");

		TA_ASSERT(m_callback == NULL, "Data node callback already defined");
		m_callback = &updateCallback;
		
		FUNCTION_EXIT;
	}


	boost::shared_ptr<AgentSynchronisationInterface> DataNodeSyncManager::createAgentInterface(TA_Base_Core::IEntityDataPtr entityData)
	{
		FUNCTION_ENTRY("createAgentInterface");

		boost::shared_ptr<AgentSynchronisationInterface> agentInterface ((AgentSynchronisationInterface*)NULL);

		// if the requested entity is not a local entity
		if (isLocal(entityData->getKey()))
		{
			agentInterface = boost::shared_ptr<AgentSynchronisationInterface>(new DataNodeLocalSynchronizer( entityData->getAgentName(), 
																				entityData->getLocation(), *this, *m_callback ));
		}
		else
		{
            agentInterface = boost::shared_ptr<AgentSynchronisationInterface>(new DataNodeBulkPollingSynchronizer( entityData->getAgentName(), 
																				entityData->getLocation(), *this, *m_callback ));
		}

		FUNCTION_EXIT;
		return agentInterface;
	}

}

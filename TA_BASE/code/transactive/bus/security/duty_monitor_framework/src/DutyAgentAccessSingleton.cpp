/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DutyAgentAccessSingleton.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/03/10 10:15:27 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * Serves as a front end to the Duty Agent's IDutyAgentMgrCorbaDef interface.
  * Handles all Duty Agent exceptions and rethrows as DutyMonitorFrameworkException.
  * To avoid multiple CORBA connections (named objects) being created by
  * Duty Agent clients, this class has been implemented as a singleton.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"
#include "core/data_access_interface/entity_access/src/DutyManagerEntityData.h"
#include "bus/security/security_utilities/src/SecurtyCorbaMacros.h"
#include "bus/security/duty_monitor_framework/src/DutyAgentAccessSingleton.h"
namespace
{
	const short DUTY_AGENT_TIMEOUT(120); 
    const short CORBA_DELIVERY_TIMOUT(10); // secs
}

using namespace TA_Base_Bus;


DutyAgentAccessSingleton* DutyAgentAccessSingleton::s_dutyAgentAccessSingleton = NULL;
TA_Base_Core::NonReEntrantThreadLockable DutyAgentAccessSingleton::s_singletonLock;


DutyAgentAccessSingleton& DutyAgentAccessSingleton::getInstance()
{
//    FUNCTION_ENTRY("getInstance()");

	if (s_dutyAgentAccessSingleton == NULL)
	{
	    TA_Base_Core::ThreadGuard guard(s_singletonLock);

        if (s_dutyAgentAccessSingleton == NULL)
        {
		    s_dutyAgentAccessSingleton = new DutyAgentAccessSingleton();
        }
    }

//    FUNCTION_EXIT;
	return *s_dutyAgentAccessSingleton;    
}


DutyAgentAccessSingleton::DutyAgentAccessSingleton()
{
//    FUNCTION_ENTRY("DutyAgentAccessSingleton()");

    TA_ASSERT(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_LOCATIONKEY), "Location key run param not set");

    TA_Base_Core::EntityAccessFactory& entityAccessFactory = TA_Base_Core::EntityAccessFactory::getInstance();
    unsigned long locationKey(strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str(), NULL, 10));

	// Get the corba name of the local Duty Agent.
    //TA_Base_Core::CorbaNameList dutyAgentNames(
    //    entityAccessFactory.getCorbaNamesOfTypeAtLocation(TA_Base_Core::DutyAgentEntityData::getStaticType(), locationKey));
	  TA_Base_Core::CorbaNameList dutyAgentNames(
        entityAccessFactory.getCorbaNamesOfTypeAtLocation(TA_Base_Core::DutyAgentEntityData::getStaticType(), locationKey,true));


    // Make sure there's exactly one Duty Agent entity configured for this location.
    if (dutyAgentNames.empty())
    {
        std::ostringstream msg;
        //msg << "There are no entities of type DutyAgent with location key set to " << locationKey << ".";
		msg << "位置" << locationKey << "中未找到DutyAgent的实例";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());

        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }
    else if (dutyAgentNames.size() > 1)
    {
        std::ostringstream msg;
        //msg << "There are multiple entities of type DutyAgent with location key set to " << locationKey << ".";
		msg << "位置" << locationKey << "中找到多个DutyAgent的实例";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());

        TA_THROW(DutyMonitorFrameworkException(msg.str()));
    }    

    // Set the named object's corba name so that the Duty Agent can be resolved correctly.
    m_dutyAgent.setCorbaName(dutyAgentNames[0]);

    // Get a control station entity in order to retrieve the timeout on access control popups.
    // This timeout affects Duty Agent response times so the CORBA timeout needs to be adjusted accordingly.
    TA_Base_Core::IEntityDataList controlStationEntities(entityAccessFactory.getEntitiesOfType(TA_Base_Core::ControlStation::getStaticType()));
    
    if (controlStationEntities.empty())
    {
        m_dutyAgent.setObjectTimeout(DUTY_AGENT_TIMEOUT);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "set dutyagent object time out to %d",DUTY_AGENT_TIMEOUT);
    }
    else
    {
        TA_Base_Core::ControlStation* controlStation = dynamic_cast<TA_Base_Core::ControlStation*>(controlStationEntities[0]);

        // Allow an extra few seconds for network delays.
		TA_Base_Core::IEntityDataList dutyManagerEntities(entityAccessFactory.getEntitiesOfType(TA_Base_Core::DutyManagerEntityData::getStaticType()));
		unsigned short timeout = 0;
		if (!dutyManagerEntities.empty())
		{
			TA_Base_Core::DutyManagerEntityData* dutymanager = dynamic_cast<TA_Base_Core::DutyManagerEntityData*>(dutyManagerEntities[0]);
			timeout = static_cast<short>(dutymanager->getCorbaTimeoutSeconds());
			// Clean up.
			while (!dutyManagerEntities.empty())
			{
				delete dutyManagerEntities.back();
				dutyManagerEntities.pop_back();
			}
		}

		if (timeout == TA_Base_Core::DutyManagerEntityData::USE_DEFAULT_TIMEOUT)
		{
			timeout = CORBA_DELIVERY_TIMOUT;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "dutyagent object time out to %d",timeout);
		timeout = static_cast<short>(controlStation->getAccessControlPopupTimeout()) + timeout;

        m_dutyAgent.setObjectTimeout(timeout);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "set dutyagent object time out to %d",timeout);

		m_queryTimeout = timeout;
		m_actionTimeout = CORBA_DELIVERY_TIMOUT;
		m_queryTimeOutFlag = true;

        // Clean up.
        while (!controlStationEntities.empty())
        {
            delete controlStationEntities.back();
            controlStationEntities.pop_back();
        }
    }

//    FUNCTION_EXIT;
}


DutyAgentAccessSingleton::~DutyAgentAccessSingleton()
{
//    FUNCTION_ENTRY("~DutyAgentAccessSingleton()");
    
      s_dutyAgentAccessSingleton = NULL;

//    FUNCTION_EXIT;
}


void DutyAgentAccessSingleton::getDutyState(const SessionId& sessionId, SubsystemStateList& subsystemStates, 
                                            SubsystemDutyList& subsystemDuties, RegionDutyList& regionDuties)
{
    FUNCTION_ENTRY("getDutyState(const SessionId& sessionId, SubsystemStateList& subsystemStates, SubsystemDutyList& subsystemDuties, RegionDutyList& regionDuties)");
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] begin DutyAgentAccessSingleton::getDutyState");

    try
    {
        DutyAgentTypeCorbaDef::SubsystemStateSequence_var subsystemStateSeq;
        DutyAgentTypeCorbaDef::SubsystemDutySequence_var subsystemDutySeq;
        DutyAgentTypeCorbaDef::RegionDutySequence_var regionDutySeq;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "begin to call mgrGetDutyState");

		setQueryTimeout();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyAgentAccessSingleton::getDutyState, after setQueryTimeout(true), and begin CORBA_CALL mgrGetDutyState");

		CORBA_CALL( m_dutyAgent,
		            mgrGetDutyState,
		            ( sessionId.c_str(), subsystemStateSeq.out(), subsystemDutySeq.out(), regionDutySeq.out() ) );

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyAgentAccessSingleton::getDutyState, end CORBA_CALL mgrGetDutyState");

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "end to call mgrGetDutyState");
        subsystemStates.clear();
        subsystemStates.reserve(subsystemStateSeq->length());

        for (unsigned long stateIdx=0; stateIdx < subsystemStateSeq->length(); stateIdx++)
        {
            subsystemStates.push_back(subsystemStateSeq[stateIdx]);
        }

        subsystemDuties.clear();
        subsystemDuties.reserve(subsystemDutySeq->length());

        for (unsigned long subsystemIdx = 0; subsystemIdx < subsystemDutySeq->length(); subsystemIdx++)
        {
            subsystemDuties.push_back(subsystemDutySeq[subsystemIdx]);
        }

        regionDuties.clear();
        regionDuties.reserve(regionDutySeq->length());

        for (unsigned long regionIdx = 0; regionIdx < regionDutySeq->length(); regionIdx++)
        {
            regionDuties.push_back(regionDutySeq[regionIdx]);
        }
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.")); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态")); 
    }
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] end DutyAgentAccessSingleton::getDutyState");
    FUNCTION_EXIT;
}


void DutyAgentAccessSingleton::getSessionDutyState(const SessionId& sessionId, SubsystemStateList& subsystemStates, 
                                            SubsystemDutyList& subsystemDuties, RegionDutyList& regionDuties)
{
    FUNCTION_ENTRY("getDutyState(const SessionId& sessionId, SubsystemStateList& subsystemStates, SubsystemDutyList& subsystemDuties, RegionDutyList& regionDuties)");
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] begin DutyAgentAccessSingleton::getDutyState");

    try
    {
        DutyAgentTypeCorbaDef::SubsystemStateSequence_var subsystemStateSeq;
        DutyAgentTypeCorbaDef::SubsystemDutySequence_var subsystemDutySeq;
        DutyAgentTypeCorbaDef::RegionDutySequence_var regionDutySeq;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "begin to call mgrGetDutyState");

		setQueryTimeout();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyAgentAccessSingleton::getDutyState, after setQueryTimeout(true), and begin CORBA_CALL mgrGetDutyState");

		CORBA_CALL( m_dutyAgent,
		            mgrGetSessionDutyState,
		            ( sessionId.c_str(), subsystemStateSeq.out(), subsystemDutySeq.out(), regionDutySeq.out() ) );

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyAgentAccessSingleton::getDutyState, end CORBA_CALL mgrGetDutyState");

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "end to call mgrGetDutyState");
        subsystemStates.clear();
        subsystemStates.reserve(subsystemStateSeq->length());

        for (unsigned long stateIdx=0; stateIdx < subsystemStateSeq->length(); stateIdx++)
        {
            subsystemStates.push_back(subsystemStateSeq[stateIdx]);
        }

        subsystemDuties.clear();
        subsystemDuties.reserve(subsystemDutySeq->length());

        for (unsigned long subsystemIdx = 0; subsystemIdx < subsystemDutySeq->length(); subsystemIdx++)
        {
            subsystemDuties.push_back(subsystemDutySeq[subsystemIdx]);
        }

        regionDuties.clear();
        regionDuties.reserve(regionDutySeq->length());

        for (unsigned long regionIdx = 0; regionIdx < regionDutySeq->length(); regionIdx++)
        {
            regionDuties.push_back(regionDutySeq[regionIdx]);
        }
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.", 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetDutyState(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve duty state from Duty Agent.")); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的权限状态")); 
    }
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] end DutyAgentAccessSingleton::getDutyState");
    FUNCTION_EXIT;
}

SubsystemStateList DutyAgentAccessSingleton::getSubsystemDutyState(const SessionId& sessionId, const SubsystemKeyList& subsystems, unsigned long locationKey)
{
    FUNCTION_ENTRY("getSubsystemDutyState(SessionId& sessionId, SubsystemKeyList&, locationKey)");

    try
    {
		// Create a key sequence as input param to the corba function
        DutyAgentTypeCorbaDef::KeySequence subsystemsKeySeq;
        subsystemsKeySeq.length(subsystems.size());

        for (unsigned long r = 0; r < subsystems.size(); r++)
        {
            subsystemsKeySeq[r] = subsystems[r];
        }

		setQueryTimeout();
		// Get the subsystem state sequence thru corba function call
		DutyAgentTypeCorbaDef::SubsystemStateSequence_var subsystemStateSeq;
		CORBA_CALL_RETURN( subsystemStateSeq,
		                   m_dutyAgent,
		                   mgrGetSubsystemDutyState,
		                   ( sessionId.c_str(), subsystemsKeySeq, locationKey ) );
		//  Convert the subsystem state sequence to vector
        SubsystemStateList subStateList;
        subStateList.reserve(subsystemStateSeq->length());

        for (unsigned long s = 0; s < subsystemStateSeq->length(); s++)
        {
            subStateList.push_back(subsystemStateSeq[s]);
        }

		// Return the state list
		FUNCTION_EXIT;
		return subStateList;

    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in getSubsystemDutyState(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve subsystem state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的子系统状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in getSubsystemDutyState(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve subsystem state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的子系统状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in getSubsystemDutyState(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve subsystem state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的子系统状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in getSubsystemDutyState(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve subsystem state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的子系统状态", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in getSubsystemDutyState(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve subsystem state from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的子系统状态", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in getSubsystemDutyState(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve subsystem state from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的子系统状态", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in getSubsystemDutyState(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve subsystem state from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的子系统状态", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in getSubsystemDutyState(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve subsystem state from Duty Agent.", 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的子系统状态", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in getSubsystemDutyState(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve subsystem state from Duty Agent.")); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到相应的子系统状态")); 
    }

    FUNCTION_EXIT;
}

TransferableRegionList DutyAgentAccessSingleton::getTransferableRegions(const SessionInfo& session)
{
    FUNCTION_ENTRY("getTransferableRegions(const SessionId& sessionId)");

    try
    {
		setQueryTimeout();
        DutyAgentTypeCorbaDef::TransferableRegionSequence_var transferableRegionSeq;

		TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
			TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session);

		CORBA_CALL_RETURN( transferableRegionSeq,
		                   m_dutyAgent,
		                   mgrGetTransferableRegions,
		                   ( corbaSession ) );

        TransferableRegionList transferableRegions;
        transferableRegions.reserve(transferableRegionSeq->length());

        for (unsigned long r = 0; r < transferableRegionSeq->length(); r++)
        {
            transferableRegions.push_back(transferableRegionSeq[r]);
        }

        FUNCTION_EXIT;
        return transferableRegions;
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableRegions(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableRegions(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableRegions(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableRegions(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableRegions(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableRegions(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableRegions(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableRegions(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableRegions(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.")); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表")); 
    }
}


void DutyAgentAccessSingleton::getTransferableRegions(const SessionId& sessionId, 
                                                      TransferableRegionList& transferableRegions, 
                                                      SubsystemTreeMap& uncontrolledSubsystems)
{
    FUNCTION_ENTRY("getTransferableRegions(const SessionId& sessionId, "
                                          "TransferableRegionList& transferableRegions, "
                                          "SubsystemTreeMap& uncontrolledSubsystems)");

    try
    {
		setQueryTimeout();
        DutyAgentTypeCorbaDef::SubsystemTreeSequence_var uncontrolledSubsystemTreeSeq;        

        DutyAgentTypeCorbaDef::TransferableRegionSequence_var transferableRegionSeq;
		CORBA_CALL_RETURN( transferableRegionSeq,
		                   m_dutyAgent,
		                   regMgrGetTransferableRegions,
		                   ( sessionId.c_str(), uncontrolledSubsystemTreeSeq.out() ) );
        transferableRegions.clear();
        transferableRegions.reserve(transferableRegionSeq->length());

        for (unsigned long r = 0; r < transferableRegionSeq->length(); r++)
        {
            transferableRegions.push_back(transferableRegionSeq[r]);
        }

        uncontrolledSubsystems.clear();

        for (unsigned long t = 0; t < uncontrolledSubsystemTreeSeq->length(); t++)
        {
            SubsystemKeyList subsystemKeys;
            subsystemKeys.reserve(uncontrolledSubsystemTreeSeq[t].subsystems.length());

            for (unsigned long s = 0; s < uncontrolledSubsystemTreeSeq[t].subsystems.length(); s++)
            {
                subsystemKeys.push_back(uncontrolledSubsystemTreeSeq[t].subsystems[s]);
            }

            // Where the region is already mapped append the extra subsystems, otherwise insert a new mapping.
            SubsystemTreeMap::iterator treeIt(uncontrolledSubsystems.find(uncontrolledSubsystemTreeSeq[t].region));
            if (treeIt != uncontrolledSubsystems.end())
            {
                treeIt->second.insert(treeIt->second.begin(), subsystemKeys.begin(), subsystemKeys.end());
            }
            else
            {
                uncontrolledSubsystems.insert(std::make_pair(uncontrolledSubsystemTreeSeq[t].region, subsystemKeys));
            }
        }
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetTransferableRegions(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetTransferableRegions(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetTransferableRegions(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetTransferableRegions(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetTransferableRegions(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetTransferableRegions(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetTransferableRegions(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetTransferableRegions(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.", 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetTransferableRegions(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable regions from Duty Agent.")); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的位置列表")); 
    }

    FUNCTION_EXIT;
}


SubsystemTreeMap DutyAgentAccessSingleton::getRemovableRegions(const SessionId& sessionId)
{
    FUNCTION_ENTRY("getRemovableRegions(const SessionId& sessionId)");

    try
    {
		setQueryTimeout();

        DutyAgentTypeCorbaDef::SubsystemTreeSequence_var removableRgnTreeSeq;
		CORBA_CALL_RETURN( removableRgnTreeSeq,
		                   m_dutyAgent,
		                   regMgrGetRemovableRegions,
		                   ( sessionId.c_str() ) );

        SubsystemTreeMap removableRegions;

        for (unsigned long t = 0; t < removableRgnTreeSeq->length(); t++)
        {
            SubsystemKeyList subsystemKeys;
            subsystemKeys.reserve(removableRgnTreeSeq[t].subsystems.length());

            for (unsigned long s = 0; s < removableRgnTreeSeq[t].subsystems.length(); s++)
            {
                subsystemKeys.push_back(removableRgnTreeSeq[t].subsystems[s]);
            }

            // Where the region is already mapped append the extra subsystems, otherwise insert a new mapping.
            SubsystemTreeMap::iterator treeIt(removableRegions.find(removableRgnTreeSeq[t].region));
            if (treeIt != removableRegions.end())
            {
                treeIt->second.insert(treeIt->second.begin(), subsystemKeys.begin(), subsystemKeys.end());
            }
            else
            {
                removableRegions.insert(std::make_pair(removableRgnTreeSeq[t].region, subsystemKeys));
            }
        }
   
        FUNCTION_EXIT;
        return removableRegions;
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetRemovableRegions(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve removable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可去除权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetRemovableRegions(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve removable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可去除权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetRemovableRegions(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve removable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可去除权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetRemovableRegions(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve removable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可去除权限的位置列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetRemovableRegions(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve removable regions from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可去除权限的位置列表", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetRemovableRegions(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve removable regions from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可去除权限的位置列表", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetRemovableRegions(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve removable regions from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可去除权限的位置列表", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetRemovableRegions(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve removable regions from Duty Agent.", 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可去除权限的位置列表", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrGetRemovableRegions(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve removable regions from Duty Agent.")); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可去除权限的位置列表")); 
    }
}


TransferableSubsystemList DutyAgentAccessSingleton::getTransferableSubsystems(const SessionInfo& sessionInfo, const RegionKeyList& regionKeys)
{
    FUNCTION_ENTRY("getTransferableSubsystems(const SessionId& sessionId, const RegionKeyList& regionKeys)");

    try
    {
        DutyAgentTypeCorbaDef::KeySequence regionKeySeq;
        regionKeySeq.length(regionKeys.size());

        for (unsigned long r = 0; r < regionKeys.size(); r++)
        {
            regionKeySeq[r] = regionKeys[r];
        }

		//TD15346++
		std::string temp = "TD15346: session: ";
		temp += sessionInfo.SessionId;
		temp += ", regionKey size: ";
		temp += regionKeys.size();
		temp += ". Before call to m_dutyAgent->mgrGetTransferableSubsystems()";
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, temp.c_str());
		//++TD15346

		setQueryTimeout();
		TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
				TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(sessionInfo);
        DutyAgentTypeCorbaDef::TransferableSubsystemSequence_var transferableSubsystemSeq;
		CORBA_CALL_RETURN( transferableSubsystemSeq,
		                   m_dutyAgent,
		                   mgrGetTransferableSubsystems,
		                   ( corbaSession, regionKeySeq ) );

        TransferableSubsystemList transferableSubsystems;
        transferableSubsystems.reserve(transferableSubsystemSeq->length());

        for (unsigned long s = 0; s < transferableSubsystemSeq->length(); s++)
        {
            transferableSubsystems.push_back(transferableSubsystemSeq[s]);
        }

        FUNCTION_EXIT;
        return transferableSubsystems;
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableSubsystems(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable subsystems from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的子系统列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableSubsystems(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable subsystems from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的子系统列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableSubsystems(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable subsystems from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的子系统列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableSubsystems(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable subsystems from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的子系统列表", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableSubsystems(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable subsystems from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的子系统列表", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableSubsystems(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable subsystems from Duty Agent.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的子系统列表", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableSubsystems(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable subsystems from Duty Agent.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的子系统列表", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableSubsystems(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable subsystems from Duty Agent.", 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的子系统列表", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrGetTransferableSubsystems(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve transferable subsystems from Duty Agent.")); 
		TA_THROW(DutyMonitorFrameworkException("未能从DutyAgent获取到可转移权限的子系统列表")); 
    }
}


void DutyAgentAccessSingleton::transferRegionDuty(const TA_Base_Bus::SessionInfo& sessionInfo, const RegionKeyList& regionKeys)
{
    FUNCTION_ENTRY("transferRegionDuty(const SessionId& sessionId, const RegionKeyList& regionKeys)");

    try
    {
        DutyAgentTypeCorbaDef::KeySequence regionKeySeq;
        regionKeySeq.length(regionKeys.size());

        for (unsigned long r = 0; r < regionKeys.size(); r++)
        {
            regionKeySeq[r] = regionKeys[r];
        }
		setQueryTimeout(false);
		TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession = 
			TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(sessionInfo);

		CORBA_CALL( m_dutyAgent,
		            mgrTransferRegionDuty,
		            ( corbaSession, regionKeySeq ) );

    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferRegionDuty(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferRegionDuty(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferRegionDuty(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferRegionDuty(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferRegionDuty(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferRegionDuty(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferRegionDuty(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferRegionDuty(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferRegionDuty(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.")); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败")); 
    }

    FUNCTION_EXIT;
}


void DutyAgentAccessSingleton::transferRegionDuty(const SessionId& requestingSessionId, const SessionId& targetSessionId, const RegionKeyList& regionKeys)
{
    FUNCTION_ENTRY("transferRegionDuty(const SessionId& requestingSessionId, const SessionId& targetSessionId, const RegionKeyList& regionKeys)");

    try
    {
        DutyAgentTypeCorbaDef::KeySequence regionKeySeq;
        regionKeySeq.length(regionKeys.size());

        for (unsigned long r = 0; r < regionKeys.size(); r++)
        {
            regionKeySeq[r] = regionKeys[r];
        }
		setQueryTimeout(false);
		CORBA_CALL( m_dutyAgent,
		            regMgrTransferRegionDuty,
		            ( requestingSessionId.c_str(), targetSessionId.c_str(), regionKeySeq ) );
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrTransferRegionDuty(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrTransferRegionDuty(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrTransferRegionDuty(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrTransferRegionDuty(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrTransferRegionDuty(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrTransferRegionDuty(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrTransferRegionDuty(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrTransferRegionDuty(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.", 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrTransferRegionDuty(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer region duty.")); 
		TA_THROW(DutyMonitorFrameworkException("转移位置权限失败")); 
    }

    FUNCTION_EXIT;
}


void DutyAgentAccessSingleton::removeRegionDuty(const SessionId& requestingSessionId, const SessionId& targetSessionId, const RegionKeyList& regionKeys)
{
    FUNCTION_ENTRY("removeRegionDuty(const SessionId& requestingSessionId, const SessionId& targetSessionId, const RegionKeyList& regionKeys)");

    try
    {
		setQueryTimeout(false);
        DutyAgentTypeCorbaDef::KeySequence regionKeySeq;
        regionKeySeq.length(regionKeys.size());

        for (unsigned long r = 0; r < regionKeys.size(); r++)
        {
            regionKeySeq[r] = regionKeys[r];
        }

		CORBA_CALL( m_dutyAgent,
		            regMgrRemoveRegionDuty,
		            ( requestingSessionId.c_str(), targetSessionId.c_str(), regionKeySeq ) );
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrRemoveRegionDuty(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to remove region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("移去除位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrRemoveRegionDuty(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to remove region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("移去除位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrRemoveRegionDuty(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to remove region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("移去除位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrRemoveRegionDuty(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to remove region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("移去除位置权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrRemoveRegionDuty(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to remove region duty.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("移去除位置权限失败", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrRemoveRegionDuty(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to remove region duty.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("移去除位置权限失败", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrRemoveRegionDuty(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to remove region duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("移去除位置权限失败", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrRemoveRegionDuty(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to remove region duty.", 
		TA_THROW(DutyMonitorFrameworkException("移去除位置权限失败", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in regMgrRemoveRegionDuty(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to remove region duty.")); 
		TA_THROW(DutyMonitorFrameworkException("移去除位置权限失败")); 
    }

    FUNCTION_EXIT;
}


void DutyAgentAccessSingleton::transferSubsystemDuty(const TA_Base_Bus::SessionInfo& sessionInfo, const SubsystemTreeMap& subsystems)
{
    FUNCTION_ENTRY("transferSubsystemDuty(const SessionId& sessionId, const SubsystemTreeMap& subsystems)");

    try
    {
        DutyAgentTypeCorbaDef::SubsystemTreeSequence subsystemTreeSeq;
        subsystemTreeSeq.length(subsystems.size());

        unsigned long t = 0;
        for (SubsystemTreeMap::const_iterator treeIt = subsystems.begin(); treeIt != subsystems.end(); treeIt++, t++)
        {
            const SubsystemKeyList& subsystemKeys = treeIt->second;
            DutyAgentTypeCorbaDef::SubsystemTree subsystemTree;

            subsystemTree.region = treeIt->first;
            subsystemTree.subsystems.length(subsystemKeys.size());
            
            for (unsigned long s = 0; s < subsystemKeys.size(); s++)
            {
                subsystemTree.subsystems[s] = subsystemKeys[s];
            }

            subsystemTreeSeq[t] = subsystemTree;
        }
		setQueryTimeout(false);
		TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession = TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(sessionInfo);
		CORBA_CALL( m_dutyAgent,
		            mgrTransferSubsystemDuty,
		            //( sessionId.c_str(), subsystemTreeSeq ) );
					( corbaSession, subsystemTreeSeq ) );
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferSubsystemDuty(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer subsystem duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移子系统权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferSubsystemDuty(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer subsystem duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移子系统权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferSubsystemDuty(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer subsystem duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移子系统权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferSubsystemDuty(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer subsystem duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移子系统权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferSubsystemDuty(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer subsystem duty.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("转移子系统权限失败", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferSubsystemDuty(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer subsystem duty.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("转移子系统权限失败", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferSubsystemDuty(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer subsystem duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移子系统权限失败", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferSubsystemDuty(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer subsystem duty.", 
		TA_THROW(DutyMonitorFrameworkException("转移子系统权限失败", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferSubsystemDuty(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer subsystem duty.")); 
		TA_THROW(DutyMonitorFrameworkException("转移子系统权限失败")); 
    }

    FUNCTION_EXIT;
}


//void DutyAgentAccessSingleton::transferAllSubsystemDuty(const SessionId& sessionId)
void DutyAgentAccessSingleton::transferAllSubsystemDuty(const TA_Base_Bus::SessionInfo& session)
{
    FUNCTION_ENTRY("transferAllSubsystemDuty(const SessionId& sessionId)");

    try
    {
		setQueryTimeout(false);
		TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession = TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session);
	
		CORBA_CALL( m_dutyAgent,
		            mgrTransferAllSubsystemDuty,
		            ( corbaSession ) );
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferAllSubsystemDuty(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer all subsystem duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移所有的子系统权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferAllSubsystemDuty(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer all subsystem duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移所有的子系统权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferAllSubsystemDuty(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer all subsystem duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移所有的子系统权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferAllSubsystemDuty(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer all subsystem duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移所有的子系统权限失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferAllSubsystemDuty(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer all subsystem duty.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("转移所有的子系统权限失败", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferAllSubsystemDuty(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer all subsystem duty.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("转移所有的子系统权限失败", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferAllSubsystemDuty(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer all subsystem duty.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("转移所有的子系统权限失败", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferAllSubsystemDuty(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer all subsystem duty.", 
		TA_THROW(DutyMonitorFrameworkException("转移所有的子系统权限失败", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrTransferAllSubsystemDuty(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to transfer all subsystem duty.")); 
		TA_THROW(DutyMonitorFrameworkException("转移所有的子系统权限失败")); 
    }

    FUNCTION_EXIT;
}


void DutyAgentAccessSingleton::degradeAllSubsystems(const SessionId& sessionId)
{
    FUNCTION_ENTRY("degradeAllSubsystems(const SessionId& sessionId)");

    try
    {
		setQueryTimeout(false);
		CORBA_CALL( m_dutyAgent,
		            mgrDegradeAllSubsystems,
		            ( sessionId.c_str() ) );
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrDegradeAllSubsystems(). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform system-wide delegation.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("权限全线下放失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrDegradeAllSubsystems(). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform system-wide delegation.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("权限全线下放失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrDegradeAllSubsystems(). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform system-wide delegation.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("权限全线下放失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrDegradeAllSubsystems(). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform system-wide delegation.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("权限全线下放失败", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrDegradeAllSubsystems(). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform system-wide delegation.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("权限全线下放失败", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrDegradeAllSubsystems(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform system-wide delegation.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("权限全线下放失败", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrDegradeAllSubsystems(). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform system-wide delegation.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("权限全线下放失败", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrDegradeAllSubsystems(). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform system-wide delegation.", 
		TA_THROW(DutyMonitorFrameworkException("权限全线下放失败", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in mgrDegradeAllSubsystems(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform system-wide delegation.")); 
		TA_THROW(DutyMonitorFrameworkException("权限全线下放失败")); 
    }

    FUNCTION_EXIT;
}
void DutyAgentAccessSingleton::degradeSubsystemsAtRegions(const SessionId& sessionId,const SubsystemKeyList& subsystems, const  RegionKeyList& regions)
{
	FUNCTION_ENTRY("DutyAgentAccessSingleton::degradeSubsystemsAtRegions(SessionId& sessionId, RegionKeyList& ,SubsystemKeyList&))");

    try
    {
		// Create a key sequence as input param to the corba function
        DutyAgentTypeCorbaDef::KeySequence subsystemsKeySeq;
        subsystemsKeySeq.length(subsystems.size());
		
		
		DutyAgentTypeCorbaDef::KeySequence regionsKeySeq;
		regionsKeySeq.length(regions.size());

        for (unsigned long r = 0; r < subsystems.size(); r++)
        {
            subsystemsKeySeq[r] = subsystems[r];
        }

		for(unsigned long i = 0; i < regions.size(); i++)
		{
			regionsKeySeq[i] = regions[i];
		}
		
		// Degrade the specified subsystems at the specified location
		CORBA_CALL( m_dutyAgent,
		            mgrDegradeSubsystemsAtRegions,
		            ( sessionId.c_str(), subsystemsKeySeq, regionsKeySeq ) );
		FUNCTION_EXIT;
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", 
			std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.")); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交")); 
    }

    FUNCTION_EXIT;	
}
void DutyAgentAccessSingleton::degradeSubsystemsAtLocation(const SessionId& sessionId, const SubsystemKeyList& subsystems, unsigned long locationKey)
{
    FUNCTION_ENTRY("degradeSubsystemsAtLocation(SessionId& sessionId, SubsystemKeyList&, locationKey))");

    try
    {
		// Create a key sequence as input param to the corba function
        DutyAgentTypeCorbaDef::KeySequence subsystemsKeySeq;
        subsystemsKeySeq.length(subsystems.size());

        for (unsigned long r = 0; r < subsystems.size(); r++)
        {
            subsystemsKeySeq[r] = subsystems[r];
        }

		setQueryTimeout(false);
		// Degrade the specified subsystems at the specified location
		CORBA_CALL( m_dutyAgent,
		            mgrDegradeSubsystemsAtLocation,
		            ( sessionId.c_str(), subsystemsKeySeq, locationKey ) );
		FUNCTION_EXIT;
    }
    catch (const TA_Base_Core::AccessDeniedException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught AccessDeniedException: %s", ex.reason.in());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.reason.in())); 
    }
    catch (const TA_Base_Core::BadParameterException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught BadParameterException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.reason.in())); 
    }
    catch (const TA_Base_Core::DatabaseErrorException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught DatabaseErrorException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.reason.in())); 
    }
    catch (const TA_Base_Core::OperationModeException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught OperationModeException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.reason.in())); 
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught ObjectResolutionException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.what())); 
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.what())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.what())); 
    }
    catch (const DutyAgentTypeCorbaDef::UnhandledException& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). "
                    "Caught DutyAgentTypeCorbaDef::UnhandledException: %s", ex.reason.in());

		//TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", ex.reason.in())); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", ex.reason.in())); 
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). "
                    "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.", 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交", 
                              std::string("CORBA::Exception: ") + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex))); 
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Duty Agent access failure in degradeSubsystemsAtLocation(...). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Duty Agent failed to perform selected systems degradation at location.")); 
		TA_THROW(DutyMonitorFrameworkException("不能在所选的子系统中执行权限移交")); 
    }

    FUNCTION_EXIT;
}

void DutyAgentAccessSingleton::setQueryTimeout(bool queryFlag /* = true*/)
{
	FUNCTION_ENTRY("setTimeout");
	if(queryFlag == m_queryTimeOutFlag ) return;
	
	m_dutyAgent.setObjectTimeout(m_queryTimeout);

// 	if(queryFlag) //for query
// 	{
// 		if (m_queryTimeout >= DUTY_AGENT_TIMEOUT)
// 		{
// 			m_dutyAgent.setObjectTimeout(m_queryTimeout);
// 			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
// 				"[CL - 21875] DutyAgentAccessSingleton::setQueryTimeout, for query, TimeOut = %d",m_queryTimeout);
// 		}
// 		else
// 		{
// 			m_dutyAgent.setObjectTimeout(DUTY_AGENT_TIMEOUT);
// 			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
// 				"[CL - 21875] DutyAgentAccessSingleton::setQueryTimeout, for query, TimeOut = %d",DUTY_AGENT_TIMEOUT);
// 		}
// 	}
// 	else
// 	{
// 		m_dutyAgent.setObjectTimeout(m_actionTimeout);
// 		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
// 			"[CL - 21875] DutyAgentAccessSingleton::setQueryTimeout, for action, TimeOut = %d",m_actionTimeout);
// 	}
	
	m_queryTimeOutFlag = queryFlag;
	
	FUNCTION_EXIT;
}
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DutyRightsSingleton.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 10:20:13 $
  * Last modified by:  $Author: liwei.gong $
  *
  * Singleton class to facilitate access to the operator's access rights for the Security subsystem.
  * Where the rights to a particular access controlled action cannot be determined, access is
  * automatically denied.
  *
  **/

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

#define DR_BEGIN_TRY    try

#define DR_END_TRY(actionType)                                                                                 \
catch (const TA_Base_Core::RightsException &ex)                                                                \
{                                                                                                              \
    std::ostringstream msg;                                                                                    \
    msg << "Unable to determine the operator's "#actionType" rights. Access denied. " << ex.what();            \
    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::RightsException", msg.str().c_str());                       \
}                                                                                                              \
catch (const TA_Base_Core::TransactiveException &ex)                                                           \
{                                                                                                              \
    std::ostringstream msg;                                                                                    \
    msg << "Unable to determine the operator's "#actionType" rights. Access denied. " << ex.what();            \
    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", msg.str().c_str());                  \
}                                                                                                              \
catch (...)                                                                                                    \
{                                                                                                              \
    std::ostringstream msg;                                                                                    \
    msg << "Unable to determine the operator's "#actionType" rights. Access denied. ";                         \
    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", msg.str().c_str());                                   \
}


using namespace TA_Base_Bus;


DutyRightsSingleton *DutyRightsSingleton::s_dutyRightsSingleton = NULL;
TA_Base_Core::ReEntrantThreadLockable DutyRightsSingleton::s_singletonLock;


DutyRightsSingleton &DutyRightsSingleton::getInstance()
{
    FUNCTION_ENTRY("getInstance()");

	if (s_dutyRightsSingleton == NULL)
	{
	    TA_Base_Core::ThreadGuard guard(s_singletonLock);

        if (s_dutyRightsSingleton == NULL)
        {
		    s_dutyRightsSingleton = new DutyRightsSingleton();
        }
    }

    FUNCTION_EXIT;
	return *s_dutyRightsSingleton;
}


DutyRightsSingleton::DutyRightsSingleton() :
    m_dutyAgentResourceKey(0),
    m_rightsLibrary(NULL)
{
    FUNCTION_ENTRY("DutyRightsSingleton()");

    // Initialise the rights library
    try
    {
        TA_ASSERT(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_LOCATIONKEY), "Location key run param not set");

        unsigned long locationKey(strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str(), NULL, 10));

        TA_Base_Core::IEntityDataList dutyAgentEntities(
            TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::DutyAgentEntityData::getStaticType(), locationKey));

        if (dutyAgentEntities.empty())
        {
            FUNCTION_EXIT;
            return;
        }

        unsigned long dutyAgentEntityKey(dutyAgentEntities[0]->getKey());

        while (!dutyAgentEntities.empty())
        {
            TA_Base_Core::IEntityDataList::iterator entityIt(dutyAgentEntities.begin());
            delete *entityIt;
			*entityIt = NULL;
            dutyAgentEntities.erase(entityIt);
        }

        std::auto_ptr<TA_Base_Core::IResource> dutyAgentResource(
            TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(dutyAgentEntityKey));

        m_dutyAgentResourceKey = dutyAgentResource->getKey();
        m_rightsLibrary = RightsLibraryFactory().buildRightsLibrary();
    }
    catch (...)
    {
        delete m_rightsLibrary;
        m_rightsLibrary = NULL;
    }

    FUNCTION_EXIT;
}


DutyRightsSingleton::~DutyRightsSingleton()
{
    FUNCTION_ENTRY("~DutyRightsSingleton()");

    delete m_rightsLibrary;
    m_rightsLibrary = NULL;

    s_dutyRightsSingleton = NULL;

    FUNCTION_EXIT;
}


bool DutyRightsSingleton::canTransferRegions()
{
    FUNCTION_ENTRY("canTransferRegions()");

    if (m_rightsLibrary)
    {
        DR_BEGIN_TRY
        {
            std::string currentSessionId(DutyDataStore::getSessionCache().getCurrentSessionId());
            EDecisionModule decisionModule;
            std::string reasonForDenial;

            FUNCTION_EXIT;
            return m_rightsLibrary->isActionPermittedOnResource(currentSessionId,
                                                                m_dutyAgentResourceKey, 
                                                                aca_DUTY_TRANSFER_REGION, 
                                                                reasonForDenial, 
                                                                decisionModule);
        }
        DR_END_TRY(DUTY_TRANSFER_REGION);
    }

    FUNCTION_EXIT;
    return false;
}


bool DutyRightsSingleton::canTransferSubsystems(long lActionId)//TD19218 LiDan 27,Jun,2007
{
    FUNCTION_ENTRY("canTransferSubsystems()");

    if (m_rightsLibrary)
    {
        DR_BEGIN_TRY
        {
            std::string currentSessionId(DutyDataStore::getSessionCache().getCurrentSessionId());
            EDecisionModule decisionModule;
            std::string reasonForDenial;

            FUNCTION_EXIT;
            return m_rightsLibrary->isActionPermittedOnResource(currentSessionId,
                                                                m_dutyAgentResourceKey, 
																//aca_DUTY_TRANSFER_SUBSYSTEM, //TD19218 LiDan 27,Jun,2007
																lActionId,//TD19218 LiDan 27,Jun,2007
                                                                reasonForDenial, 
                                                                decisionModule);
        }
        DR_END_TRY(DUTY_TRANSFER_SUBSYSTEM);
    }

    FUNCTION_EXIT;
    return false;
}


bool DutyRightsSingleton::canDegradeSubsystems()
{
    FUNCTION_ENTRY("canDegradeSubsystems()");

    if (m_rightsLibrary)
    {
        DR_BEGIN_TRY
        {
            std::string currentSessionId(DutyDataStore::getSessionCache().getCurrentSessionId());
            EDecisionModule decisionModule;
            std::string reasonForDenial;

            FUNCTION_EXIT;
            return m_rightsLibrary->isActionPermittedOnResource(currentSessionId,
                                                                m_dutyAgentResourceKey, 
                                                                aca_DUTY_DEGRADE_SUBSYSTEMS, 
                                                                reasonForDenial, 
                                                                decisionModule);
        }
        DR_END_TRY(DUTY_DEGRADE_SUBSYSTEMS);
    }

    FUNCTION_EXIT;
    return false;
}


void DutyRightsSingleton::receiveSessionUpdate(const SessionUpdate& sessionUpdate)
{
    FUNCTION_ENTRY("receiveSessionUpdate(const SessionUpdate& sessionUpdate)");

    // Profile override may affect the operator's access rights. The rights library subscribes
    // to session updates so that its rights cache can be kept up to date, however, there is
    // nothing to guarantee that it will receive these updates before SessionCache does, so a
    // SessionCache observer who queries the rights as a result of a session change notification
    // can potentially obtain an incorrect answer. This method enables SessionCache to reset
    // the rights cache after it has received a session update and before notifying its own
    // observers. Subsequent rights queries will be re-directed by the rights library to the 
    // Rights Agent to increase the odds of getting the correct result.

    if (sessionUpdate.updateType == TA_Security::ProfileUpdate)
    {
        if (m_rightsLibrary)
        {
            m_rightsLibrary->resetCache();
        }
    }
    
    FUNCTION_EXIT;
}

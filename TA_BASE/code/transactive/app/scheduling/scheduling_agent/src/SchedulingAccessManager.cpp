/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingAccessManager.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  *
  *
  * This class is used to manage access to the Scheduling Agent's
  * CORBA interface.
  *
  */


#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/RightsException.h"
#include "core/exceptions/src/CorbaException.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAccessManager.h"


namespace TA_Base_App
{

SchedulingAccessManager* SchedulingAccessManager::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable SchedulingAccessManager::m_singletonLock;


SchedulingAccessManager::SchedulingAccessManager()
    :
    m_rightsLibrary(NULL),
    m_resourceID(0)
{
    FUNCTION_ENTRY("SchedulingAccessManager");

    FUNCTION_EXIT;
}


SchedulingAccessManager::~SchedulingAccessManager()
{
    FUNCTION_ENTRY("~SchedulingAccessManager");

    delete m_rightsLibrary;
    m_rightsLibrary = NULL;

    FUNCTION_EXIT;
}


void SchedulingAccessManager::initialise(
    unsigned long entityKey)
{
    FUNCTION_ENTRY("initialise");

    TA_ASSERT(entityKey != 0, "Entity key must not be 0");

    TA_Base_Bus::RightsLibraryFactory rlFactory;

    m_rightsLibrary = rlFactory.buildRightsLibrary();

    try
    {
		//TD15947++
		//Agents are bound to duty since they are associated to a physical subsystem.
		//This was done for events visibility.
		//For the scheduling agent located only in OCC it will be impossible to get duty if the operator is running on a station mft.
		//In order to correct this we just pass in the parent key resource which is not associated to any physical subsystem.
		//Thus we do not need to acquire duty in order to perform scheduling operations. 
		//Only application rights checking are performed.
		//This is the previous behavior before events visibility was introduced.

        //m_resourceID = resource->getKey();

		TA_Base_Core::IEntityData * schedulingAgentEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
		TA_Base_Core::IEntityData * parentEntity =  TA_Base_Core::EntityAccessFactory::getInstance().getEntity(schedulingAgentEntity->getParent());

		m_resourceID = parentEntity->getKey();

		//++TD15947
		
		delete parentEntity;
		delete schedulingAgentEntity;

    }
    catch(...)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception");
        throw;
    }

    TA_ASSERT(m_resourceID != 0, "Resource ID must not be 0");

    FUNCTION_EXIT;
}


SchedulingAccessManager& SchedulingAccessManager::getInstance()
{
    TA_THREADGUARD(m_singletonLock);
    
    //
    // If this method has been called before and initialise was not called,
    // assert.
    //
    TA_ASSERT(m_instance == NULL || m_instance->m_resourceID != 0,
              "SchedulingAccessManager::initialise() must be called the "
              "first time getInstance() is invoked");

    if(m_instance == NULL)
    {
        m_instance = new SchedulingAccessManager();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
    }

    return *m_instance;
}


void SchedulingAccessManager::checkIfActionIsPermitted(
    const std::string& sessionID, unsigned long actionID)
{
    std::string errorMessage;
    TA_Base_Bus::EDecisionModule decisionModuleIgnored = TA_Base_Bus::UNDEFINED_DM;
    bool permitted = false;

    //
    // A RightsException can be thrown if there is a problem determining
    // whether access is allowed
    //
    try
    {
        //
        // Check if the action is permitted
        //		
        permitted = m_rightsLibrary->isActionPermittedOnResource(
                        sessionID,
                        m_resourceID,
                        actionID,
                        errorMessage,
                        decisionModuleIgnored);
    }
    catch(const TA_Base_Core::RightsException& ex)
    {
        // if it is caused by the scheduling agent being uncontactable, then throw a different exception
        if (ex.getFailType() == TA_Base_Core::RightsException::UNCONTACTABLE)
        {
            throw TA_Base_Bus::SchedulingAgentCorbaDef::NoRightsAgentException();
        }

        TA_THROW_CORBA(TA_Base_Core::AccessDeniedException(ex.what()));
    }
    catch(...)
    {
        TA_THROW_CORBA(TA_Base_Core::AccessDeniedException());
    }

    //
    // If the action isn't permitted, throw an AccessDeniedException
    //
    if( ! permitted )
    {
        TA_THROW_CORBA(TA_Base_Core::AccessDeniedException(errorMessage.c_str()));
    }
}



}; // TA_Base_App


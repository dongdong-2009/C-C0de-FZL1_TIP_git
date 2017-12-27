/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/ConfigData.cpp $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class stores configuration data required by the agent.
  * If queries are made on data that is currently not cached, 
  * the database is looked up and added to the storage.
  * ConfigData subscribes to online updates.
  * 
  */

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

namespace
{
    const unsigned long MINIMUM_TABLE_POLL_FREQUENCY(1);       // 1 sec
    const unsigned long MAXIMUM_TABLE_POLL_FREQUENCY(60 * 60); // 1 hour
}

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App;

ConfigData* ConfigData::s_configData = NULL;
TA_Base_Core::NonReEntrantThreadLockable ConfigData::s_singletonLock;


ConfigData& ConfigData::getInstance()
{
//    FUNCTION_ENTRY("getInstance()");

    if (s_configData == NULL)
    {
        TA_Base_Core::ThreadGuard guard(s_singletonLock);

        if (s_configData == NULL)
        {
            s_configData = new ConfigData();
        }
    }

//    FUNCTION_EXIT;
    return *s_configData;
}


void ConfigData::destroyInstance()
{
    FUNCTION_ENTRY("destroyInstance()"); 

    TA_Base_Core::ThreadGuard guard(s_singletonLock);

    delete s_configData;

    FUNCTION_EXIT;
}

  
ConfigData::ConfigData() :
    m_opaEntityData(NULL)
{
    FUNCTION_ENTRY("ConfigData()");

    try
    {
        std::string entityName(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));
        TA_ASSERT(!entityName.empty(), "Entity name run param not set");

        // Retrieve agent entity configuration from the database.
        m_opaEntityData = dynamic_cast<TA_Base_Core::OnlinePrintingAgentEntityData*>(
            TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName));
        TA_ASSERT(m_opaEntityData != NULL, "Entity data pointer is NULL");

        // Create a local copy of non-updateable agent parameters.
        m_printCommand = m_opaEntityData->getPrintCommand();
        m_columns = PrintOutputFormatter::parseColumnSpec(m_opaEntityData->getOutputColumns());

        // Register for agent entity configuration updates.
        TA_Base_Core::OnlineUpdateListener::getInstance().registerInterest(
            TA_Base_Core::ENTITY_OWNER, *this, m_opaEntityData->getKey());
    }
    catch (const TA_Base_Core::DatabaseException& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", ex.what());

        TA_THROW(OnlinePrintingAgentException("Failed to retrieve configuration data", ex.what()));
    }
    catch (const TA_Base_Core::DataException& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", ex.what());

        TA_THROW(OnlinePrintingAgentException("Failed to retrieve configuration data", ex.what()));
    }
    catch (...)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unknown exception", "");

        TA_THROW(OnlinePrintingAgentException("Failed to retrieve configuration data"));
    }

    FUNCTION_EXIT;
}


ConfigData::~ConfigData()
{
    FUNCTION_ENTRY("~ConfigData()");

    TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::ENTITY_OWNER, *this);

    {
        TA_Base_Core::ThreadGuard guard(m_observerLock);
        m_observers.clear();
    }

    {
        TA_Base_Core::ThreadGuard guard(m_configDataLock);
        delete m_opaEntityData;
        m_opaEntityData = NULL;
    }

    s_configData = NULL;

    FUNCTION_EXIT;
}


void ConfigData::registerObserver(IConfigDataObserver& configDataObserver)
{
    FUNCTION_ENTRY("registerObserver(IConfigDataObserver& configDataObserver)");

    TA_Base_Core::ThreadGuard guard(m_observerLock);
    m_observers.insert(&configDataObserver);

    FUNCTION_EXIT;
}


void ConfigData::deregisterObserver(IConfigDataObserver& configDataObserver)
{
    FUNCTION_ENTRY("deregisterObserver(IConfigDataObserver& configDataObserver)");

    TA_Base_Core::ThreadGuard guard(m_observerLock);
    m_observers.erase(&configDataObserver);

    FUNCTION_EXIT;
}


unsigned long ConfigData::getAgentEntityKey()
{
//    FUNCTION_ENTRY("getAgentEntityKey()");
    TA_ASSERT(m_opaEntityData != NULL, "Entity data pointer is NULL");

    TA_Base_Core::ThreadGuard guard(m_configDataLock);
    
//    FUNCTION_EXIT;  
    return m_opaEntityData->getKey();
}


unsigned long ConfigData::getAgentEntityTypeKey()
{
//    FUNCTION_ENTRY("getAgentEntityTypeKey()");
    TA_ASSERT(m_opaEntityData != NULL, "Entity data pointer is NULL");

    TA_Base_Core::ThreadGuard guard(m_configDataLock);
    
//    FUNCTION_EXIT;  
    return m_opaEntityData->getTypeKey();
}


std::string ConfigData::getAgentEntityName()
{
//    FUNCTION_ENTRY("getAgentEntityName()");
    TA_ASSERT(m_opaEntityData != NULL, "Entity data pointer is NULL");

    TA_Base_Core::ThreadGuard guard(m_configDataLock);
    
//    FUNCTION_EXIT;  
    return m_opaEntityData->getName();
}

std::string ConfigData::getAgentEntityAssetName()
{
    TA_ASSERT(m_opaEntityData != NULL, "Entity data pointer is NULL");

    TA_Base_Core::ThreadGuard guard(m_configDataLock);
    
    return m_opaEntityData->getAssetName();
}

unsigned long ConfigData::getAgentEntitySubsystemKey()
{
//    FUNCTION_ENTRY("getAgentEntitySubsystemKey()");
    TA_ASSERT(m_opaEntityData != NULL, "Entity data pointer is NULL");

    TA_Base_Core::ThreadGuard guard(m_configDataLock);
    
//    FUNCTION_EXIT;  
    return m_opaEntityData->getSubsystem();
}


unsigned long ConfigData::getAgentEntityLocationKey()
{
//    FUNCTION_ENTRY("getAgentEntityLocationKey()");
    TA_ASSERT(m_opaEntityData != NULL, "Entity data pointer is NULL");

    TA_Base_Core::ThreadGuard guard(m_configDataLock);
    
//    FUNCTION_EXIT;  
    return m_opaEntityData->getLocation();
}


const std::string& ConfigData::getPrintCommand()
{
//    FUNCTION_ENTRY("getPrintCommand()");
//    FUNCTION_EXIT;
    return m_printCommand;  // never changes so no thread guards
}


const ColumnList& ConfigData::getColumns()
{
//    FUNCTION_ENTRY("getColumns()");
//    FUNCTION_EXIT;
    return m_columns;  // never changes so no thread guards
}


unsigned long ConfigData::getTablePollFrequency()
{
//    FUNCTION_ENTRY("getTablePollFrequency()");

    TA_ASSERT(m_opaEntityData != NULL, "Entity data pointer is NULL");

    TA_Base_Core::ThreadGuard guard(m_configDataLock);
    unsigned long tablePollFrequency(m_opaEntityData->getViewPollFrequency());
    
    // Check that the configured value is within bounds.
    if (tablePollFrequency < MINIMUM_TABLE_POLL_FREQUENCY || tablePollFrequency > MAXIMUM_TABLE_POLL_FREQUENCY)
    {
        std::ostringstream msg;
        msg << "Configured table polling frequency is outside the allowed range "
            << MINIMUM_TABLE_POLL_FREQUENCY << " - " << MAXIMUM_TABLE_POLL_FREQUENCY << " seconds.";

        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, msg.str().c_str());
        TA_THROW(OnlinePrintingAgentException(msg.str()));
    }    

//    FUNCTION_EXIT;  
    return tablePollFrequency;
}


bool ConfigData::isPrintingEnabled()
{
//    FUNCTION_ENTRY("isPrintingEnabled()");

    TA_ASSERT(m_opaEntityData != NULL, "Entity data pointer is NULL");

    TA_Base_Core::ThreadGuard guard(m_configDataLock);
    
//    FUNCTION_EXIT;  
    return m_opaEntityData->isPrintingEnabled();
}


bool ConfigData::printAlarmEventsOnly()
{
//    FUNCTION_ENTRY("printAlarmEventsOnly()");

    TA_ASSERT(m_opaEntityData != NULL, "Entity data pointer is NULL");

    TA_Base_Core::ThreadGuard guard(m_configDataLock);
    
//    FUNCTION_EXIT;  
    return m_opaEntityData->printAlarmEventsOnly();
}


void ConfigData::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    FUNCTION_ENTRY("processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)");

    // Ignore all unrelated notifications. This should not happen as we only registered interest in the single agent entity.
    if (updateEvent.getKey() != m_opaEntityData->getKey())
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Ignoring all configuration change events not related to agent entity.");
        FUNCTION_EXIT;
        return;
    }

    // Ignore additions/deletions.
    if (updateEvent.getModifications() != TA_Base_Core::Update)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Ignoring all configuration change events other than 'update'.");
        FUNCTION_EXIT;
        return;
    }

    EntityParameterList updatedParams(updateEvent.getChangedParams());

    // Ignore bogus updates.
    if (updatedParams.empty())
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Ignoring empty configuration update.");
        FUNCTION_EXIT;
        return;
    }

    EntityParameterList notifyParameters;

    // Force any updateable parameters to be re-read from the database the next time they are accessed.
    for (EntityParameterList::const_iterator paramIt(updatedParams.begin()); paramIt != updatedParams.end(); paramIt++)
    {
        if (*paramIt == TA_Base_Core::OnlinePrintingAgentEntityData::VIEW_POLL_FREQUENCY_PARAM ||
            *paramIt == TA_Base_Core::OnlinePrintingAgentEntityData::IS_PRINTING_ENABLED_PARAM ||
            *paramIt == TA_Base_Core::OnlinePrintingAgentEntityData::PRINT_ALARM_EVENTS_ONLY_PARAM)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Online updateable parameter has been modified (%s)", paramIt->c_str()); 

            TA_ASSERT(m_opaEntityData != NULL, "Entity data pointer is NULL");

            TA_Base_Core::ThreadGuard guard(m_configDataLock);
            m_opaEntityData->invalidate();

            notifyParameters.push_back(*paramIt);
        }
    }

    // Notify observers about changes to online updateable parameters.
    if (!notifyParameters.empty())
    {
        ConfigDataObserverSet observers;

        {
            TA_Base_Core::ThreadGuard guard(m_observerLock);     
            observers = m_observers;
        }

        for (ConfigDataObserverSet::iterator obsIt(observers.begin()); obsIt != observers.end(); obsIt++)
        {
            (*obsIt)->parametersChanged(notifyParameters);
        }
    }

    FUNCTION_EXIT;
}

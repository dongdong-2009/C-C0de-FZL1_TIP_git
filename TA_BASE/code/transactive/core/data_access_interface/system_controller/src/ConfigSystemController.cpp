/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/ConfigSystemController.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2016/10/18 11:29:58 $
  * Last modified by:  $Author: Ouyang $
  *
  * This class is an implementation of the IConfigSystemController
  * interface. It provides read and write access to the data stored in a single
  * entry in the sn_system_controller table.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/system_controller/src/ConfigSystemController.h"
#include "core/data_access_interface/system_controller/src/SystemControllerHelper.h"
#include "core/data_access_interface/system_controller/src/GroupMonitorAccessFactory.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Core
{
    const std::string ConfigSystemController::NAME = "Name";
    const std::string ConfigSystemController::AGENT_POLL_PERIOD = "Agent Poll Period";
    const std::string ConfigSystemController::HEARTBEAT_SEND_PERIOD = "Heartbeat Send Period";
    const std::string ConfigSystemController::HEARTBEAT_SEND_NUMBER = "Heartbeat Send Number";

    const std::string ConfigSystemController::AGENT_RESPONSE_TIMEOUT = "Agent Response Timeout";
    const std::string ConfigSystemController::SERVER = "Server";
    const std::string ConfigSystemController::GROUP = "Group";
    const std::string ConfigSystemController::DEBUG_LOG_DIR = "Debug Log Directory";
    const std::string ConfigSystemController::BIN_DIR = "Bin Directory";
    const std::string ConfigSystemController::MONITORED_GROUPS = "Monitored Groups";
    const std::string ConfigSystemController::PROCESS = "Process ";

    const std::string ConfigSystemController::ADDED_CONTROL = "Run as Control";
    const std::string ConfigSystemController::ADDED_MONITOR = "Run as Monitor";
    const std::string ConfigSystemController::REMOVED = "Not Run";

    long ConfigSystemController::s_nextAvailableIdentifier = 0;


    ConfigSystemController::ConfigSystemController(unsigned long key)
        : m_isValidData(false),
          m_helper( new SystemControllerHelper(key) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigSystemController::ConfigSystemController()
        : m_isValidData(false),
          m_helper( new SystemControllerHelper() ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


	ConfigSystemController::ConfigSystemController( const ConfigSystemController& theSystemController)
        : m_isValidData(false),
          m_helper( new SystemControllerHelper(*(theSystemController.m_helper)) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++),
          // Set the new monitored groups to be a copy and leave the original one blank
          m_newMonitoredGroups(theSystemController.m_newMonitoredGroups),
          m_processes(theSystemController.m_processes)
    {
    }


    ConfigSystemController::~ConfigSystemController()
    {
        try
        {
            if (m_helper != NULL)
            {
                delete m_helper;
		        m_helper = NULL;
            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught by the destructor");
        }
    }


    unsigned long ConfigSystemController::getKey()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getKey();
    }


    std::string ConfigSystemController::getName()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getName();
    }


    unsigned long ConfigSystemController::getAgentPollPeriod()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getAgentPollPeriod();
    }


    unsigned long ConfigSystemController::getHeartbeatSendPeriod()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getHeartbeatSendPeriod();
    }


    unsigned long ConfigSystemController::getHeartbeatSendNumber()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getHeartbeatSendNumber();
    }


    unsigned long ConfigSystemController::getAgentResponseTimeout()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getAgentResponseTimeout();
    }


    unsigned long ConfigSystemController::getServerEntityKey()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getServerEntityKey();
    }


    std::string ConfigSystemController::getServerByName()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getEntityName( m_helper->getServerEntityKey() );
    }


    unsigned long ConfigSystemController::getGroupId()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getGroupId();
    }


    std::string ConfigSystemController::getGroupByName()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getGroupName( m_helper->getGroupId() );
    }


    std::string ConfigSystemController::getDebugLogDir()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getDebugLogDir();
    }


    std::string ConfigSystemController::getBinDir()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getBinDir();
    }


    time_t ConfigSystemController::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getDateCreated();
    }


    time_t ConfigSystemController::getDateModified()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getDateModified();
    }


    std::set<unsigned long> ConfigSystemController::getMonitoredGroups()
    {
        if (!m_isValidData && !isNew())
        {
            reload();
        }

        return m_newMonitoredGroups;
    }

        
    ConfigSystemController::ProcessMap ConfigSystemController::getProcesses()
    {
        if (!m_isValidData && !isNew())
        {
            reload();
        }

        return m_processes;
    }

        
    void ConfigSystemController::setName(const std::string& name)
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        updateChanges(NAME, m_helper->getName(), name);

        m_helper->setName(name);
    }

        
    void ConfigSystemController::setAgentPollPeriod(unsigned long agentPollPeriod)
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        std::ostringstream oldPollPeriod;
        oldPollPeriod << m_helper->getAgentPollPeriod();

        std::ostringstream newPollPeriod;
        newPollPeriod << agentPollPeriod;

        updateChanges(AGENT_POLL_PERIOD, oldPollPeriod.str(), newPollPeriod.str());

        m_helper->setAgentPollPeriod(agentPollPeriod);
    }


    void ConfigSystemController::setHeartbeatSendPeriod(unsigned long heartbeatSendPeriod)
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        std::ostringstream oldSendPeriod;
        oldSendPeriod << m_helper->getHeartbeatSendPeriod();

        std::ostringstream newSendPeriod;
        newSendPeriod << heartbeatSendPeriod;

        updateChanges(HEARTBEAT_SEND_PERIOD, oldSendPeriod.str(), newSendPeriod.str());

        m_helper->setHeartbeatSendPeriod(heartbeatSendPeriod);
    }


    void ConfigSystemController::setHeartbeatSendNumber(unsigned long heartbeatSendNumber)
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        std::ostringstream oldSendNumber;
        oldSendNumber << m_helper->getHeartbeatSendNumber();

        std::ostringstream newSendNumber;
        newSendNumber << heartbeatSendNumber;

        updateChanges(HEARTBEAT_SEND_NUMBER, oldSendNumber.str(), newSendNumber.str());

        m_helper->setHeartbeatSendNumber(heartbeatSendNumber);
    }

    
    void ConfigSystemController::setAgentResponseTimeout(unsigned long agentResponseTimeout)
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        std::ostringstream oldTimeout;
        oldTimeout << m_helper->getAgentResponseTimeout();

        std::ostringstream newTimeout;
        newTimeout << agentResponseTimeout;

        updateChanges(AGENT_RESPONSE_TIMEOUT, oldTimeout.str(), newTimeout.str());

        m_helper->setAgentResponseTimeout(agentResponseTimeout);
    }


    void ConfigSystemController::setServerEntityKey(unsigned long serverEntityKey)
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        std::ostringstream oldServerEntityKey;
        oldServerEntityKey << m_helper->getServerEntityKey();

        std::ostringstream newServerEntityKey;
        newServerEntityKey << serverEntityKey;

        updateChanges(SERVER, oldServerEntityKey.str(), newServerEntityKey.str());

        m_helper->setServerEntityKey(serverEntityKey);
    }


    void ConfigSystemController::setGroupId(unsigned long groupId)
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        std::ostringstream oldGroup;
        oldGroup << m_helper->getGroupId();

        std::ostringstream newGroup;
        newGroup << groupId;

        updateChanges(GROUP, oldGroup.str(), newGroup.str());

        m_helper->setGroupId(groupId);
    }


    void ConfigSystemController::setDebugLogDir(const std::string& debugLogDir)
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        updateChanges(DEBUG_LOG_DIR, m_helper->getDebugLogDir(), debugLogDir);
        
        m_helper->setDebugLogDir(debugLogDir);
    }


    void ConfigSystemController::setBinDir(const std::string& binDir)
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        updateChanges(BIN_DIR, m_helper->getBinDir(), binDir);
        
        m_helper->setBinDir(binDir);
    }


    void ConfigSystemController::setMonitoredGroup( unsigned long groupKey, bool isMonitored)
    {
        FUNCTION_ENTRY("setMonitoredGroup");

        if (!m_isValidData && !isNew())
        {
            reload();
        }

        if (isMonitored)
        {
            // This means we're adding a new group so add it to the list of new groups
            m_newMonitoredGroups.insert(groupKey);
        }
        else
        {
            // This means we're removing a group so remove it from the list of new groups
            m_newMonitoredGroups.erase( groupKey );
        }

        // Now figure out if the two sets (original and new) are different.
        std::ostringstream setDifference;
        std::set_symmetric_difference(m_newMonitoredGroups.begin(), m_newMonitoredGroups.end(),
                                      m_originalMonitoredGroups.begin(), m_originalMonitoredGroups.end(),
                                      std::ostream_iterator<unsigned long>(setDifference,""));

        // If this is empty then there were no set differences
        // We just push back any string as we will generate the correct values later.
        if (setDifference.str().empty())
        {
            updateChanges(MONITORED_GROUPS, "old", "old");
        }
        else
        {
            updateChanges(MONITORED_GROUPS, "old", "new");
        }

        FUNCTION_EXIT;
    }


    void ConfigSystemController::setProcess(unsigned long key, OperatingMode mode, bool isRun)
    {
        FUNCTION_ENTRY("setProcess");

        if (!m_isValidData && !isNew())
        {
            reload();
        }

        // Generate a string for the key so we can add this to our list of changes
        std::ostringstream keyString;
        keyString << key;

        if (isRun) // We are adding the process
        {
            // Determine what we will store in our updates for the new value
            std::string newValue = ADDED_CONTROL;
            if (mode == MONITOR)
            {
                newValue = ADDED_MONITOR;
            }

            ProcessMap::iterator matching = m_processes.find( key);
            if (matching != m_processes.end())
            {
                // We already have an entry so just update it with the new operating mode

                std::string oldValue = ADDED_CONTROL;
                if (matching->second == MONITOR)
                {
                    oldValue = ADDED_MONITOR;
                }

                matching->second = mode;

                updateChanges(keyString.str(), oldValue, newValue);
            }
            else
            {
                // Add the item into the map
                updateChanges(keyString.str(), REMOVED, newValue);

                m_processes.insert( ProcessMap::value_type(key,mode) );
            }

            FUNCTION_EXIT;
            return;
        }

        // else we are removing the process

        ProcessMap::iterator matching = m_processes.find( key );
        if (matching != m_processes.end() )
        {
            // Determine what we will store in our updates for the old value
            std::string oldValue = ADDED_CONTROL;
            if (matching->second == MONITOR)
            {
                oldValue = ADDED_MONITOR;
            }

            updateChanges( keyString.str(), oldValue, REMOVED);

            // This is the item we need to remove
            m_processes.erase( matching );

            FUNCTION_EXIT;
            return;
        }

        FUNCTION_EXIT;
    }
        

    void ConfigSystemController::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        m_helper->invalidate();

        m_isValidData = false;

        m_systemControllerChanges.clear();

        m_newMonitoredGroups.clear();
        m_originalMonitoredGroups.clear();

        m_processes.clear();

        FUNCTION_EXIT;
    }

    
    void ConfigSystemController::deleteThisSystemController()
    {
        FUNCTION_ENTRY("deleteThisSystemController");

        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
		
		// TD 11416 Disallow delete of SystemController if it is linked to other tables
		// Do not delete the child data

        // writeMonitoredGroupChanges();

        // Delete all links to managed processes
        // std::ostringstream sql;
        // sql << "delete SN_MANAGED_PROCESS where SNSCON_ID = " << m_helper->getKey();

        // TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Write)->executeModification( sql.str() );

        // Now delete the System Controller entry
        m_helper->deleteThisSystemController();

        // Clear the new monitored groups so all existing ones will be deleted
        m_newMonitoredGroups.clear();

        FUNCTION_EXIT;
    }


    void ConfigSystemController::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        m_helper->applyChanges();

        writeMonitoredGroupChanges();

        writeProcessChanges();

        m_systemControllerChanges.clear();

        FUNCTION_EXIT;
    }

    
    ItemChanges ConfigSystemController::getAllItemChanges()
    {
        ItemChanges changes;

        for (ItemChanges::iterator iter = m_systemControllerChanges.begin(); iter != m_systemControllerChanges.end(); ++iter)
        {
            if( (iter->first.compare(NAME) == 0) || 
                (iter->first.compare(AGENT_POLL_PERIOD) == 0) ||
                (iter->first.compare(HEARTBEAT_SEND_PERIOD) == 0) ||
                (iter->first.compare(DEBUG_LOG_DIR) == 0) ||
				(iter->first.compare(BIN_DIR) == 0) )
            {
                // For any of these we just use the values stored so add them straight into the new map
                changes.insert(ItemChanges::value_type( iter->first, iter->second ) );
                continue;
            }


			if( std::string::npos == iter->first.find_first_not_of("0123456789") )
            {
                // The change contains all numbers so this must be a process key

                // So first get the process key out of the string
                std::istringstream processKeyString;
                processKeyString.str( iter->first );
                unsigned long processKey;
                processKeyString >> processKey;

                std::string profileNameString = PROCESS;
                profileNameString += m_helper->getEntityName( processKey );
                
                changes.insert(ItemChanges::value_type( profileNameString, iter->second) );
                continue;
            }

            // For all other stored changes we need to convert keys into names

            // First get the unsigned long keys out of the strings if possible

            std::istringstream oldKeyString;
            oldKeyString.str( iter->second.oldValue );
            unsigned long oldKey;
            oldKeyString >> oldKey;

            std::istringstream newKeyString;
            newKeyString.str( iter->second.newValue );
            unsigned long newKey;
            newKeyString >> newKey;

            Values convertedValues;

            if (iter->first.compare(SERVER) == 0)
            {
                convertedValues.newValue = m_helper->getEntityName( newKey );
                convertedValues.oldValue = m_helper->getEntityName( oldKey );
            }
            else if (iter->first.compare(GROUP) == 0)
            {
                convertedValues.newValue = m_helper->getGroupName( newKey );
                convertedValues.oldValue = m_helper->getGroupName( oldKey );
            }
            else if (iter->first.compare(MONITORED_GROUPS) == 0)
            {
                convertedValues.newValue = getGroupNames( m_newMonitoredGroups );
                convertedValues.oldValue = getGroupNames( m_originalMonitoredGroups );
            }

            changes.insert(ItemChanges::value_type( iter->first, convertedValues) );
        }

        return changes;
    }


    bool ConfigSystemController::isNew()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");

        return m_helper->isNew();
    }


    void ConfigSystemController::reload()
    {
        FUNCTION_ENTRY("reload");

        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);
        
        // Create the SQL string to retrieve the data of the system controller
        // based upon the key
        //delete by lin
        //std::ostringstream sql;
        //sql << "select SNSCGR_ID from SN_GROUP_MONITOR where SNSCON_ID = " << m_helper->getKey();

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "KEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're 
        // done with it.
        //IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
        

		//waitforcheck delete by lin
		SQLStatement strSql;      
        databaseConnection->prepareSQLStatement(strSql, SN_GROUP_MONITOR_SELECT_22502,
			  m_helper->getKey());
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        do
        {
            for (unsigned int i = 0; i < data->getNumRows(); ++i)
            {
                unsigned long key = data->getUnsignedLongData(i, keyColumn);
                m_originalMonitoredGroups.insert( key );
                m_newMonitoredGroups.insert( key );
            }

            delete data;
            data = NULL;
        }while (databaseConnection->moreData(data) );


        //sql.str("");
        //sql << "select MANAGED_PROCESS_PKEY, DEFAULT_OPERATING_MODE from SN_MANAGED_PROCESS where SNSCON_ID = "
        //    << m_helper->getKey();

        std::string processKeyColumn = "key";
        std::string operatingModeColumn = "operating";
        columnNames.clear();
        columnNames.push_back(processKeyColumn);
        columnNames.push_back(operatingModeColumn);
      
        //data = databaseConnection->executeQuery(sql.str(),columnNames);
        
		//waitforcheck delete by lin
		SQLStatement strSqlTwo;
		databaseConnection->prepareSQLStatement(strSqlTwo, SN_SYSTEM_CONTROLLER_SELECT_23513,
			 m_helper->getKey());
		data = databaseConnection->executeQuery(strSqlTwo,columnNames);

        do
        {
            for (unsigned int i = 0; i < data->getNumRows(); ++i)
            {
                unsigned long key = data->getUnsignedLongData(i, processKeyColumn);

                OperatingMode mode = MONITOR;
                if (data->getIntegerData(i, operatingModeColumn) == CONTROL)
                {
                    mode = CONTROL;
                }

                m_processes.insert( ProcessMap::value_type(key,mode) );
            }

            delete data;
            data = NULL;
        }while (databaseConnection->moreData( data ) );
 

        // Need to record that we now have valid data.
        m_isValidData = true;

        FUNCTION_EXIT;
    }

    
    void ConfigSystemController::updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_systemControllerChanges.find(name);

        if (matching != m_systemControllerChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_systemControllerChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_systemControllerChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


    std::string ConfigSystemController::getGroupNames(std::set<unsigned long>& groupKeys)
    {
        FUNCTION_ENTRY("getGroupNames");
        std::string names("");

        for (std::set<unsigned long>::iterator iter = groupKeys.begin(); iter != groupKeys.end(); ++iter)
        {
            if (iter != groupKeys.begin())
            {
                // If this is not the first element then we need to insert a comma
                names += ",";
            }

            try
            {
                names += GroupMonitorAccessFactory::getInstance().getGroupName(*iter);
            }
            catch ( const DataException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the group name. Using 'Unknown'");
                names += "Unknown";
            }
            catch ( const DatabaseException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the group name. Using 'Unknown'");
                names += "Unknown";
            }
        }

        return names;
        FUNCTION_EXIT;
    }


    void ConfigSystemController::writeMonitoredGroupChanges()
    {
        FUNCTION_ENTRY("writeMonitoredGroupChanges");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);

        
        // First determine which groups have been removed and no longer need to be in the database
        std::ostringstream setDifference1;
        std::set_difference(m_originalMonitoredGroups.begin(), m_originalMonitoredGroups.end(),
                            m_newMonitoredGroups.begin(), m_newMonitoredGroups.end(),
                            std::ostream_iterator<unsigned long>(setDifference1,","));

        // If this contains a list of monitored groups then we must delete them
        if (!setDifference1.str().empty())
        {
            // First delete the trailing comma that will have been inserted
            std::string keys = setDifference1.str().substr(0, setDifference1.str().length()-1);
            //std::ostringstream sql;
            //sql << "delete SN_GROUP_MONITOR where SNSCON_ID = " << m_helper->getKey();
            //sql << " and SNSCGR_ID in (" << keys << ")";

            //databaseConnection->executeModification( sql.str() );
            
			//waitforcheck delete by lin
			SQLStatement strSql;			
			databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_DELETE_23902,
				 m_helper->getKey(), keys); 
			databaseConnection->executeModification(strSql);

        }

        // Now determine which groups need to be added to the database

        std::ostringstream setDifference2;
        std::set_difference(m_newMonitoredGroups.begin(), m_newMonitoredGroups.end(),
                            m_originalMonitoredGroups.begin(), m_originalMonitoredGroups.end(),
                            std::ostream_iterator<unsigned long>(setDifference2,","));

        // If this contains a list of monitored groups then we must add them
        if (!setDifference2.str().empty())
        {
            // They are separated by commas so we must step through and insert each one
            std::string::size_type startPos = 0;
            std::string::size_type commaPos = setDifference2.str().find(",",startPos);
			while( commaPos != std::string::npos)
            {
                std::string key = setDifference2.str().substr(startPos, commaPos - startPos);
                //std::ostringstream sql;
                //sql << "insert into SN_GROUP_MONITOR(SNSCON_ID,SNSCGR_ID) values (";
                //sql << m_helper->getKey() << "," << key << ")";

                //databaseConnection->executeModification( sql.str() );
                
				//waitforcheck delete by lin
				SQLStatement strSql;	
				databaseConnection->prepareSQLStatement(strSql, SN_GROUP_MONITOR_INSERT_22503,
					 m_helper->getKey(), key);
				databaseConnection->executeModification( strSql );

                startPos = commaPos + 1;
                commaPos = setDifference2.str().find(",",startPos);
            }
        }

        m_originalMonitoredGroups = m_newMonitoredGroups;

        FUNCTION_EXIT;
    }


    void ConfigSystemController::writeProcessChanges()
    {
        FUNCTION_ENTRY("writeProcessChanges");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Write);

		//delete by lin
        //std::ostringstream sql;
        //sql << "select MANAGED_PROCESS_PKEY, DEFAULT_OPERATING_MODE from SN_MANAGED_PROCESS where SNSCON_ID = " << m_helper->getKey();

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string processKeyColumn = "KEY";
        std::string modeColumn = "Mode";
        columnNames.push_back(processKeyColumn);
        columnNames.push_back(modeColumn);

        // execute the query
        //IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
        
		//waitforcheck delete by lin
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23513,
				m_helper->getKey());
		IData* data = databaseConnection->executeQuery(strSql,columnNames);
				
        ProcessMap localProcessesToAdd = m_processes;
        ProcessMap databaseProcessesToRemove;

        // Now we step through the keys retrieved from the database.
        // If we find the key in both lists then the database is up-to-date and nothing needs to be done to the
        // lists (ie we dont' add it to the databaseProcessesToRemove list). At the end the keys left in the localProcessesToAdd
        // list need to be added to the database and the processes left in databaseProcessesToRemove must be removed from the
        // database.
        do
        {
            for (unsigned int i = 0; i < data->getNumRows(); ++i)
            {
                unsigned long currentKey = data->getUnsignedLongData(i, processKeyColumn);
                OperatingMode mode = MONITOR;
                if (data->getIntegerData(i, modeColumn) == CONTROL)
                {
                    mode = CONTROL;
                }
            
                ProcessMap::iterator matching = localProcessesToAdd.find( currentKey );
                if (matching != localProcessesToAdd.end())
                {
                    if (matching->second == mode)
                    {
                        // This key has been found and the mode matches so no changes have been made
                        // We remove it from the localProcessesToAdd list as we do not need to remove it from the database
                        localProcessesToAdd.erase( matching );
                    }
                    else
                    {
                        // The mode is different so we need to remove this item from the database. A new item
                        // will be added for the key with the correct mode.
                        databaseProcessesToRemove.insert( ProcessMap::value_type(currentKey,mode) );
                    }
                }
                else
                {
                    // This key has obviously been removed from our local list so it will need to be removed from the database
                    databaseProcessesToRemove.insert( ProcessMap::value_type(currentKey,mode) );
                }
            }

            delete data;
            data = NULL;
        }while( databaseConnection->moreData(data) );


        // Now we add and delete from/to the database as required
       
        // First lets delete old values from the database
        for (ProcessMap::iterator iter = databaseProcessesToRemove.begin(); iter != databaseProcessesToRemove.end(); ++iter)
        {
			//delete by lin
            //std::ostringstream sql;
            //sql << "delete SN_MANAGED_PROCESS where SNSCON_ID = " << m_helper->getKey();
            //sql << " and MANAGED_PROCESS_PKEY = " << iter->first << " and DEFAULT_OPERATING_MODE = ";
            //sql << iter->second;

            //databaseConnection->executeModification(sql.str().c_str());
            
			//waitforcheck delete by lin
			SQLStatement strSql;	
			databaseConnection->prepareSQLStatement(strSql, SN_MANAGED_PROCESS_DELETE_24004,
				 m_helper->getKey(), iter->first, iter->second);
			databaseConnection->executeModification(strSql);
        }

        // Now lets add new values to the database
        for (ProcessMap::iterator iterToAdd = localProcessesToAdd.begin();
             iterToAdd != localProcessesToAdd.end(); ++iterToAdd)
        {
			 //delete by lin
             //std::ostringstream sql;
             //sql << "insert into SN_MANAGED_PROCESS(SNSCON_ID, MANAGED_PROCESS_PKEY, DEFAULT_OPERATING_MODE) ";
             //sql << "values(" << m_helper->getKey() << "," << iterToAdd->first << "," << iterToAdd->second << ")";

            //databaseConnection->executeModification(sql.str().c_str());
            
			//waitforcheck delete by lin
			SQLStatement strSql;
			databaseConnection->prepareSQLStatement(strSql, SN_MANAGED_PROCESS_INSERT_24005,
				 m_helper->getKey(), iterToAdd->first, iterToAdd->second);
			databaseConnection->executeModification(strSql);

        }

        FUNCTION_EXIT;
    }

} // namespace TA_Base_Core

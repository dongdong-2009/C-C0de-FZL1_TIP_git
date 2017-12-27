/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AlarmRuleData.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/20 22:28:15 $
  * Last modified by:  $Author: huirong.luo $
  *
  * AlarmRuleData is an implementation of IAlarmRuleData.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/alarm_rule/src/AlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleHelper.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>


namespace TA_Base_Core
{
    const std::string AlarmRuleData::ENTITY_PARAM       = "Entity";
    const std::string AlarmRuleData::ENTITY_TYPE_PARAM  = "Entity Type";
    const std::string AlarmRuleData::RULE_TRIGGER_PARAM = "Rule Trigger";
    const std::string AlarmRuleData::ALARM_TYPE_PARAM   = "Alarm Type";
	const std::string AlarmRuleData::ALARM_DESCRIPTION_PARAM = "Alarm Description";

    
    long AlarmRuleData::s_nextAvailableIdentifier = 0;

	AlarmRuleData::AlarmRuleData(AlarmRuleHelper* helper)
		:
        m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
        TA_ASSERT(helper != NULL, "Helper must not be NULL");
        
        m_alarmRuleHelper = helper;
	}

    AlarmRuleData::AlarmRuleData(const AlarmRuleData& theRule)
        :
        m_alarmRuleHelper(new AlarmRuleHelper(*theRule.m_alarmRuleHelper))
    {
    }


    AlarmRuleData::~AlarmRuleData()
    {
        delete m_alarmRuleHelper;
        m_alarmRuleHelper = NULL;
    }


    AlarmRuleHelper& AlarmRuleData::getHelper() const
    {
        return *m_alarmRuleHelper;
    }


    unsigned long AlarmRuleData::getUniqueIdentifier()
    {
        return m_uniqueIdentifier;
    }


    ItemChanges AlarmRuleData::getAllItemChanges()
    {
        return m_alarmRuleChanges;
    }


    bool AlarmRuleData::hasChanged()
    {
        return !m_alarmRuleChanges.empty();
    }


    bool AlarmRuleData::isNew()
    {
        return m_alarmRuleHelper->isNew();
    }


    unsigned long AlarmRuleData::getKey()
    {
        return m_alarmRuleHelper->getKey();
    }


    std::string AlarmRuleData::getName()
    {
        std::ostringstream name;
        name <<
            "Entity: "      << getEntityName()     << ", " <<
            "Entity Type: " << getEntityTypeName() << ", " <<
            "Alarm Type: " << getAlarmTypeName() << ", " <<
            "Alarm Description: "  << getAlarmDescription();

        return name.str();
    }


    void AlarmRuleData::invalidate()
    {
        m_alarmRuleHelper->invalidate();
        m_alarmRuleChanges.clear();
    }


    void AlarmRuleData::applyChanges()
    {
        m_alarmRuleHelper->applyChanges();

        m_alarmRuleChanges.clear();
    }

    
    void AlarmRuleData::deleteThisObject()
    {
        m_alarmRuleHelper->deleteThisObject();
    }


    void AlarmRuleData::setName(const std::string& name)
    {
        TA_ASSERT(false, "This method does not apply to Alarm Rules.");
    }



    unsigned long AlarmRuleData::getEntityTypeKey()
    {
        return m_alarmRuleHelper->getEntityTypeKey();
    }


    void AlarmRuleData::setEntityTypeKey(unsigned long entityTypeKey)
    {
        unsigned long oldEntityTypeKey = m_alarmRuleHelper->getEntityTypeKey();

        m_alarmRuleHelper->setEntityTypeKey(entityTypeKey);

		std::string oldEntityType;
		try
		{
			oldEntityType = getEntityTypeName(oldEntityTypeKey);
		}
		catch ( const TA_Base_Core::DataException& )
		{
			
			oldEntityType = "";	
			
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
				"EntityType with key %d does not exist. Setting value to empty.", 
				oldEntityTypeKey);
		}

        std::string entityType    = getEntityTypeName(entityTypeKey);

        updateAlarmRuleChanges(
            ENTITY_TYPE_PARAM, oldEntityType, entityType);
    }


    void AlarmRuleData::setEntityType(const std::string& entityTypeName)
    {
        FUNCTION_ENTRY("setEntityType");

        if (entityTypeName.empty())
        {
            setEntityTypeKey(0);
            FUNCTION_EXIT;
            return;
        }

        IData* data = NULL;

        try
        {
            // Get a connection to the database
			// TD19819 Yanrong++
            //IDatabase* databaseConnection =
            //    TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Write);
			IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
			// ++Yanrong

           /* std::ostringstream sql; 
            sql << "SELECT pkey FROM EntityType where name = '" 
                << databaseConnection->escapeQueryString(entityTypeName).c_str() << "'";*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4001,
//			std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYTYPE_SELECT_4001,
				databaseConnection->escapeQueryString(entityTypeName) );

            std::string keyColumn = "key";
            std::vector<std::string> columnNames;
            columnNames.push_back(keyColumn);

            data = databaseConnection->executeQuery(strSql, columnNames);

            if (data->getNumRows() != 1)
            {
                const char* message = "No entitytype with the specified name";
                TA_THROW( DataException(message, DataException::NO_VALUE, "") );
            }

            setEntityTypeKey( data->getUnsignedLongData(0, keyColumn) );
        }
        catch( ... )
        {
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }
            throw;
        }

        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }


    std::string AlarmRuleData::getEntityTypeName()
    {
        return m_alarmRuleHelper->getEntityTypeName();
    }


    unsigned long AlarmRuleData::getEntityKey()
    {
        return m_alarmRuleHelper->getEntityKey();
    }


    void AlarmRuleData::setEntityKey(unsigned long entityKey)
    {
        unsigned long oldEntityKey = m_alarmRuleHelper->getEntityKey();

        m_alarmRuleHelper->setEntityKey(entityKey);

		std::string oldEntity;

		try
		{
			oldEntity = getEntityName(oldEntityKey);
		}
		catch ( const TA_Base_Core::DataException& )
		{
			
			oldEntity = "";	

			LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Entity with key %d does not exist. Setting value to empty.", 
						 oldEntityKey);
		}

        std::string entity    = getEntityName(entityKey);
        
        updateAlarmRuleChanges(
            ENTITY_PARAM, oldEntity, entity);
    }

    void AlarmRuleData::setEntity(const std::string& entityName)
    {
        FUNCTION_ENTRY("setEntity");

        if (entityName.empty())
        {
            setEntityKey(0);
            FUNCTION_EXIT;
            return;
        }


        IEntityData* data = NULL;
        try
        {
            data = EntityAccessFactory::getInstance().getEntity(entityName);

            setEntityKey( data->getKey() );
        }
        catch ( ... )
        {
            // If any exceptions occur, clean-up and then pass them on
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }
            throw;
        }
        
        delete data;
        data = NULL;
    }

    std::string AlarmRuleData::getEntityName()
    {
        return m_alarmRuleHelper->getEntityName();
    }


    unsigned long AlarmRuleData::getAlarmType()
    {
        return m_alarmRuleHelper->getAlarmType();
    }


    void AlarmRuleData::setAlarmType(unsigned long alarmTypeKey)
    {
        unsigned long oldAlarmTypeKey = m_alarmRuleHelper->getAlarmType();

        m_alarmRuleHelper->setAlarmType(alarmTypeKey);

        std::string oldAlarmTypeName = getAlarmTypeName(oldAlarmTypeKey);
        std::string alarmTypeName    = getAlarmTypeName(alarmTypeKey);

        updateAlarmRuleChanges(
            ALARM_TYPE_PARAM, oldAlarmTypeName, alarmTypeName);

    }

    std::string AlarmRuleData::getAlarmDescription()
    {
        return m_alarmRuleHelper->getAlarmDescription();
    }


    void AlarmRuleData::setAlarmDescription(const std::string& description)
    {
        std::string oldAlarmDescription = m_alarmRuleHelper->getAlarmDescription();

        m_alarmRuleHelper->setAlarmDescription(description);

        std::string alarmDescription    = description;

        updateAlarmRuleChanges(
            ALARM_DESCRIPTION_PARAM, oldAlarmDescription, alarmDescription);

    }

    std::string AlarmRuleData::getAlarmTypeName()
    {
        return m_alarmRuleHelper->getAlarmTypeName();
    }

	std::string AlarmRuleData::getAlarmTypeDisplayName()
	{
		return m_alarmRuleHelper->getAlarmTypeDisplayName();
	}

    void AlarmRuleData::setAlarmTypeByName(const std::string& alarmTypeName)
    {
        if(alarmTypeName.empty())
        {
            unsigned long	oldAlarmTypeKey = m_alarmRuleHelper->getAlarmType();
            std::string oldAlarmTypeName = getAlarmTypeName(oldAlarmTypeKey);

            m_alarmRuleHelper->setAlarmType(0);

            updateAlarmRuleChanges(ALARM_TYPE_PARAM, oldAlarmTypeName, alarmTypeName);
            return;
        }

        IAlarmTypeData* alarmType =
            AlarmTypeAccessFactory::getInstance().getAlarmType(
                alarmTypeName, false);
        TA_ASSERT(alarmType != NULL, "Alarm Type must not be NULL");

		unsigned long typeKey;

        try
        {
            typeKey = alarmType->getKey();
        }
        catch ( ... )
        {
            // If any exceptions occur, clean-up and then pass them on
            if (alarmType != NULL)
            {
                delete alarmType;
                alarmType = NULL;
            }
            throw;
        }
        
        delete alarmType;
        alarmType = NULL;

        setAlarmType(typeKey);
    }

	void AlarmRuleData::setAlarmTypeByDisplayName(const std::string& alarmTypeDispName)
	{
		if(alarmTypeDispName.empty())
		{
			unsigned long	oldAlarmTypeKey = m_alarmRuleHelper->getAlarmType();
			std::string oldAlarmTypeDispName = getAlarmTypeDisplayName(oldAlarmTypeKey);

			m_alarmRuleHelper->setAlarmType(0);

			updateAlarmRuleChanges(ALARM_TYPE_PARAM, oldAlarmTypeDispName, alarmTypeDispName);
			return;
		}

		IAlarmTypeData* alarmType =
			AlarmTypeAccessFactory::getInstance().getAlarmTypeByDisplayName(
			alarmTypeDispName, false);

        TA_ASSERT(alarmType != NULL, "Alarm Type must not be NULL");

		unsigned long typeKey;

		try
		{
			typeKey = alarmType->getKey();
		}
		catch ( ... )
		{
			// If any exceptions occur, clean-up and then pass them on
			if (alarmType != NULL)
			{
				delete alarmType;
				alarmType = NULL;
			}
			throw;
		}

		delete alarmType;
		alarmType = NULL;

		setAlarmType(typeKey);
	}


    std::string AlarmRuleData::getRuleType()
    {
        return m_alarmRuleHelper->getRuleType();
    }


    std::string AlarmRuleData::getRuleTrigger()
    {
        return m_alarmRuleHelper->getRuleTrigger();
    }

    time_t AlarmRuleData::getDateCreated()
    {
        return m_alarmRuleHelper->getDateCreated();
    }

    time_t AlarmRuleData::getDateModified()
    {
        return m_alarmRuleHelper->getDateModified();
    }


    void AlarmRuleData::setRuleTrigger(const std::string& ruleTrigger)
    {
        std::string oldRuleTrigger = m_alarmRuleHelper->getRuleTrigger();

        m_alarmRuleHelper->setRuleTrigger(ruleTrigger);

        updateAlarmRuleChanges(
            RULE_TRIGGER_PARAM, oldRuleTrigger, ruleTrigger);
    }


	std::string AlarmRuleData::getCreatedBySessionKey()
    {
        return m_alarmRuleHelper->getCreatedBySessionKey();
    }


	unsigned long AlarmRuleData::getCreatedByProfileKey()
	{
		return m_alarmRuleHelper->getCreatedByProfileKey();
	}

    void AlarmRuleData::updateAlarmRuleChanges(
        const std::string& name,
        unsigned long oldValue,
        unsigned long newValue)
    {
        std::string oldValueStr = getUnsignedLongAsString(oldValue);
        std::string newValueStr = getUnsignedLongAsString(newValue);

        updateAlarmRuleChanges(name, oldValueStr, newValueStr);
    }


    void AlarmRuleData::updateAlarmRuleChanges(
        const std::string& name,
        const std::string& oldValue,
        const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAlarmRuleChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_alarmRuleChanges.find(name);

        if (matching != m_alarmRuleChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_alarmRuleChanges.erase(matching);
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
            m_alarmRuleChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }



    std::string AlarmRuleData::getUnsignedLongAsString(
        unsigned long number)
    {
        std::stringstream numberStream;
        numberStream << number;
        return numberStream.str();
    }


    unsigned long AlarmRuleData::getStringAsUnsignedLong(
        const std::string& theString)
    {
        std::stringstream stringStream;
        stringStream << theString;

        unsigned long number = 0;
        if(stringStream >> number)
        {
            return number;
        }

        std::string message = "Could not convert string to number: "
                            + theString;
        TA_THROW(DataException(message.c_str(), DataException::WRONG_TYPE, ""));
    }


    std::string AlarmRuleData::getEntityName(unsigned long entityKey)
    {
        // Get the entity name, if it exists
        std::string entityName = "";
        if(entityKey != 0)
        {
            IEntityData* entityData =
                EntityAccessFactory::getInstance().getEntity(entityKey);

            try
            {
                entityName = entityData->getName();
            }
            catch(...)
            {
                delete entityData;
                entityData = NULL;
                throw;
            }
            delete entityData;
            entityData = NULL;
        }

        return entityName;
    }    


    std::string AlarmRuleData::getEntityTypeName(unsigned long entityTypeKey)
    {
        // Get the entity type name, if it exists
        return m_alarmRuleHelper->getEntityTypeName(entityTypeKey);
    }    


    std::string AlarmRuleData::getAlarmTypeName(unsigned long alarmTypeKey)
    {
        // Get the alarm type name, if it exists
        std::string alarmTypeName = "";
        if(alarmTypeKey != 0)
        {
            IAlarmTypeData* alarmTypeData =
                AlarmTypeAccessFactory::getInstance().getAlarmTypeByKey(
                    alarmTypeKey, false);

            try
            {
                alarmTypeName = alarmTypeData->getName();
            }
            catch(...)
            {
                delete alarmTypeData;
                alarmTypeData = NULL;
                throw;
            }
            delete alarmTypeData;
            alarmTypeData = NULL;
        }

        return alarmTypeName;
    }

	std::string AlarmRuleData::getAlarmTypeDisplayName(unsigned long alarmTypeKey)
	{
		// Get the alarm type name, if it exists
		std::string alarmTypeDisplayName = "";
		if(alarmTypeKey != 0)
		{
			IAlarmTypeData* alarmTypeData =
				AlarmTypeAccessFactory::getInstance().getAlarmTypeByKey(
				alarmTypeKey, false);

			try
			{
				alarmTypeDisplayName = alarmTypeData->getDisplayName();
			}
			catch(...)
			{
				delete alarmTypeData;
				alarmTypeData = NULL;
				throw;
			}
			delete alarmTypeData;
			alarmTypeData = NULL;
		}

		return alarmTypeDisplayName;
	}

    unsigned long AlarmRuleData::getEntityKeyFromName(const std::string& name)
    {
        // Get the entity key, if it exists
        unsigned long key = 0;
        if(name != "")
        {
            IEntityData* entityData =
                EntityAccessFactory::getInstance().getEntity(name);

            try
            {
                key = entityData->getKey();
            }
            catch(...)
            {
                delete entityData;
                entityData = NULL;
                throw;
            }
            delete entityData;
            entityData = NULL;
        }

        return key;
    }    

    unsigned long AlarmRuleData::getAlarmTypeKeyFromName(const std::string& alarmTypeName)
    {
        // Get the alarm type key, if it exists
        unsigned long alarmTypeKey = 0;
        if(alarmTypeName != "")
        {
            IAlarmTypeData* alarmTypeData =
                AlarmTypeAccessFactory::getInstance().getAlarmType(
                    alarmTypeName, false);

            try
            {
                alarmTypeKey = alarmTypeData->getKey();
            }
            catch(...)
            {
                delete alarmTypeData;
                alarmTypeData = NULL;
                throw;
            }
            delete alarmTypeData;
            alarmTypeData = NULL;
        }

        return alarmTypeKey;
    }

	unsigned long AlarmRuleData::getAlarmTypeKeyFromDisplayName(const std::string& alarmTypeDisplayName)
	{
		// Get the alarm type key, if it exists
		unsigned long alarmTypeKey = 0;
		if(alarmTypeDisplayName != "")
		{
			IAlarmTypeData* alarmTypeData =
				AlarmTypeAccessFactory::getInstance().getAlarmTypeByDisplayName(
				alarmTypeDisplayName, false);

			try
			{
				alarmTypeKey = alarmTypeData->getKey();
			}
			catch(...)
			{
				delete alarmTypeData;
				alarmTypeData = NULL;
				throw;
			}
			delete alarmTypeData;
			alarmTypeData = NULL;
		}

		return alarmTypeKey;
	}


} // closes TA_Base_Core

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/ConfigAlarmPlanAssoc.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/28 10:18:09 $
 * Last modified by:  $Author: weikun.lin $
 * 
 * ConfigAlarmPlanAssoc is an implementation of IConfigAlarmPlanAssoc. It holds the data specific to 
 * alarm plan associations and allows reads and writes.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/plans/src/ConfigAlarmPlanAssoc.h"
#include "core/data_access_interface/plans/src/AlarmPlanAssocHelper.h"
#include "core/data_access_interface/plans/src/PlanAccessFactory.h"
#include "core/data_access_interface/plans/src/IPlan.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    const std::string ConfigAlarmPlanAssoc::ENTITY = "Entity";
    const std::string ConfigAlarmPlanAssoc::ENTITYTYPE = "Entity Type";
    const std::string ConfigAlarmPlanAssoc::ALARMTYPE = "Alarm Type";
    const std::string ConfigAlarmPlanAssoc::PLAN = "Plan";

    long ConfigAlarmPlanAssoc::s_nextAvailableIdentifier =0;


    ConfigAlarmPlanAssoc::ConfigAlarmPlanAssoc(unsigned long key, 
			                                   unsigned long entityKey,
			                                   unsigned long entityTypeKey,
			                                   unsigned long alarmType,
			                                   const std::string& planPath,
                                               time_t dateCreated,
                                               time_t dateModified)
			: m_helper( new AlarmPlanAssocHelper(key,entityKey,entityTypeKey,alarmType,planPath,dateCreated,dateModified) ),
              m_uniqueIdentifier(++s_nextAvailableIdentifier),
              m_isValidData(true)
    {
        m_isUsingEntity = (entityKey != 0);
    }


    ConfigAlarmPlanAssoc::ConfigAlarmPlanAssoc( )
        : m_helper( new AlarmPlanAssocHelper() ),
          m_uniqueIdentifier(++s_nextAvailableIdentifier),
          m_isUsingEntity(false),
          m_isValidData(true)
    {
    }

    
    ConfigAlarmPlanAssoc::ConfigAlarmPlanAssoc( const ConfigAlarmPlanAssoc& rhs)
        : m_helper( new AlarmPlanAssocHelper( *(rhs.m_helper) ) ),
          m_uniqueIdentifier(++s_nextAvailableIdentifier),
          m_isUsingEntity(rhs.m_isUsingEntity),
          m_isValidData(true)
    {
    }


	ConfigAlarmPlanAssoc::~ConfigAlarmPlanAssoc( )
    {
        try
        {
		    if ( NULL != m_helper )
		    {
			    delete m_helper;
			    m_helper = NULL;
		    }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught in destructor");
        }
    }


	unsigned long ConfigAlarmPlanAssoc::getKey()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getKey();
    }
	
	
	std::string ConfigAlarmPlanAssoc::getName()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getName();
    }

    
    unsigned long ConfigAlarmPlanAssoc::getAlarmType()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getAlarmType();
    }


	unsigned long ConfigAlarmPlanAssoc::getEntityKey()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getEntityKey();
    }


	unsigned long ConfigAlarmPlanAssoc::getEntityTypeKey()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getEntityTypeKey();
    }


	std::string ConfigAlarmPlanAssoc::getPlanPath()
	{
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getPlanPath();
	}


    time_t ConfigAlarmPlanAssoc::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getDateCreated();
    }


    time_t ConfigAlarmPlanAssoc::getDateModified()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getDateModified();
    }

    bool ConfigAlarmPlanAssoc::isUsingEntity()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        if (!m_isValidData)
        {
            m_isUsingEntity = (m_helper->getEntityKey() != 0);
            m_isValidData = true;
        }

        return m_isUsingEntity;
    }
        
    void ConfigAlarmPlanAssoc::setName(const std::string& name)
    {
        // Do nothing in here as we have nothing to set. The name is derived from other fields
    }
		
        
    void ConfigAlarmPlanAssoc::setAlarmTypeKey(unsigned long alarmType)
    {
        FUNCTION_ENTRY("setAlarmTypeKey");

        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
                
        updateChanges(ALARMTYPE, m_helper->getAlarmType(), alarmType);

        m_helper->setAlarmType(alarmType);

        FUNCTION_EXIT;
    }


	void ConfigAlarmPlanAssoc::setEntityKey(unsigned long entityKey)
    {
        FUNCTION_ENTRY("setEntityKey");

        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        
        updateChanges(ENTITY, m_helper->getEntityKey(), entityKey);

        m_helper->setEntityKey(entityKey);

        if (entityKey == 0)
        {
            m_isUsingEntity = false;
            m_isValidData = true;
        }

        FUNCTION_EXIT;
    }


	void ConfigAlarmPlanAssoc::setEntityTypeKey(unsigned long entityTypeKey)
    {
        FUNCTION_ENTRY("setEntityTypeKey");

        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        
        updateChanges(ENTITYTYPE, m_helper->getEntityTypeKey(), entityTypeKey);

        m_helper->setEntityTypeKey(entityTypeKey);

        if (entityTypeKey == 0)
        {
            m_isUsingEntity = true;
            m_isValidData = true;
        }

        FUNCTION_EXIT;
    }


	void ConfigAlarmPlanAssoc::setPlanPath(const std::string& planPath)
    {
        FUNCTION_ENTRY("setPlanPath");

        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        
        std::string oldPlanPath(m_helper->getPlanPath());

        // This will validate the path.
        m_helper->setPlanPath(planPath);

        updateChanges(PLAN, oldPlanPath, planPath);

        FUNCTION_EXIT;
    }

    
    void ConfigAlarmPlanAssoc::setAlarmType(const std::string& alarmTypeName)
    {
        FUNCTION_ENTRY("setAlarmType");

        if (alarmTypeName.empty())
        {
            setAlarmTypeKey(0);
        }

        IAlarmTypeData* data = NULL;
        try
        {
            data = AlarmTypeAccessFactory::getInstance().getAlarmType(alarmTypeName,false);

            setAlarmTypeKey( data->getKey() );
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

        FUNCTION_EXIT;
    }

	void ConfigAlarmPlanAssoc::setAlarmTypeByDisplayName(const std::string& alarmTypeDisplayName)
	{
		FUNCTION_ENTRY("setAlarmType");

		if (alarmTypeDisplayName.empty())
		{
			setAlarmTypeKey(0);
		}

		IAlarmTypeData* data = NULL;
		try
		{
			data = AlarmTypeAccessFactory::getInstance().getAlarmTypeByDisplayName(alarmTypeDisplayName,false);

			setAlarmTypeKey( data->getKey() );
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

		FUNCTION_EXIT;
	}

    void ConfigAlarmPlanAssoc::setEntity(const std::string& entityName)
    {
        FUNCTION_ENTRY("setEntity");

        if (entityName.empty())
        {
            setEntityKey( 0 );
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

        FUNCTION_EXIT;
    }


    void ConfigAlarmPlanAssoc::setEntityType(const std::string& entityTypeName)
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
            // get a connection to the database
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

            /*std::ostringstream sql; 
            sql << "select pkey from entitytype where name = '" 
                << databaseConnection->escapeQueryString(entityTypeName).c_str() << "'";*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4021,databaseConnection->escapeQueryString(entityTypeName));
//			std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4021,databaseConnection->escapeQueryString(entityTypeName));
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYTYPE_SELECT_4021,databaseConnection->escapeQueryString(entityTypeName));

            std::vector<std::string> columnNames;
            columnNames.push_back("key");

            data = databaseConnection->executeQuery(strSql, columnNames);

            if (data->getNumRows() != 1)
            {
                TA_THROW( DataException("No entitytype with the specified name", DataException::NO_VALUE, "") );
            }

            setEntityTypeKey( data->getUnsignedLongData(0,"key") );
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


    std::string ConfigAlarmPlanAssoc::getAlarmTypeName()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->retrieveAlarmTypeName( m_helper->getAlarmType() );
    }

	std::string ConfigAlarmPlanAssoc::getAlarmTypeDisplayName()
	{
		TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
		return m_helper->retrieveAlarmTypeDispalyName( m_helper->getAlarmType() );
	}
        
    std::string ConfigAlarmPlanAssoc::getEntityName()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->retrieveEntityName( m_helper->getEntityKey() );
    }

        
    std::string ConfigAlarmPlanAssoc::getEntityTypeName()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->retrieveEntityTypeName( m_helper->getEntityTypeKey() );
    }
   
        
    ItemChanges ConfigAlarmPlanAssoc::getAllItemChanges()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");

        ItemChanges changes;

        // Convert each of the keys into names
        for (ItemChanges::const_iterator iter = m_alarmPlanChanges.begin(); iter != m_alarmPlanChanges.end(); ++iter)
        {
            Values value;
            if( 0 == iter->first.compare(ENTITY) )
            {
                value.newValue = m_helper->retrieveEntityName( ::atol(iter->second.newValue.c_str()) );
                value.oldValue = m_helper->retrieveEntityName( ::atol(iter->second.oldValue.c_str()) );
            }
            else if( 0 == iter->first.compare(ENTITYTYPE) )
            {
                value.newValue = m_helper->retrieveEntityTypeName( ::atol(iter->second.newValue.c_str()) );
                value.oldValue = m_helper->retrieveEntityTypeName( ::atol(iter->second.oldValue.c_str()) );
            }
            else if( 0 == iter->first.compare(ALARMTYPE) )
            {
                value.newValue = m_helper->retrieveAlarmTypeName( ::atol(iter->second.newValue.c_str()) );
                value.oldValue = m_helper->retrieveAlarmTypeName( ::atol(iter->second.oldValue.c_str()) );
            }
            else if( 0 == iter->first.compare(PLAN) )
            {
                value = iter->second; // contains plan path
            }

            changes.insert( ItemChanges::value_type(iter->first,value) );
        }

        return changes;
    }


    bool ConfigAlarmPlanAssoc::isNew()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        
        return m_helper->isNew();
    }

        
    void ConfigAlarmPlanAssoc::invalidate()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        m_helper->invalidate();

        m_isValidData = false;

        m_alarmPlanChanges.clear();
    }


    void ConfigAlarmPlanAssoc::applyChanges()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        
        m_helper->applyChanges();
        
        m_alarmPlanChanges.clear();
    }


    void ConfigAlarmPlanAssoc::deleteThisAssociation()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        
        m_helper->deleteThisAssociation();
        
        m_alarmPlanChanges.clear();
    }


    template <typename ItemType>
    void ConfigAlarmPlanAssoc::updateChanges(const std::string& name, const ItemType& oldValue, const ItemType& newValue)
    {
        FUNCTION_ENTRY("updateChanges");

        // Convert old and new values to string format.
        std::ostringstream oldValueStrm, newValueStrm;
        oldValueStrm << oldValue;
        newValueStrm << newValue;

        Values values;
        values.oldValue = oldValueStrm.str();
        values.newValue = newValueStrm.str();

        // Search for this name in the map
        ItemChanges::iterator matching = m_alarmPlanChanges.find(name);

        if (matching != m_alarmPlanChanges.end())
        {
            // We already have this attribute name in the list.
            if (values.newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_alarmPlanChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = values.newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (values.oldValue != values.newValue)
        {
            // If the old and new values are different then we can add this item to the map
            m_alarmPlanChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


} // closes TA_Base_Core



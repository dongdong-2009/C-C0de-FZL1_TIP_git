/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AlarmRuleHelper.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/20 22:28:15 $
  * Last modified by:  $Author: huirong.luo $
  *
  * This class provides the actual database code for the IAlarmRuleData
  * interface. It provides access to the data stored in a single entry
  * in the AlarmRule table.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>

#include "core/data_access_interface/alarm_rule/src/AlarmRuleHelper.h"
#include "core/data_access_interface/alarm_rule/src/IAlarmRuleActionParameterData.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleActionParameterHelper.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"

#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/ISession.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;
typedef TA_Base_Core::IAlarmRuleActionParameterData::ActionParameterVector ActionParameterVector;

namespace TA_Base_Core
{

    AlarmRuleHelper::AlarmRuleHelper(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleType,
        const std::string& ruleTrigger)
        :
        m_pkey(0),
        m_entityTypeKey(entityTypeKey),
        m_entityKey(entityKey),
        m_alarmTypeKey(alarmTypeKey),
        m_ruleType(ruleType),
        m_ruleTrigger(ruleTrigger),
		m_alarmDescription(""),
        m_dateCreated(0),
        m_dateModified(0), 
        m_isNew(true),
        m_isValidData(false),
        m_createdBySessionKey(""),
		m_createdByProfileKey(ULONG_MAX),
		m_createdByOperatorName("")  
    {
    }


    AlarmRuleHelper::AlarmRuleHelper(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleType,
        const std::string& ruleTrigger,
		const std::string& alarmDescription,
		const std::string& createdBySessionKey  /*=""*/)
        :
        m_pkey(0),
        m_entityTypeKey(entityTypeKey),
        m_entityKey(entityKey),
        m_alarmTypeKey(alarmTypeKey),
        m_ruleType(ruleType),
        m_ruleTrigger(ruleTrigger),
		m_alarmDescription(alarmDescription),
        m_dateCreated(0),
        m_dateModified(0), 
        m_isNew(true),
        m_isValidData(false),
        m_createdBySessionKey(createdBySessionKey),
		m_createdByProfileKey(ULONG_MAX),
		m_createdByOperatorName("")
    {
    }

    
    AlarmRuleHelper::AlarmRuleHelper(unsigned long pkey)
        :
        m_pkey(pkey),
        m_entityTypeKey(0),
        m_entityKey(0),
        m_alarmTypeKey(0),
        m_ruleType(""),
        m_ruleTrigger(""),
		m_alarmDescription(""),
        m_dateCreated(0),
        m_dateModified(0), 
        m_isNew(false),
        m_isValidData(false),
        m_createdBySessionKey(""),
		m_createdByProfileKey(ULONG_MAX),
		m_createdByOperatorName("")
    {
    }


    AlarmRuleHelper::AlarmRuleHelper(
        const std::string& ruleType, const std::string ruleTrigger)
        :
        m_pkey(0),
        m_entityTypeKey(0),
        m_entityKey(0),
        m_alarmTypeKey(0),
        m_ruleType(ruleType),
        m_ruleTrigger(ruleTrigger),
		m_alarmDescription(""),
        m_dateCreated(0),
        m_dateModified(0), 
        m_isNew(true),
        m_isValidData(false),
        m_createdBySessionKey(""),
		m_createdByProfileKey(ULONG_MAX),
		m_createdByOperatorName("")
    {
    }


    AlarmRuleHelper::AlarmRuleHelper(
        AlarmRuleHelper& original)
        :
        m_pkey(0),
        m_entityTypeKey(original.getEntityTypeKey()),
        m_entityKey(original.getEntityKey()),
        m_alarmTypeKey(original.getAlarmType()),
        m_ruleType(original.getRuleType()),
        m_ruleTrigger(original.getRuleTrigger()),
		m_alarmDescription(original.getAlarmDescription()),
        m_dateCreated(0),
        m_dateModified(0), 
//         m_parameters(original.m_parameters),
//         m_deletedParameters(original.m_deletedParameters),
        m_isNew(true),
        m_isValidData(false),
        m_createdBySessionKey(original.getCreatedBySessionKey()),
		m_createdByProfileKey(original.getCreatedByProfileKey()),
		m_createdByOperatorName(original.getCreatedByOperatorName())
    {
		// Perform deep copy on actionParamaters m_parameters
		ActionParameterVector::iterator itr = m_parameters.begin();
		for (itr; itr != m_parameters.end(); ++itr)
		{
			IAlarmRuleActionParameterData* parameter =
				new AlarmRuleActionParameterHelper(*((AlarmRuleActionParameterHelper*)(*itr)));

			m_parameters.push_back(parameter);
		}

		// Perform deep copy on actionParamaters deletedParameters
		ActionParameterVector::iterator itrdel = m_deletedParameters.begin();
		for (itrdel; itrdel != m_deletedParameters.end(); ++itrdel)
		{
			IAlarmRuleActionParameterData* parameter =
				new AlarmRuleActionParameterHelper(*((AlarmRuleActionParameterHelper*)(*itr)));

			m_deletedParameters.push_back(parameter);
		}
    }


    AlarmRuleHelper::~AlarmRuleHelper()
    {
        cleanUpDeletedParameters();
        cleanUpParameters();
    }


    void AlarmRuleHelper::cleanUpDeletedParameters()
    {
        for(ActionParameterVector::iterator it = m_deletedParameters.begin();
            it != m_deletedParameters.end();
            it ++)
        {
            // This will automatically delete its related parameters.
            delete *it;
            (*it) = NULL;
        }
        m_deletedParameters.clear();
    }

    void AlarmRuleHelper::cleanUpParameters()
    {
        for(ActionParameterVector::iterator it = m_parameters.begin();
            it != m_parameters.end();
            it ++)
        {
            // This will automatically delete its related parameters.
            delete *it;
            (*it) = NULL;
        }

        m_parameters.clear();
    }


    unsigned long AlarmRuleHelper::getKey()
    {
        TA_ASSERT(!m_isNew,
                  "A key has not yet been assigned. "
                  "The data must be written to the database first.");
        
        return m_pkey;
    }


    std::string AlarmRuleHelper::getName()
    {
        TA_ASSERT(true, "This method does not apply to Alarm Rules.");
        return "";
    }
    

    void AlarmRuleHelper::setName(const std::string& name)
    {
        TA_ASSERT(true, "This method does not apply to Alarm Rules.");
    }


    unsigned long AlarmRuleHelper::getEntityTypeKey()
    {
        reload();
        return m_entityTypeKey;
    }

    void AlarmRuleHelper::setEntityTypeKey(unsigned long entityTypeKey)
    {
        reload();
        m_entityTypeKey = entityTypeKey;
    }


    std::string AlarmRuleHelper::getEntityTypeName()
    {
        reload();

        return getEntityTypeName(m_entityTypeKey);
    }


    std::string AlarmRuleHelper::getEntityTypeName(unsigned long entityTypeKey)
    {
        // Get the entity type name, if it exists
        std::string name = "";
        if(entityTypeKey != 0)
        {
            /*std::ostringstream sql;
            sql << "SELECT name FROM EntityType WHERE pkey = " << entityTypeKey;*/

            std::string nameColumn = "Name";
            std::vector<std::string> columnNames;
            columnNames.push_back(nameColumn);

			// TD19819 Yanrong++
            //IData* data = DatabaseFactory::getInstance().getDatabase(Alarms_Cd,Write)->executeQuery(
            //                  sql.str(), columnNames);
			IDatabase* pDatabase = DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd,Read);
			// ++Yanrong
//			std::string sql = defPrepareSQLStatement(pDatabase->GetDbServerType(), 				Entitytype_STD_SELECT_4011, entityTypeKey);
//			std::string sql  = pDatabase->prepareSQLStatement(ENTITYTYPE_SELECT_4011, entityTypeKey);
		SQLStatement sql;
		pDatabase->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4011, entityTypeKey);
//			TA_ASSERT(sql.size() > 0, "AlarmRuleHelper::getEntityTypeName: Get SQL Statement error");

			IData* data = pDatabase->executeQuery(sql, columnNames);

            if (data->getNumRows() != 1)
            {
                delete data;
                data = NULL;

                const char* message = "Query did not return exactly 1 row";
                TA_THROW(
                    DataException(
                        message, DataException::MISSING_MANDATORY, ""));
            }
            else
            {
                try
                {
                    name = data->getStringData( 0, nameColumn );
                }
                catch(...)
                {
                    delete data;
                    data = NULL;
                    throw;
                }
                delete data;
                data = NULL;
            }
        }

        return name;
    }

    
    unsigned long AlarmRuleHelper::getEntityKey()
    {
        reload();
        return m_entityKey;
    }

    void AlarmRuleHelper::setEntityKey(unsigned long entityKey)
    {
        reload();
        m_entityKey = entityKey;
    }

	std::string AlarmRuleHelper::getAlarmDescription()
	{
		reload();
		return m_alarmDescription;
	}

	void AlarmRuleHelper::setAlarmDescription(const std::string& description)
	{
		reload();
		m_alarmDescription = description;
	}

    std::string AlarmRuleHelper::getEntityName()
    {
        reload();

        // Get the entity name, if it exists
        std::string name = "";
        if(m_entityKey != 0)
        {
            IEntityData* entityData =
                EntityAccessFactory::getInstance().getEntity(m_entityKey);

            try
            {
                name = entityData->getName();
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
        
        return name;
    }

    
    unsigned long AlarmRuleHelper::getAlarmType()
    {
        reload();
        return m_alarmTypeKey;
    }

    void AlarmRuleHelper::setAlarmType(const unsigned long alarmTypeKey)
    {
        reload();
        m_alarmTypeKey = alarmTypeKey;
    }

    std::string AlarmRuleHelper::getAlarmTypeName()
    {
        reload();

        // Get the alarm type name, if it exists
        std::string name = "";
        if(m_alarmTypeKey != 0)
        {
            IAlarmTypeData* alarmTypeData =
                AlarmTypeAccessFactory::getInstance().getAlarmTypeByKey(
                    m_alarmTypeKey, false);

            try
            {
                name = alarmTypeData->getName();
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

        return name;
    }

	std::string AlarmRuleHelper::getAlarmTypeDisplayName()
	{
		reload();

		// Get the alarm type name, if it exists
		std::string name = "";
		if(m_alarmTypeKey != 0)
		{
			IAlarmTypeData* alarmTypeData =
				AlarmTypeAccessFactory::getInstance().getAlarmTypeByKey(
				m_alarmTypeKey, false);

			try
			{
				name = alarmTypeData->getDisplayName();
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

		return name;
	}
    
    std::string AlarmRuleHelper::getRuleType()
    {
        reload();
        return m_ruleType;
    }

    void AlarmRuleHelper::setRuleType(const std::string& ruleType)
    {
        reload();
        m_ruleType = ruleType;
    }

    std::string AlarmRuleHelper::getRuleTrigger()
    {
        reload();
        return m_ruleTrigger;
    }

    void AlarmRuleHelper::setRuleTrigger(const std::string& ruleTrigger)
    {
        reload();
        m_ruleTrigger = ruleTrigger;
    }

	std::string AlarmRuleHelper::getCreatedBySessionKey()
    {
        reload();
        return m_createdBySessionKey;
    }

	unsigned long AlarmRuleHelper::getCreatedByProfileKey()
	{
		reload();
		
		if (m_createdBySessionKey.empty() == false && m_createdByProfileKey==ULONG_MAX )
		{		
			try
			{
				TA_Base_Core::ISession* session = TA_Base_Core::SessionAccessFactory::getInstance().getSession(m_createdBySessionKey);
				if (session != NULL)
				{
					unsigned long  key= session->getProfileKey();
					delete session;
					return key;
				}
				else
				{
					return ULONG_MAX;
				}
			}
			catch(...)
			{
				return ULONG_MAX;
			}		 
		}
		return m_createdByProfileKey;  //resolve inactive session problem
		//return ULONG_MAX;
	}


	void AlarmRuleHelper::setCreatedByProfileKey(unsigned long profileKey)
	{
			m_createdByProfileKey=profileKey;
	}

	std::string  AlarmRuleHelper::getCreatedByOperatorName()
	{
		reload();    
		return m_createdByOperatorName;   //resolve inactive session problem
	}

	void AlarmRuleHelper::setCreatedByOperatorName(std::string operatorName)
	{
		m_createdByOperatorName=operatorName.c_str();
	}

    time_t AlarmRuleHelper::getDateCreated()
    {
        reload();
        return m_dateCreated;
    }


    time_t AlarmRuleHelper::getDateModified()
    {
        reload();
        return m_dateModified;
    }


    bool AlarmRuleHelper::isNew()
    {
        return m_isNew;
    }


    ActionParameterVector AlarmRuleHelper::getParameters()
    {
        reload();
        return m_parameters;
    }

    IAlarmRuleActionParameterData* AlarmRuleHelper::addNewParameter(
        const std::string& type, const std::string& value /* = "" */)
    {
        TA_ASSERT(type != "", "Type must not be blank");

        reload();

        IAlarmRuleActionParameterData* parameter =
            new AlarmRuleActionParameterHelper(*this, type, value);

        m_parameters.push_back(parameter);

        return parameter;
    }


    IAlarmRuleActionParameterData* AlarmRuleHelper::addCopiedParameter(
        const AlarmRuleActionParameterHelper& parameterToCopy)
    {
        reload();

        IAlarmRuleActionParameterData* parameter =
            new AlarmRuleActionParameterHelper(parameterToCopy);

        m_parameters.push_back(parameter);

        return parameter;
    }


    void AlarmRuleHelper::deleteParameter(IAlarmRuleActionParameterData* parameter)
    {
        TA_ASSERT(parameter != NULL, "Parameter must not be NULL");

        reload();

        // Make sure the parameter is in the list
        ActionParameterVector::iterator it =
            std::find(m_parameters.begin(), m_parameters.end(), parameter);

        
        // If the parameter was not found, throw an exception
        if(it == m_parameters.end())
        {
            std::ostringstream errorMessage;
            errorMessage <<
                "Attempt to delete non-existent parameter with type '" <<
                parameter->getType() << "' "
                "and value '" << parameter->getValue() << "'";
            const char* message = errorMessage.str().c_str();

            throw DataException(message, DataException::NO_VALUE, "");
        }


        // Move the parameter from the parameters vector to the
        // m_deletedParameters vector. It will actually be deleted
        // from the database the next time applyChanges is called.
        m_deletedParameters.push_back(*it);
        
        m_parameters.erase(
            std::remove(m_parameters.begin(), m_parameters.end(), parameter));
    }


    void AlarmRuleHelper::deleteParameter(unsigned long pkey)
    {
        reload();

        // Find the parameter in the list
        for(ActionParameterVector::iterator it = m_parameters.begin();
            it != m_parameters.end();
            it ++)
        {
            if( (*it)->getKey() == pkey )
            {
                // Move the parameter from the parameters vector to the
                // m_deletedParameters vector. It will actually be deleted
                // from the database the next time applyChanges is called.
                m_deletedParameters.push_back(*it);
        
                m_parameters.erase(
                    std::remove(
                        m_parameters.begin(), m_parameters.end(), *it) );
                
                return;
            }
        }

        // If the parameter was not found, throw an exception
        std::ostringstream errorMessage;
        errorMessage << "No parameter exists with pkey " << pkey;
        const char* message = errorMessage.str().c_str();

        throw DataException(message, DataException::NO_VALUE, "");
    }




    void AlarmRuleHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");
     
        // If there are any fields that are blank, add their names to a vector
        // of failed field names.
        std::vector<std::string> fieldNames;
        if (m_ruleType == "")
        {
            fieldNames.push_back("Rule Type");
        }
        if (m_ruleTrigger == "")
        {
            fieldNames.push_back("Rule Trigger");
        }

        // If we have at least one entry in the vector
        // there must be a blank field, so throw an exception.
        if (fieldNames.size() > 0)
        {
            throw TA_Base_Core::DataConfigurationException(
                "Alarm rule data not fully specified. "
                "Alarm rule cannot be written to database",
                fieldNames);
        }


        // New alarm rules get inserted
        if(m_isNew)
        {
            createNewAlarmRule();
        }
        // Existing alarm rules get updated
        else
        {
            updateAlarmRule();
        }


        // Delete from the database parameters that have been deleted
        // from this object.
        for(ActionParameterVector::iterator deleteIt = m_deletedParameters.begin();
            deleteIt != m_deletedParameters.end();
            deleteIt ++)
        {
            if( !(*deleteIt)->isNew() )
            {
                (*deleteIt)->deleteThisObject();
            }
            delete *deleteIt;
            *deleteIt = NULL;
        }
        m_deletedParameters.clear();


        // Apply changes to parameters
        // NOTE: The Action Parameters must be applied after this object,
        //       as the AlarmRuleActionParameterHelper needs to know the pkey
        //       of this object before it can write itself.
        //       If this object has not yet been written to the database,
        //       it will not have a pkey.
        for(ActionParameterVector::iterator it = m_parameters.begin();
            it != m_parameters.end();
            it ++)
        {
            (*it)->applyChanges();
        }



        FUNCTION_EXIT;
    }


    void AlarmRuleHelper::deleteThisObject()
    {
        FUNCTION_ENTRY("deleteThisObject");
        
        TA_ASSERT(!m_isNew, "New Alarm Rules cannot be deleted from the database.");

         // Get the database connection
		//TD18044 - change database connection type to retrieve 
		// directly from central DB for the next value
        //IDatabase* databaseConnection =
        //        TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Write);
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Write);
		//++TD18044

        // Delete the action parameters associated with this alarm rule
        for(ActionParameterVector::iterator it = m_parameters.begin();
            it != m_parameters.end();
            it ++)
        {
            if( !(*it)->isNew() )
            {
                (*it)->deleteThisObject();
            }
            delete *it;
            (*it) = NULL;
        }
        
        m_parameters.clear();


        // Delete the alarm rule itself
        /*std::ostringstream sql;
        sql << "DELETE FROM AlarmRule WHERE pkey = " << m_pkey;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AlarmRule_STD_DELETE_1401, m_pkey);
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARMRULE_DELETE_1401, m_pkey);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARMRULE_DELETE_1401, m_pkey);
//		TA_ASSERT(sql.size() > 0, "AlarmRuleHelper::deleteThisObject: Get SQL Statement error");
        databaseConnection->executeModification(sql);

        FUNCTION_EXIT;
    }


    void AlarmRuleHelper::createNewAlarmRule()
    {
        FUNCTION_ENTRY("createNewAlarmRule");

        TA_ASSERT(m_isNew, "Only new Alarm Rules may be created.");

        
        // Get the database connection
		//TD18044 - change database connection type to retrieve 
		// directly from central DB for the next value
        //IDatabase* databaseConnection =
        //        TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Write);
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Write);
		//++TD18044

        std::string ruleTrigger = databaseConnection->escapeInsertString(m_ruleTrigger);
        std::string ruleType    = databaseConnection->escapeInsertString(m_ruleType);
        
        // Ensure that there is not already a rule with the same
        // fields in the database.
        std::ostringstream getPkeyUsingAllParameters;
        /*getPkeyUsingAllParameters <<
            "SELECT pkey from AlarmRule " <<
            "WHERE " <<
                "entitytypekey = "  << m_entityTypeKey << " AND " <<
                "entitykey     = "  << m_entityKey     << " AND " <<
                "rule_trigger  = '" << ruleTrigger     << "' AND " <<
                "ruletype      = '" << ruleType        << "' AND ";*/
        
        if(m_alarmTypeKey == 0)
        {
            getPkeyUsingAllParameters << " (alarmtype IS NULL or alarmtype=0) AND ";  //support NULL on Mysql
        }
        else
        {
            getPkeyUsingAllParameters << "alarmtype = " << m_alarmTypeKey << " AND ";
        }

        if(m_alarmDescription.empty() == true)
        {
            getPkeyUsingAllParameters << "(alarmdescription IS NULL or alarmdescription ='')"; //support NULL on Mysql
        }
        else
        {
            getPkeyUsingAllParameters << "alarmdescription LIKE '" << m_alarmDescription << "'";
        }

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AlarmRule_STD_SELECT_1008,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ALARMRULE_SELECT_1008,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ALARMRULE_SELECT_1008,
			m_entityTypeKey, m_entityKey, ruleTrigger, ruleType, getPkeyUsingAllParameters.str());
//		TA_ASSERT(strSql.size() > 0, "createNewAlarmRule:: Get SQL Statement error");

        // Execute the query. The method can throw a DatabaseException.
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // If there is already an Alarm Rule with those characteristics,
        // throw an exception.
        if (data->getNumRows() > 0)
        {
            // Clean up the pointer
            delete data;
            data = NULL;

            std::string reasonMessage =
                "Alarm rule with those characteristics already exists";

            TA_THROW(DataException(reasonMessage.c_str(),
                                   DataException::NOT_UNIQUE,
                                   "entity, entity type, alarm type, rule trigger and rule type") );
        }
        // Clean up the pointer
        delete data;
        data = NULL;

        // Get the pkey
        //std::string getPkeySql = "SELECT ALARMRULE_SEQ.NEXTVAL FROM Dual";
//		std::string getPkeySql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AlarmRule_Oracle_SELECT_1051);
//		std::string getPkeySql  = databaseConnection->prepareSQLStatement(ALARMRULE_SELECT_1051);
		SQLStatement getPkeySql;
		databaseConnection->prepareSQLStatement(getPkeySql, ALARMRULE_SELECT_1051);

        data = databaseConnection->executeQuery(getPkeySql, columnNames);

        try
        {
            m_pkey = data->getUnsignedLongData(0, keyColumn);
        }
        catch(...)
        {
            // Clean up the pointer
            delete data;
            data = NULL;

            throw;
        }

        // Clean up the pointer
        delete data;
        data = NULL;

        // Insert the alarm rule
        /*std::ostringstream sql;
        sql << "INSERT INTO AlarmRule "
            << "(" 
            << "pkey, "
            << "entitytypekey, "
            << "entitykey, "
            << "alarmtype, "
            << "rule_trigger, "
            << "ruletype  ";*/

		std::ostringstream ostrColumns;
		if (m_alarmDescription.empty() == false)
		{
            ostrColumns << ", alarmdescription";
		}

        if(m_createdBySessionKey.empty() == false)
        {
            ostrColumns << ", created_by_session_key";
			ostrColumns << ", created_by_profile_key";
			ostrColumns << ", created_by_operator_name";            
        }

        /*sql << ") "
            << "VALUES "
            << "("
            << m_pkey                << ", "
            << m_entityTypeKey       << ", "
            << m_entityKey           << ", ";*/

		std::ostringstream ostrTypeKey;
		if( m_alarmTypeKey == 0)
		{
			ostrTypeKey << "NULL" << ", ";
		}
		else
		{
			ostrTypeKey << m_alarmTypeKey << ", ";
		}

        /*sql << "'" << ruleTrigger    << "', "
            << "'" << ruleType       << "'";*/

		std::ostringstream ostrLastValues;
		if (m_alarmDescription.empty() == false)
		{
            ostrLastValues << ", '" << m_alarmDescription << "' ";
		}

        if(m_createdBySessionKey.empty() == false)
        {
            ostrLastValues << ", '" << m_createdBySessionKey << "' ";
			ostrLastValues << ", " << m_createdByProfileKey << " ";
			ostrLastValues << ", '" << m_createdByOperatorName << "' ";            
        }
                    
        //sql << ")";
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AlarmRule_STD_INSERT_1301,
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARMRULE_INSERT_1301,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARMRULE_INSERT_1301,
			ostrColumns.str(), m_pkey, m_entityTypeKey, m_entityKey, ostrTypeKey.str(), ruleTrigger, 
			ruleType, ostrLastValues.str());
//		TA_ASSERT(sql.size() > 0, "createNewAlarmRule: Get SQL Statement error");

        databaseConnection->executeModification(sql);

        // Now need to update the date fields
        /*sql.str("");
		sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from AlarmRule where pkey = " << m_pkey;*/
//		sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AlarmRule_Oracle_SELECT_1052, m_pkey);
//		sql  = databaseConnection->prepareSQLStatement(ALARMRULE_SELECT_1052, m_pkey);

		databaseConnection->prepareSQLStatement(sql, ALARMRULE_SELECT_1052, m_pkey);
//		TA_ASSERT(sql.size() > 0, "createNewAlarmRule: Get SQL Statement error");
        
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(sql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for pkey = " << m_pkey;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_dateCreated = data->getDateData(0, createdColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;


        m_isNew = false;
        m_isValidData = true;

        FUNCTION_EXIT;
    }



    void AlarmRuleHelper::updateAlarmRule()
    {
        FUNCTION_ENTRY("updateAlarmRule");

        TA_ASSERT(!m_isNew, "Only existing Alarm Rules may be updated.");

    
        // Get a connection to the database
		//TD18044 - change database connection type to retrieve 
		// directly from central DB for the next value
        //IDatabase* databaseConnection =
        //        TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Write);
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Write);
		//++TD18044

        // Escape fields which may contain quotes
        std::string ruleTrigger = databaseConnection->escapeInsertString(m_ruleTrigger);
        std::string ruleType    = databaseConnection->escapeInsertString(m_ruleType);
        
        // Ensure that there is not already a rule with the same
        // fields in the database.
        std::ostringstream getPkeyUsingAllParameters;
        /*getPkeyUsingAllParameters <<
            "SELECT pkey from AlarmRule " <<
            "WHERE " <<
                "entitytypekey = "  << m_entityTypeKey << " AND " <<
                "entitykey     = "  << m_entityKey     << " AND " <<
                "rule_trigger  = '" << ruleTrigger     << "' AND " <<
                "ruletype      = '" << ruleType        << "' AND ";*/

        if( m_alarmTypeKey == 0)
        {
            getPkeyUsingAllParameters << "(alarmtype IS NULL or alarmtype=0) AND "; //support NULL on Mysql
        }
        else
        {
            getPkeyUsingAllParameters << "alarmtype = " << m_alarmTypeKey << " AND ";
        }

        if(m_alarmDescription.empty() == true)
        {
            getPkeyUsingAllParameters << "(alarmdescription IS NULL or alarmdescription='' ) AND ";  //support NULL on Mysql
        }
        else
        {
            getPkeyUsingAllParameters << "alarmdescription LIKE '" << m_alarmDescription << "' AND ";
        }
        getPkeyUsingAllParameters << "pkey != " << m_pkey;
		
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AlarmRule_STD_SELECT_1008,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ALARMRULE_SELECT_1008,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ALARMRULE_SELECT_1008,
			m_entityTypeKey, m_entityKey, ruleTrigger, ruleType, getPkeyUsingAllParameters.str());
//		TA_ASSERT(strSql.size() > 0, "updateAlarmRule: Get SQL Statement error");

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // If there is already an Alarm Rule with those characteristics,
        // throw an exception.
        if (data->getNumRows() > 0)
        {
            // Clean up the pointer
            delete data;
            data = NULL;

            std::string reasonMessage =
                "Alarm rule with those characteristics already exists";

            TA_THROW(DataException(reasonMessage.c_str(),
                                   DataException::NOT_UNIQUE,
                                   "entity, entity type, alarm type, rule trigger and rule type") );
        }
        // Clean up the pointer
        delete data;
        data = NULL;

        /*std::ostringstream sql;
        sql << "UPDATE AlarmRule " 
			<< "SET " 
			<<	"entityTypeKey = "  << m_entityTypeKey << ", "
            <<  "entityKey = "      << m_entityKey     << ", ";*/
		std::ostringstream ostrTypeKey;
		if( m_alarmTypeKey == 0)
		{	
			ostrTypeKey << "alarmType = NULL" << ", ";
		}
		else
		{
			ostrTypeKey << "alarmType =" << m_alarmTypeKey << ", ";
		}

		/*sql <<  "rule_trigger = '"  << ruleTrigger     << "', "
            <<  "ruleType = '"      << ruleType        << "' ";*/
		std::ostringstream ostrLast;
		if (m_alarmDescription.empty() == false)
		{
			ostrLast << ", alarmdescription = '" << m_alarmDescription << "' ";
		}
		ostrLast << "WHERE pkey = " << m_pkey;
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AlarmRule_STD_UPDATE_1151,
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARMRULE_UPDATE_1151,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARMRULE_UPDATE_1151,
			m_entityTypeKey, m_entityKey, ostrTypeKey.str(), ruleTrigger, ruleType, ostrLast.str());
//		TA_ASSERT(sql.size() > 0, "updateAlarmRule: Get SQL Statement error");
        databaseConnection->executeModification(sql);
        
        // Now need to update the date fields
        /*sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
             << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
             << "from AlarmRule where pkey = " << m_pkey;*/
//		sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AlarmRule_Oracle_SELECT_1053, m_pkey);
//		sql  = databaseConnection->prepareSQLStatement(ALARMRULE_SELECT_1053, m_pkey);

		databaseConnection->prepareSQLStatement(sql, ALARMRULE_SELECT_1053, m_pkey);
//		TA_ASSERT(sql.size() > 0, "updateAlarmRule: Get SQL Statement error");

        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object
        // when we're done with it
        data = databaseConnection->executeQuery(sql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for pkey = " << m_pkey;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }

        try
        {
            m_dateCreated = data->getDateData(0,createdColumn);
            m_dateModified = data->getDateData(0,modifiedColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        m_isValidData = true;

        FUNCTION_EXIT;
    }


    void AlarmRuleHelper::invalidate()
    {
        m_isValidData = false;
    }


    void AlarmRuleHelper::reload()
    {
        if(m_isNew || m_isValidData)
        {
            return;
        }
        
        forceReload();
    }


    void AlarmRuleHelper::forceReload()
    {
        FUNCTION_ENTRY("forceReload");

        forceReloadAlarmRule();
        forceReloadParameters();

        // Need to record that we now have valid data
        m_isValidData = true;

        FUNCTION_EXIT;
    }


    void AlarmRuleHelper::forceReloadAlarmRule()
    {
        // Get a connection to the database
		// TD19819 Yanrong++
        //IDatabase* databaseConnection =
        //    TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);
		IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
		// ++Yanrong

        // Get the attributes of this alarm rule
        /*std::ostringstream sql;
        sql <<
            "SELECT " <<
                "entitytypekey, "     <<
                "entitykey, "         <<
                "nvl(alarmtype, 0) as ALARM_TYPE, " <<
                "rule_trigger, "      <<
                "ruletype, "          <<
				"alarmdescription, "  <<
                "nvl(created_by_session_key, ' ') as CREATED_BY_SESSION_KEY, " <<
				"nvl(created_by_Profile_key, 0) as CREATED_BY_PROFILE_KEY, " <<
				"nvl(created_by_Operator_Name, ' ') as CREATED_BY_OPERATOR_NAME, " <<
                "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), " <<
                "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') " <<
            "FROM AlarmRule WHERE pkey = " << m_pkey;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AlarmRule_Oracle_SELECT_1054, m_pkey);
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARMRULE_SELECT_1054, m_pkey);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARMRULE_SELECT_1054, m_pkey);
//		TA_ASSERT(sql.size() > 0, "forceReloadAlarmRule: Get SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string entityTypeKeyColumn = "ENTITY_TYPE_KEY";
        std::string entityKeyColumn     = "ENTITY_KEY";
        std::string alarmTypeColumn     = "ALARM_TYPE";
        std::string ruleTriggerColumn   = "RULE_TRIGGER";
        std::string ruleTypeColumn      = "RULE_TYPE";
		std::string alarmDescriptionColumn = "ALARM_DESCRIPTION";
        std::string createdBySessionKeyColumn = "CREATED_BY_SESSION_KEY";
		std::string createdByProfileKeyColumn = "CREATED_BY_PROFILE_KEY";
		std::string createdByOperatorNameColumn = "CREATED_BY_OPERATOR_NAME";
        std::string dateModifiedColumn  = "DATE_MODIFIED";
        std::string dateCreatedColumn   = "DATE_CREATED";
        
        std::vector<std::string> columnNames;
        columnNames.push_back(entityTypeKeyColumn);
        columnNames.push_back(entityKeyColumn);
        columnNames.push_back(alarmTypeColumn);
        columnNames.push_back(ruleTriggerColumn);
        columnNames.push_back(ruleTypeColumn);
        columnNames.push_back(alarmDescriptionColumn);
        columnNames.push_back(createdBySessionKeyColumn);
		columnNames.push_back(createdByProfileKeyColumn);
		columnNames.push_back(createdByOperatorNameColumn);
        columnNames.push_back(dateModifiedColumn);
        columnNames.push_back(dateCreatedColumn);

        // Execute the query. The method can throw a DatabaseException.
        IData* data = databaseConnection->executeQuery( sql, columnNames );

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        try
        {
            // No entry found with the specified pkey
            if (0 == data->getNumRows())
            {
                std::ostringstream reasonMessage;
                reasonMessage << "No alarm rule found for pkey = " << m_pkey;
                const char* message = reasonMessage.str().c_str();
                
                TA_THROW( DataException( message, DataException::NO_VALUE, " " ) );
            }
            // More than one entry found for the pkey
            else if (1 < data->getNumRows())
            {
                std::ostringstream reasonMessage;
                reasonMessage << "More than one rule found for pkey = " << m_pkey;
                const char* message = reasonMessage.str().c_str();
                
                TA_THROW( DataException(message, DataException::NOT_UNIQUE, " ") );
            }

            // Assign the data as appropriate to the member variables.
            m_entityTypeKey = data->getUnsignedLongData(0, entityTypeKeyColumn);
            m_entityKey     = data->getUnsignedLongData(0, entityKeyColumn);
            m_alarmTypeKey  = data->getUnsignedLongData(0, alarmTypeColumn);
            m_ruleType      = data->getStringData(0, ruleTypeColumn);
            m_ruleTrigger   = data->getStringData(0, ruleTriggerColumn);
			m_alarmDescription = data->getStringData(0, alarmDescriptionColumn);
            m_dateCreated   = data->getDateData(0, dateCreatedColumn);
            m_createdBySessionKey = data->getStringData(0, createdBySessionKeyColumn);

			if (m_createdBySessionKey.empty() == false && m_createdBySessionKey == " ")
			{
				m_createdBySessionKey = "";
			}
			m_createdByProfileKey = data->getUnsignedLongData(0,  createdByProfileKeyColumn);
			m_createdByOperatorName = data->getStringData(0, createdByOperatorNameColumn);

            m_dateModified = data->getDateData(0, dateModifiedColumn, 0);
        }
        catch(...)
        {
            delete data;
            data = NULL;

            throw;
        }

        // Now that we're done with the IData object,
        // it's our responsibility to delete it
        delete data;
        data = NULL;
        
    }



    void AlarmRuleHelper::forceReloadParameters()
    {
        // Get a connection to the database
		//TD18044 - change database connection type to retrieve 
		// directly from central DB for the next value
        //IDatabase* databaseConnection =
        //        TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Write);
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
		//++TD18044

        // Clean up the existing parameters.
        cleanUpDeletedParameters();
        cleanUpParameters();

        // Get the parameters that are direct children of this Alarm Rule
        /*std::ostringstream sql;
        sql <<
            "SELECT AEARPA_ID FROM AE_ALARM_RULE_PARAM " <<
            "WHERE ALARM_RULE_KEY = " << m_pkey;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AE_ALARM_RULE_PARAM_STD_SELECT_9001, m_pkey);
//		std::string sql  = databaseConnection->prepareSQLStatement(AE_ALARM_RULE_PARAM_SELECT_9001, m_pkey);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, AE_ALARM_RULE_PARAM_SELECT_9001, m_pkey);
//		TA_ASSERT(sql.size() > 0, "forceReloadParameters: Get SQL Statement error");

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        IData* data = databaseConnection->executeQuery( sql, columnNames );

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        try
        {
            // For each parameter found, create an object and add it to the
            // vector
            for(unsigned long i = 0; i < data->getNumRows(); i ++)
            {
                unsigned long pkey = data->getUnsignedLongData(i, keyColumn);

                IAlarmRuleActionParameterData* parameter =
                    new AlarmRuleActionParameterHelper(pkey);

                m_parameters.push_back(parameter);
            }
        }
        catch(...)
        {
            delete data;
            data = NULL;

            throw;
        }

        // Now that we're done with the IData object,
        // it's our responsibility to delete it
        delete data;
        data = NULL;
    }


    std::string AlarmRuleHelper::getUnsignedLongAsString(unsigned long number)
    {
        std::stringstream numberStream;
        numberStream << number;
        return numberStream.str();
    }


    unsigned long AlarmRuleHelper::getStringAsUnsignedLong(
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




} // closes TA_Base_Core


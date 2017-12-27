/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AlarmRuleActionParameterHelper.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class provides the actual database code for the
  * IAlarmRuleActionParameterData interface.
  * It provides access to the data stored in a single entry
  * in the AlarmRuleActionParameter table.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/alarm_rule/src/AlarmRuleActionParameterHelper.h"
#include "core/data_access_interface/alarm_rule/src/IAlarmRuleData.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_Core
{
    AlarmRuleActionParameterHelper::AlarmRuleActionParameterHelper(
        AlarmRuleHelper& parentAlarmRule,
        const std::string& type,
        const std::string& value /*= ""*/)
        :
        m_pkey(0),
        m_type(type),
        m_value(value),
        m_relatedParameter(NULL),
        m_isNew(true),
        m_isValidData(false)
    {
        TA_ASSERT(type != "", "Type must not be blank");

        // If the parent alarm rule has not been written to the database yet,
        // we can't get its pkey, so just store a pointer to it.
        if( parentAlarmRule.isNew() )
        {
            m_parentAlarmRule = &parentAlarmRule;
        }
        // If the parent alarm rule has been written to the database,
        // get its pkey and store NULL in the m_parentAlarmRule pointer.
        else
        {
            m_alarmRuleKey = parentAlarmRule.getKey();
            m_parentAlarmRule = NULL;
        }
    }


    AlarmRuleActionParameterHelper::AlarmRuleActionParameterHelper(
        const std::string& type,
        const std::string& value /*= ""*/)
        :
        m_pkey(0),
        m_alarmRuleKey(0),
        m_type(type),
        m_value(value),
        m_relatedParameter(NULL),
        m_parentAlarmRule(NULL),
        m_isNew(true),
        m_isValidData(false)
    {
        TA_ASSERT(type != "", "Type must not be blank");
    }


    AlarmRuleActionParameterHelper::AlarmRuleActionParameterHelper(
        const AlarmRuleActionParameterHelper& parameter)
    {
        m_pkey                   = 0;
        m_alarmRuleKey           = parameter.m_alarmRuleKey;
        m_type                   = parameter.m_type;
        m_value                  = parameter.m_value;
        m_relatedParameter       = NULL;
        m_parentAlarmRule        = parameter.m_parentAlarmRule;
        m_isNew                  = true;
        m_isValidData            = false;
    }


    AlarmRuleActionParameterHelper::AlarmRuleActionParameterHelper(
        unsigned long pkey)
        :
        m_pkey(pkey),
        m_alarmRuleKey(0),
        m_type(""),
        m_value(""),
        m_relatedParameter(NULL),
        m_parentAlarmRule(NULL),
        m_isNew(false),
        m_isValidData(false)
    {
    }


    AlarmRuleActionParameterHelper::~AlarmRuleActionParameterHelper()
    {
        if(m_relatedParameter != NULL)
        {
            delete m_relatedParameter;
            m_relatedParameter = NULL;
        }
    }


    unsigned long AlarmRuleActionParameterHelper::getKey()
    {
        TA_ASSERT(!m_isNew,
                  "A key has not yet been assigned. "
                  "The data must be written to the database first.");
        
        return m_pkey;
    }


    bool AlarmRuleActionParameterHelper::isNew()
    {
        return m_isNew;
    }


    std::string AlarmRuleActionParameterHelper::getType()
    {
        reload();
        return m_type;
    }

    void AlarmRuleActionParameterHelper::setType(const std::string& type)
    {
        reload();
        m_type = type;
    }


    std::string AlarmRuleActionParameterHelper::getValue()
    {
        reload();
        return m_value;
    }

    void AlarmRuleActionParameterHelper::setValue(
        const std::string& value)
    {
        reload();
        m_value = value;
    }



    IAlarmRuleActionParameterData*
        AlarmRuleActionParameterHelper::getRelatedParameter()
    {
        reload();
        return m_relatedParameter;
    }

    
    void AlarmRuleActionParameterHelper::addRelatedParameter(
        const std::string& type, const std::string& value /*= ""*/)
    {
        reload();

        TA_ASSERT(m_relatedParameter == NULL,
                  "Related parameter must be deleted before a new "
                  "one can be added");
        
        m_relatedParameter = new AlarmRuleActionParameterHelper(type, value);
    }
    
    void AlarmRuleActionParameterHelper::deleteRelatedParameter()
    {
        reload();

        TA_ASSERT(m_relatedParameter != NULL,
                  "Related parameter must exist before it can be deleted");

        if(!m_relatedParameter->isNew())
        {
            m_relatedParameter->deleteThisObject();
        }
        delete m_relatedParameter;
        m_relatedParameter = NULL;
    }



    unsigned long AlarmRuleActionParameterHelper::getAlarmRuleKey()
    {
        reload();
        
        TA_ASSERT(m_parentAlarmRule == NULL,
                  "The alarm rule must be written to the database "
                  "before its key can be attained");

        return m_alarmRuleKey;
    }
    
    
    void AlarmRuleActionParameterHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");
     

        // If the type field is blank add its name to a vector
        // of failed field names.
        if (m_type == "")
        {
            std::vector<std::string> fieldNames;
            fieldNames.push_back("Parameter Type");

            throw TA_Base_Core::DataConfigurationException(
                "Alarm rule data not fully specified. "
                "Alarm rule cannot be written to database",
                fieldNames);
        }
        

        // The parent alarm rule must have been applied for the following
        // to work
        TA_ASSERT( m_parentAlarmRule == NULL || !m_parentAlarmRule->isNew(),
                   "Parent rule has not been applied to the database" );

        // Get the key from the parent alarm if its not NULL
        // (Otherwise it should have been retrieved already)
        if( m_parentAlarmRule != NULL )
        {
            m_alarmRuleKey = m_parentAlarmRule->getKey();
            m_parentAlarmRule = NULL;
        }



        // Apply the related parameter's changes first so that its pkey
        // can be retrieved if it is new.
        if(m_relatedParameter != NULL)
        {
            m_relatedParameter->applyChanges();
        }

        // New parameters get inserted
        if(m_isNew)
        {
            createNewAlarmRuleActionParameter();
        }
        // Existing parameters get updated
        else
        {
            updateAlarmRuleActionParameter();
        }


        FUNCTION_EXIT;
    }


    void AlarmRuleActionParameterHelper::deleteThisObject()
    {
        FUNCTION_ENTRY("deleteThisObject");
        
        TA_ASSERT(!m_isNew,
                  "New parameters cannot be deleted from the database.");

         // Get the database connection
		//TD18044 - change database connection type to retrieve 
		// directly from central DB for the next value
        //IDatabase* databaseConnection =
        //        TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Write);
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Write);
		//++TD18044


        // Delete the related parameter if there is one.
        if(m_relatedParameter != NULL)
        {
            if(!m_relatedParameter->isNew())
            {
                m_relatedParameter->deleteThisObject();
            }

            delete m_relatedParameter;
            m_relatedParameter = NULL;
        }

        // Delete the parameter
        /*std::ostringstream deleteActionParametersSql;
        deleteActionParametersSql <<
            "DELETE FROM AE_ALARM_RULE_PARAM " <<
            "WHERE AEARPA_ID = " << m_pkey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AE_ALARM_RULE_PARAM_STD_DELETE_9401, m_pkey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AE_ALARM_RULE_PARAM_DELETE_9401, m_pkey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AE_ALARM_RULE_PARAM_DELETE_9401, m_pkey);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void AlarmRuleActionParameterHelper::createNewAlarmRuleActionParameter()
    {
        FUNCTION_ENTRY("createNewAlarmRuleActionParameter");

        TA_ASSERT(m_isNew, "Only new parameters may be created.");

         // Get the database connection
		//TD18044 - change database connection type to retrieve 
		// directly from central DB for the next value
        //IDatabase* databaseConnection =
        //        TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Write);
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Write);
		//++TD18044

        std::string type  = databaseConnection->escapeInsertString(m_type);
        std::string value = databaseConnection->escapeInsertString(m_value);


        unsigned long relatedParameterKey = 0;
        if(m_relatedParameter != NULL)
        {
            relatedParameterKey = m_relatedParameter->getKey();
        }
        

        // TODO: check the sql for this
        // Get the pkey for this parameter
        /*std::string getPkeySql =
            "SELECT AEARPA_SEQ.NEXTVAL FROM Dual";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AE_ALARM_RULE_PARAM_Oracle_DELETE_9051);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AE_ALARM_RULE_PARAM_SELECT_9051);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AE_ALARM_RULE_PARAM_SELECT_9051);


        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        try
        {
            m_pkey = data->getUnsignedLongData(0, keyColumn);
        }
        catch(...)
        {
            delete data;
            data = NULL;

            throw;
        }
        delete data;
        data = NULL;
        

        // Insert the parameter
        /*std::ostringstream sql;
        sql <<
            "INSERT INTO AE_ALARM_RULE_PARAM " <<
            "(" <<
                "AEARPA_ID, "              <<
                "ALARM_RULE_KEY, "         <<
                "ACTION_PARAMETER_TYPE, "  <<
                "ACTION_PARAMETER_VALUE, " <<
                "RELATED_PARAMETER_KEY"    <<
            ") " <<
            "VALUES " <<
            "(" <<
                        m_pkey               << ", "  <<
                        m_alarmRuleKey       << ", "  <<
                "'" <<  type                 << "', " <<
                "'" <<  value                << "', " <<
                        relatedParameterKey  <<
            ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AE_ALARM_RULE_PARAM_STD_INSERT_9301, m_pkey,
//		strSql  = databaseConnection->prepareSQLStatement(AE_ALARM_RULE_PARAM_INSERT_9301, m_pkey,

		databaseConnection->prepareSQLStatement(strSql, AE_ALARM_RULE_PARAM_INSERT_9301, m_pkey,
			m_alarmRuleKey, type, value, relatedParameterKey);


        databaseConnection->executeModification(strSql);
        
        m_isNew = false;
        m_isValidData = true;

        FUNCTION_EXIT;
    }



    void AlarmRuleActionParameterHelper::updateAlarmRuleActionParameter()
    {
        FUNCTION_ENTRY("updateAlarmRuleActionParameter");

        TA_ASSERT(!m_isNew, "Only existing parameters may be updated.");

    
        // Get a connection to the database
		//TD18044 - change database connection type to retrieve 
		// directly from central DB for the next value
        //IDatabase* databaseConnection =
        //        TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Write);
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Write);
		//++TD18044


        std::string type  = databaseConnection->escapeInsertString(m_type);
        std::string value = databaseConnection->escapeInsertString(m_value);

        unsigned long relatedParameterKey = 0;
        if(m_relatedParameter != NULL)
        {
            relatedParameterKey = m_relatedParameter->getKey();
        }


        /*std::ostringstream sql;

        sql <<
            "UPDATE AE_ALARM_RULE_PARAM " <<
            "SET " <<
                "ALARM_RULE_KEY = "          << m_alarmRuleKey      << ", "  <<
                "ACTION_PARAMETER_TYPE = '"  << type                << "', " <<
                "ACTION_PARAMETER_VALUE = '" << value               << "', " <<
                "RELATED_PARAMETER_KEY = "   << relatedParameterKey << " "   <<
            "WHERE AEARPA_ID = " << m_pkey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AE_ALARM_RULE_PARAM_STD_UPDATE_9151, m_alarmRuleKey,
//		std::string strSql  = databaseConnection->prepareSQLStatement(AE_ALARM_RULE_PARAM_UPDATE_9151, m_alarmRuleKey,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AE_ALARM_RULE_PARAM_UPDATE_9151, m_alarmRuleKey,
			type, value, relatedParameterKey, m_pkey);

        databaseConnection->executeModification(strSql);
        
        m_isValidData = true;

        FUNCTION_EXIT;
    }



    void AlarmRuleActionParameterHelper::invalidate()
    {
        m_isValidData = false;
    }



    void AlarmRuleActionParameterHelper::reload()
    {
        if(m_isNew || m_isValidData)
        {
            return;
        }
        
        forceReload();
    }


    void AlarmRuleActionParameterHelper::forceReload()
    {
        FUNCTION_ENTRY("forceReload");

        // Get a connection to the database
		// TD19819 Yanrong++
        //IDatabase* databaseConnection =
        //    TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);
		IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
		// ++Yanrong

        /*std::ostringstream sql;
        sql <<
            "SELECT " <<
                "ALARM_RULE_KEY, "         <<
                "ACTION_PARAMETER_TYPE, "  <<
                "ACTION_PARAMETER_VALUE, " <<
                "RELATED_PARAMETER_KEY "   <<
            "FROM AE_ALARM_RULE_PARAM " <<
            "WHERE AEARPA_ID = " << m_pkey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AE_ALARM_RULE_PARAM_STD_SELECT_9002, m_pkey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AE_ALARM_RULE_PARAM_SELECT_9002, m_pkey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AE_ALARM_RULE_PARAM_SELECT_9002, m_pkey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string alarmRuleKeyColumn          = "ALARM_RULE_KEY";
        std::string typeColumn                  = "TYPE";
        std::string valueColumn                 = "VALUE";
        std::string relatedParameterKeyColumn   = "RELATED_PARAMETER_KEY";
        
        std::vector<std::string> columnNames;
        columnNames.push_back(alarmRuleKeyColumn);
        columnNames.push_back(typeColumn);
        columnNames.push_back(valueColumn);
        columnNames.push_back(relatedParameterKeyColumn);

        // Execute the query. The method can throw a DatabaseException.
        IData* data = databaseConnection->executeQuery( strSql, columnNames );


        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        try
        {
            // No entry found with the specified pkey
            if (0 == data->getNumRows())
            {
                std::ostringstream reasonMessage;
                reasonMessage << "No parameter found for pkey = " << m_pkey;
                const char* message = reasonMessage.str().c_str();
			    
                TA_THROW( DataException( message, DataException::NO_VALUE, "" ) );
            }
            // More than one entry found for the pkey
            else if (1 < data->getNumRows())
            {
                std::ostringstream reasonMessage;
                reasonMessage <<
                    "More than one parameter found for pkey = " << m_pkey;
                const char* message = reasonMessage.str().c_str();
			    
                TA_THROW( DataException(message, DataException::NOT_UNIQUE, "") );
            }


            std::string type = data->getStringData(0, typeColumn);
            if(type == "")
            {
                const char* message = "The type field must not be blank.";
                TA_THROW(
                    DataException(message, DataException::MISSING_MANDATORY, "") );
            }

            unsigned long newRelatedParameterKey =
                data->getUnsignedLongData(0, relatedParameterKeyColumn);

            unsigned long oldRelatedParameterKey = 0;
            if(m_relatedParameter != NULL)
            {
                oldRelatedParameterKey = m_relatedParameter->getKey();
            }


            // If the related parameter has changed
            if(oldRelatedParameterKey != newRelatedParameterKey)
            {
                // If there was previously a related parameter,
                // delete it
                if(oldRelatedParameterKey != 0)
                {
                    delete m_relatedParameter;
                    m_relatedParameter = NULL;
                }

                // If there is now a related parameter, create it
                if(newRelatedParameterKey != 0)
                {
                    m_relatedParameter =
                        new AlarmRuleActionParameterHelper(newRelatedParameterKey);
                }
            }

            // Assign the data as appropriate to the member variables.
		    m_alarmRuleKey = data->getUnsignedLongData(0, alarmRuleKeyColumn);
            m_type         = type;
            m_value        = data->getStringData(0, valueColumn);
            
            
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
        
        // Need to record that we now have valid data
        m_isValidData = true;

        FUNCTION_EXIT;
    }



} // closes TA_Base_Core


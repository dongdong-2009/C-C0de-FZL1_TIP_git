/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/SuppressionRuleHelper.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Provides an interface to Alarm Rules of type suppression in the database.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/data_access_interface/alarm_rule/src/SuppressionRuleHelper.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionByDescriptionRuleData.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	const std::string SuppressionRuleHelper::VALUE = "VALUE";
	const std::string SuppressionRuleHelper::ASSETNAME = "ASSETNAME";

    SuppressionRuleHelper::SuppressionRuleHelper(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger)
        :
        AlarmRuleHelper(
            entityTypeKey,
            entityKey,
            alarmTypeKey,
			SuppressionRuleData::RULE_TYPE,
            ruleTrigger),
		m_value(""),
		m_assetName("")
	{
		m_loaded = false;
    }

    SuppressionRuleHelper::SuppressionRuleHelper(
    unsigned long entityTypeKey,
    unsigned long entityKey,
    unsigned long alarmTypeKey,
    const std::string& ruleTrigger,
	const std::string& description, /*""*/
    const std::string& createdBySessionKey /*=""*/,
	const std::string& assetName, /*=""*/
	const std::string& value /*""*/)
    :
    AlarmRuleHelper(
        entityTypeKey,
        entityKey,
        alarmTypeKey,
        SuppressionRuleData::RULE_TYPE,
        ruleTrigger,
		description,
        createdBySessionKey)
    {
		m_assetName = assetName,
		m_value = value;
		changeParameters();

		m_loaded = false;
    }


    SuppressionRuleHelper::SuppressionRuleHelper()
        :
        AlarmRuleHelper(
            SuppressionRuleData::RULE_TYPE,
            IAlarmRuleData::RULE_TRIGGER_SUBMIT),
		m_value(""),
		m_assetName("")
    {
		m_loaded = false;
    }


    SuppressionRuleHelper::SuppressionRuleHelper(unsigned long pkey)
        :
        AlarmRuleHelper(pkey),
		m_value(""),
		m_assetName("")
    {
		m_loaded = false;
    }


    SuppressionRuleHelper::SuppressionRuleHelper(
        SuppressionRuleHelper& original)
        :
        AlarmRuleHelper(
            original.getEntityTypeKey(),
            original.getEntityKey(),
            original.getAlarmType(),
            original.getRuleType(),
            original.getRuleTrigger(),
			original.getAlarmDescription())
    {
		m_value = original.getValue();
		m_assetName = original.getAssetName();
		m_operator = original.getOperatorName();
		changeParameters();
		// no need to reload the data as they are the same
		m_loaded = true;
    }


    SuppressionRuleHelper::~SuppressionRuleHelper()
    {
    }

	const std::string SuppressionRuleHelper::getDescription()
	{
		if (!m_loaded)
		{
			loadData();
		}
		return m_description;
	}

	void SuppressionRuleHelper::setDescription(const std::string description)
	{
		if (m_description.compare(description) != 0)
		{
			m_description = description;
			//iterate the parameters and apply the changes
			changeParameters();
		}
    }

	const std::string SuppressionRuleHelper::getValue()
	{
		if (!m_loaded)
		{
			loadData();
		}
		return m_value;
	}

	void SuppressionRuleHelper::setValue(const std::string value)
	{
		if (m_value.compare(value) != 0)
		{
			m_value = value;
			// apply the changes
			changeParameters();
		}
	}

	const std::string SuppressionRuleHelper::getAssetName()
	{
		if (!m_loaded)
		{
			loadData();
		}
		return m_assetName;
	}

	void SuppressionRuleHelper::setAssetName(std::string assetName)
	{
		if (m_assetName.compare(assetName) != 0)
		{
			m_assetName = assetName;
			changeParameters();
		}
	}

	const std::string SuppressionRuleHelper::getOperatorName()
	{
		if (!m_loaded)
		{
			loadData();
		}
		return m_operator;
	}

    void SuppressionRuleHelper::setRuleTrigger(const std::string& trigger)
    {
        TA_ASSERT(trigger == IAlarmRuleData::RULE_TRIGGER_SUBMIT,
                  "Suppression can only occur when an alarm is submitted");
    }

	void SuppressionRuleHelper::createNewAlarmRule()
	{
        FUNCTION_ENTRY("SuppressionRuleHelper::createNewAlarmRule");

        TA_ASSERT(isNew(), "Only new Alarm Rules may be created.");

        
         // Get the database connection
		// TD19819 Yanrong++
        //IDatabase* databaseConnection =
        //        TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);
		IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
		// ++Yanrong


        std::string ruleTrigger = databaseConnection->escapeInsertString(getRuleTrigger());
        std::string ruleType    = databaseConnection->escapeInsertString(getRuleType());
        
        // Ensure that there is not already a rule with the same
        // fields in the database.
        std::ostringstream getPkeyUsingAllParameters;
        /*getPkeyUsingAllParameters <<
            "SELECT pkey from AlarmRule " <<
            "WHERE " <<
                "entitytypekey = "  << getEntityTypeKey() << " AND " <<
                "entitykey     = "  << getEntityKey()     << " AND " <<
                "rule_trigger  = '" << ruleTrigger     << "' AND " <<
                "ruletype      = '" << ruleType        << "' AND ";*/
        
        if(getAlarmType() == 0)
        {
            getPkeyUsingAllParameters << "(alarmtype IS NULL or alarmtype=0 )";  //support NULL on Mysql
        }
        else
        {
            getPkeyUsingAllParameters << "alarmtype = " << getAlarmType();
        }

		// If suppression-by-type, uniqueness is a combination entity key and alarm type
		// If suppression-by-description, uniqueness is a combination of entity key, alarm type and alarm description
		if (ruleType==TA_Base_Core::SuppressionByDescriptionRuleData::RULE_TYPE)
		{
			if(getAlarmDescription().empty() == true)
			{
				getPkeyUsingAllParameters << " AND (alarmdescription IS NULL or alarmdescription='' )";  //support NULL on Mysql
			}
			else
			{
				getPkeyUsingAllParameters << " AND alarmdescription LIKE '" << getAlarmDescription() << "'";
			}
		}
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AlarmRule_STD_SELECT_1008, getEntityTypeKey(),
//		std::string strSql  = databaseConnection->prepareSQLStatement(ALARMRULE_SELECT_1008, getEntityTypeKey(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ALARMRULE_SELECT_1008, getEntityTypeKey(),
			getEntityKey(), ruleTrigger, ruleType, getPkeyUsingAllParameters.str() );

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);


        // Execute the query. The method can throw a DatabaseException.
        IData* data = databaseConnection->executeQuery( strSql, columnNames );

        
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

		// If there is no existing rule of same type, proceed to create the new rule
		AlarmRuleHelper::createNewAlarmRule();

		FUNCTION_EXIT;
	}

	void SuppressionRuleHelper::loadData()
	{
		try
		{
			// load the parameters
			ActionParameterVector parameters = getParameters();
			if (parameters.size() > 0)
			{
				for (ActionParameterVector::iterator itr = parameters.begin(); itr != parameters.end(); ++itr)
				{
					IAlarmRuleActionParameterData* paramData = *itr;
					if (paramData->getType().compare(VALUE) == 0)
					{
						m_value = paramData->getValue();
					}
					else if (paramData->getType().compare(ASSETNAME) == 0)
					{
						m_assetName = paramData->getValue();
					}
				}
			}

			// load the asset name using the entityKey, if the assetName is empty
			std::string name( getEntityName() );
			if (m_assetName.empty() == true && name.empty() == false)
			{
				std::auto_ptr<TA_Base_Core::IData> entityParam = std::auto_ptr<TA_Base_Core::IData>(TA_Base_Core::EntityAccessFactory::getInstance().getParameter(name, "AssetName"));
				if (entityParam.get() != 0 && entityParam->getNumRows() > 0 && entityParam->isNull(0, "VALUE") == false)
				{
					m_assetName = entityParam->getStringData(0, "VALUE");
				}
				else
				{
					// use the entity name if asset name is not provided
					m_assetName=name;
				}
			}

			// load the operator name using the session key
			const std::string sessionId( getCreatedBySessionKey() );
			m_operator="";
			m_operator=getCreatedByOperatorName().c_str();

			if (sessionId.empty() == false && m_operator.empty()==true )
			{
				try 
				{			
					std::auto_ptr<TA_Base_Core::ISession> session = std::auto_ptr<TA_Base_Core::ISession>(TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId));
					if (session.get() != 0)
					{
						m_operator = session->getOperatorName();
					}
					else
					{
						m_operator = "";
					}
				}
				catch(...)
				{
				    m_operator = "";
				}
			}
// 			else
// 			{
// 				m_operator = "";
// 			}

			// set the load as completed
			m_loaded = true;
		}
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not determine the type of this entity parameter");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not determine the type of this entity parameter");
        }

	}

	void SuppressionRuleHelper::changeParameters()
	{
		// delete the current parameters	
		ActionParameterVector parameters = getParameters();
		if (parameters.size() > 0)
		{
			for (ActionParameterVector::iterator itr = parameters.begin(); itr != parameters.end(); ++itr)
			{
				IAlarmRuleActionParameterData* paramData = *itr;
				deleteParameter(paramData);
			}
		}

		addNewParameter(VALUE, m_value);
		addNewParameter(ASSETNAME, m_assetName );
		//applyChanges();

		// reload the data to ensure they are saved as per changes
		m_loaded = false;
	}

}// TA_Base_Core

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/20 22:28:15 $
  * Last modified by:  $Author: huirong.luo $
  *
  * AlarmRuleAccessFactory is a singleton that is used to retrieve AlarmRuleData objects 
  * from the database. It is able to create new objects, and provide access to the
  * Alarm Rule database tables.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleData.h"
//#include "core/data_access_interface/alarm_rule/src/SuppressionRuleData.h"
//#include "core/data_access_interface/alarm_rule/src/SuppressionByDescriptionRuleData.h"
//#include "core/data_access_interface/alarm_rule/src/MMSRuleData.h"
//#include "core/data_access_interface/alarm_rule/src/CCTVRuleData.h"
//#include "core/data_access_interface/alarm_rule/src/AutomaticAcknowledgementRuleData.h"
//#include "core/data_access_interface/alarm_rule/src/PlanExecutionRuleData.h"
//#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/synchronisation/src/ThreadGuard.h"

//TD16491++
#include "core/utilities/src/RunParams.h"
//++TD16491

#include "core/data_access_interface/src/SQLCode.h"
//++oyzl

using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	//TD16491++
	const std::string AlarmRuleAccessFactory::RPARAM_MMS_SUBMITTER_TYPE = "MmsSubmitterType";
	//++TD16491

    AlarmRuleAccessFactory* AlarmRuleAccessFactory::m_instance = 0;
    NonReEntrantThreadLockable AlarmRuleAccessFactory::m_singletonLock;
   
    AlarmRuleAccessFactory& AlarmRuleAccessFactory::getInstance()
    {
        ThreadGuard guard(m_singletonLock);

        if( m_instance == 0 )
        {
			//TD16491++
			std::string strMmsType("");
			if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_MMS_SUBMITTER_TYPE.c_str()))
			{
				MMSRuleData::RULE_TYPE = TA_Base_Core::RunParams::getInstance().get(RPARAM_MMS_SUBMITTER_TYPE.c_str());
			}
			//++TD16491

            m_instance = new AlarmRuleAccessFactory();
        }
        
        return *m_instance;
    }

    
	IAlarmRuleData* AlarmRuleAccessFactory::getAlarmRule(unsigned long pkey)
	{
	    FUNCTION_ENTRY("getAlarmRule");
	
		// Get a connection to the database
		// TD19819 Yanrong++
		//IDatabase* databaseConnection = 
		//		TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);
		IDatabase* databaseConnection = 
				TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
		// ++Yanrong

		// Check if the alarm rule is actually in the database
		/*std::ostringstream sql;
        sql << "SELECT ruleType FROM alarmrule WHERE pkey = " << pkey;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),			AlarmRule_STD_SELECT_1007, pkey);
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARMRULE_SELECT_1007, pkey);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARMRULE_SELECT_1007, pkey);
//		TA_ASSERT(sql.size() > 0, "getAlarmRule: GetSQLStatement error");

		std::vector<std::string> columns;
        std::string ruleTypeColumn = "RULE_TYPE";
		columns.push_back(ruleTypeColumn);
		IData* data = databaseConnection->executeQuery(sql, columns);

        int nRows = data->getNumRows();
        
        if (nRows == 0)
		{
			//TA_THROW(DataException("Alarm rule not found",DataException::NO_VALUE,sql));			 
			TA_THROW(DataException("Alarm rule not found",DataException::NO_VALUE, sql.strCommon));

		}
		if (nRows > 1)
		{
			//TA_THROW(DataException("More than one alarm rule found", DataException::NOT_UNIQUE, sql ));				
			 
			TA_THROW(DataException("Alarm rule not found",DataException::NO_VALUE, sql.strCommon));

		}

        std::string ruleType = "";
        try
        {
            ruleType = data->getStringData(0, ruleTypeColumn);
        }
        catch(...)
        {
            delete data;
            data = NULL;
            throw;
        }
        delete data;
        data = NULL;

        FUNCTION_EXIT;
        return getAlarmRuleFromKeyAndType(pkey, ruleType);
    }


    IAlarmRuleData* AlarmRuleAccessFactory::getAlarmRuleFromKeyAndType(
        unsigned long pkey, const std::string& ruleType)
    {
        FUNCTION_ENTRY("getAlarmRuleFromKeyAndType");

        // Work out what type of Alarm Rule this is and return
        // the appropriate one
        if(ruleType == SuppressionRuleData::RULE_TYPE)
        {
            FUNCTION_EXIT;
            return new SuppressionRuleData(pkey);
        }
        else if(ruleType == SuppressionByDescriptionRuleData::RULE_TYPE)
        {
            FUNCTION_EXIT;
            return new SuppressionByDescriptionRuleData(pkey);
        }
        else if(ruleType == PlanExecutionRuleData::RULE_TYPE)
        {
            FUNCTION_EXIT;
            return new PlanExecutionRuleData(pkey);
        }
        else if(ruleType == AutomaticAcknowledgementRuleData::RULE_TYPE)
        {
            FUNCTION_EXIT;
            return new AutomaticAcknowledgementRuleData(pkey);
        }
        else if(ruleType == AvalancheSuppressionRuleData::RULE_TYPE)
        {
            FUNCTION_EXIT;
            return new AvalancheSuppressionRuleData(pkey);
        }
        else if(ruleType == MMSRuleData::RULE_TYPE)
        {
            FUNCTION_EXIT;
            return new MMSRuleData(pkey);
        }
        else if(ruleType == CCTVRuleData::RULE_TYPE)
        {
            FUNCTION_EXIT;
            return new CCTVRuleData(pkey);
        }
        else
        {
            std::string error = "Unrecognised rule type: " + ruleType;
            const char* message = error.c_str();
            TA_THROW(
                DataException(
                    message, DataException::MISSING_MANDATORY, "RULE_TYPE"));
        }
    }


    // TODO: add a lazyFetch parameter that is true(?) by default
	std::vector<IAlarmRuleData*> AlarmRuleAccessFactory::getAllAlarmRules()
	{
		FUNCTION_ENTRY("getAllAlarmRules");

		// Create the query
		/*std::string sql = "SELECT pkey, ruleType FROM AlarmRule";*/

        // Get a connection to the database
		// TD19819 Yanrong++
		//IDatabase* databaseConnection = 
		//		TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);
		IDatabase* databaseConnection = 
				TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AlarmRule_STD_SELECT_1001);
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARMRULE_SELECT_1001);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARMRULE_SELECT_1001);
//		TA_ASSERT(sql.size() > 0, "getAllAlarmRules: GetSQLStatement error");
		// ++Yanrong		

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
        std::string keyColumn = "pkey";
        std::string typeColumn = "type";
        columnNames.push_back(keyColumn);
        columnNames.push_back(typeColumn);

        // Execute the query
		IData* data = databaseConnection->executeQuery(sql, columnNames);

        // Create the vector to return
        std::vector<IAlarmRuleData*> alarmRuleVector;


		std::map<unsigned long, std::string> pkey_type_map;
		typedef std::pair<unsigned long, std::string> Map_pair; 
		unsigned long pkey = 0; 
		std::string ruleType = "";

        // Loop for each row returned
        do
        {
            alarmRuleVector.reserve(alarmRuleVector.size() + data->getNumRows());

            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception.
                // Need to catch it to do pointer clean up.
                try
                {
                    pkey = data->getUnsignedLongData(i, keyColumn);
                    ruleType = data->getStringData(i, typeColumn);
					pkey_type_map.insert(Map_pair(pkey,ruleType));
					pkey = 0;
					ruleType = "";

                }
                catch (const TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;

                    throw;
                }
            }

            delete data;
            data = NULL;
        }
        while (databaseConnection->moreData(data));


		IAlarmRuleData* rule = NULL; 
		std::map<unsigned long, std::string>::iterator it;

		for( it=pkey_type_map.begin(); it!=pkey_type_map.end(); it++){
			pkey = it->first;
			ruleType = it->second;
			rule = getAlarmRuleFromKeyAndType(pkey, ruleType);
			alarmRuleVector.push_back(rule);
		}
        
		FUNCTION_EXIT;

        // Return the vector
        return alarmRuleVector;
	}


    
	std::vector<SuppressionRuleData*>
        AlarmRuleAccessFactory::getAllSuppressionRules()
	{
		FUNCTION_ENTRY("getAllSuppressionRules");

		// Create the query
		/*std::string sqlSuppressionByType =
            "SELECT pkey FROM alarmrule "
            "WHERE ruleType = '" + SuppressionRuleData::RULE_TYPE + "'";*/
		IDatabase* pDatabase 
			= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
//		std::string sqlSuppressionByType = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1002, 
//		std::string sqlSuppressionByType  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1002, 
		SQLStatement sqlSuppressionByType;
		pDatabase->prepareSQLStatement(sqlSuppressionByType, ALARMRULE_SELECT_1002,
			SuppressionRuleData::RULE_TYPE);		
//		TA_ASSERT(sqlSuppressionByType.size() > 0, "getAllSuppressionRules: GetSQLStatement error");

        // Get the pkeys
        std::vector<unsigned long> suppressionByTypePkeys = getKeys(pDatabase, sqlSuppressionByType);

		// Create the query
		/*std::string sqlSuppressionByDesc =
            "SELECT pkey FROM alarmrule "
            "WHERE ruleType = '" + SuppressionByDescriptionRuleData::RULE_TYPE + "'";*/
//		std::string sqlSuppressionByDesc = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1002,
//		std::string sqlSuppressionByDesc  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1002,
		SQLStatement sqlSuppressionByDesc;
		pDatabase->prepareSQLStatement(sqlSuppressionByDesc, ALARMRULE_SELECT_1002,
			SuppressionByDescriptionRuleData::RULE_TYPE);		

        // Get the pkeys
        std::vector<unsigned long> suppressionByDescPkeys = getKeys(pDatabase, sqlSuppressionByDesc);

        // Create the vector to return
        std::vector<SuppressionRuleData*> suppressionRuleVector;
        suppressionRuleVector.reserve(suppressionByTypePkeys.size() + suppressionByDescPkeys.size());

        for (unsigned long i = 0; i < suppressionByTypePkeys.size(); i++)
        {
            suppressionRuleVector.push_back(new SuppressionRuleData(suppressionByTypePkeys[i]));
        }
		for (unsigned long j = 0; j < suppressionByDescPkeys.size(); j++)
		{
			suppressionRuleVector.push_back(new SuppressionByDescriptionRuleData(suppressionByDescPkeys[j]));
		}

		FUNCTION_EXIT;

        // Return the vector
        return suppressionRuleVector;
	}



    std::vector<SuppressionRuleData*> AlarmRuleAccessFactory::getSuppressionRulesByLocation(unsigned long locationKey)
    {
        FUNCTION_ENTRY("getSuppressionRulesByLocation");

		// Create the suppression by type query
        /*std::stringstream sqlSuppressionByType;
        sqlSuppressionByType << "SELECT a.pkey FROM alarmrule a, entity e "
            << "WHERE a.ruleType = '" << SuppressionRuleData::RULE_TYPE << "' "
            << "AND a.entitykey = e.pkey "
		    << "AND e.locationkey = " << locationKey;*/
		IDatabase* pDatabase 
			= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
//		std::string sqlSuppressionByType = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1003, SuppressionRuleData::RULE_TYPE,
//		std::string sqlSuppressionByType  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1003, SuppressionRuleData::RULE_TYPE,
		SQLStatement sqlSuppressionByType;
		pDatabase->prepareSQLStatement(sqlSuppressionByType, ALARMRULE_SELECT_1003, SuppressionRuleData::RULE_TYPE,
			locationKey);	
//		TA_ASSERT(sqlSuppressionByType.size() > 0, "getSuppressionRulesByLocation: Get SQL statement error");

        // Get the pkeys
        std::vector<unsigned long> suppressionByTypePkeys = getKeys(pDatabase, sqlSuppressionByType);

		// Create the suppression by description query
        /*std::stringstream sqlSuppressionByDesc;
        sqlSuppressionByDesc << "SELECT a.pkey FROM alarmrule a, entity e "
            << "WHERE a.ruleType = '" << SuppressionByDescriptionRuleData::RULE_TYPE << "' "
            << "AND a.entitykey = e.pkey "
		    << "AND e.locationkey = " << locationKey;*/
//		std::string sqlSuppressionByDesc = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1003, 
//		std::string sqlSuppressionByDesc  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1003, 
		SQLStatement sqlSuppressionByDesc;
		pDatabase->prepareSQLStatement(sqlSuppressionByDesc, ALARMRULE_SELECT_1003,
			SuppressionByDescriptionRuleData::RULE_TYPE, locationKey);	
//		TA_ASSERT(sqlSuppressionByDesc.size() > 0, "getSuppressionRulesByLocation: Get SQL statement error");

        // Get the pkeys
        std::vector<unsigned long> suppressionByDescPkeys = getKeys(pDatabase, sqlSuppressionByDesc);

        // Create the vector to return
        std::vector<SuppressionRuleData*> suppressionRuleVector;
        suppressionRuleVector.reserve(suppressionByTypePkeys.size() + suppressionByDescPkeys.size());

        for (unsigned long i = 0; i < suppressionByTypePkeys.size(); i++)
        {
            suppressionRuleVector.push_back(new SuppressionRuleData(suppressionByTypePkeys[i]));
        }
		for (unsigned long j = 0; j < suppressionByDescPkeys.size(); j++)
		{
			suppressionRuleVector.push_back(new SuppressionByDescriptionRuleData(suppressionByDescPkeys[j]));
		}

		FUNCTION_EXIT;

        // Return the vector
        return suppressionRuleVector;
    }



	std::vector<PlanExecutionRuleData*>
        AlarmRuleAccessFactory::getAllPlanExecutionRules()
	{
		FUNCTION_ENTRY("getAllPlanExecutionRules");

		// Create the query
		/*std::string sql =
            "SELECT pkey FROM alarmrule "
            "WHERE ruleType = '" + PlanExecutionRuleData::RULE_TYPE + "'";*/
		IDatabase* pDatabase 
			= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
//		std::string sql = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1002, 
//		std::string sql  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1002, 
		SQLStatement sql;
		pDatabase->prepareSQLStatement(sql, ALARMRULE_SELECT_1002,
			PlanExecutionRuleData::RULE_TYPE);
//		TA_ASSERT(sql.size() > 0, "getAllPlanExecutionRules: GetSQLStatement error");

        // Get the pkeys
        std::vector<unsigned long> pkeys = getKeys(pDatabase, sql);

        // Create the vector to return
        std::vector<PlanExecutionRuleData*> planRuleVector;
        planRuleVector.reserve(pkeys.size());

        for (unsigned long i = 0; i < pkeys.size(); i++)
        {
            planRuleVector.push_back(new PlanExecutionRuleData(pkeys[i]));
        }

		FUNCTION_EXIT;

        // Return the vector
        return planRuleVector;
	}


	std::vector<AutomaticAcknowledgementRuleData*>
        AlarmRuleAccessFactory::getAllAutomaticAcknowledgementRules()
	{
		FUNCTION_ENTRY("getAllAutomaticAcknowledgementRules");

		// Create the query
		/*std::string sql =
            "SELECT pkey FROM alarmrule "
            "WHERE ruleType = '" + AutomaticAcknowledgementRuleData::RULE_TYPE + "'";*/
		IDatabase* pDatabase 
			= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
//		std::string sql = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1002, 
//		std::string sql  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1002, 
		SQLStatement sql;
		pDatabase->prepareSQLStatement(sql, ALARMRULE_SELECT_1002,
			AutomaticAcknowledgementRuleData::RULE_TYPE);
//		TA_ASSERT(sql.size() > 0, "getAllAutomaticAcknowledgementRules: GetSQLStatement error");

        // Get the pkeys
        std::vector<unsigned long> pkeys = getKeys(pDatabase, sql);

        // Create the vector to return
        std::vector<AutomaticAcknowledgementRuleData*>
            automaticAcknowledgementRuleVector;
        automaticAcknowledgementRuleVector.reserve(pkeys.size());

        for (unsigned long i = 0; i < pkeys.size(); i++)
        {
            automaticAcknowledgementRuleVector.push_back(
                new AutomaticAcknowledgementRuleData(pkeys[i]));
        }

		FUNCTION_EXIT;

        // Return the vector
        return automaticAcknowledgementRuleVector;
	}


	std::vector<MMSRuleData*>
        AlarmRuleAccessFactory::getAllMMSRules()
	{
		FUNCTION_ENTRY("getAllMMSRules");

		// Create the query
		/*std::string sql =
            "SELECT pkey FROM alarmrule "
            "WHERE ruleType = '" + MMSRuleData::RULE_TYPE + "'";*/
		IDatabase* pDatabase 
			= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
//		std::string sql = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1002, MMSRuleData::RULE_TYPE);
//		std::string sql  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1002, MMSRuleData::RULE_TYPE);
		SQLStatement sql;
		pDatabase->prepareSQLStatement(sql, ALARMRULE_SELECT_1002, MMSRuleData::RULE_TYPE);
//		TA_ASSERT(sql.size() > 0, "getAllMMSRules: GetSQLStatement error");

        // Get the pkeys
        std::vector<unsigned long> pkeys = getKeys(pDatabase, sql);

        // Create the vector to return
        std::vector<MMSRuleData*> mmsRuleVector;
        mmsRuleVector.reserve(pkeys.size());

        for (unsigned long i = 0; i < pkeys.size(); i++)
        {
            mmsRuleVector.push_back(new MMSRuleData(pkeys[i]));
        }

		FUNCTION_EXIT;

        // Return the vector
        return mmsRuleVector;
	}

    //Mintao++ TD15302
    MMSRuleData* AlarmRuleAccessFactory::getMMSRuleForAlarmTypeKey(unsigned long alarmtypekey)
    {
		FUNCTION_ENTRY("getMMSRuleForAlarmTypeKey");

		// Create the query
		//std::ostringstream sql;
		//TD17628 marc_bugfix
		//Since what is being queried here is if there is an existing alarm rule with
		//which a MMS rule type,
		//then add a where condition 
        //sql <<  "SELECT pkey FROM alarmrule WHERE alarmtype = " << alarmtypekey;
		//sql <<  "SELECT pkey FROM alarmrule WHERE ruletype = 'MMS' and alarmtype = " << alarmtypekey;
		IDatabase* pDatabase 
			= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
//		std::string sql = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1004, alarmtypekey);
//		std::string sql  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1004, alarmtypekey);
		SQLStatement sql;
		pDatabase->prepareSQLStatement(sql, ALARMRULE_SELECT_1004, alarmtypekey);
//		TA_ASSERT(sql.size() > 0, "getMMSRuleForAlarmTypeKey: GetSQLStatement error");

        // Get the pkeys
        std::vector<unsigned long> pkeys = getKeys(pDatabase, sql);
        
        MMSRuleData* toReturn;
		//It is possible that there might be more than one alarm rule with the same alarm type and mms
		//So change the checking 		
		//if (pkeys.size() == 1)
		if (!pkeys.empty())	//for now, always return the first row
        {
            toReturn = new MMSRuleData(pkeys[0]);
        }
        else
        {
            toReturn = NULL;
        }
		FUNCTION_EXIT;
        // Return the MMS Rule data
        return toReturn;
    }
    //Mintao++ TD15302

	std::vector<CCTVRuleData*>
        AlarmRuleAccessFactory::getAllCCTVRules()
	{
		FUNCTION_ENTRY("getAllCCTVRules");

		// Create the query
		/*std::string sql =
            "SELECT pkey FROM alarmrule "
            "WHERE ruleType = '" + CCTVRuleData::RULE_TYPE + "'";*/
		IDatabase* pDatabase 
			= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
//		std::string sql = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1002, CCTVRuleData::RULE_TYPE);
//		std::string sql  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1002, CCTVRuleData::RULE_TYPE);
		SQLStatement sql;
		pDatabase->prepareSQLStatement(sql, ALARMRULE_SELECT_1002, CCTVRuleData::RULE_TYPE);
//		TA_ASSERT(sql.size() > 0, "getAllCCTVRules: GetSQLStatement error");

        // Get the pkeys
        std::vector<unsigned long> pkeys = getKeys(pDatabase, sql);

        // Create the vector to return
        std::vector<CCTVRuleData*> cctvRuleVector;
        cctvRuleVector.reserve(pkeys.size());

        for (unsigned long i = 0; i < pkeys.size(); i++)
        {
            cctvRuleVector.push_back(new CCTVRuleData(pkeys[i]));
        }

		FUNCTION_EXIT;

        // Return the vector
        return cctvRuleVector;
	}


    std::vector<AvalancheSuppressionRuleData*>
        AlarmRuleAccessFactory::getAllAvalancheSuppressionRules()
	{
		FUNCTION_ENTRY("getAllAvalancheSuppressionRules");

		// Create the query
		/*std::string sql =
            "SELECT pkey FROM alarmrule "
            "WHERE ruleType = '" + AvalancheSuppressionRuleData::RULE_TYPE + "'";*/
		IDatabase* pDatabase 
			= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
//		std::string sql = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1002, AvalancheSuppressionRuleData::RULE_TYPE);
//		std::string sql  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1002, AvalancheSuppressionRuleData::RULE_TYPE);
		SQLStatement sql;
		pDatabase->prepareSQLStatement(sql, ALARMRULE_SELECT_1002, AvalancheSuppressionRuleData::RULE_TYPE);
//		TA_ASSERT(sql.size() > 0, "getAllAvalancheSuppressionRules: GetSQLStatement error");

        // Get the pkeys
        std::vector<unsigned long> pkeys = getKeys(pDatabase, sql);

        // Create the vector to return
        std::vector<AvalancheSuppressionRuleData*>
            avalancheSuppressionRuleVector;
        avalancheSuppressionRuleVector.reserve(pkeys.size());

        for (unsigned long i = 0; i < pkeys.size(); i++)
        {
            avalancheSuppressionRuleVector.push_back(
                new AvalancheSuppressionRuleData(pkeys[i]));
        }

		FUNCTION_EXIT;

        // Return the vector
        return avalancheSuppressionRuleVector;
	}


    std::vector<AvalancheSuppressionRuleData*>
        AlarmRuleAccessFactory::getAllAvalancheSuppressionRules(
            unsigned long locationKey)
	{
		FUNCTION_ENTRY("getAllAvalancheSuppressionRules");

		// Create the query
		/*std::ostringstream sql;
        sql <<
            "SELECT pkey FROM AlarmRule " <<
            "WHERE " <<
                "ruleType = '" + AvalancheSuppressionRuleData::RULE_TYPE + "' " <<
                "AND entityKey IN " <<
                    "(SELECT pkey FROM Entity " <<
                    "WHERE locationKey = " << locationKey << ")";*/
		IDatabase* pDatabase 
			= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
//		std::string sql = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1005, 
//		std::string sql  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1005, 
		SQLStatement sql;
		pDatabase->prepareSQLStatement(sql, ALARMRULE_SELECT_1005,
			AvalancheSuppressionRuleData::RULE_TYPE, locationKey);
//		TA_ASSERT(sql.size() > 0, "getAllAvalancheSuppressionRules: GetSQLStatement error");

        // Get the pkeys
        std::vector<unsigned long> pkeys = getKeys(pDatabase, sql);

        // Create the vector to return
        std::vector<AvalancheSuppressionRuleData*>
            avalancheSuppressionRuleVector;
        avalancheSuppressionRuleVector.reserve(pkeys.size());

        for (unsigned long i = 0; i < pkeys.size(); i++)
        {
            avalancheSuppressionRuleVector.push_back(
                new AvalancheSuppressionRuleData(pkeys[i]));
        }

		FUNCTION_EXIT;

        // Return the vector
        return avalancheSuppressionRuleVector;
	}


    std::vector<AvalancheSuppressionRuleData*>
        AlarmRuleAccessFactory::getAllAvalancheSuppressionRules(
            const std::string& locationName)
	{
		FUNCTION_ENTRY("getAllAvalancheSuppressionRules");

		// Create the query
		/*std::ostringstream sql;
        sql <<
            "SELECT pkey FROM AlarmRule "
            "WHERE " <<
                "ruleType = '" + AvalancheSuppressionRuleData::RULE_TYPE + "' " <<
                "AND entityKey IN " <<
                    "(SELECT pkey FROM Entity " <<
                    "WHERE locationKey = "
                        "(SELECT pkey FROM Location "
                        "WHERE name = '" << locationName << "')" <<
                    ")";*/
		IDatabase* pDatabase 
			= TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);
//		std::string sql = defPrepareSQLStatement(pDatabase->GetDbServerType(), AlarmRule_STD_SELECT_1006,
//		std::string sql  = pDatabase->prepareSQLStatement(ALARMRULE_SELECT_1006,
		SQLStatement sql;
		pDatabase->prepareSQLStatement(sql, ALARMRULE_SELECT_1006,
			AvalancheSuppressionRuleData::RULE_TYPE, locationName);
//		TA_ASSERT(sql.size() > 0, "getAllAvalancheSuppressionRules: GetSQLStatement error");

        // Get the pkeys
        std::vector<unsigned long> pkeys = getKeys(pDatabase, sql);

        // Create the vector to return
        std::vector<AvalancheSuppressionRuleData*>
            avalancheSuppressionRuleVector;
        avalancheSuppressionRuleVector.reserve(pkeys.size());

        for (unsigned long i = 0; i < pkeys.size(); i++)
        {
            avalancheSuppressionRuleVector.push_back(
                new AvalancheSuppressionRuleData(pkeys[i]));
        }

		FUNCTION_EXIT;

        // Return the vector
        return avalancheSuppressionRuleVector;
	}




    std::vector<unsigned long> AlarmRuleAccessFactory::getKeys(
		IDatabase* pDatabase,
        const SQLStatement& sql)
    {
		FUNCTION_ENTRY("getKeys");

         
		//remove this assert
		//TA_ASSERT(strExSql != "", "SQL parameter must not be empty");*/

		// Get a connection to the database
		// TD19819 Yanrong++		
		/*IDatabase* databaseConnection = 
				TA_Base_Core::DatabaseFactory::getInstance().getDatabase(AlarmRules_Cd, Read);*/
		TA_ASSERT(0 != pDatabase, "Database connection is null");

		// ++Yanrong

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
        std::string keyColumn = "pkey";
        columnNames.push_back(keyColumn);

        // Execute the query
		IData* data = pDatabase->executeQuery(sql, columnNames);

	    // Create the vector to return
        std::vector<unsigned long> pkeys;

 	    if (data->getNumRows() == 0) // No pkeys found
        {
            // clean up the pointer
            delete data;
            data = NULL;

            return pkeys;
        }

        // Loop for each row returned
        do
        {
            pkeys.reserve(pkeys.size() + data->getNumRows());

            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception.
                // Need to catch it to do pointer clean up.
                try
                {
                    pkeys.push_back( data->getUnsignedLongData(i, keyColumn) );
                }
                catch (const TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;

                    throw;
                }
            }

            delete data;
            data = NULL;
        }
        while (pDatabase->moreData(data));

		FUNCTION_EXIT;

        // Return the vector
        return pkeys;
    }


    IAlarmRuleData* AlarmRuleAccessFactory::createAlarmRule(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleType,
        const std::string& ruleTrigger)
	{
		return new AlarmRuleData(new AlarmRuleHelper(
            entityTypeKey, entityKey, alarmTypeKey, ruleType, ruleTrigger));
	}


    SuppressionRuleData* AlarmRuleAccessFactory::createSuppressionRule(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger,
        const std::string& createdBySessionKey /*= "Unknown"*/,
		const std::string& assetName /*=""*/,
		const std::string& description /*=""*/,
		const std::string& value)
    {
        return new SuppressionRuleData(
            entityTypeKey, entityKey, alarmTypeKey, ruleTrigger, createdBySessionKey, assetName, description, value);
    }

    SuppressionByDescriptionRuleData* AlarmRuleAccessFactory::createSuppressionByDescriptionRule(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger,
		const std::string& description,
        const std::string& createdBySessionKey /*= "Unknown"*/,
		const std::string& assetName /*=""*/,
		const std::string& value /*=""*/)
    {
        return new SuppressionByDescriptionRuleData(
            entityTypeKey, entityKey, alarmTypeKey, ruleTrigger, description, createdBySessionKey, assetName, value);
    }

    SuppressionRuleData* AlarmRuleAccessFactory::createSuppressionRule()
    {
        return new SuppressionRuleData();
    }

    SuppressionByDescriptionRuleData* AlarmRuleAccessFactory::createSuppressionByDescriptionRule()
    {
        return new SuppressionByDescriptionRuleData();
    }


    PlanExecutionRuleData* AlarmRuleAccessFactory::createPlanExecutionRule(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger,
        const std::string& planPath)
    {
        return new PlanExecutionRuleData(
            entityTypeKey, entityKey, alarmTypeKey, ruleTrigger, planPath);
    }


    PlanExecutionRuleData* AlarmRuleAccessFactory::createPlanExecutionRule()
    {
        return new PlanExecutionRuleData();
    }

    
    AutomaticAcknowledgementRuleData*
        AlarmRuleAccessFactory::createAutomaticAcknowledgementRule(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger)
    {
        return new AutomaticAcknowledgementRuleData(
            entityTypeKey, entityKey, alarmTypeKey, ruleTrigger);
    }

    AutomaticAcknowledgementRuleData*
        AlarmRuleAccessFactory::createAutomaticAcknowledgementRule()
    {
        return new AutomaticAcknowledgementRuleData();
    }


    MMSRuleData* AlarmRuleAccessFactory::createMMSRule(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger)
    {
        return new MMSRuleData(
            entityTypeKey, entityKey, alarmTypeKey, ruleTrigger);
    }


    MMSRuleData* AlarmRuleAccessFactory::createMMSRule()
    {
        return new MMSRuleData();
    }

    CCTVRuleData* AlarmRuleAccessFactory::createCCTVRule(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger)
    {
        return new CCTVRuleData(
            entityTypeKey, entityKey, alarmTypeKey, ruleTrigger);
    }


    CCTVRuleData* AlarmRuleAccessFactory::createCCTVRule()
    {
        return new CCTVRuleData();
    }

    AvalancheSuppressionRuleData* 
        AlarmRuleAccessFactory::createAvalancheSuppressionRule(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger,
        unsigned long timeout,
        unsigned long timein,
        unsigned long minAlarms,
        const std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>&
            entityKeyAlarmTypePairs)
    {
        return new AvalancheSuppressionRuleData(
            entityTypeKey,
            entityKey,
            alarmTypeKey,
            ruleTrigger,
            timeout,
            timein,
            minAlarms,
            entityKeyAlarmTypePairs);
    }

    AvalancheSuppressionRuleData*
        AlarmRuleAccessFactory::createAvalancheSuppressionRule()
    {
        return new AvalancheSuppressionRuleData();
    }

    SuppressionRuleData*
        AlarmRuleAccessFactory::copySuppressionRule(
            const SuppressionRuleData& original)
    {
        return new SuppressionRuleData(original);
    }

    SuppressionByDescriptionRuleData*
        AlarmRuleAccessFactory::copySuppressionByDescriptionRule(
            const SuppressionByDescriptionRuleData& original)
    {
        return new SuppressionByDescriptionRuleData(original);
    }


    PlanExecutionRuleData*
        AlarmRuleAccessFactory::copyPlanExecutionRule(
            const PlanExecutionRuleData& original)
    {
        return new PlanExecutionRuleData(original);
    }

    
    AutomaticAcknowledgementRuleData*
        AlarmRuleAccessFactory::copyAutomaticAcknowledgementRule(
            const AutomaticAcknowledgementRuleData& original)
    {
        return new AutomaticAcknowledgementRuleData(original);
    }

    MMSRuleData*
        AlarmRuleAccessFactory::copyMMSRule(
            const MMSRuleData& original)
    {
        return new MMSRuleData(original);
    }


    CCTVRuleData*
        AlarmRuleAccessFactory::copyCCTVRule(
            const CCTVRuleData& original)
    {
        return new CCTVRuleData(original);
    }


    AvalancheSuppressionRuleData*
        AlarmRuleAccessFactory::copyAvalancheSuppressionRule(
            const AvalancheSuppressionRuleData& original)
    {
        return new AvalancheSuppressionRuleData(original);
    }


    std::vector<std::string> AlarmRuleAccessFactory::getAllRuleTriggers()
    {
        std::vector<std::string> ruleTriggers;

        ruleTriggers.reserve(3);

        ruleTriggers.push_back(IAlarmRuleData::RULE_TRIGGER_ACKNOWLEDGE);
        ruleTriggers.push_back(IAlarmRuleData::RULE_TRIGGER_SUBMIT);
        ruleTriggers.push_back(IAlarmRuleData::RULE_TRIGGER_CLOSE);

        return ruleTriggers;
    }



} // closes TA_Base_Core

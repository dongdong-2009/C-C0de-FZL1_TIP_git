/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ActionAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ActionAccessFactory is a singleton that is used to retrieve action objects either from the
  * database or newly created. All action objects returned will adhear to the IAction interface.
  * Data is primarily retrieved from the SE_ACTION table
  *
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/ActionAccessFactory.h"
#include "core/data_access_interface/src/Action.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/SQLCode.h"
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	ActionAccessFactory::ActionAccessFactory()
	{
		initializeAction();
	}

    ActionAccessFactory* ActionAccessFactory::m_instance = 0;

    ActionAccessFactory& ActionAccessFactory::getInstance()
    {

        if( m_instance == 0 )
        {
            m_instance = new ActionAccessFactory();
        }

        return *m_instance;
    }


	void ActionAccessFactory::initializeAction()
	{
        FUNCTION_ENTRY("ActionAccessFactory::initializeAction()");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OfflineSecurity_Cd, Read);
		
        /*std::ostringstream sql;
        sql << "select m.seacti_id, g.name from se_action_group g, se_action_group_map m where m.seagro_id = g.seagro_id";*/

//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_action_group_STD_SELECT_11501);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11501);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_ACTION_GROUP_SELECT_11501);
//		TA_ASSERT(sql.size() > 0, "ActionAccessFactory::initializeAction(): Prepare SQL Statement error");

  

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("SEACTI_ID");
        columnNames.push_back("NAME");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql, columnNames);

        do
        {

			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					unsigned long actionId  = data->getUnsignedLongData(i, columnNames[0]);
					std::string actionGroup = data->getStringData(i, columnNames[1]);

					m_actionToActionGroupMap[actionId].push_back(actionGroup);
				}
				//data exception 
				catch (TA_Base_Core::DataException&)
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

        while ( databaseConnection->moreData(data) );

	}

	
    void ActionAccessFactory::getActionGroupByAction(unsigned long actionId, std::vector<std::string>& actionGroups)
    {
		FUNCTION_ENTRY("ActionAccessFactory::getActionGroupByAction");

		ActionToActionGroupMap::iterator it = m_actionToActionGroupMap.find( actionId );

		if ( it != m_actionToActionGroupMap.end() )
		{
            actionGroups = it->second;
		}
		FUNCTION_EXIT;
    }
	

	void ActionAccessFactory::getActionBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IAction*>& actions)
    {
        FUNCTION_ENTRY("getActionBySql");

        // get a connection to the database
        //IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OfflineSecurity_Cd, Read);
		TA_ASSERT(pDatabase != NULL, "Database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
		columnNames.push_back("IS_DISPLAY_ONLY");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql, columnNames);

        do
        {

			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{

				try
				{
					unsigned long actionId = data->getUnsignedLongData(i, columnNames[0]);					
					std::vector<std::string> actionGroups;
					getActionGroupByAction(actionId, actionGroups);

					actions.push_back( new Action( i, *data, actionGroups) );
				}
				//data exception
				catch (TA_Base_Core::DataException&)
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

        while ( pDatabase->moreData(data) );

        FUNCTION_EXIT;
    }

    IAction* ActionAccessFactory::getAction( const unsigned long actionKey )
    {
        FUNCTION_ENTRY("getAction");

	// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OfflineSecurity_Cd, Read);

//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_ACTION_STD_SELECT_12001, actionKey);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_ACTION_SELECT_12001, actionKey);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_ACTION_SELECT_12001, actionKey);
//		TA_ASSERT(sql.size() > 0, "ActionAccessFactory::getAction(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << "select SEACTI_ID, NAME, DESCRIPTION from SE_ACTION where SEACTI_ID = " << actionKey;*/				
		
		std::vector< IAction* > actions;
		getActionBySql(databaseConnection, sql, actions);

        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if ( 0 == actions.size())
        {

            std::ostringstream message;
			message << "No data found for action with key " << actionKey;
           //  TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );
             
			 std::string strExSql;
			 int nDbType = databaseConnection->getDbServerType();
			 switch (nDbType)
			 {
			 case enumOracleDb:
				 strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
				 break;
			 case enumMysqlDb:
				 strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
				 break;		
			 }
			 TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );  
             
        }

		TA_ASSERT(1 == actions.size(), "Action key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return actions[0];
    }

    std::vector<IAction*> ActionAccessFactory::getAllActions()
    {
        FUNCTION_ENTRY("getAllActions");

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OfflineSecurity_Cd, Read);

		//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_ACTION_STD_SELECT_12002);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_ACTION_SELECT_12002);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_ACTION_SELECT_12002);
//		TA_ASSERT(sql.size() > 0, "ActionAccessFactory::getAllActions(): Prepare SQL Statement error");

		/*std::ostringstream sql;		
		sql << " select SEACTI_ID, NAME, DESCRIPTION from SE_ACTION";*/

		std::vector< IAction* > actions;
		getActionBySql(databaseConnection, sql, actions);

        FUNCTION_EXIT;
		return actions;
    }

//limin++ for centralize duty/rights
    void ActionAccessFactory::getActionGroupMap( const std::set< unsigned long >& groupIdList, ActionGroupMap& actionGroupMap )
    {
        TA_ASSERT( 0 < groupIdList.size() && groupIdList.size() < 256, "group id list is too small or large" );
        actionGroupMap.clear();

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( OfflineSecurity_Cd, Read );
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector< std::string > columnNames;
        columnNames.push_back( "SEAGRO_ID" );
        columnNames.push_back( "SEACTI_ID" );

        std::stringstream sql;
        //sql << "SELECT seagro_id, seacti_id FROM se_action_group_map WHERE seagro_id IN(";
        
        for ( std::set< unsigned long >::const_iterator iter = groupIdList.begin(); iter != groupIdList.end(); ++iter )
        {

            if ( iter != groupIdList.begin() )
            {
                sql << ", ";
            }

            sql << *iter;
        }
        
        //sql << ") ORDER BY seagro_id";

//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_action_group_STD_SELECT_11502, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11502, sql.str());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_ACTION_GROUP_SELECT_11502, sql.str());
//		TA_ASSERT(strSql.size() > 0, "getActionGroupMap(): Prepare SQL Statement error");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery( strSql, columnNames );
       
        do
        {

            for ( unsigned long i = 0; i < data->getNumRows(); i++ )
            {

                try
                {
                    unsigned long groupId =  data->getUnsignedLongData( i, columnNames[ 0 ] );
                    unsigned long actionId = data->getUnsignedLongData( i, columnNames[ 1 ] );
                    actionGroupMap[ groupId ].push_back( actionId );
                }
				//data exception
                catch ( TA_Base_Core::DataException& )
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

        while ( databaseConnection->moreData( data ) );

    }
	
	void ActionAccessFactory::getNonPhysicalSubsystemActionGroup(const std::vector<unsigned long>& profileList, std::multimap<unsigned long, unsigned long>& groupIdList)
	{
        TA_ASSERT( 1 == profileList.size() || 2 == profileList.size() , "profile id list is too small or large" );

		groupIdList.clear();
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( OfflineSecurity_Cd, Read );
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector< std::string > columnNames;
        columnNames.push_back( "SUBSYSTEMKEY" );
        columnNames.push_back( "SEAGRO_ID" );
		
        std::stringstream sql;
		//sql << "select spa.subsystemkey, spa.seagro_id from se_profile_access spa, subsystem sub where sub.is_physical=0 and sub.PKEY = spa.SUBSYSTEMKEY and ";
		
		if(1==profileList.size())
		{
			sql << "spa.SEPROF_ID = " << profileList[0];
		}
		
		else
		{
			sql << "(spa.SEPROF_ID = " << profileList[0] << " or spa.SEPROF_ID = " << profileList[1] << ")";
		}
		
		//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_profile_access_STD_SELECT_12501, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_ACCESS_SELECT_12501, sql.str());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_ACCESS_SELECT_12501, sql.str());
//		TA_ASSERT(strSql.size() > 0, "getNonPhysicalSubsystemActionGroup(): Prepare SQL Statement error");	
		
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "SQL sentence: %s", strSql.c_str());
				
		std::string strExSql;
		int nExDbType = databaseConnection->getDbServerType();
		switch (nExDbType)
		{
		case enumOracleDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
			break;
		case enumMysqlDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
			break;		
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "SQL sentence: %s", strExSql.c_str());

		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery( strSql, columnNames );
		
        do
        {

            for ( unsigned long i = 0; i < data->getNumRows(); i++ )
            {

                try
                {
                    unsigned long subsystemId =  data->getUnsignedLongData( i, columnNames[ 0 ] );
                    unsigned long groupId =  data->getUnsignedLongData( i, columnNames[ 1 ] );
                    groupIdList.insert(std::make_pair(subsystemId, groupId));

                }
				//data exception
                catch ( TA_Base_Core::DataException& )
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

        while ( databaseConnection->moreData( data ) );


	}
} // closes TA_Base_Core

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ActionHelper.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#ifdef __WIN32__
	#pragma warning(disable:4786)
	#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include <vector>

#include "core/data_access_interface/src/ActionHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	ActionHelper::ActionHelper(unsigned long key)
		:
		m_isValidData( false ),
		m_key( key),
        m_name(""),
        m_description(""),
		m_isDisplayOnly(false)
	{
	}

	ActionHelper::ActionHelper( const unsigned long key, const std::string& name, const std::string& description )
		:
		m_isValidData( false ),
		m_key( key ),
        m_name( name ),
        m_description( description )
    {
    }

    ActionHelper::ActionHelper( const unsigned long key, const std::string& name, const std::string& description ,const bool isDisplayOnly)
		:
		m_isValidData( false ),
		m_key( key ),
        m_name( name ),
        m_description( description ),
		m_isDisplayOnly(isDisplayOnly)
    {
    }

    ActionHelper::ActionHelper(const unsigned long row, TA_Base_Core::IData& data, std::vector<std::string>& actionGroups)
		:
		m_isValidData(false)
    {
        FUNCTION_ENTRY("ActionHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data, actionGroups);
        FUNCTION_EXIT;
    }


	unsigned long ActionHelper::getKey()
    {
        return( m_key );
    }


    std::string ActionHelper::getName()
    {

		if( !m_isValidData && m_name.empty() )
        {
            reload();
        }

        return( m_name );
    }

    std::string ActionHelper::getDescription()
    {

		if( !m_isValidData && m_description.empty() )
        {
            reload();
        }

        return( m_description );
    }

    
    std::vector<std::string> ActionHelper::getActionGroups()
	{

		if( !m_isValidData )
        {
            reload();
        }

		// return a copy
        return( m_actionGroups );
	}

    void ActionHelper::invalidate()
    {
        m_isValidData = false;

		m_name = "";
        m_description = "";
		m_actionGroups.clear();
    }

	bool ActionHelper::isDisplayOnly()
	{

		if( !m_isValidData )
        {
            reload();
        }

		// return a copy
        return( m_isDisplayOnly );
	}

	void ActionHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();
		
		columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
		columnNames.push_back("IS_DISPLAY_ONLY");
	}

    void ActionHelper::reload()
    {
		FUNCTION_ENTRY("ActionHelper::reload()");
		
		// get the name and the ActionGroup
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OfflineSecurity_Cd, Read);

		// Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

		/*std::ostringstream sql;
		sql << "select SEACTI_ID, NAME, DESCRIPTION from SE_ACTION where SEACTI_ID = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_ACTION_STD_SELECT_12001, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_ACTION_SELECT_12001, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_ACTION_SELECT_12001, m_key);

		std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery( strSql, columnNames ));
		
		if( NULL == data.get() || (0 == data->getNumRows()) ) 
        {
            // If there is no entry then this is an error as the key as accurate before
            std::ostringstream reasonMessage;
            reasonMessage << "No data found for action with key = " << m_key;
            TA_THROW( DataException(reasonMessage.str().c_str(), DataException::NO_VALUE, "") );
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of Action records."); 

        /*sql.str("");
        sql << "select g.name from se_action_group g, se_action_group_map m where m.seagro_id = g.seagro_id and m.seacti_id = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_action_group_STD_SELECT_11512, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11512, m_key);

		databaseConnection->prepareSQLStatement(strSql, SE_ACTION_GROUP_SELECT_11512, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        columnNames.clear();
        columnNames.push_back("NAME");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data2 = databaseConnection->executeQuery(strSql, columnNames);
		
		std::vector<std::string> actionGroups;

        do
        {

			for (unsigned long i = 0; i < data2->getNumRows(); i++)
			{

				try
				{
					std::string actionGroup = data2->getStringData(i, columnNames[0]);
					actionGroups.push_back(actionGroup);
				}
				//data exception
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data2;
					data2 = NULL;
					throw;
				}
			}
			delete data2;
			data2 = NULL;
        }

        while ( databaseConnection->moreData(data2) );

        reloadUsing(0, *data, actionGroups);
		FUNCTION_EXIT;
    }


    void ActionHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data, std::vector<std::string>& actionGroups)
    {
        FUNCTION_ENTRY("ActionHelper::reloadUsing(row, data, actionGroups)");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);
		
		m_key  = data.getUnsignedLongData( row, columnNames[0] );
        m_name = data.getStringData( row, columnNames[1] );		
		m_description = data.getStringData( row, columnNames[2] );
		m_isDisplayOnly=data.getBooleanData(row, columnNames[3]);
		m_actionGroups = actionGroups;

		// Need to record that we now have valid data
		m_isValidData = true;
        
        FUNCTION_EXIT;
    }
	
} // closes TA_Base_Core

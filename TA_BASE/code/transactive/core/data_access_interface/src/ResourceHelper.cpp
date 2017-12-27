/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ResourceHelper.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#pragma warning(disable:4284)
#endif

#include "core/data_access_interface/src/ResourceHelper.h"
#include "core/data_access_interface/src/IData.h"
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
    ResourceHelper::ResourceHelper(const unsigned long key) 
		: m_isValidData( false ),
          m_key( key ),
          m_subsystem( 0 )
	{
	}

	ResourceHelper::ResourceHelper()
		: m_isValidData( false ),
          m_key( 0 ),
          m_subsystem( 0 )
	{
	}

    ResourceHelper::ResourceHelper(const unsigned long row, TA_Base_Core::IData& data)
		:   
		m_isValidData( false ),
		m_key( 0 ),
		m_subsystem( 0 )
    {
        FUNCTION_ENTRY("ProfileHelper(const unsigned long, TA_Base_Core::IData&)");        

        reloadUsing(row, data);
        FUNCTION_EXIT;
    }

	ResourceHelper::ResourceHelper( const ResourceHelper& theHelper)
        : m_isValidData(theHelper.m_isValidData),
		  m_key(theHelper.m_key),
		  m_subsystem(theHelper.m_subsystem)
    {
    }

	ResourceHelper::~ResourceHelper()
	{
	}

    unsigned long ResourceHelper::getKey()
    {
        return m_key;
    }

	unsigned long ResourceHelper::getSubsystem()
	{
		if( !m_isValidData )
		{
			reload();
		}
		return m_subsystem;
	}

	void ResourceHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

        columnNames.push_back("PKEY");
        columnNames.push_back("SUBSYSTEMKEY");
	}	

	
    void ResourceHelper::reload()
    {
		FUNCTION_ENTRY( "reload" );
     
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        // create the SQL string to retrieve the data of the gui
        // based upon the key
        /*std::ostringstream sql;
		sql << "select PKEY, SUBSYSTEMKEY from SE_RESOURCE_V where PKEY = " << m_key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4502, m_key);
//		std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4502, m_key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ENTITY_SELECT_4502, m_key);
//		TA_ASSERT(sql.size() > 0, "ResourceHelper::reload: Get SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(sql,columnNames));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reasonMessage;
			reasonMessage << "No resource found for pkey = " << m_key;
			FUNCTION_EXIT;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of resource records."); 
        reloadUsing(0, *data);

		FUNCTION_EXIT;
    }

	void ResourceHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key       = data.getUnsignedLongData( row, columnNames[0] );
        m_subsystem = data.getUnsignedLongData( row, columnNames[1] );

        m_isValidData = true;
        FUNCTION_EXIT;
    }

} // TA_Base_Core

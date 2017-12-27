/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/RegionHelper.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RegionHelper is an object that is held by all region objects. It contains all
  * data common across regions, and manipulation methods for the data. It helps avoid
  * re-writing code for each region object.
  *
  */

#ifdef __WIN32__
#pragma warning(disable: 4786)
#endif

#include <vector>

#include "core/data_access_interface/src/RegionHelper.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    RegionHelper::RegionHelper( ) 
        : m_key(0),
          m_name(""),
          m_description(""), 
		  m_displayName(""),
          m_dateCreated(0),
          m_dateModified(0), 
          m_isNew(true),
		  m_isValidData(false)
    {
    }

    RegionHelper::RegionHelper( const unsigned long key )
        : m_key(key),
          m_name(""),
          m_description(""), 
		  m_displayName(""),
          m_dateCreated(0),
          m_dateModified(0), 
          m_isNew(false),
		  m_isValidData(false)
    {
    }
    
    RegionHelper::RegionHelper( const unsigned long key, const std::string& name, const std::string& description,
                                    const time_t dateCreated, const time_t dateModified)
        : m_key(key),
          m_name(name),
          m_description(description),
		  m_displayName(name),
          m_dateCreated(dateCreated),
          m_dateModified(dateModified), 
          m_isNew(false),
		  m_isValidData(true)
    {
    }


    RegionHelper::RegionHelper(const unsigned long row, TA_Base_Core::IData& data)
		:
		m_key(0),
		m_name( "" ),
		m_displayName(""),
		m_isNew(false),
		m_isValidData( false )
    {
        FUNCTION_ENTRY("RegionHelper(const unsigned long, TA_Base_Core::IData&)");        

        reloadUsing(row, data);
        FUNCTION_EXIT;
    }
	

	RegionHelper::RegionHelper( const RegionHelper& theRegionHelperToCopy) :
        m_key(0),
        m_name(theRegionHelperToCopy.m_name),
        m_description(theRegionHelperToCopy.m_description),
		m_displayName(theRegionHelperToCopy.m_displayName),
        m_dateCreated(theRegionHelperToCopy.m_dateCreated),
        m_dateModified(theRegionHelperToCopy.m_dateModified),
        m_isNew(true),
		m_isValidData(theRegionHelperToCopy.m_isValidData)
    {
    }

    RegionHelper::~RegionHelper()
    {
    }

    unsigned long RegionHelper::getKey()
    {
        TA_ASSERT(!m_isNew, "Cannot retrieve the key on a region before it has been applied to the database");
        return m_key;
    }

    std::string RegionHelper::getName()
    {
        // We don't want to call reload() if this is a new region and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing region and the data needs to be loaded
            reload();
        }
        return m_name;
    }

    std::string RegionHelper::getDescription()
    {
        // We don't want to call reload() if this is a new region and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing region and the data needs to be loaded
            reload();
        }
        return m_description;
    }

	std::string RegionHelper::getDisplayName()
	{
		// We don't want to call reload() if this is a new region and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing region and the data needs to be loaded
			reload();
		}
		return m_displayName;
	}

    time_t RegionHelper::getDateCreated()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date created can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateCreated;
    }

    time_t RegionHelper::getDateModified()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateModified;
    }

    void RegionHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new entity");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


	void RegionHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

        columnNames.push_back("SEREGI_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
		columnNames.push_back("DISPLAY_NAME");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
	}	
	
	
    void RegionHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        /*std::ostringstream sql;
        sql << " select SEREGI_ID, NAME, DESCRIPTION,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SE_REGION WHERE SEREGI_ID = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_REGION_Oracle_SELECT_8555, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_REGION_SELECT_8555, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_REGION_SELECT_8555, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery( strSql, columnNames ));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reasonMessage;
            reasonMessage << "No region found for pkey = " << m_key;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of region records."); 
        reloadUsing(0, *data);
		
        FUNCTION_EXIT;
    }

	void RegionHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key  = data.getUnsignedLongData( row, columnNames[0] );
        m_name = data.getStringData( row, columnNames[1] );
        m_description = data.getStringData( row, columnNames[2]);
		m_displayName = data.getStringData( row, columnNames[3] );
        m_dateModified = data.getDateData(row,columnNames[4], 0);
        m_dateCreated = data.getDateData( row, columnNames[5]);
 
        m_isValidData = true;
        FUNCTION_EXIT;
    }

} // closes TA_Base_Core



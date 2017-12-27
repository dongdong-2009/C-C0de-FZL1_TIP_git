/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/trends/src/TrendHelper.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * This is the base Trend class that supports all get and set operation on trends
  *
  * NOTE: This will also directly read the TREND_TEMPLATE table to decide if the trend requires a display
  */

#ifdef _MSC_VER
  #pragma warning(disable:4786)
  #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
  #pragma warning(disable:4284)
#endif // _MSC_VER

#include <vector>
#include <set>
#include <time.h>

#include "core/data_access_interface/trends/src/TrendHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/trends/src/Globals.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string TrendHelper::DELETED_FLAG("#");
    const std::string TrendHelper::DEFAULT_GRAPH_TYPE(GRAPH_TYPES[0]);
    const long TrendHelper::DEFAULT_TIME_COMPONENT(3600);

    
    TrendHelper::TrendHelper(const std::string& name) 
		: m_isValidData(false),
          m_isNew(false),
          m_name(name),
          m_description(""),
          m_graphType(DEFAULT_GRAPH_TYPE),
          m_rangeSeconds(DEFAULT_TIME_COMPONENT),
          m_samplePeriod(60),
          m_isLive(true),
          m_offsetTime(DEFAULT_TIME_COMPONENT),
          m_extraInfo(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_originalName(name)
	{
        // Set the start time to the current time minus an hour
        time_t timeNow;
		time(&timeNow);
        m_startDateTime = timeNow - DEFAULT_TIME_COMPONENT;
	}


	TrendHelper::TrendHelper()
		: m_isValidData(false),
          m_isNew(true),
          m_name(""),
          m_description(""),
          m_graphType(DEFAULT_GRAPH_TYPE),
          m_rangeSeconds(DEFAULT_TIME_COMPONENT),
          m_samplePeriod(60),
          m_isLive(true),
          m_offsetTime(DEFAULT_TIME_COMPONENT),
          m_extraInfo(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_originalName("")
	{
        // Set the start time to the current time minus an hour
        time_t timeNow;
		time(&timeNow);
        m_startDateTime = timeNow - DEFAULT_TIME_COMPONENT;
	}


	TrendHelper::TrendHelper( const TrendHelper& theHelper)
        : m_isValidData(false),
          m_isNew(true),
          m_name(""),
          m_description(theHelper.m_description),
          m_graphType(theHelper.m_graphType),
          m_rangeSeconds(theHelper.m_rangeSeconds),
          m_samplePeriod(theHelper.m_samplePeriod),
          //m_isLive(theHelper.isLive),
		  m_isLive( const_cast<TrendHelper&>(theHelper).isLive() ),
          m_offsetTime(theHelper.m_offsetTime),
          m_extraInfo(theHelper.m_extraInfo),
          m_dateCreated(0),
          m_dateModified(0),
          m_originalName(theHelper.m_originalName),
          m_startDateTime(theHelper.m_startDateTime)
    {
        m_items = theHelper.m_items;
    }



	TrendHelper::~TrendHelper()
	{
	}


    std::string TrendHelper::getName()
    {
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_name;
    }


	void TrendHelper::setName( const std::string& name )
	{
        FUNCTION_ENTRY("setName" );

        if(m_originalName.empty())
        {
            m_originalName = name;
        }

        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        
        m_name = name;
        
        FUNCTION_EXIT;
	}
    

    time_t TrendHelper::getDateCreated()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t TrendHelper::getDateModified()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_dateModified;
    }
        
    
    std::string TrendHelper::getDescription()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_description;
    }


    long TrendHelper::getRangeSeconds()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_rangeSeconds;
    }

	
    long TrendHelper::getSampleTypeInSeconds()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_samplePeriod;
    }

	
    time_t TrendHelper::getStartDateTime()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_startDateTime;
    }

	
    bool TrendHelper::isLive()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_isLive;
    }
	

    long TrendHelper::getOffsetInSeconds()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_offsetTime;
    }

	
    std::map<std::string, ItemInfo> TrendHelper::getItems()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_items;
    }


    std::string TrendHelper::getExtraInfo()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_extraInfo;
    }


    std::string TrendHelper::getGraphTypeAsString()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        return m_graphType;
    }


    void TrendHelper::decomposeTimeComponent(long rangeTimeInSeconds, long& hours, long& minutes)
    {
        const double SECONDS_IN_HOUR = 60 * 60;
        const double SECONDS_IN_MINUTE = 60;

        hours = 0;
        minutes = 0;
        
        hours = static_cast<long>(rangeTimeInSeconds / SECONDS_IN_HOUR);

        if(hours <= 0)
        {
            hours = 0;
            minutes = static_cast<long>(rangeTimeInSeconds / SECONDS_IN_MINUTE);
        }
        
        if(hours > 0)
        {
            int mod = rangeTimeInSeconds % static_cast <int> (SECONDS_IN_HOUR);

            if(mod == 0)
            {
                minutes = 0;
            }
            else
            {
                //int leftOver = rangeTimeInSeconds - (rangeTimeInSeconds / mod);
                //minutes = leftOver / SECONDS_IN_MINUTE;
                minutes = static_cast<long>(mod / SECONDS_IN_MINUTE);
            }
        }
    }


    std::string TrendHelper::getSamplePeriodAsString()
    {
        // This method cannot be called until the sample period has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }

        if(m_samplePeriod == 0)
        {
            // Live Updates
            return "0";
        }

        // get a connection to the database
        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(Trend_Ad, Read);

      /*  std::ostringstream sql;
        sql << "select distinct INTERVAL from DR_TREND_CONFIG where INTERVAL_SECONDS = ";
        sql << m_samplePeriod;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), DR_TREND_CONFIG_Oracle_SELECT_39552,m_samplePeriod);
//		std::string strSql  = databaseConnection->prepareSQLStatement(DR_TREND_CONFIG_SELCT_39552,m_samplePeriod);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, DR_TREND_CONFIG_SELCT_39552,m_samplePeriod);
        
        std::vector<std::string> columnNames;
        columnNames.push_back("IntervalName");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified interval
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
			reasonMessage << "No entry found for interval = " << m_samplePeriod;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for interval = " << m_samplePeriod;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }
        
        return (data->getStringData(0, "IntervalName"));
    }


    void TrendHelper::setExtraInfo(const std::string& info)
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        
        m_extraInfo = info;

    }


    void TrendHelper::setDescription(const std::string& description)
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        
        m_description = description;

    }


    void TrendHelper::setGraphType(const std::string& graphType)
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        
        m_graphType = graphType;
    }
	

    void TrendHelper::setRangeSeconds(long rangeSeconds)
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        
        m_rangeSeconds = rangeSeconds;
    }

	
    void TrendHelper::setSamplePeriod(long sampleInSeconds)
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        
        m_samplePeriod = sampleInSeconds;
    }

	
    void TrendHelper::setStartDateTime(time_t startDateTime)
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        
        m_startDateTime = startDateTime;
    }

	
    void TrendHelper::setLive(bool live)
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        
        m_isLive = live;
    }

	
    void TrendHelper::setOffset(long offsetInSeconds)
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        
        m_offsetTime = offsetInSeconds;
    }

	
    void TrendHelper::setItems(const std::map<std::string, ItemInfo>& items)
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData & !m_isNew)
        {
            reload();
        }
        
        m_items.clear();
        m_items = items;
        //m_items.insert(m_items.end(), items.begin(), items.end());
    }
   

    void TrendHelper::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "TrendHelper::invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new Trend");

        m_isValidData = false;
        m_description = "";
        m_graphType = DEFAULT_GRAPH_TYPE;
        m_rangeSeconds = DEFAULT_TIME_COMPONENT;
        m_samplePeriod = 60;
        m_isLive = false;
        m_offsetTime = DEFAULT_TIME_COMPONENT;
        m_extraInfo = "";
        m_items.clear();
        m_name = m_originalName;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "TrendHelper::invalidate" );
    }


    void TrendHelper::writeData()
    {
        FUNCTION_ENTRY("writeData");

        // Write the Trend to the database

        // Need to check if this is a new trend or an existing trend - 
        // as a new trend will be inserted, while an existing trend will be updated
        if (m_isNew) // This is a new trend
        {
            addNewTrend();
        }
        else // This is an existing profile
        {
            modifyExistingTrend();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;
        FUNCTION_EXIT;
    }


    void TrendHelper::deleteTrend()
    {
        FUNCTION_ENTRY("deleteTrend");

        TA_ASSERT(!m_isNew, "This operator does not yet exist in the database, and therefore cannot be deleted");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Trend_Ad, Write);
        
       /* std::ostringstream sql;
        sql << "delete TREND_TEMPLATE where NAME = '" << m_originalName << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TREND_TEMPLATE_STD_DELETE_39401, m_originalName);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TREND_TEMPLATE_DELETE_39401, m_originalName);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TREND_TEMPLATE_DELETE_39401, m_originalName);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }



    void TrendHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        
		// Ensure the gui has already been written to the database. It doesn't make
        // any sense to reload an gui that doesn't exist in the database.
        TA_ASSERT( !m_isNew, "This trend does not yet exist in the database. Therefore it cannot be reloaded.");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Trend_Ad, Read);

       /* std::ostringstream sql;
        sql << "select NAME, DESCRIPTION, GRAPH_TYPE, TIME_RANGE, TO_CHAR(START_TIME, 'YYYYMMDDHH24MISS'), ";
        sql << "SAMPLE_PERIOD, LIVE, ITEMS, EXTRA_INFO, TO_CHAR(DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')";
        sql << "from TREND_TEMPLATE where NAME = '";
        sql << m_originalName;
        sql << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TREND_TEMPLATE_Oracle_SELECT_39051, m_originalName);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TREND_TEMPLATE_SELECT_39051, m_originalName);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TREND_TEMPLATE_SELECT_39051, m_originalName);

        std::vector<std::string> columnNames;
        columnNames.push_back("Name");
        columnNames.push_back("Description");
        columnNames.push_back("GraphType");
        columnNames.push_back("TimeRange");
        columnNames.push_back("StartTime");
        columnNames.push_back("SamplePeriod");
        columnNames.push_back("Live");
        columnNames.push_back("Items");
        columnNames.push_back("ExtraInfo");
        columnNames.push_back("DateModified");
        columnNames.push_back("DateCreated");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
			reasonMessage << "No trend found for name = " << m_originalName;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
			reasonMessage << "More than one trend found for name = " << m_originalName;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }
        
        m_description = data->getStringData(0, "Description");
        m_graphType = convertGraphTypeToString(data->getIntegerData(0, "GraphType"));
        m_rangeSeconds = data->getUnsignedLongData(0, "TimeRange");
        m_startDateTime = data->getDateData(0, "StartTime");
        m_samplePeriod = data->getUnsignedLongData(0, "SamplePeriod");
        m_isLive = data->getBooleanData(0, "Live");
        parseItems(data->getStringData(0, "Items"), m_items);
        m_extraInfo = data->getStringData(0, "ExtraInfo");
        m_dateCreated = data->getDateData( 0, "DateCreated" );
        m_dateModified = data->getDateData(0, "DateModified", 0);

        if(data != NULL)
        {
            delete data;
            data = NULL;
        }
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


	
    void TrendHelper::modifyExistingTrend()
    {
        FUNCTION_ENTRY("modifyExistingTrend");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "Trend %s already exists. It's data will be updated.",m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Trend_Ad, Write);

        // Create a select statement that will ensure the uniqueness of the entity name.
     /*   std::string formatSQL = "select NAME from TREND_TEMPLATE where NAME = '"
            + databaseConnection->escapeQueryString(m_originalName) + "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TREND_TEMPLATE_STD_SELECT_39001,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TREND_TEMPLATE_SELECT_39001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TREND_TEMPLATE_SELECT_39001,
			databaseConnection->escapeQueryString(m_originalName));

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string nameColumn = "NAME";
        columnNames.push_back(nameColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 rows returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_originalName != data->getStringData(0,nameColumn))
            {   // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "A trend already exists with name = " << m_originalName;
                throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name");
            }
            // else - it matches, so that's okay - it just means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one trend exists with name = " << m_originalName;
            throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name");
        }

       int live = 0;
        if(m_isLive)
        {
            live = 1;
        }

        struct tm *t;
	    
        char str[22] = {0};
        //if(ACE_OS::localtime_r(&m_startDateTime, &t) != NULL)
        t = localtime(&m_startDateTime);
	    //{
            //
		    // Ensure that seconds will be inserted into database as 0.
            //
		    sprintf(str, "%04d%02d%02d%02d%02d%02d", 
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	    //}

        //std::ostringstream sql;
        //sql << "update TREND_TEMPLATE set NAME = '";
        //sql << (databaseConnection->escapeInsertString(m_name)).c_str();
        //sql << "', DESCRIPTION = '" << m_description;
        //sql << "', GRAPH_TYPE = " << convertGraphTypeStringToNumber(m_graphType);
        //sql << ", TIME_RANGE = " << m_rangeSeconds;
        //sql << ", START_TIME = TO_DATE('" << str << "', 'YYYYMMDDHH24MISS'), ";
        //sql << "SAMPLE_PERIOD = " << m_samplePeriod;
        //sql << ", LIVE = " << live;
        //sql << ", ITEMS = '" << createItemString(m_items);
        //sql << "', EXTRA_INFO = '" << m_extraInfo;
        //sql << "' where NAME = '" << m_originalName << "'";
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TREND_TEMPLATE_Oracle_UPDATE_39201,
//		strSql  = databaseConnection->prepareSQLStatement(TREND_TEMPLATE_UPDATE_39201,

		databaseConnection->prepareSQLStatement(strSql, TREND_TEMPLATE_UPDATE_39201,
			databaseConnection->escapeInsertString(m_name), m_description, convertGraphTypeStringToNumber(m_graphType),
			m_rangeSeconds,str,m_samplePeriod, live, createItemString(m_items),m_extraInfo, m_originalName);

        databaseConnection->executeModification(strSql);

        //now need to update the date fields
      /*  sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from TREND_TEMPLATE where NAME = '" << m_name << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TREND_TEMPLATE_Oracle_SELECT_39052, m_name);
//		strSql  = databaseConnection->prepareSQLStatement(TREND_TEMPLATE_SELECT_39052, m_name);

		databaseConnection->prepareSQLStatement(strSql, TREND_TEMPLATE_SELECT_39052, m_name);

        // Can use the same columnNames vector from above.
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one entry found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and description") );
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

        m_originalName = m_name;

        FUNCTION_EXIT;
    }

        
    void TrendHelper::addNewTrend()
    {
        FUNCTION_ENTRY("addNewTrend");
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Trend %s is a new trend. It will be added to the database.", m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Trend_Ad, Write);

        // First check there is not already a trend with this name (it is a uniqueness constraint).

       /* std::ostringstream formatSQL;
        formatSQL << "select NAME from TREND_TEMPLATE where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TREND_TEMPLATE_STD_SELECT_39001, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(TREND_TEMPLATE_SELECT_39001, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TREND_TEMPLATE_SELECT_39001,
			databaseConnection->escapeQueryString(m_name));

        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Check the number of rows returned (the actual data is not important) - it
        // should be 0
        if (0 != data->getNumRows()) // Name already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "A trend already exists with name " << m_name;
            using TA_Base_Core::DataException;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name"));
        }

        // delte the IData pointer now that it's finised with
        delete data;
        data = NULL;

        int live = 0;
        if(m_isLive)
        {
            live = 1;
        }

        std::string itemString = createItemString(m_items);

        struct tm *t;

        char str[22] = {0};
        //if(ACE_OS::localtime_r(&m_startDateTime, &t) != NULL)
        t = localtime(&m_startDateTime);
	    //{
            //
		    // Ensure that seconds will be inserted into database as 0.
            //
		    sprintf(str, "%04d%02d%02d%02d%02d%02d", 
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	    //}

       /* std::ostringstream sql;
        sql << "insert into TREND_TEMPLATE (NAME, DESCRIPTION, GRAPH_TYPE, TIME_RANGE, START_TIME,";
        sql << " SAMPLE_PERIOD, LIVE, ITEMS, EXTRA_INFO) ";
        sql << "values ('" << (databaseConnection->escapeInsertString(m_name)).c_str();
        sql << "', '" << (databaseConnection->escapeInsertString(m_description)).c_str();
        sql << "', " << convertGraphTypeStringToNumber(m_graphType);
        sql << ", " << m_rangeSeconds << ", TO_DATE('" << str << "', 'YYYYMMDDHH24MISS'), ";
        sql << m_samplePeriod << ", " << live << ", '";
        sql << (databaseConnection->escapeInsertString(itemString)).c_str() << "', '";
        sql << (databaseConnection->escapeInsertString(m_extraInfo)).c_str() << "')";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TREND_TEMPLATE_Oracle_INSERT_39351,
//		strSql  = databaseConnection->prepareSQLStatement(TREND_TEMPLATE_INSERT_39351,

		databaseConnection->prepareSQLStatement(strSql, TREND_TEMPLATE_INSERT_39351,
			databaseConnection->escapeInsertString(m_name), databaseConnection->escapeInsertString(m_description),
			convertGraphTypeStringToNumber(m_graphType), m_rangeSeconds, str, m_samplePeriod, live, 
			databaseConnection->escapeInsertString(itemString), databaseConnection->escapeInsertString(m_extraInfo));
        
        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this location and update the date fields
       /* sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from TREND_TEMPLATE where NAME = '";
        sql << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TREND_TEMPLATE_Oracle_SELECT_39053,
//		strSql  = databaseConnection->prepareSQLStatement(TREND_TEMPLATE_SELECT_39053,

		databaseConnection->prepareSQLStatement(strSql, TREND_TEMPLATE_SELECT_39053,
			databaseConnection->escapeQueryString(m_name));

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage <<  "No data found for name = " << m_name.c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage <<  "More than one entry found for name = " << m_name.c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and description"));
        }

        // There should only be a single returned item (as any other 
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

        m_originalName = m_name;

        FUNCTION_EXIT;
    }

    
    //
    // parseItems
    //
    void TrendHelper::parseItems(const std::string& itemsString, std::map<std::string, ItemInfo>& items)
    {
        FUNCTION_ENTRY("parseItems");

        // Format:<item name>\<colour>\<index>,<item name>\<colour>\<index>, ...
        
        std::string::size_type beginPos = 0;
		std::string::size_type pos = itemsString.find("\\");
        
		while(pos != std::string::npos && pos != itemsString.length())
        {
            // Retrieve item name
            std::string::size_type len = (pos - beginPos);
            std::string itemName = itemsString.substr(beginPos, len);

            ItemInfo itemInfo;

            // Retrieve display name
            beginPos = pos + 1;
            pos = itemsString.find("\\", beginPos);
            len = (pos - beginPos);
            std::string displayName = itemsString.substr(beginPos, len);
            itemInfo.displayName = displayName;

            // Retrieve colour (hex digit)
            beginPos = pos + 1;
            pos = itemsString.find("\\", beginPos);
            len = (pos - beginPos);
            std::string colourStr = itemsString.substr(beginPos, len);
          
            
            // Convert colour to a COLORREF
            //itemInfo.colour = convertHexStringToColorRef(colourStr);
            itemInfo.colour = colourStr;
            
            // Retrieve index
            beginPos = pos + 1;
            pos = itemsString.find(",", beginPos);

			if(pos == std::string::npos)
            {
                pos = itemsString.length();
            }
            
            len = (pos - beginPos);
            itemInfo.index = atoi(itemsString.substr(beginPos, len).c_str());
            items[itemName] = itemInfo;

            if(pos != itemsString.length())
            {
                beginPos = pos + 1;
                pos = itemsString.find("\\", beginPos);
            }
        }
        
        FUNCTION_EXIT;
    }


    //
    // convertHexStringToColorRef
    //
    /*COLORREF TrendHelper::convertHexStringToColorRef(const std::string& hexString)
    {
        FUNCTION_ENTRY("convertHexStringToColorRef");

        std::string red = "0x" + hexString.substr(0, 2);
        std::string green = "0x" + hexString.substr(2, 2);
        std::string blue = "0x" + hexString.substr(4, 2);

        unsigned long uRed = strtoul(red.c_str(), NULL, 0);
        unsigned long uGreen = strtoul(green.c_str(), NULL, 0);
        unsigned long uBlue = strtoul(blue.c_str(), NULL, 0);
        
        COLORREF colour = RGB(uRed, uGreen, uBlue);

        FUNCTION_EXIT;
        return colour;
    }*/


    /*std::string TrendHelper::createHexStringFromColourRef(const COLORREF& colour)
    {
        FUNCTION_ENTRY("createHexStringFromColourRef");

        int rValue = colour.getRValue();
        int gValue = colour.getGValue();
        int bValue = colour.getBValue();

        // Convert to hexidecimal string
        char buffer[100];
        sprintf(buffer, "%x%x%x", rValue, gValue, bValue);

        FUNCTION_EXIT;
        return std::string(buffer);
    }*/

    //
    // parseTimeRange
    //
    void TrendHelper::parseTimeRange(unsigned long timeDate, int& days, time_t& time)
    {
        FUNCTION_ENTRY("parseTimeRange");

        const int SECONDS_IN_DAY = 24 * 60 * 60;

        days = timeDate / SECONDS_IN_DAY;
        time = timeDate - days * SECONDS_IN_DAY;

        FUNCTION_EXIT;
    }

   
    std::string TrendHelper::createItemString(const std::map<std::string, ItemInfo>& items)
    {
        //FUNCTION_ENTRY("createItemString");

        // Format:<item name>\<display name>\<colour>\<index>,<item name>\<display name>\<colour>\<index>, ...

        std::ostringstream str;
        str << "";

        std::map<std::string, ItemInfo>::const_iterator iter = items.begin();
        for(; iter != items.end(); ++iter)
        {
            if(iter->first == EMPTY_STRING)
            {
                continue;
            }

            if(iter != items.begin() && str.str() != "")
            {
                str << ",";
            }

            str << iter->first;
            str << "\\";
            str << iter->second.displayName;
            str << "\\";
            //str << createHexStringFromColourRef(iter->second.colour);
            str << iter->second.colour;
            str << "\\";
            str << iter->second.index;
        }

        //FUNCTION_EXIT;
        return str.str();
    }


    long TrendHelper::getTotalTimeRangeInSeconds()
    {
        //const int SECS_IN_DAY = 24 * 60 * 60;
        //return m_rangeDayComponent * SECS_IN_DAY + m_rangeTimeComponent;
        return m_rangeSeconds;
    }

    
    std::string TrendHelper::convertGraphTypeToString(int index)
    {
        switch(index)
        {
            case 0:  return LINE;
               
            case 1:  return BAR;

            case 2:  return COMPARISON;

            case 3:  return XY;

            default:
				TA_ASSERT(false, "Invalid graph type index");
				return "";
        }
    }


    int TrendHelper::convertGraphTypeStringToNumber(const std::string& graphTypeString)
    {
        for(int i = 0; i < NUM_GRAPH_TYPES; ++i)
        {
            if(GRAPH_TYPES[i] == graphTypeString)
            {
                return i;
            }
        }

        TA_ASSERT(false, "Invalid graph type string");
		return -1;
    }

} // closes TA_Base_Core

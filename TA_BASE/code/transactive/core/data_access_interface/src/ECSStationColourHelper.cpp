/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ECSStationColourHelper.cpp $
 * @author Andy Siow
 * @version $Revision: #2 $
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by: $Author: Ouyang $
 * 
 * Provides access to the Alarm Station data in the database.
 */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif // Win32


#include <string>
#include <time.h>

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/ECSStationColourHelper.h"
#include "core/data_access_interface/src/SQLCode.h"

namespace TA_Base_Core
{

    ECSStationColourHelper::ECSStationColourHelper(const unsigned long key)
		:
        m_name(""),
        m_key(key),
        m_dateCreated(0),
        m_dateModified(0), 
        m_isValidData(false)
    {
    }


    ECSStationColourHelper::ECSStationColourHelper(const unsigned long row, TA_Base_Core::IData& data)
    	:
        m_isValidData(false)
    {
        FUNCTION_ENTRY("ECSStationColourHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }


    ECSStationColourHelper::~ECSStationColourHelper()
    {
    }

    void ECSStationColourHelper::setName(const std::string name)
    {
        if ( ! m_isValidData )
        {
            reload();
        }
        m_name = name;
    }


    void ECSStationColourHelper::setStationColourKey(unsigned long colour, IECSStationColourData::EColourType type)
    {
        if ( ! m_isValidData )
        {
            reload();
        }

        std::map<IECSStationColourData::EColourType, unsigned long>::iterator matching;
        matching = m_ecsStationColourKey.find(type);
        if (matching != m_ecsStationColourKey.end())
        {
            matching->second = colour;
        }
        else
        {
            m_ecsStationColourKey.insert(std::map<IECSStationColourData::EColourType, unsigned long>::value_type(type, colour) );
        }
    }


    unsigned long ECSStationColourHelper::getKey()
    {
        return m_key;
    }


    std::string ECSStationColourHelper::getName()
    {
        if ( ! m_isValidData )
        {
            reload();
        }
        return m_name;
    }


    unsigned long ECSStationColourHelper::getStationColourKey(IECSStationColourData::EColourType type)
    {
        if ( ! m_isValidData )
        {
            reload();
        }

        std::map<IECSStationColourData::EColourType, unsigned long>::iterator matching;
        matching = m_ecsStationColourKey.find(type);
        if (matching != m_ecsStationColourKey.end())
        {
            return matching->second;
        }

        TA_THROW( DataException( "Alarm Station Colour type could not be found",DataException::NO_VALUE,"" ) );
    }

    time_t ECSStationColourHelper::getDateCreated()
    {
        if ( ! m_isValidData )
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t ECSStationColourHelper::getDateModified()
    {
        if( ! m_isValidData )
        {
            reload();
        }
        
        return m_dateModified;
    }


    void ECSStationColourHelper::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "ECSStationColourHelper::invalidate" );
		m_isValidData = false;
    }


	void ECSStationColourHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

		columnNames.push_back("ECSCOL_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("CURRENT_ECS_STATION_COLOUR");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
	}

	void ECSStationColourHelper::reload()
	{
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "ECSStationColourHelper::reload" );

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);

		// create the SQL string to retrieve the data of the Station
        /*std::ostringstream sql;
        sql << "select ECSCOL_ID, NAME, CURRENT_ECS_STATION_COLOUR,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
			<< " from EC_STATION_COLOUR where ECSCOL_ID = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_COLOUR_Oracle_SELECT_14053, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_STATION_COLOUR_SELECT_14053, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_COLOUR_SELECT_14053, m_key);

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
            char reasonMessage[256] = {0};
			sprintf( reasonMessage, "No ECS Station Colour found for pkey = %lu", m_key );
			TA_THROW( DataException( reasonMessage,DataException::NO_VALUE,"" ) );
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of ECS Station colour records."); 
		reloadUsing(0, *data);
		
        LOG ( SourceInfo, DebugUtil::FunctionExit, "ECS Station colour::reload" );
	}


    void ECSStationColourHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key  = data.getUnsignedLongData(row, columnNames[0]);
        m_name = data.getStringData(row, columnNames[1]);
        m_ecsStationColourKey.insert(std::map<IECSStationColourData::EColourType, unsigned long>::value_type(
                                     IECSStationColourData::CURRENT_PROPOSED_CF, data.getUnsignedLongData(row, columnNames[2])) );
		m_dateModified = data.getDateData(row, columnNames[3], 0);
        m_dateCreated = data.getDateData(row, columnNames[4]);

        // Need to record that we now have valid data
        m_isValidData = true;
        
        FUNCTION_EXIT;
    }


    void ECSStationColourHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        std::vector<std::string> fieldNames;

        // Check the name field isn't blank
        if(m_name == "")
        {
            fieldNames.push_back("name");
        }

        // If there are any fields missing
        if( !fieldNames.empty() )
        {
            // Throw an exception
            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "ECS Station colour data not fully specified. "
                        "ECS Station colour cannot be written to database",
                        fieldNames));
        }
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);


        // Create a select statement that will ensure the uniqueness of the entity name.
        //std::string formatSQL = "select PKEY from ECS_STATION_COLOUR where NAME = '"
        //    + databaseConnection->escapeQueryString(m_name) + "'";
        /*std::ostringstream formatSQL;
        formatSQL << "select ECSCOL_ID from EC_STATION_COLOUR where NAME = '"
				  << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_COLOUR_STD_SELECT_14001,
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_STATION_COLOUR_SELECT_14001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_COLOUR_SELECT_14001,
			databaseConnection->escapeQueryString(m_name) );

        // Set up the vector to pass to executeQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "KEY";
        columnNames.push_back(keyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 row returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0,keyColumn))
            {
                // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "An ecs station colour already exists with name = "
                              << m_name.c_str();
                TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
            }
            // else - it matches, so that's okay - it means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one ecs station colour already exists with name = "
                          << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        delete data;
        data =  NULL;

		// create the SQL string to write the data of the Station
        /*std::ostringstream sql;
        sql << " update EC_STATION_COLOUR"
            << " set NAME = '" << databaseConnection->escapeInsertString(m_name) << "',"
            << " CURRENT_ECS_STATION_COLOUR = " << m_ecsStationColourKey[IECSStationColourData::CURRENT_PROPOSED_CF]
            << " where ECSCOL_ID = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_COLOUR_STD_UPDATE_14151,
//		strSql  = databaseConnection->prepareSQLStatement(EC_STATION_COLOUR_UPDATE_14151,

		databaseConnection->prepareSQLStatement(strSql, EC_STATION_COLOUR_UPDATE_14151,
			databaseConnection->escapeInsertString(m_name), m_ecsStationColourKey[IECSStationColourData::CURRENT_PROPOSED_CF], m_key);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        databaseConnection->executeModification(strSql);

        
        //now need to update the date fields
        /*sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from EC_STATION_COLOUR where ECSCOL_ID = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_COLOUR_Oracle_SELECT_14054, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(EC_STATION_COLOUR_SELECT_14054, m_key);

		databaseConnection->prepareSQLStatement(strSql, EC_STATION_COLOUR_SELECT_14054, m_key);

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

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
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
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
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



        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


} //close namespace TA_Base_Core

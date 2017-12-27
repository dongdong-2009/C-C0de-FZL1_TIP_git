/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/LocationAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * LocationAccessFactory is a singleton that is used to retrieve Location objects either from the
  * database or newly created. All Location objects returned will adhear to the ILocation interface.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/ConfigLocation.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    // These need to be set to be the same as the names for the
    // OCC and depot in the database.
    const std::string LocationAccessFactory::OCC_LOCATION_NAME       = "OCC";
	const std::string LocationAccessFactory::OCC_COM_LOCATION_NAME   = "OCC_COM";
	const std::string LocationAccessFactory::OCC_INT_LOCATION_NAME   = "OCC_INT";
	const std::string LocationAccessFactory::OCC_SCADA_LOCATION_NAME = "OCC_SCADA";

	const unsigned int ALL_LOCATION = 0;

    // Jade: 12 Mar 04 - The depot is not DPT, but KCD (Defect #1951)
    const std::string LocationAccessFactory::DEPOT_LOCATION_NAME = "GBD"; //GBD is the depot in 955
	//TODO : Need to consider not hardcoding this. Find a way to group both occ and depot locations
	//hongran++ TD 15473
	const std::string LocationAccessFactory::DEPOT_ND_LOCATION_NAME = "ND";
	const std::string LocationAccessFactory::DEPOT_SD_LOCATION_NAME = "SD";
	const std::string LocationAccessFactory::DEPOT_MD_LOCATION_NAME = "MD";
	//++hongran TD 15473

	//955 depots
	const std::string LocationAccessFactory::DEPOT_GBD_LOCATION_NAME = "GBD";
	const std::string LocationAccessFactory::DEPOT_GDCC_LOCATION_NAME="GDCC";
	
	LocationAccessFactory* LocationAccessFactory::m_instance = 0;

    LocationAccessFactory& LocationAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new LocationAccessFactory();
        }
        return *m_instance;
    }


    void LocationAccessFactory::getLocationBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector< ILocation* >& locations, const bool readWrite)
    {
        // get a connection to the database
     //   IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
		TA_ASSERT(0 != pDatabase, "getLocationBySql: the database connection is null");
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("ORDER_ID");
		columnNames.push_back("DISPLAY_NAME");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
		columnNames.push_back("IS_DISPLAY_ONLY");
        columnNames.push_back("TYPE_NAME");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql,columnNames);
		
        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    if (readWrite)
                    {
                        if (0 == data->getUnsignedLongData( i, columnNames[0] ))
                        {
                            // if we have the location where the key is 0 then we are not interested in it
                            // The user cannot configure this particular location
                            continue;
                        }

                        locations.push_back( new ConfigLocation(i, *data));
                    }
                    else
                    {
						locations.push_back( new Location(i, *data));
                    }

                }
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
    }


    std::vector<ILocation*> LocationAccessFactory::getAllLocations(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllLocations");

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		// create the SQL string to retrieve all the locations of the alarm
        /*std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, ORDER_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'),IS_DISPLAY_ONLY, TYPE_NAME FROM LOCATION order by ORDER_ID asc";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_Oracle_SELECT_7051);
//		std::string sql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7051);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, LOCATION_SELECT_7051);
//		TA_ASSERT(sql.size() > 0, "getAllLocations: Get SQL statement error");

		std::vector< ILocation* > locations;
		getLocationBySql( databaseConnection, sql, locations, readWrite);

        FUNCTION_EXIT;
		return locations;
    }

	//xinsong++, exclude location=0
    std::vector<ILocation*> LocationAccessFactory::getAllAgentLocations(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllLocations");

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
		
		// create the SQL string to retrieve all the locations of the alarm
        /*std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, ORDER_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'),IS_DISPLAY_ONLY, TYPE_NAME FROM LOCATION where PKEY <> " << ALL_LOCATION
			<< " order by ORDER_ID asc";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			LOCATION_Oracle_SELECT_7052, ALL_LOCATION);
//		std::string sql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7052, ALL_LOCATION);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, LOCATION_SELECT_7052, ALL_LOCATION);
//		TA_ASSERT(sql.size() > 0, "getAllAgentLocations: Get SQL Statement error");
		
		std::vector< ILocation* > locations;
		getLocationBySql(databaseConnection, sql, locations, readWrite);
		
        FUNCTION_EXIT;
		return locations;
    }


	
	ILocation* LocationAccessFactory::getLocationByKey( const unsigned long locationKey, const bool readWrite )
	{
		FUNCTION_ENTRY("getLocationByKey");

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		// create the SQL string to retrieve all the locations of the alarm
        /*std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, ORDER_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')),IS_DISPLAY_ONLY, TYPE_NAME FROM LOCATION WHERE PKEY = " << locationKey;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			LOCATION_Oracle_SELECT_7053, locationKey);
//		std::string sql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7053, locationKey);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, LOCATION_SELECT_7053, locationKey);
//		TA_ASSERT(sql.size() > 0, "getLocationByKey: Get SQL Statement error");

		std::vector< ILocation* > locations;
		getLocationBySql(databaseConnection, sql, locations, readWrite);

        if ( 0 == locations.size())
        {
            std::ostringstream message;
			message << "No data found for Location with key " << locationKey;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );            

			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
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

		TA_ASSERT(1 == locations.size(), "Location key unique constraint violated");

        FUNCTION_EXIT;
		return locations[0];
	}


    ILocation* LocationAccessFactory::getLocationByName( const std::string& locationName, const bool readWrite )
	{
		FUNCTION_ENTRY("getLocationByName");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		// create the SQL string to retrieve all the locations of the alarm
        /*std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, ORDER_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'),IS_DISPLAY_ONLY, TYPE_NAME FROM LOCATION WHERE NAME = '"
			<< databaseConnection->escapeQueryString(locationName) << "'";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			LOCATION_Oracle_SELECT_7054, databaseConnection->escapeQueryString(locationName));
//		std::string sql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7054, databaseConnection->escapeQueryString(locationName));
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, LOCATION_SELECT_7054, databaseConnection->escapeQueryString(locationName));
//		TA_ASSERT(sql.size() > 0, "getLocationByName: Get SQL Statement error");

		std::vector< ILocation* > locations;
		getLocationBySql(databaseConnection, sql, locations, readWrite);

        if ( 0 == locations.size())
        {
            std::ostringstream message;
			message << "No data found for Location with name " << locationName;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
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

		TA_ASSERT(1 == locations.size(), "Location key unique constraint violated");

        FUNCTION_EXIT;
		return locations[0];
	}

    std::vector<ILocation*> LocationAccessFactory::getLocationsByProfile(unsigned long profileKey, const bool readWrite)
    {
        FUNCTION_ENTRY("getLocationsByProfile");

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		// create the SQL string to retrieve all the locations of the alarm
        /*std::ostringstream sql;
        sql << " select a.PKEY, a.NAME, a.DESCRIPTION, a.ORDER_ID,"
			<< " TO_CHAR(a.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(a.DATE_CREATED,'YYYYMMDDHH24MISS'),a.IS_DISPLAY_ONLY, TYPE_NAME FROM LOCATION a, SE_PROFILE_LOCATION b"
			<< " where a.PKEY = b.LOCATIONKEY and b.SEPROF_ID = " << profileKey
			<< " order by a.ORDER_ID asc";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			LOCATION_Oracle_SELECT_7055, profileKey);
//		std::string sql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7055, profileKey);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, LOCATION_SELECT_7055, profileKey);
//		TA_ASSERT(sql.size() > 0, "getLocationsByProfile: Get SQL Statement error");

		std::vector< ILocation* > locations;
		getLocationBySql(databaseConnection, sql, locations, readWrite);

        // Return the vector of locations
        FUNCTION_EXIT;
        return locations;
    }

#ifdef PROJECT_4669

    ILocation::ELocationType LocationAccessFactory::getLocationType( const std::string& locationName) const
    {
        try
        {
            std::auto_ptr<ILocation> location( const_cast<LocationAccessFactory*>(this)->getLocationByName( locationName ) );
            return Location::convertType( location->getTypeName() );
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "unknown location type name for: %s", locationName.c_str() );
        }

        return convertLocationName2Type( locationName );
    }


    ILocation::ELocationType LocationAccessFactory::getLocationType(const unsigned long locationKey) const
    {
        std::auto_ptr<ILocation> location( const_cast<LocationAccessFactory*>(this)->getLocationByKey( locationKey ) );

        try
        {
            return Location::convertType( location->getTypeName() );
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "unknown location type name for: %d", locationKey );
        }

        return convertLocationName2Type( location->getName() );
    }


    unsigned long LocationAccessFactory::getOccLocationKey()
    {
        return getLocationKeyFromName(OCC_LOCATION_NAME);
    }

    unsigned long LocationAccessFactory::getOccComLocationKey()
    {
        return getLocationKeyFromName(OCC_COM_LOCATION_NAME);
    }

    unsigned long LocationAccessFactory::getOccIntLocationKey()
    {
        return getLocationKeyFromName(OCC_INT_LOCATION_NAME);
    }

    unsigned long LocationAccessFactory::getOccScadaLocationKey()
    {
        return getLocationKeyFromName(OCC_SCADA_LOCATION_NAME);
    }


    unsigned long LocationAccessFactory::getDepotLocationKey()
    {
        return getLocationKeyFromName(DEPOT_LOCATION_NAME);
    }

	unsigned long LocationAccessFactory::getDepotGBDLocationKey()
	{
		return getLocationKeyFromName(DEPOT_GBD_LOCATION_NAME);
	}

	unsigned long LocationAccessFactory::getDepotGDCCLocationKey()
	{
		return getLocationKeyFromName(DEPOT_GDCC_LOCATION_NAME);
	}

	//hongran++ TD 15473
	unsigned long LocationAccessFactory::getDepotSDLocationKey()
    {
        return getLocationKeyFromName(DEPOT_SD_LOCATION_NAME);
    }

	unsigned long LocationAccessFactory::getDepotNDLocationKey()
    {
        return getLocationKeyFromName(DEPOT_ND_LOCATION_NAME);
    }

	unsigned long LocationAccessFactory::getDepotMDLocationKey()
    {
        return getLocationKeyFromName(DEPOT_MD_LOCATION_NAME);
    }
	//++hongran TD 15473

	std::string LocationAccessFactory::getOccComLocationName()
	{
		return OCC_COM_LOCATION_NAME;
	}

#endif // #ifdef PROJECT_4669


    unsigned long LocationAccessFactory::getLocationKeyFromName(
        const std::string& name)
    {
        ILocation* location = getLocationByName(name);

        unsigned long locationKey = 0;
        try
        {
            locationKey = location->getKey();
			delete location;
			location = NULL;
        }
        catch(...)
        {
            delete location;
            location = NULL;
            throw;
        }

        return locationKey;
    }

	std::vector<unsigned long> LocationAccessFactory::getOrderIdsFromRegionKeys(std::vector<unsigned long> theRegionKeys,
		std::map<unsigned long, std::string>& theOrderIdDisplayNameMap)
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
			"RegionDutyPropertyPage:: getOrderIdFromRegionKey(), now begin.");

		std::vector<unsigned long> theOrderIds;
		theOrderIds.reserve(theRegionKeys.size());

		unsigned long orderId;
		std::string regionName;

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		std::vector<std::string> columnNames;
		columnNames.push_back("PKEY");
		columnNames.push_back("NAME");
		columnNames.push_back("DESCRIPTION");
		columnNames.push_back("ORDER_ID");
		columnNames.push_back("DISPLAY_NAME");
		columnNames.push_back("DATE_MODIFIED");
		columnNames.push_back("DATE_CREATED");
		columnNames.push_back("IS_DISPLAY_ONLY");
        columnNames.push_back("TYPE_NAME");

		IData* data = NULL;
		SQLStatement sql;

		int iLog = 0;

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
			"RegionDutyPropertyPage:: getOrderIdFromRegionKey(), before for - loop.");

		for (std::vector<unsigned long>::const_iterator regionIt = theRegionKeys.begin(); regionIt != theRegionKeys.end(); regionIt++)
		{
			databaseConnection->prepareSQLStatement(sql, LOCATION_SELECT_7053, *regionIt);

			try
			{
				data = databaseConnection->executeQuery( sql, columnNames );

				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
					"RegionDutyPropertyPage:: getOrderIdFromRegionKey(), the %d time in the for loop, data->getNumRows()=%ld",iLog+1,data->getNumRows());

				orderId = data->getUnsignedLongData(0,"ORDER_ID");
				regionName = data->getStringData(0,"DISPLAY_NAME");
			}
			catch( const TA_Base_Core::DatabaseException& )
			{
				delete data;
				data = NULL;
				LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "RegionDutyPropertyPage:: getOrderIdFromRegionKey(), throw a DatabaseException during execute selectSQL.");
			}
			catch( const TA_Base_Core::DataException& )
			{
				delete data;
				data = NULL;
				LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "RegionDutyPropertyPage:: getOrderIdFromRegionKey(), throw a DataException during execute selectSQL.");
			}

			if ( data != NULL)
			{
				theOrderIds.push_back(orderId);

				//m_OrderIdRegionKeyMap.insert(std::map<unsigned long, unsigned long>::value_type (orderId,*regionIt));
				theOrderIdDisplayNameMap.insert(std::map<unsigned long, std::string>::value_type (orderId,regionName));

				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
					"RegionDutyPropertyPage:: getOrderIdFromRegionKey(), the %d time in the for loop, the PKEY of regionIt=%ld, theOrderId=%ld.",iLog+1,*regionIt,orderId);

				delete data;
				data = NULL;
			}

			iLog++;
		}

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
			"RegionDutyPropertyPage:: getOrderIdFromRegionKey(), now end.");

		return theOrderIds;
	}

	std::map<std::string, unsigned char> LocationAccessFactory::getTIMSOriginValues()
	{
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		// create the SQL string to retrieve all the locations of the alarm
        /*std::ostringstream sql;
        sql << "SELECT pkey, 40 + dense_rank() OVER (ORDER BY pkey ASC) -1 AS origin, name "
			<< " FROM LOCATION WHERE pkey <> 0 AND name NOT IN ('OCC', 'TDS', 'KCD') "
			<< " UNION SELECT pkey, 10, name FROM LOCATION WHERE name = 'OCC' "
			<< " UNION SELECT pkey, 31, name FROM LOCATION WHERE name = 'KCD' "
			<< " UNION SELECT pkey, 35, name FROM LOCATION WHERE name = 'TDS' "
			<< std::ends;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_Oracle_SELECT_7056);
//		std::string sql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7056);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, LOCATION_SELECT_7056);
//		TA_ASSERT(sql.size() > 0, "getTIMSOriginValues: Get SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn( "PKEY" );
		std::string originColumn( "ORIGIN" );
		std::string nameColumn( "NAME" );
		std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        columnNames.push_back(originColumn);
        columnNames.push_back(nameColumn);

		std::map<std::string, unsigned char> originValues;

		IData* data = NULL;
		try
		{
			data = databaseConnection->executeQuery(sql,columnNames);
		}
		catch( ... )
		{
			delete data;
			data = NULL;
			throw;
		}

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified alarm_id
        {
			delete data;
            return originValues;
        }

        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                try
                {
					unsigned char origin = static_cast<unsigned char>(data->getUnsignedLongData(i, originColumn));
					std::string name = data->getStringData(i, nameColumn);

                    originValues.insert( std::make_pair(name, origin) );
                }
                catch (...)
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

		return originValues;

	}

    IConfigLocation* LocationAccessFactory::createLocation()
    {
        return new ConfigLocation();
    }


    IConfigLocation* LocationAccessFactory::copyLocation(const IConfigLocation* locationToCopy)
    {
        FUNCTION_ENTRY("copyLocation");

        TA_ASSERT(locationToCopy !=  NULL, "The location to copy was NULL");
        
        const ConfigLocation* cast = dynamic_cast<const ConfigLocation*>(locationToCopy);

        TA_ASSERT(cast != NULL, "Location passed could not be converted into a ConfigLocation");

        FUNCTION_EXIT;
        return new ConfigLocation(*cast);
    }


    std::vector<ILocation*> LocationAccessFactory::getLocationsByType( ILocation::ELocationType locationType, const bool readWrite )
    {
		FUNCTION_ENTRY("getLocationsByType");

        std::vector< ILocation* > locations;

        std::vector< ILocation* > allLocations = getAllLocations( readWrite );

        for ( size_t i = 0; i < allLocations.size(); ++i )
        {
            ILocation* location = allLocations[i];

            try
            {
                if ( location->getLocationType() == locationType )
                {
                    locations.push_back( location );
                    continue;
                }
            }
            catch (...)
            {
            }

            delete location;
            location = NULL;
        }

        FUNCTION_EXIT;
		return locations;
    }


    ILocation::ELocationType LocationAccessFactory::convertLocationName2Type(const std::string& locationName) const
    {
        if (0 == locationName.compare(OCC_LOCATION_NAME))
        {
            return ILocation::OCC;
        }
        else if (0 == locationName.compare(OCC_COM_LOCATION_NAME))
        {
            return ILocation::OCC;
        }
        else if (0 == locationName.compare(OCC_INT_LOCATION_NAME))
        {
            return ILocation::OCC;
        }
        else if (0 == locationName.compare(OCC_SCADA_LOCATION_NAME))
        {
            return ILocation::OCC;
        }
        //hongran++ TD 15473
        else if (0 == locationName.compare(DEPOT_SD_LOCATION_NAME))
        {
            return ILocation::DPT;
        }
        else if (0 == locationName.compare(DEPOT_ND_LOCATION_NAME))
        {
            return ILocation::DPT;
        }
        else if (0 == locationName.compare(DEPOT_MD_LOCATION_NAME))
        {
            return ILocation::DPT;
        }else if(0 == locationName.compare(DEPOT_GBD_LOCATION_NAME))
        {
            return ILocation::DPT;
        }
        else if(0 == locationName.compare(DEPOT_GDCC_LOCATION_NAME))
        {
            return ILocation::DPT;
        }
        //++hongran TD 15473
        else if (0 == locationName.compare(DEPOT_LOCATION_NAME))
        {
            return ILocation::DPT;
        }
        else
        {
            return ILocation::STATION;
        }
    }

} // closes TA_Base_Core

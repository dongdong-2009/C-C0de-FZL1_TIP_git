/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/tis_agent/src/PidStationIdAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#pragma warning(disable:4786)


#include "core/data_access_interface/tis_agent/src/PidStationIdAccessFactory.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    static const std::string STATION_ID_TABLE = "TI_STATION_ID";
    static const std::string PKEY_COL = "TISTID_ID";
    static const std::string PKEY_SEQ = "TISTID_SEQ.NEXTVAL";
    static const std::string STATION_COL = "STATION_ID";
    static const std::string TA_LOCATION_COL = "TA_LOCATION";
    static const std::string DATECREATED_COL = "DATE_CREATED";
    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
	static const std::string LOCATION_NAME_COL = "NAME";

    PidStationIdAccessFactory* PidStationIdAccessFactory::m_instance = 0;

    PidStationIdAccessFactory& PidStationIdAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PidStationIdAccessFactory();
        }
        return *m_instance;
    }


    void PidStationIdAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    
	PidStationIdAccessFactory::LocationNameToStationIdMap PidStationIdAccessFactory::getLocationNameToStationIdMap()
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TI_STATION_ID_SELECT_95011);

       /* std::ostringstream sql;
		sql << "select "
            << PKEY_COL << ", "
            << STATION_COL << ", "
            << TA_LOCATION_COL << ", "
            << "TO_CHAR(" << DATECREATED_COL << ",'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(nvl(" << DATEMODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
            << " from " << STATION_ID_TABLE;*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(LOCATION_NAME_COL);
        columnNames.push_back(STATION_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);
        
        // create the vector of pointers to IEntityData
        LocationNameToStationIdMap newMap;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                try
                {
					std::string locationName = data->getStringData( i, LOCATION_NAME_COL );
					unsigned long stationId = data->getUnsignedLongData(i,STATION_COL);
					newMap.insert( std::make_pair( locationName, stationId ) );
                }
                catch (TA_Base_Core::DataException& de)
                {
                    // guard against memory leaks
                    // and prevent one load failure from stopping all items loading
                    LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", de.what() );
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return newMap;
    }
    
} // closes TA_IRS_Core



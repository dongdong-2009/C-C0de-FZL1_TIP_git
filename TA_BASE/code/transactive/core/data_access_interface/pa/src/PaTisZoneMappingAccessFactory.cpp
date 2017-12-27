/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTisZoneMappingAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaTisZoneMappingAccessFactory is a singleton that is used to retrieve PaDvaMessage objects either from the
  * database or newly created. All PaDvaMessage objects returned will adhere to the IPaDvaMessage interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/pa/src/PaTisZoneMappingAccessFactory.h"


#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/pa/src/PaDvaMessage.h"
#include "core/data_access_interface/pa/src/ConfigPaDvaMessage.h"
#include "core/data_access_interface/pa/src/PaTisZoneMap.h"
#include "core/data_access_interface/pa/src/ConfigPaTisZoneMap.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    PaTisZoneMappingAccessFactory* PaTisZoneMappingAccessFactory::m_instance = 0;

	static const std::string PA_TIS_ZONE_MAP_TABLE_NAME		= "PA_ZONE_HELPER";
    static const std::string PA_ZONE_TABLE_NAME	= "PA_ZONE";

	static const std::string PA_ZONE_TABLE_KEY_COL				= "PA_ZONE.PAZONE_ID";
	static const std::string PA_ZONE_TABLE_LOCATION_KEY_COL		= "PA_ZONE.LOCATIONKEY";

	static const std::string MAP_TABLE_KEY_COL					= "PA_ZONE_HELPER.PAZHEL_ID";
	static const std::string MAP_TABLE_PA_ZONE_KEY_COL			= "PA_ZONE_HELPER.PAZONE_ID";
	static const std::string MAP_TABLE_TIS_ZONE_ENTITY_COL		= "PA_ZONE_HELPER.PID_ZONE_EQUIPMENT";

    static const std::string PA_TIS_ZONE_MAPPING_SEQUENCE_COUNTER	= "PAZHEL_SEQ";

    static const std::string MAP_TABLE_DATEMODIFIED_COL		= "PA_ZONE_HELPER.DATE_MODIFIED";
    static const std::string MAP_TABLE_DATECREATED_COL		= "PA_ZONE_HELPER.DATE_CREATED";


    PaTisZoneMappingAccessFactory& PaTisZoneMappingAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaTisZoneMappingAccessFactory();
        }
        return *m_instance;
    }


    void PaTisZoneMappingAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaTisZoneMaps PaTisZoneMappingAccessFactory::getAllPaTisZoneMappings(const bool readWrite )
    {
        return getPaTisZoneMappingsReturnedFromStatement("", readWrite);
    }


    IPaTisZoneMaps PaTisZoneMappingAccessFactory::getPaTisZoneMappingByLocationKey(unsigned long locationKey, const bool readWrite )
    {
        std::ostringstream sql;
        sql << MAP_TABLE_PA_ZONE_KEY_COL << " in (select  " 
            << PA_ZONE_TABLE_KEY_COL << " from " << PA_ZONE_TABLE_NAME
            << " where " << PA_ZONE_TABLE_LOCATION_KEY_COL << " = " << locationKey << ")";

        return getPaTisZoneMappingsReturnedFromStatement(sql.str(), readWrite);   
    }


    IPaTisZoneMaps PaTisZoneMappingAccessFactory::getPaTisZoneMappingsReturnedFromStatement( const std::string& whereClause,  const bool readWrite)
    {
		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
		
        // create the SQL string to retrieve the data from the pa_station_dva_message and the
        // pa_tis_message_map tables based upon the primary key of a PaDvaMessage record        
        std::ostringstream sql;        
      /*  sql << "select " 
			<< PA_ZONE_TABLE_KEY_COL << ", "
			<< PA_ZONE_TABLE_LOCATION_KEY_COL << ", " 
			<< DVA_MSG_TABLE_ID_COL << ", "
			<< DVA_MSG_TABLE_LABEL_COL << ", "
			<< DVA_MSG_TABLE_TYPE_COL << ", "
            << "TO_CHAR(" << DVA_MSG_TABLE_DATECREATED_COL << ", 'YYYYMMDDHH24MISS'), " 
            << "TO_CHAR(" << DVA_MSG_TABLE_DATEMODIFIED_COL << ", 'YYYYMMDDHH24MISS')"
            << " from "  
            << PA_ZONE_TABLE_NAME; */
		if( !whereClause.empty() )
		{
			sql << " where " << whereClause;
		}

		//sql << std::endl;
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_TIS_MESSAGE_MAP_Oracle_SELECT_41053, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_SELECT_44554, sql.str());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_SELECT_44554, sql.str());
        // Set up the columnNames vector to be passed to executeQuery()       
        std::vector<std::string> columnNames;
        columnNames.push_back(MAP_TABLE_KEY_COL);
        columnNames.push_back(MAP_TABLE_TIS_ZONE_ENTITY_COL);
        columnNames.push_back(MAP_TABLE_PA_ZONE_KEY_COL);
        columnNames.push_back(MAP_TABLE_DATECREATED_COL);
        columnNames.push_back(MAP_TABLE_DATEMODIFIED_COL);
		
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
		
		// create the vector of pointers to ITrackElement
        IPaTisZoneMaps returnValue;

        do
        {
            unsigned long rowCount = data->getNumRows();
			
            for (unsigned long i = 0; i < rowCount; ++i)
            {
                if (readWrite == true)
                {
                    returnValue.push_back(new ConfigPaTisZoneMap(i, *data));
                }
                else
                {
                    returnValue.push_back(new PaTisZoneMap(i, *data));
                }
            }

            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data) == true);


		return returnValue;
		


//        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
//                    <IPaDvaMessages, ConfigPaDvaMessage, PaDvaMessage>
//                    (PaDvaMessageHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaTisZoneMap* PaTisZoneMappingAccessFactory::createPaTisZoneMapping()
    {
		//TA_ASSERT(false, "this function should be call when use distribute DB");
        return new ConfigPaTisZoneMap();    
    }


    IConfigPaTisZoneMap* PaTisZoneMappingAccessFactory::copyPaTisZoneMapping(const IConfigPaTisZoneMap* paDvaMessageToCopy)
    {
		//TA_ASSERT(false, "this function should be call when use distribute DB");
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "The copyPaTisZoneMapping function should not be called.");

//        FUNCTION_ENTRY("copyPaDvaMessage");
//
//        TA_ASSERT(paDvaMessageToCopy !=  NULL, "The PaDvaMessage to copy was NULL");
//        
//        const ConfigPaDvaMessage* cast = dynamic_cast<const ConfigPaDvaMessage*>(paDvaMessageToCopy);
//
//        TA_ASSERT(cast != NULL, "PaDvaMessage passed could not be converted into a ConfigPaDvaMessage");
//
//        FUNCTION_EXIT;
//        return new ConfigPaDvaMessage(*cast);
		return new ConfigPaTisZoneMap();

    }

    TA_Base_Core::IPaTisZoneMaps PaTisZoneMappingAccessFactory::getPaTisZoneMappingByPaZoneKey( unsigned long zoneKey, const bool readWrite /*= false*/ )
    {
        std::ostringstream sql;
        sql << MAP_TABLE_PA_ZONE_KEY_COL << " = " 
            << zoneKey;

        return getPaTisZoneMappingsReturnedFromStatement(sql.str(), readWrite);   
    }


} // closes TA_Base_Core



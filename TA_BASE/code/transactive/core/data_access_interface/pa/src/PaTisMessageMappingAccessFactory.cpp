/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTisMessageMappingAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaTisMessageMappingAccessFactory is a singleton that is used to retrieve PaDvaMessage objects either from the
  * database or newly created. All PaDvaMessage objects returned will adhere to the IPaDvaMessage interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/pa/src/PaTisMessageMappingAccessFactory.h"


#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/pa/src/PaDvaMessage.h"
#include "core/data_access_interface/pa/src/ConfigPaDvaMessage.h"
#include "core/data_access_interface/pa/src/PaTisMessageMap.h"
#include "core/data_access_interface/pa/src/ConfigPaTisMessageMap.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    PaTisMessageMappingAccessFactory* PaTisMessageMappingAccessFactory::m_instance = 0;

	static const std::string PA_TIS_MESSAGE_MAP_TABLE_NAME		= "PA_TIS_MESSAGE_MAP";
    static const std::string PA_STATION_DVA_MESSAGE_TABLE_NAME	= "PA_DVA_MESSAGE";

	static const std::string DVA_MSG_TABLE_KEY_COL				= "PA_DVA_MESSAGE.PADMES_ID";
	static const std::string DVA_MSG_TABLE_LOCATION_KEY_COL		= "PA_DVA_MESSAGE.LOCATIONKEY";

	static const std::string MAP_TABLE_KEY_COL					= "PA_TIS_MESSAGE_MAP.PKEY";
	static const std::string MAP_TABLE_ID_COL					= "PA_TIS_MESSAGE_MAP.PADMES_ID";
	static const std::string MAP_TABLE_TISMESSAGETAG_COL		= "PA_TIS_MESSAGE_MAP.TIS_MESSAGE_TAG";
	static const std::string MAP_TABLE_TISLIBRARYSECTION_COL	= "PA_TIS_MESSAGE_MAP.TIS_LIBRARY_SECTION";
	static const std::string MAP_TABLE_TISLIBRARYVERSION_COL	= "PA_TIS_MESSAGE_MAP.TIS_LIBRARY_VERSION";

    static const std::string PA_TIS_MESSAGE_MAPPING_SEQUENCE_COUNTER	= "PATMMAP_SEQ";

    static const std::string MAP_TABLE_DATEMODIFIED_COL		= "PA_TIS_MESSAGE_MAP.DATE_MODIFIED";
    static const std::string MAP_TABLE_DATECREATED_COL		= "PA_TIS_MESSAGE_MAP.DATE_CREATED";

    static const std::string TIS_TABLE_NAME = "TI_PREDEFINED_MESSAGE";
    static const std::string TIS_KEY_COL_NAME = "TIPMES_ID";
    static const std::string TIS_MESSAGE_TAG_COL = "MESSAGE_TAG";
    static const std::string TIS_LIBRARY_SECTION_COL = "LIBRARY_SECTION";
    static const std::string TIS_DESCRIPTION_COL = "DESCRIPTION";

    static const std::string TIS_LIBRARY_VER_KEY_COL = "TIPMLI_ID";
    static const std::string TIS_LIBRARY_VER_TABLE = "TI_PREDEFINED_MESSAGE_LIBRARY";
    static const std::string TIS_LIBRARY_VERSION_COL = "VERSION";

    TisMessage::TisMessage()
    :
    pKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
    tisMessageTag(TA_Base_Core::DatabaseKey::getInvalidKey()),
    tisLibrarySection(TA_Base_Core::DatabaseKey::getInvalidKey()),
    tisLibraryVersion(TA_Base_Core::DatabaseKey::getInvalidKey())
    {
    }

    PaTisMessageMappingAccessFactory& PaTisMessageMappingAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaTisMessageMappingAccessFactory();
        }
        return *m_instance;
    }


    void PaTisMessageMappingAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaTisMessageMaps PaTisMessageMappingAccessFactory::getAllPaTisMessageMappings(const bool readWrite )
    {
        return getPaTisMessageMappingsReturnedFromStatement("", readWrite);
    }


    IPaTisMessageMaps PaTisMessageMappingAccessFactory::getPaTisMessageMappingByLocationKey(unsigned long locationKey, const bool readWrite )
    {
        std::ostringstream sql;
        sql << MAP_TABLE_ID_COL << " in (select  " 
            << DVA_MSG_TABLE_KEY_COL << " from " << PA_STATION_DVA_MESSAGE_TABLE_NAME
            << " where " << DVA_MSG_TABLE_LOCATION_KEY_COL << " = " << locationKey << ")";

        return getPaTisMessageMappingsReturnedFromStatement(sql.str(), readWrite);   
    }


    IPaTisMessageMaps PaTisMessageMappingAccessFactory::getPaTisMessageMappingsReturnedFromStatement( const std::string& whereClause,  const bool readWrite)
    {
		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
		
        // create the SQL string to retrieve the data from the pa_station_dva_message and the
        // pa_tis_message_map tables based upon the primary key of a PaDvaMessage record        
        std::ostringstream sql;        
      /*  sql << "select " 
			<< DVA_MSG_TABLE_KEY_COL << ", "
			<< DVA_MSG_TABLE_LOCATION_KEY_COL << ", " 
			<< DVA_MSG_TABLE_ID_COL << ", "
			<< DVA_MSG_TABLE_LABEL_COL << ", "
			<< DVA_MSG_TABLE_TYPE_COL << ", "
            << "TO_CHAR(" << DVA_MSG_TABLE_DATECREATED_COL << ", 'YYYYMMDDHH24MISS'), " 
            << "TO_CHAR(" << DVA_MSG_TABLE_DATEMODIFIED_COL << ", 'YYYYMMDDHH24MISS')"
            << " from "  
            << PA_STATION_DVA_MESSAGE_TABLE_NAME; */
		if( !whereClause.empty() )
		{
			sql << " where " << whereClause;
		}

		//sql << std::endl;
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_TIS_MESSAGE_MAP_Oracle_SELECT_41053, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_TIS_MESSAGE_MAP_SELECT_41053, sql.str());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_TIS_MESSAGE_MAP_SELECT_41053, sql.str());
        // Set up the columnNames vector to be passed to executeQuery()       
        std::vector<std::string> columnNames;

        columnNames.push_back(MAP_TABLE_KEY_COL);
        columnNames.push_back(MAP_TABLE_ID_COL);
		columnNames.push_back(MAP_TABLE_TISMESSAGETAG_COL);
        columnNames.push_back(MAP_TABLE_TISLIBRARYSECTION_COL);
        columnNames.push_back(MAP_TABLE_TISLIBRARYVERSION_COL);
        columnNames.push_back(MAP_TABLE_DATECREATED_COL);
        columnNames.push_back(MAP_TABLE_DATEMODIFIED_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
		
		// create the vector of pointers to ITrackElement
        IPaTisMessageMaps returnValue;

        do
        {
            unsigned long rowCount = data->getNumRows();
			
            for (unsigned long i = 0; i < rowCount; ++i)
            {
                if (readWrite == true)
                {
                    returnValue.push_back(new ConfigPaTisMessageMap(i, *data));
                }
                else
                {
                    returnValue.push_back(new PaTisMessageMap(i, *data));
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
    

    IConfigPaTisMessageMap* PaTisMessageMappingAccessFactory::createPaTisMessageMapping()
    {
		//TA_ASSERT(false, "this function should be call when use distribute DB");
        return new ConfigPaTisMessageMap();    
    }


    IConfigPaTisMessageMap* PaTisMessageMappingAccessFactory::copyPaTisMessageMapping(const IConfigPaTisMessageMap* paDvaMessageToCopy)
    {
		//TA_ASSERT(false, "this function should be call when use distribute DB");
		LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "The copyPaTisMessageMapping function should not be called.");

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
		return new ConfigPaTisMessageMap();

    }


    std::vector<TisMessage> PaTisMessageMappingAccessFactory::enumerateTisMessages()
    {
        std::vector<std::string> columnNames;

        std::string msgTable("a.");
        std::string verTable("b.");
        
        columnNames.push_back(msgTable + TIS_KEY_COL_NAME);
        columnNames.push_back(msgTable + TIS_MESSAGE_TAG_COL);
        columnNames.push_back(msgTable + TIS_LIBRARY_SECTION_COL);
        columnNames.push_back(msgTable + TIS_DESCRIPTION_COL);
        columnNames.push_back(verTable + TIS_LIBRARY_VERSION_COL);

        unsigned int i;
        std::ostringstream sql;
        //sql << "select ";
        for (i = 0; i < columnNames.size(); i ++)
        {
            if (i > 0)
            {
                sql << ", ";
            }
            sql << columnNames[i];
        }
        
        // Join with the TIS message version table, to get version information
       /* sql << " from " << TIS_TABLE_NAME << " a, " << TIS_LIBRARY_VER_TABLE << " b"
            << " where a." << TIS_LIBRARY_VER_KEY_COL << " = " << "b." << TIS_LIBRARY_VER_KEY_COL;*/
        
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_SELECT_38508,sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38508,sql.str());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38508,sql.str());

        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
               
        std::vector<TisMessage> result;
        
        if (data.get() == 0)
        {
            // Return empty set in the event of no rows
            return result;
        }

        // Fill in resulting structures
        for (i = 0; i < data->getNumRows(); ++i)
        {
            TisMessage msg;

            msg.pKey            = data->getUnsignedLongData(i, msgTable + TIS_KEY_COL_NAME);

            if (!data->isNull(i, msgTable + TIS_MESSAGE_TAG_COL))
            {
                msg.tisMessageTag   = data->getUnsignedLongData(i, msgTable + TIS_MESSAGE_TAG_COL);
            }
            if (!data->isNull(i, msgTable + TIS_LIBRARY_SECTION_COL))
            {
                msg.tisLibrarySection   = data->getUnsignedLongData(i, msgTable + TIS_LIBRARY_SECTION_COL);
            }            
            if (!data->isNull(i, msgTable + TIS_DESCRIPTION_COL))
            {
                msg.description = data->getStringData(i, msgTable + TIS_DESCRIPTION_COL);
            }

            // One of the few fields that isn't possibly null
            msg.tisLibraryVersion = data->getUnsignedLongData(i, verTable + TIS_LIBRARY_VERSION_COL);

            result.push_back(msg);
        }

        return result;
    }

    TA_Base_Core::IPaTisMessageMap* PaTisMessageMappingAccessFactory::getPaTisMessageMappingByMessageId( unsigned long messageId, const bool readWrite /*= false */ )
    {
        // should handle memory leak here
        std::ostringstream sql;
        sql << MAP_TABLE_ID_COL << " =  " << messageId;

        TA_Base_Core::IPaTisMessageMaps paTisMsgMaps = getPaTisMessageMappingsReturnedFromStatement(sql.str(), readWrite);

        if ( paTisMsgMaps.empty() )
        {
            std::stringstream reason;
            reason << "Can not find the pa tis map for id = " << messageId;
            TA_THROW( TA_Base_Core::DataException( reason.str().c_str(), TA_Base_Core::DataException::NO_VALUE, MAP_TABLE_ID_COL ));
        }
        else if ( 1u < paTisMsgMaps.size() )
        {
            std::stringstream reason;
            reason << "Found multiple pa tis map for id = " << messageId;
            TA_THROW( TA_Base_Core::DataException( reason.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, MAP_TABLE_ID_COL ));
        }

        return paTisMsgMaps[0];
    }

} // closes TA_Base_Core



/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaZoneGroupAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaZoneGroupAccessFactory is a singleton that is used to retrieve PaZoneGroup objects either from the
  * database or newly created. All PaZoneGroup objects returned will adhere to the IPaZoneGroup interface.
  */
#pragma warning(disable:4786 4290 4284)


#include "core/data_access_interface/pa/src/PaZoneGroupAccessFactory.h"

#include "core/data_access_interface/pa/src/PaZoneGroup.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/ConfigPaZoneGroup.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"


namespace TA_Base_Core
{
    PaZoneGroupAccessFactory* PaZoneGroupAccessFactory::m_instance = 0;

    static const std::string KEY_COL_NAME = "PAZGRO_ID";
    static const std::string PA_ZONE_GROUP_TABLE_NAME = "PA_ZONE_GROUP";
    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";

    PaZoneGroupAccessFactory& PaZoneGroupAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaZoneGroupAccessFactory();
        }
        return *m_instance;
    }


    void PaZoneGroupAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaZoneGroup* PaZoneGroupAccessFactory::getPaZoneGroup(const unsigned long key,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string to retrieve the name of the PaZoneGroup
        // this is a check to ensure an PaZoneGroup with the specified
        // PAZONE_ID actually exists.        
       /* std::ostringstream sql;
        sql << "select " << KEY_COL_NAME 
            << " from " << PA_ZONE_GROUP_TABLE_NAME
            << " where " << KEY_COL_NAME << " = " << key; */  
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_ZONE_GROUP_STD_SELECT_45501, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_GROUP_SELECT_45501, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_GROUP_SELECT_45501, key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL_NAME);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "No data found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "More than one entry found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the PaZoneGroup object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaZoneGroup* thePaZoneGroup;
        
        // decide which PaZoneGroup object to return
        if (readWrite) // need a config PaZoneGroup
        {
            thePaZoneGroup = new ConfigPaZoneGroup(key);
        }
        else // need a standard PaZoneGroup
        {
            thePaZoneGroup = new PaZoneGroup(key);
        }

        // Return the PaZoneGroup pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaZoneGroup;
    }


    IPaZoneGroups PaZoneGroupAccessFactory::getAllPaZoneGroups( const bool readWrite)
    {
        return getPaZoneGroupsReturnedFromStatement("", readWrite);
    }


    IPaZoneGroups PaZoneGroupAccessFactory::getPaZoneGroupsByLocationKey(unsigned long locationKey, const bool readWrite )
    {
        std::ostringstream sql;
        sql << LOCATIONKEY_COL << " = " << locationKey;

        return getPaZoneGroupsReturnedFromStatement(sql.str(), readWrite);   
    }


    IPaZoneGroups PaZoneGroupAccessFactory::getPaZoneGroupsReturnedFromStatement(const std::string& whereClause, 
                                                                                 const bool readWrite)
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaZoneGroups, ConfigPaZoneGroup, PaZoneGroup>
                    (PaZoneGroupHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaZoneGroup* PaZoneGroupAccessFactory::createPaZoneGroup()
    {            
        return new ConfigPaZoneGroup();    
    }


    IConfigPaZoneGroup* PaZoneGroupAccessFactory::copyPaZoneGroup(const IConfigPaZoneGroup* paZoneGroupToCopy)
    {
        FUNCTION_ENTRY("copyPaZoneGroup");

        TA_ASSERT(paZoneGroupToCopy !=  NULL, "The PaZoneGroup to copy was NULL");
        
        const ConfigPaZoneGroup* cast = dynamic_cast<const ConfigPaZoneGroup*>(paZoneGroupToCopy);

        TA_ASSERT(cast != NULL, "PaZoneGroup passed could not be converted into a ConfigPaZoneGroup");

        FUNCTION_EXIT;
        return new ConfigPaZoneGroup(*cast);

    }


} // closes TA_Base_Core



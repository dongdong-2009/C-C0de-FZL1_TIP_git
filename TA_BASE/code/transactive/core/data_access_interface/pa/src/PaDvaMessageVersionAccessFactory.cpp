/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaDvaMessageVersionAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaDvaMessageVersionAccessFactory is a singleton that is used to retrieve PaDvaMessageVersion objects either from the
  * database or newly created. All PaDvaMessageVersion objects returned will adhere to the IPaDvaMessageVersion interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/pa/src/PaDvaMessageVersionAccessFactory.h"

#include "core/data_access_interface/pa/src/PaDvaMessageVersion.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/ConfigPaDvaMessageVersion.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    PaDvaMessageVersionAccessFactory* PaDvaMessageVersionAccessFactory::m_instance = 0;

    static const std::string KEY_COL = "PADMVE_ID";
    static const std::string PA_DVA_MESSAGE_VERSION_TABLE_NAME = "PA_DVA_MESSAGE_VERSION";
    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";

    PaDvaMessageVersionAccessFactory& PaDvaMessageVersionAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaDvaMessageVersionAccessFactory();
        }
        return *m_instance;
    }


    void PaDvaMessageVersionAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaDvaMessageVersion* PaDvaMessageVersionAccessFactory::getPaDvaMessageVersionByLocationKey(const unsigned long locationKey,const bool readWrite)
    {
        return getPaDvaMessageVersionUsing(LOCATIONKEY_COL, locationKey, readWrite);
    }


    IPaDvaMessageVersion* PaDvaMessageVersionAccessFactory::getPaDvaMessageVersion(const unsigned long key,const bool readWrite)
    {
        return getPaDvaMessageVersionUsing(KEY_COL, key, readWrite);
    }


    IPaDvaMessageVersion* PaDvaMessageVersionAccessFactory::getPaDvaMessageVersionUsing(const std::string& colName, 
                                                                                            unsigned long colValue,
                                                                                            bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string to retrieve the name of the PaDvaMessageVersion
        // this is a check to ensure an PaDvaMessageVersion with the specified
        // PATDME_ID actually exists.        
       /* std::ostringstream sql;
        sql << "select " << KEY_COL 
            << " from " << PA_DVA_MESSAGE_VERSION_TABLE_NAME
            << " where " << databaseConnection->escapeQueryString(colName) << " = " << colValue;   */ 
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_DVA_MESSAGE_VERSION_STD_SELECT_43001,
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_DVA_MESSAGE_VERSION_SELECT_43001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_DVA_MESSAGE_VERSION_SELECT_43001,
			databaseConnection->escapeQueryString(colName), colValue);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);

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
			sprintf(reasonMessage, 
                            "No data found for item where %s = %lu", 
                            colName.c_str(), 
                            colValue);			
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, 
                            "No data found for item where %s = %lu", 
                            colName.c_str(), 
                            colValue);			
            TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        unsigned long key = data->getUnsignedLongData(0, KEY_COL);

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the PaDvaMessageVersion object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaDvaMessageVersion* thePaDvaMessageVersion;
        
        // decide which PaDvaMessageVersion object to return
        if (readWrite) // need a config PaDvaMessageVersion
        {
            thePaDvaMessageVersion = new ConfigPaDvaMessageVersion(key);
        }
        else // need a standard PaDvaMessageVersion
        {
            thePaDvaMessageVersion = new PaDvaMessageVersion(key);
        }

        // Return the PaDvaMessageVersion pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaDvaMessageVersion;
    }


    IPaDvaMessageVersions PaDvaMessageVersionAccessFactory::getAllPaDvaMessageVersions( const bool readWrite)
    {
        return getPaDvaMessageVersionsReturnedFromStatement("", readWrite);
    }


    IPaDvaMessageVersions PaDvaMessageVersionAccessFactory::getPaDvaMessageVersionsReturnedFromStatement(
        const std::string& whereClause, 
        const bool readWrite)
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaDvaMessageVersions, ConfigPaDvaMessageVersion, PaDvaMessageVersion>
                        (PaDvaMessageVersionHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaDvaMessageVersion* PaDvaMessageVersionAccessFactory::copyPaDvaMessageVersion(const IConfigPaDvaMessageVersion* paDvaMessageVersionToCopy)
    {
        FUNCTION_ENTRY("copyPaDvaMessageVersion");

        TA_ASSERT(paDvaMessageVersionToCopy !=  NULL, "The PaDvaMessageVersion to copy was NULL");
        
        const ConfigPaDvaMessageVersion* cast = dynamic_cast<const ConfigPaDvaMessageVersion*>(paDvaMessageVersionToCopy);

        TA_ASSERT(cast != NULL, "PaDvaMessageVersion passed could not be converted into a ConfigPaDvaMessageVersion");

        FUNCTION_EXIT;
        return new ConfigPaDvaMessageVersion(*cast);

    }

    IConfigPaDvaMessageVersion* PaDvaMessageVersionAccessFactory::createPaDvaMessageVersion()
    {            
        return new ConfigPaDvaMessageVersion();    
    }

} // closes TA_Base_Core



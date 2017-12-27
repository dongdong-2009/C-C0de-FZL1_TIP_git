/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/pa/src/PaPriorityGroupAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * PaPriorityGroupAccessFactory is a singleton that is used to retrieve PaStation objects either from the
  * database or newly created. All PaStation objects returned will adhere to the IPaStation interface.
  */
#pragma warning(disable:4786 4290)

#include "core/data_access_interface/pa/src/PaTableConsts.h"
#include "core/data_access_interface/pa/src/PaPriorityGroupAccessFactory.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    PaPriorityGroupAccessFactory* PaPriorityGroupAccessFactory::m_instance = 0;

    PaPriorityGroupAccessFactory& PaPriorityGroupAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaPriorityGroupAccessFactory();
        }
        return *m_instance;
    }


    void PaPriorityGroupAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    unsigned long PaPriorityGroupAccessFactory::getPaPriorityGroupIdBySessionId( const std::string& strSessionId )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        /*std::stringstream stmQuery;
        stmQuery << "select " << PA_PRIORITY_GROUP_PA_PRIORITY_ID_COL << " from " << PA_PRIORITY_GROUP_TABLE;
        stmQuery << ", SE_PROFILE, TA_SESSION";
        stmQuery << " where ";
        stmQuery << PA_PRIORITY_GROUP_KEY_COL << " = SE_PROFILE.PA_PRIORITY_GROUP_KEY" << " and ";
        stmQuery << "TA_SESSION.SEPROF_ID = SE_PROFILE.SEPROF_ID" << " and ";
        stmQuery << "TA_SESSION.PKEY = '" << strSessionId << "'";*/

		SQLStatement stmQuery;
		databaseConnection->prepareSQLStatement(stmQuery,PA_PRIORITY_GROUP_SELECT_109001, strSessionId);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PA_PRIORITY_GROUP_PA_PRIORITY_ID_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = 0;
        try
        {
            data = databaseConnection->executeQuery(stmQuery, columnNames);
        }
        catch ( DatabaseException& expDatabase )
        {
            delete data;
            data = 0;
            TA_THROW(DataException(expDatabase.what(),DataException::NO_VALUE,""));
        }
        catch( ... )
        {
            delete data;
            data = 0;
            TA_THROW(DataException("Catch unknow exception",DataException::NO_VALUE,""));
        }
        
        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = 0;
            
            char reasonMessage[256];
            sprintf(reasonMessage, "No data found for session id = %s",strSessionId.c_str());
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = 0;
            
            char reasonMessage[256];
            sprintf(reasonMessage, "More than one entry found for session id = %s",strSessionId.c_str());			
            TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        unsigned long ulPriorityId = data->getUnsignedLongData( 0, PA_PRIORITY_GROUP_PA_PRIORITY_ID_COL );

        delete data;
        data = 0;

        return ulPriorityId;
    }

    void PaPriorityGroupAccessFactory::getAllPaPriorityGroupNames( std::map<unsigned long, std::string>& mapKeyToNames )
    {
        mapKeyToNames.clear();

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        /*std::stringstream stmQuery;
        stmQuery << "select " << PA_PRIORITY_GROUP_KEY_COL << ", " << PA_PRIORITY_GROUP_PA_PRIORITY_NAME_COL;
        stmQuery << " from " << PA_PRIORITY_GROUP_TABLE;*/

		SQLStatement stmQuery;
		databaseConnection->prepareSQLStatement(stmQuery, PA_PRIORITY_GROUP_SELECT_109002);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PA_PRIORITY_GROUP_KEY_COL);
        columnNames.push_back(PA_PRIORITY_GROUP_PA_PRIORITY_NAME_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = 0;
        try
        {
            data = databaseConnection->executeQuery(stmQuery, columnNames);
        }
        catch ( DatabaseException& expDatabase )
        {
            delete data;
            data = 0;
            TA_THROW(DataException(expDatabase.what(),DataException::NO_VALUE,""));
        }
        catch( ... )
        {
            delete data;
            data = 0;
            TA_THROW(DataException("Catch unknow exception",DataException::NO_VALUE,""));
        }

        do 
        {
            if ( 0 == data )
            {
                break;
            }

            for ( unsigned long ulLoop = 0; ulLoop < data->getNumRows(); ++ulLoop )
            {
                unsigned long ulKey = data->getUnsignedLongData( ulLoop, PA_PRIORITY_GROUP_KEY_COL );
                mapKeyToNames[ulKey] = data->getStringData( ulLoop, PA_PRIORITY_GROUP_PA_PRIORITY_NAME_COL );
            }

            delete data;
            data = 0;
        } 
        while (databaseConnection->moreData( data ));
    }
} // closes TA_Base_Core



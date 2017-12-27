/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/system_controller/src/SubSystemControllerAccessFactory.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * SubSystemControllerAccessFactory is a singleton that is
  * used to retrieve ISystemControllerData objects from
  * the database.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif

#include "SubSystemControllerAccessFactory.h"
#include "core/data_access_interface/system_controller/src/SystemControllerData.h"
#include "core/data_access_interface/system_controller/src/ConfigSystemController.h"

#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <string>
#include <vector>
#include <sstream>

namespace TA_Base_App
{

    SubSystemControllerAccessFactory* SubSystemControllerAccessFactory::m_instance = 0;

    SubSystemControllerAccessFactory& SubSystemControllerAccessFactory::getInstance()
    {
        if (m_instance == NULL)
        {
            m_instance = new SubSystemControllerAccessFactory();
        }
        return *m_instance;
    }

    void SubSystemControllerAccessFactory::getAllSystemControllerNames( std::vector<std::string>& vecHostNames )
    {
        FUNCTION_ENTRY("getAllSystemControllerNames");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::SysController_Cd, TA_Base_Core::Read );
        
        // create the SQL string to retrieve all the groups
        std::string sql ("select SN_SYSTEM_CONTROLLER.NAME as SERVERNAME, LOCATION.NAME, SN_SYSTEM_CONTROLLER.SERVER_ENTITY_PKEY from LOCATION, SN_SYSTEM_CONTROLLER, ENTITY where LOCATION.PKEY = ENTITY.LOCATIONKEY and ENTITY.PKEY = SN_SYSTEM_CONTROLLER.SERVER_ENTITY_PKEY order by LOCATION.PKEY");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string servernameColumn( "SERVERNAME" );
        std::string nameColumn( "NAME" );
        std::string keyColumn( "SERVER_ENTITY_PKEY" );
        std::vector<std::string> columnNames;
        columnNames.push_back(servernameColumn);
        columnNames.push_back(nameColumn);
        columnNames.push_back(keyColumn);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::SQLStatement sqlStatement;
        sqlStatement.strCommon = sql;
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sqlStatement,columnNames);

        vecHostNames.clear();

        // Loop for each row returned
        do
        {
            if ( NULL != data )
            {
                // Fill a vector with the host names returned from the query.
                unsigned long numRows = data->getNumRows();
                for (unsigned long i = 0; i < numRows; i++)
                {
                    std::stringstream stmFormat;
                    stmFormat << data->getStringData(i, servernameColumn) << "|";
                    stmFormat << data->getStringData(i, nameColumn) << "|";
                    stmFormat <<  data->getUnsignedLongData(i, keyColumn);
                    vecHostNames.push_back( stmFormat.str() );
                }
                delete data;
                data = NULL;
            }
        }
        while( databaseConnection->moreData(data) );
    }


} // namespace TA_Base_Core

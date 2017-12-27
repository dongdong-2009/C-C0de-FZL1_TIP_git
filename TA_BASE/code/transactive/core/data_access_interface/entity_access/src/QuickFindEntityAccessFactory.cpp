/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/QuickFindEntityAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * QuickFindAccessFactory is a singleton that is used to retrieve entity objects which should be available
  * for quick launch in the Control Station.
  * All objects returned will adhear to the ISession interface.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/data_access_interface/entity_access/src/QuickFindEntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/QuickFindEntity.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include <sstream>

namespace TA_Base_Core
{
    QuickFindEntityAccessFactory* QuickFindEntityAccessFactory::s_instance = 0;


    QuickFindEntityAccessFactory& QuickFindEntityAccessFactory::getInstance()
    {
        if( s_instance == 0 )
        {
            s_instance = new QuickFindEntityAccessFactory();
        }
        return *s_instance;
    }


    IQuickFindEntity* QuickFindEntityAccessFactory::getEntity(const unsigned long key)
    {
        // get a connection to the database
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the name of the gui
        // this also doubles as a check to ensure an gui with the specified
        // PKEY actually exists.
        /*std::ostringstream sql;
        sql << "select ENTITY_PKEY, ENTITY_NAME, ENTITY_DESCRIPTION, ENTITYTYPE_GUIAPPLICATIONKEY from co_entity_quick_find_v where pkey = " << key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4510, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4510, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4510, key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "PKEY";
        std::string nameColumn = "NAME";
        std::string descriptionColumn = "DESCRIPTION";
        std::string guiColumn = "GUIAPPLICATIONKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        columnNames.push_back(nameColumn);
        columnNames.push_back(descriptionColumn);
        columnNames.push_back(guiColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // Bring the DataException into the namespace
            using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for entity key = " << key;			
                TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for entity key = " << key;			
                TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Retrieve the key from the returned data.
        unsigned long pkey = data->getUnsignedLongData(0,keyColumn);
        std::string name = data->getStringData(0,nameColumn);
        std::string description = data->getStringData(0,descriptionColumn);
        unsigned long gui = data->getUnsignedLongData(0,guiColumn);

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        return new QuickFindEntity(pkey,name,description,gui);
    }


    std::vector<IQuickFindEntity*> QuickFindEntityAccessFactory::getAllQuickFindEntities()
    {
        // get a connection to the database
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the name of the gui
        // this also doubles as a check to ensure an gui with the specified
        // PKEY actually exists.
        /*std::ostringstream sql;
        sql << "select ENTITY_PKEY, ENTITY_NAME, ENTITY_DESCRIPTION, ENTITYTYPE_GUIAPPLICATIONKEY from co_entity_quick_find_v";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4509);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4509);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4509);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "PKEY";
        std::string nameColumn = "NAME";
        std::string descriptionColumn = "DESCRIPTION";
        std::string guiColumn = "GUIAPPLICATION";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        columnNames.push_back(nameColumn);
        columnNames.push_back(descriptionColumn);
        columnNames.push_back(guiColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        std::vector<IQuickFindEntity*> tempVector;

        do
        {
            try
            {
                for (unsigned int i = 0; i < data->getNumRows(); ++i)
                {
                    // Retrieve the key from the returned data.
                    unsigned long key = data->getUnsignedLongData(i,keyColumn);
                    std::string name = data->getStringData(i,nameColumn);
                    std::string description = data->getStringData(i,descriptionColumn);
                    unsigned long gui = data->getUnsignedLongData(i,guiColumn);
                    tempVector.push_back( new QuickFindEntity(key,name,description,gui) );
                }
            }
            catch ( ... ) // Before any exceptions propagate out we must delete our data object
            {
                delete data;
                data = NULL;
                throw;
            }

            // Now that we're finished with the IData object, we need to delete it.
            // This clears it ready for the next lot if necessary.
            delete data;
            data = NULL;

        }while( databaseConnection->moreData(data) );

        return tempVector;
    }


} // closes TA_Base_Core



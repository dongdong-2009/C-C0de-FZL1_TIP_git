/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/configuration/src/ComponentAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ComponentAccessFactory is a singleton that is used to retrieve 
  * IComponent objects.
  */

#ifdef __WIN32__
#pragma warning (disable:4786)
#endif

#include "core\data_access_interface\configuration\src\ComponentAccessFactory.h"
#include "core\data_access_interface\configuration\src\Component.h"
#include "core\data_access_interface\src\DatabaseFactory.h"
#include "core\data_access_interface\src\SQLCode.h"

#include "core\exceptions\src\DataException.h"

using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    // This is used to indicate if an entry has no entity type associated with it.
    const std::string ComponentAccessFactory::NO_ENTITY_TYPE = "none";


    ComponentAccessFactory::~ComponentAccessFactory()
    {

    }


    std::vector<IComponent*> ComponentAccessFactory::getAllComponents()
    {
		// create the SQL string to retrieve all the locations of the alarm
        /*std::string sql;
        sql = "SELECT c.cocomp_id, c.category, c.component, c.library, c.help, NVL(t.name,'" + NO_ENTITY_TYPE + "') as ENTITYNAME ";
        sql+= "FROM CO_COMPONENT c, ENTITYTYPE t ";
        sql+= "WHERE c.entitytypekey = t.pkey(+) ";
        sql+= "ORDER BY c.category, c.component";*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn( "KEY" );
		std::string categoryColumn( "CATEGORY" );
		std::string componentColumn( "COMPONENT" );
		std::string libraryColumn( "LIBRARY" );
		std::string helpColumn( "HELP" );
        std::string entityTypeNameColumn( "ENTITYNAME" );

        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
		columnNames.push_back(categoryColumn);
		columnNames.push_back(componentColumn);
		columnNames.push_back(libraryColumn);
		columnNames.push_back(helpColumn);
		columnNames.push_back(entityTypeNameColumn);

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_Oracle_SELECT_4051);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4051);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYTYPE_SELECT_4051);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
		std::vector< IComponent* > components;

        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); ++i)
            {
                try
                {
                    std::string entityType = data->getStringData( i, entityTypeNameColumn );

                    if (entityType.compare(NO_ENTITY_TYPE) == 0)
                    {
                        entityType = "";
                    }
					// No need to load the data as it is already loaded
                    components.push_back( new Component( data->getUnsignedLongData( i, keyColumn),
                                                         data->getStringData( i, categoryColumn),
                                                         data->getStringData( i, componentColumn),
                                                         data->getStringData( i, libraryColumn),
                                                         data->getStringData( i, helpColumn),
                                                         entityType ) );
                }
                catch (const TA_Base_Core::DataException&)
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

		return components;
    }

} //end namespace TA_Base_Core



/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DataPointAccessFactory.cpp $
  * @author:  HoaVu
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * DataPointAccessFactory is a singleton that is used to retrieve
  * DataPoint objects defined in database.
  *
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4503)
	#pragma warning(disable:4723)	// potential divide by 0
#endif // defined _MSC_VER

#include <sstream>

#include "core/data_access_interface/entity_access/src/DataPointAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

	DataPointAccessFactory::DataPointAccessFactory()
	{
	}


	DataPointAccessFactory::~DataPointAccessFactory()
	{
	}


    void DataPointAccessFactory::getAllDpEventDetails ( const std::string& agentName, PkeyEventDetailsMap & pkeyEventDetailsMap )
	{
		// get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		// set up sql string to get all all event related data from the database
		/*std::stringstream sql;
		sql << "SELECT dp.pkey, dp.parentkey, dnEpv.VALUE, dpEpv.VALUE ";
		sql << "FROM ENTITYPARAMETERVALUE dnEpv, ENTITYPARAMETERVALUE dpEpv, ENTITY dp ";

		sql << "WHERE dnEpv.parameterkey = ( SELECT pkey FROM ENTITYPARAMETER WHERE NAME LIKE '" << DataNodeEntityData::DATANODE_ASSET_NAME << "' ";
		sql << "AND typekey = ( SELECT pkey FROM ENTITYTYPE WHERE NAME LIKE '" << DataNodeEntityData::getStaticType() << "' ) ) ";

        sql << "AND dp.parentkey = dnEpv.entitykey ";

        sql << "AND dpEpv.parameterkey = ( SELECT pkey FROM ENTITYPARAMETER WHERE NAME LIKE '" << DataPointEntityData::DATAPOINT_EVENT_MESSAGE << "' ";
		sql << "AND typekey = ( SELECT pkey FROM ENTITYTYPE WHERE NAME LIKE '" << DataPointEntityData::getStaticType() << "' ) ) ";
		sql << "AND dpEpv.entitykey IN ( SELECT pkey FROM ENTITY WHERE agentkey = (SELECT pkey FROM ENTITY WHERE name = '" << agentName << "') AND deleted = 0 "; 
        sql << "AND typekey = ( SELECT pkey FROM ENTITYTYPE WHERE NAME LIKE '" << DataPointEntityData::getStaticType() << "' ) ) ";

        sql << "AND dp.pkey = dpEpv.entitykey ";
		sql << "AND dp.deleted = 0";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_STD_SELECT_20505, DataNodeEntityData::getStaticType(),
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_SELECT_20505, DataNodeEntityData::getStaticType(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_SELECT_20505, DataNodeEntityData::getStaticType(),
			DataPointEntityData::getStaticType(), agentName, DataPointEntityData::getStaticType() );

        // set up the columns
        std::string entitykeyColumn = "pkey";
        std::string parentKeyColumn = "parentkey";
		std::string assetNameColumn = "value";
		std::string eventMessageColumn = "value_1";
        std::vector<std::string> columnNames;
        columnNames.push_back(entitykeyColumn);
        columnNames.push_back(parentKeyColumn);
		columnNames.push_back(assetNameColumn);
        columnNames.push_back(eventMessageColumn);

		IData* data = NULL;

		try
		{
            // Execute the query. The method can throw a DatabaseException.
            // This is documented in the comment of this method.
            // We are responsible for deleting the returned IData object when we're done with it
            data = databaseConnection->executeQuery( strSql, columnNames);

            if ( 0 == data->getNumRows() ) // No entry found with the specified alarm_id
            {
                // clean up the pointer
                delete data;
                data = NULL;
				return;
            }
			
			do
			{
				// populate the list from the result set
				populateEventDetails ( data, pkeyEventDetailsMap, columnNames );

				// we must delete the data
				delete data;
				data = NULL;
			}
			while ( databaseConnection->moreData(data) );
		}
		catch( const DatabaseException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
			delete data;
			data = NULL;
			TA_THROW(e);
		}
		catch( const DataException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
			delete data;
			data = NULL;
			TA_THROW(e);
		}
		catch( const TransactiveException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
			delete data;
			data = NULL;
			TA_THROW(e);
		}
	}


	void DataPointAccessFactory::populateEventDetails ( IData * data, PkeyEventDetailsMap& pkeyEventDetailsMap, std::vector<std::string> columnNames )
	{
		// for each rows of the result set
        for (unsigned long i = 0; i < data->getNumRows();i++)
		{
			unsigned long pkey = data->getUnsignedLongData(i, columnNames[0]);
			DataPointEventDetails eventDetails;

			eventDetails.parentKey = data->getUnsignedLongData(i, columnNames[1]);
			eventDetails.assetName = data->getStringData(i, columnNames[2]);
			eventDetails.eventMessage = data->getStringData(i, columnNames[3]);

			// find if the current entity key already in the map
			PkeyEventDetailsMap::iterator itr = pkeyEventDetailsMap.find ( pkey );

			// if found none
			if ( pkeyEventDetailsMap.end() == itr )
			{
				// insert the pair to map
				pkeyEventDetailsMap [ pkey ] = eventDetails;
			}
			// else do nothing
		}
	}


    void DataPointAccessFactory::getAllDataPointEntitiesWithNameLikeToken ( const std::string& token,
                                                                            std::vector< DataPointEntityData *> & dataPointEntitiesList )
    {
        std::vector< IEntityData *> theDbEntities;
		theDbEntities.clear();

		// get the list of entities from the database
		try
		{
			theDbEntities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken ( DataPointEntityData::getStaticType(), token );

			// for each of the db entities returned
			std::vector< IEntityData *>::iterator itr;
			for ( itr = theDbEntities.begin();
				  itr != theDbEntities.end();
				  itr++ )
			{
				// convert the db entity to data point entity
				DataPointEntityData * dp = 0;
				dp = dynamic_cast < DataPointEntityData * > ( *itr );

				// push it onto the returned list
				dataPointEntitiesList.push_back ( dp );
			}
		}
		catch ( ... )
		{
			// do nothing
		}

		theDbEntities.clear();

    }


    void DataPointAccessFactory::getAllDataPointEntitiesWithAddressLikeToken ( const std::string& token,
                                                                            std::vector< DataPointEntityData *> & dataPointEntitiesList )
    {
        std::vector< IEntityData *> theDbEntities;
		theDbEntities.clear();

		// get the list of entities from the database
		try
		{
			theDbEntities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithAddressLikeToken ( DataPointEntityData::getStaticType(), token );

			// for each of the db entities returned
			std::vector< IEntityData *>::iterator itr;
			for ( itr = theDbEntities.begin();
				  itr != theDbEntities.end();
				  itr++ )
			{
				// convert the db entity to data point entity
				DataPointEntityData * dp = 0;
				dp = dynamic_cast < DataPointEntityData * > ( *itr );

				// push it onto the returned list
				dataPointEntitiesList.push_back ( dp );
			}
		}
		catch ( ... )
		{
			// do nothing
		}

		theDbEntities.clear();
    }


} // closes TA_Base_Core


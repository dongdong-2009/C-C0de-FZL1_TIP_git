 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASubscribersAccessFactory.cpp $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RASubscribersAccessFactory is a singleton that is used to retrieve operator objects either from the
  * database or newly created. All operator objects returned will adhear to the IRASubscribers interface.
  */
#if 0
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif
#endif

#include <sstream>

#include "core/data_access_interface/radio/src/RASubscribersAccessFactory.h"
#include "core/data_access_interface/radio/src/RASubscribers.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    TA_Base_Core::NonReEntrantThreadLockable RASubscribersAccessFactory::m_singletonLock;
	RASubscribersAccessFactory* RASubscribersAccessFactory::s_instance = NULL;


    RASubscribersAccessFactory& RASubscribersAccessFactory::getInstance()
    {
        ThreadGuard guard( m_singletonLock );

        if (s_instance == NULL)
        {
            s_instance = new RASubscribersAccessFactory;
        }
        return *s_instance;
    }


    RASubscribersAccessFactory::~RASubscribersAccessFactory()
    {

    }


    IRASubscribers* RASubscribersAccessFactory::getSubscriberDetails(std::string TSI)
    {
        // Construct a new RASubscribers type with the key we have
        IRASubscribers* myRASubscribers = new RASubscribers(TSI);

        // Force it to load the details so we know if this key is valid or not. If this key is not valid
        // this call will throw an exception
        try
        {
            myRASubscribers->getTSI(); // was getName() TODO: check this
        }
        catch ( ... )
        {
            delete myRASubscribers;
            throw;
        }

        return myRASubscribers;
    }

    
    std::vector<IRASubscribers*> RASubscribersAccessFactory::getIndividualSubscribers(const bool readWrite) const
    {
		// Perform SQL here
		std::vector< IRASubscribers* > groups;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Read);

		// create the SQL string 
        //std::string sql("select PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID from TA_SESSION");
       /* std::stringstream s;
        s   << "select  "
            << "	   RASUBS_ID, "
            << "	   SUBSCRIBER_NAME,  "
            << "	   LOCATION,  "
            << "	   SUBSCRIBER_TYPE,  "
            << "	   nvl(LOCATION_KEY,0) as LOCATION_KEY "
            << " from  "
            << "	 RA_SUBSCRIBERS  "
            << " where  "
            << "	  SUBSCRIBER_TYPE NOT IN ('G','P') "
            << std::ends;
        std::string sql(s.str()); */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_SELECT_34556);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_SELECT_34556);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_SELECT_34556);
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::string rasubsIDColumn("RASUBS_ID");
		std::string subscriberNameColumn("SUBSCRIBER_NAME");
		std::string locationColumn ("LOCATION");
        std::string subscriberTypeColumn ("SUBSCRIBER_TYPE");
		std::string locationKeyColumn ("LOCATION_KEY");
        std::vector<std::string> columnNames;
        columnNames.push_back(rasubsIDColumn);
        columnNames.push_back(subscriberNameColumn);
        columnNames.push_back(locationColumn);
        columnNames.push_back(subscriberTypeColumn);
        columnNames.push_back(locationKeyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
					// Create the subsystem
                    IRASubscribers* group = new RASubscribers( 
                        data->getStringData(i, rasubsIDColumn ),
                        data->getStringData(i, subscriberNameColumn ),
                        static_cast<IRASubscribers::SubscriberType>(data->getStringData(i, subscriberTypeColumn )[0]),
                        data->getStringData(i, locationColumn ),
                        data->getUnsignedLongData(i, locationKeyColumn ),
                        false); // false indicates not a new session
                    
                    groups.push_back( group );
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

		return groups;

    }

    std::vector<IRASubscribers*> RASubscribersAccessFactory::searchSubscribers(const std::string& searchText, const bool readWrite) const
    {
		// Perform SQL here
		std::vector< IRASubscribers* > subscribers;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Read);

		// create the SQL string 
        //std::string sql("select PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID from TA_SESSION");
       /* std::stringstream s;
        s   << "select  "
            << "	   RASUBS_ID, "
            << "	   SUBSCRIBER_NAME,  "
            << "	   LOCATION,  "
            << "	   SUBSCRIBER_TYPE,  "
            << "	   nvl(LOCATION_KEY,0) as LOCATION_KEY "
            << " from  "
            << "	 RA_SUBSCRIBERS  "
            << " where  "
            << "	  upper(SUBSCRIBER_NAME) like '%' || " 
            << " upper('" << databaseConnection->escapeQueryString(searchText) << "') || '%' "
            << std::ends;
        std::string sql(s.str()); */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_SELECT_34557,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_SELECT_34557,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_SELECT_34557,
			databaseConnection->escapeQueryString(searchText));
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::string rasubsIDColumn("RASUBS_ID");
		std::string subscriberNameColumn("SUBSCRIBER_NAME");
		std::string locationColumn ("LOCATION");
        std::string subscriberTypeColumn ("SUBSCRIBER_TYPE");
		std::string locationKeyColumn ("LOCATION_KEY");
        std::vector<std::string> columnNames;
        columnNames.push_back(rasubsIDColumn);
        columnNames.push_back(subscriberNameColumn);
        columnNames.push_back(locationColumn);
        columnNames.push_back(subscriberTypeColumn);
        columnNames.push_back(locationKeyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
					// Create the subsystem
                    IRASubscribers* member = new RASubscribers( 
                        data->getStringData(i, rasubsIDColumn ),
                        data->getStringData(i, subscriberNameColumn ),
                        static_cast<IRASubscribers::SubscriberType>(data->getStringData(i, subscriberTypeColumn )[0]),
                        data->getStringData(i, locationColumn ),
                        data->getUnsignedLongData(i, locationKeyColumn ),
                        false); // false indicates not a new session
                    
                    subscribers.push_back( member );
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

		return subscribers;

    }


    
    IRASubscribers* RASubscribersAccessFactory::createSubscriber(IRASubscribers * x)
    {
        // dummy method to conform to AF usage, RASubscribers(...) constructor more 
        // than effective at creating new object, no need to wrap here. 
        // usage: new_subscriber = createSubscriber(new RASubscriber(...))
        return x;
    }

	//TD18317, zhanghongzhi
	std::string RASubscribersAccessFactory::getLocalDatabaseName()
    {
		std::string localDatabaseName;    
		if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
		{
			throw DatabaseException("db-connection-file not set");
		}
		try
		{
			std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			localDatabaseName = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Radio_Cd, Write);

			/*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			DataConnections connections = connectionStrings.getConnectionList(Radio_Cd, Write);
			localDatabaseName = connections[0].first;  */
		}
		catch(...) // most likely a file not found error
		{
			throw DatabaseException("Unable to find database connection infomation");
		}
		return localDatabaseName;
	}

    void RASubscribersAccessFactory::deleteSubscriber(IRASubscribers* subscriber)
    {
        if (subscriber == NULL)
        {
            return;
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Write);

		// do a cascade delete to avoid FK problems...
		/** @note Should probably use APIs, but we would need to do 
		* 1. search for ALL groups this subscriber is a member of;
		* 2. call GroupAF::deleteMemberFromGroup for each group membership
		* Just calling delete from <table> is a lot easier...
		*/
		if (subscriber->getSubscriberType() == IRASubscribers::INDIVIDUAL ||
			subscriber->getSubscriberType() == IRASubscribers::MOBILE ||
			subscriber->getSubscriberType() == IRASubscribers::DISPATCHER )
		{
		  /*	std::stringstream s;		//TD18317, zhanghongzhi
			s   << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			    << "delete from RA_GROUP_MEMBERS where MEMBER_RASUBS_ID = ''"
				<< subscriber->getTSI(false) << "'' " 
				<< "'); END;"
				<< std::ends;			
			std::string sql(s.str());  */
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34953,
//			std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34953,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34953,
				getLocalDatabaseName(), subscriber->getTSI(false));
			databaseConnection->executeModification(strSql);
		} 
		else if (subscriber->getSubscriberType() == IRASubscribers::GROUP )
		{
			{
				//std::stringstream s;	//TD18317, zhanghongzhi
				//s   << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
				//    << "delete from RA_PATCH_MEMBERS where GROUP_RASUBS_ID = ''"
				//	<< subscriber->getTSI(false) << "'' " 
				//	<< "'); END;"
				//	<< std::ends;
				//std::string sql(s.str());
//				std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34954,
//				std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34954,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34954,
					getLocalDatabaseName(), subscriber->getTSI(false));
				databaseConnection->executeModification(strSql);
			}
			{
				//std::stringstream s;	//TD18317, zhanghongzhi
				//s   << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
				//    << "delete from RA_GROUP_MEMBERS where GROUP_RASUBS_ID = ''"
				//	<< subscriber->getTSI(false) << "'' " 
				//	<< "'); END;"
				//	<< std::ends;
				//std::string sql(s.str());
//				std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34955,
//				std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34955,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34955,
					getLocalDatabaseName(), subscriber->getTSI(false));
				databaseConnection->executeModification(strSql);
			}
		} 
		else if (subscriber->getSubscriberType() == IRASubscribers::PATCH_GROUP )
		{
			//std::stringstream s;		//TD18317, zhanghongzhi
			//s   << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			//    << "delete from RA_PATCH_MEMBERS where PATCH_RASUBS_ID = ''"
			//	<< subscriber->getTSI(false) << "'' " 
			//	<< "'); END;"
			//	<< std::ends;
			//std::string sql(s.str());
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34956,
//			std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34956,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34956,
				getLocalDatabaseName(), subscriber->getTSI(false));
			databaseConnection->executeModification(strSql);
		}

		// Now that FK is out of the way, let's do what we are here for...
		{
			//std::stringstream s;		//TD18317, zhanghongzhi
			//s   << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			//    << "delete from RA_SUBSCRIBERS where RASUBS_ID = ''"
			//	<< subscriber->getTSI(false) << "'' " 
			//	<< "'); END;"
			//	<< std::ends;
			//std::string sql(s.str());
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34957,
//			std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34957,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34957,
				getLocalDatabaseName(), subscriber->getTSI(false));
			databaseConnection->executeModification(strSql);
		}
    }

    void RASubscribersAccessFactory::deleteGroup(const std::string& groupITSI)
    {   
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Write);

       /* std::stringstream deleteGroupMembersStream;
		deleteGroupMembersStream 
			<< "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			<< "delete from RA_GROUP_MEMBERS where GROUP_RASUBS_ID = ''"
            << groupITSI << "'' " 
			<< "'); END;"
			<< std::ends;
        std::string sql(deleteGroupMembersStream.str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34958, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34958, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34958,
			getLocalDatabaseName(), groupITSI);
        databaseConnection->executeModification(strSql);

        // Now that the entries in group members have been removed, need to remove the entires in patch members
       /* std::stringstream deletePatchGroupStream;
		deletePatchGroupStream 
			<< "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			<< "delete from RA_PATCH_MEMBERS where GROUP_RASUBS_ID = ''"
            << groupITSI << "'' " 
			<< "'); END;"
			<< std::ends;
        sql = "";
        sql = deletePatchGroupStream.str();*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34959, 
//		strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34959, 

		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34959,
			getLocalDatabaseName(), groupITSI);
        databaseConnection->executeModification(strSql);

        // Finally, delete the group itself from the subscribers table
       /* std::stringstream deleteGroupStream;
		deleteGroupStream 
			<< "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			<< "delete from RA_SUBSCRIBERS where RASUBS_ID = ''"
            << groupITSI << "'' " 
			<< "'); END;"
			<< std::ends;
        sql = "";
        sql = deleteGroupStream.str();*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34960,
//		strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34960,

		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34960,
			getLocalDatabaseName(), groupITSI);
        databaseConnection->executeModification(strSql);
    }

    void RASubscribersAccessFactory::deletePatch(const std::string& patchITSI)
    {   
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Write);

       /* std::stringstream deletePatchMembersStream;
        deletePatchMembersStream 
			<< "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			<< "delete from RA_PATCH_MEMBERS where PATCH_RASUBS_ID = ''"
            << patchITSI << "'' " 
			<< "'); END;"
			<< std::ends;
        std::string sql(deletePatchMembersStream.str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34961,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34961,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34961,
			getLocalDatabaseName(), patchITSI);
        databaseConnection->executeModification(strSql);

        // Delete the group itself from the subscribers table
       /* std::stringstream deletePatchStream;
        deletePatchStream 
			<< "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			<< "delete from RA_SUBSCRIBERS where RASUBS_ID = ''"
            << patchITSI << "'' " 
			<< "'); END;"
			<< std::ends;
        sql = "";
        sql = deletePatchStream.str();*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34962,
//		strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34962,

		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34962,
			getLocalDatabaseName(), patchITSI);
        databaseConnection->executeModification(strSql);
    }

} //end namespace TA_Base_Core



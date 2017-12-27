 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RAPatchMembersAccessFactory.cpp $
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

#include "core/data_access_interface/radio/src/RAPatchMembersAccessFactory.h"
#include "core/data_access_interface/radio/src/RASubscribers.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    TA_Base_Core::NonReEntrantThreadLockable RAPatchMembersAccessFactory::m_singletonLock;
	RAPatchMembersAccessFactory* RAPatchMembersAccessFactory::s_instance = NULL;


    RAPatchMembersAccessFactory& RAPatchMembersAccessFactory::getInstance()
    {
        ThreadGuard guard( m_singletonLock );

        if (s_instance == NULL)
        {
            s_instance = new RAPatchMembersAccessFactory;
        }
        return *s_instance;
    }


    RAPatchMembersAccessFactory::~RAPatchMembersAccessFactory()
    {

    }


    /**
    *
    *
    */
    std::vector<IRASubscribers*> RAPatchMembersAccessFactory::getPatchSubscribers(const bool readWrite) const
    {
		// Perform SQL here
		std::vector< IRASubscribers* > groups;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Read);

		// create the SQL string 
        //std::string sql("select PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID from TA_SESSION");
        /*std::stringstream s;
        s   << "select unique "
            << "   PATCH_RASUBS_ID,"
            << "	   RASUBS.SUBSCRIBER_NAME,"
            << "	   RASUBS.SUBSCRIBER_TYPE,"
            << "	   RASUBS.LOCATION, "
            << "	   nvl(RASUBS.LOCATION_KEY,0) as LOCATION_KEY"
            << " from "
            << "  RA_PATCH_MEMBERS RAPMEM inner join RA_SUBSCRIBERS RASUBS "
            << "    on (PATCH_RASUBS_ID = RASUBS_ID) "
            << std::ends;
        std::string sql(s.str()); */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_SELECT_34553);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_SELECT_34553);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_SELECT_34553);
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::string groupIDColumn("PATCH_RASUBS_ID");
		std::string subscriberNameColumn("SUBSCRIBER_NAME");
        std::string subscriberTypeColumn ("SUBSCRIBER_TYPE");
		std::string locationColumn ("LOCATION");
		std::string locationKeyColumn ("LOCATION_KEY");
        std::vector<std::string> columnNames;
        columnNames.push_back(groupIDColumn);
        columnNames.push_back(subscriberNameColumn);
        columnNames.push_back(subscriberTypeColumn);
        columnNames.push_back(locationColumn);
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
                        data->getStringData(i, groupIDColumn ),
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

    /**
    *
    *
    */
    RAPatchMembersAccessFactory::PatchMembersType 
        RAPatchMembersAccessFactory::getPatchMembers(const std::string& patchTSI, const bool readWrite) const
    {
		// Perform SQL here
		PatchMembersType groupMembers;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Read);

		// create the SQL string 
        //std::string sql("select PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID from TA_SESSION");
        /*std::stringstream s;
        s   << "select  "
            << "	   0 AS RAPMEM_ID,  "
            << "	   PATCH_RASUBS_ID AS RASUBS_ID, "
            << "	   P.SUBSCRIBER_NAME, "
            << "	   P.SUBSCRIBER_TYPE, "
            << " 	   P.LOCATION, "
            << "	   nvl(P.LOCATION_KEY,0) as LOCATION_KEY"
            << " from  "
            << "     RA_PATCH_MEMBERS RAPMEM inner join RA_SUBSCRIBERS P "
            << "       ON (PATCH_RASUBS_ID = RASUBS_ID) "
            << " where  RAPMEM.PATCH_RASUBS_ID = '" << databaseConnection->escapeQueryString(patchTSI) << "' "
            << " union "
            << " select  "
            << "	   RAPMEM_ID,  "
            << "	   G.RASUBS_ID, "
            << "	   G.SUBSCRIBER_NAME, "
            << "	   G.SUBSCRIBER_TYPE, "
            << "	   G.LOCATION, "
            << "	   nvl(G.LOCATION_KEY,0) as LOCATION_KEY "
            << " from  "
            << "     RA_PATCH_MEMBERS RAPMEM inner join RA_SUBSCRIBERS G "
            << "       ON (GROUP_RASUBS_ID = RASUBS_ID) "
            << " where  RAPMEM.PATCH_RASUBS_ID = '" << databaseConnection->escapeQueryString(patchTSI) << "' "
            << " order by RAPMEM_ID "
            << std::ends;
        std::string sql(s.str());*/ 
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_SELECT_34554,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_SELECT_34554,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_SELECT_34554,
			databaseConnection->escapeQueryString(patchTSI), databaseConnection->escapeQueryString(patchTSI));
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::string ragmemIDColumn("RAPMEM_ID");
        std::string rasubsIDColumn("RASUBS_ID");
		std::string subscriberNameColumn("SUBSCRIBER_NAME");
        std::string subscriberTypeColumn ("SUBSCRIBER_TYPE");
		std::string locationColumn ("LOCATION");
		std::string locationKeyColumn ("LOCATION_KEY");
        std::vector<std::string> columnNames;
        columnNames.push_back(ragmemIDColumn);
        columnNames.push_back(rasubsIDColumn);
        columnNames.push_back(subscriberNameColumn);
        columnNames.push_back(subscriberTypeColumn);
        columnNames.push_back(locationColumn);
        columnNames.push_back(locationKeyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
        // Loop for each row returned
        bool donePatch = false;
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    if (!donePatch) 
                    {
                        IRASubscribers* group = new RASubscribers( 
                            data->getStringData(i, rasubsIDColumn ),
                            data->getStringData(i, subscriberNameColumn ),
                            static_cast<IRASubscribers::SubscriberType>(data->getStringData(i, subscriberTypeColumn )[0]),
                            data->getStringData(i, locationColumn ),
                            data->getUnsignedLongData(i, locationKeyColumn ),
                            false); // false indicates not a new session
                        groupMembers.first = group;
                        donePatch = true;
                    } 
                    else 
                    {
                        IRASubscribers* subscriber = new RASubscribers( 
                            data->getStringData(i, rasubsIDColumn ),
                            data->getStringData(i, subscriberNameColumn ),
                            static_cast<IRASubscribers::SubscriberType>(data->getStringData(i, subscriberTypeColumn )[0]),
                            data->getStringData(i, locationColumn ),
                            data->getUnsignedLongData(i, locationKeyColumn ),
                            false); // false indicates not a new session
                        groupMembers.second.push_back( subscriber );
                    }
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

		return groupMembers;
    }
	
	//TD18317, zhanghongzhi
	std::string RAPatchMembersAccessFactory::getLocalDatabaseName()
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

    void RAPatchMembersAccessFactory::addGroupToPatch( IRASubscribers& patchSubscriber,  IRASubscribers& groupSubscriber)
    {
        // check that group is a group, and member is a member
        TA_ASSERT(IRASubscribers::PATCH_GROUP == patchSubscriber.getSubscriberType(), "patchSubscriber is not a patch_group");
        TA_ASSERT(IRASubscribers::GROUP == groupSubscriber.getSubscriberType(), "groupSubscriber is not a group");

        // attempt to add - don't worry about duplicates unique key on group/member will save us !
        // getTSI() may reload - this is a good sanity check for purity of data - fk constraint will get you even if we didn't reload.

        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Write);

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Writing RAPatchMember (Patch, Group) = (%s, %s) to the database.",
            patchSubscriber.getTSI().c_str(), patchSubscriber.getTSI().c_str());
	 
       /* std::stringstream s;		//TD18317, zhanghongzhi
		s   << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
            << "insert into RA_PATCH_MEMBERS (PATCH_RASUBS_ID, GROUP_RASUBS_ID) "
            << " values ( "
            << " ''" << patchSubscriber.getTSI() << "'', "
            << " ''" << groupSubscriber.getTSI() << "'' "
            << " )" 
			<< "'); END;"
            << std::ends;
        std::string sql(s.str()); */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_INSERT_34852,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_INSERT_34852,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_INSERT_34852,
			getLocalDatabaseName(), patchSubscriber.getTSI(), groupSubscriber.getTSI());

        // This will throw an exception upon failure - ready to catch??
        databaseConnection->executeModification(strSql);

    }


    void RAPatchMembersAccessFactory::deleteGroupFromPatch( IRASubscribers& patchSubscriber,  IRASubscribers& groupSubscriber)
    {
        // check that group is a group, and member is a member
        TA_ASSERT(IRASubscribers::PATCH_GROUP == patchSubscriber.getSubscriberType(), "patchSubscriber is not a patch_group");
        TA_ASSERT(IRASubscribers::GROUP == groupSubscriber.getSubscriberType(), "groupSubscriber is not a group");

        // attempt to add - don't worry about duplicates unique key on patch/groupwill save us !
        // getTSI() may reload - this is a good sanity check for purity of data - fk constraint will get you even if we didn't reload.

        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Write);

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Writing RAPatchMember (Patch, Group) = (%s, %s) to the database.",
            patchSubscriber.getTSI().c_str(), groupSubscriber.getTSI().c_str());
	 
       /* std::stringstream s;		//TD18317, zhanghongzhi
		s   << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
            << "delete from RA_PATCH_MEMBERS where PATCH_RASUBS_ID = "
            << " ''" << patchSubscriber.getTSI() << "'' "
            << " and GROUP_RASUBS_ID = ''" << groupSubscriber.getTSI() << "'' "
			<< "'); END;"
            << std::ends;
        std::string sql(s.str()); */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34952,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34952,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34952,
			getLocalDatabaseName(), patchSubscriber.getTSI(), groupSubscriber.getTSI());

        // This will throw an exception upon failure - ready to catch??
        databaseConnection->executeModification(strSql);

    }

} //end namespace TA_Base_Core



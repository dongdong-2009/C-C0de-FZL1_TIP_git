 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RAGroupMembersAccessFactory.cpp $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RAGroupMembersAccessFactory is a singleton that is used to retrieve operator objects either from the
  * database or newly created. All operator objects returned will adhear to the IRAGroupMembers interface.
  */
#if 0
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif
#endif

#include <sstream>

#include "core/data_access_interface/radio/src/RAGroupMembersAccessFactory.h"
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
    TA_Base_Core::NonReEntrantThreadLockable RAGroupMembersAccessFactory::m_singletonLock;
	RAGroupMembersAccessFactory* RAGroupMembersAccessFactory::s_instance = NULL;


    RAGroupMembersAccessFactory& RAGroupMembersAccessFactory::getInstance()
    {
        ThreadGuard guard( m_singletonLock );

        if (s_instance == NULL)
        {
            s_instance = new RAGroupMembersAccessFactory;
        }
        return *s_instance;
    }


    RAGroupMembersAccessFactory::~RAGroupMembersAccessFactory()
    {

    }


    std::vector<IRASubscribers*> RAGroupMembersAccessFactory::getGroupSubscribers(const bool readWrite) const
    {
		// Perform SQL here
		std::vector< IRASubscribers* > groups;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Read);

		// create the SQL string 
        //std::string sql("select PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID from TA_SESSION");
        /*std::stringstream s;
        s   << "select unique "
            << "   GROUP_RASUBS_ID,"
            << "	   RASUBS.SUBSCRIBER_NAME,"
            << "	   RASUBS.SUBSCRIBER_TYPE,"
            << "	   RASUBS.LOCATION,"
            << "	   nvl(RASUBS.LOCATION_KEY,0) as LOCATION_KEY "
            << " from "
            << "     RA_GROUP_MEMBERS RAGMEM inner join RA_SUBSCRIBERS RASUBS "
            << "       on (GROUP_RASUBS_ID = RASUBS_ID) "
            << std::ends;
        std::string sql(s.str());*/ 
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_SELECT_34551);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_SELECT_34551);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_SELECT_34551);
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::string groupIDColumn("GROUP_RASUBS_ID");
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

    RAGroupMembersAccessFactory::GroupMembersType 
        RAGroupMembersAccessFactory::getGroupMembers(const std::string& groupTSI, const bool readWrite) const
    {
		// Perform SQL here
		GroupMembersType groupMembers;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Read);

		// create the SQL string 
        //std::string sql("select PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID from TA_SESSION");
        /*std::stringstream s;
        s   << "select  "
            << "	   0 AS RAGMEM_ID,  "
            << "	   GROUP_RASUBS_ID AS RASUBS_ID, "
            << "	   G.SUBSCRIBER_NAME, "
            << "	   G.SUBSCRIBER_TYPE, "
            << " 	   G.LOCATION, "
            << "	   nvl(G.LOCATION_KEY,0) as LOCATION_KEY"
            << " from  "
            << "	 RA_GROUP_MEMBERS RAGMEM inner join RA_SUBSCRIBERS G "
            << "       ON (GROUP_RASUBS_ID = RASUBS_ID) "
            << " where RAGMEM.GROUP_RASUBS_ID = '" << databaseConnection->escapeQueryString(groupTSI) << "' "
            << " union "
            << " select  "
            << "	   RAGMEM_ID,  "
            << "	   M.RASUBS_ID, "
            << "	   M.SUBSCRIBER_NAME, "
            << "	   M.SUBSCRIBER_TYPE, "
            << "	   M.LOCATION, "
            << "	   nvl(M.LOCATION_KEY,0) as LOCATION_KEY"
            << " from  "
            << "	 RA_GROUP_MEMBERS RAGMEM inner join RA_SUBSCRIBERS   M "
            << "       ON (MEMBER_RASUBS_ID = RASUBS_ID) "
            << " where RAGMEM.GROUP_RASUBS_ID = '" << databaseConnection->escapeQueryString(groupTSI) << "' "
            << " order by RAGMEM_ID "
            << std::ends;
        std::string sql(s.str()); */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_SELECT_34552,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_SELECT_34552,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_SELECT_34552,
			databaseConnection->escapeQueryString(groupTSI), databaseConnection->escapeQueryString(groupTSI));
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::string ragmemIDColumn("RAGMEM_ID");
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
        bool doneGroup = false;
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    if (!doneGroup) 
                    {
                        IRASubscribers* group = new RASubscribers( 
                            data->getStringData(i, rasubsIDColumn ),
                            data->getStringData(i, subscriberNameColumn ),
                            static_cast<IRASubscribers::SubscriberType>(data->getStringData(i, subscriberTypeColumn )[0]),
                            data->getStringData(i, locationColumn ),
                            data->getUnsignedLongData(i, locationKeyColumn ),
                            false); // false indicates not a new session
                        groupMembers.first = group;
                        doneGroup = true;
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
	std::string RAGroupMembersAccessFactory::getLocalDatabaseName()
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
			localDatabaseName = connections[0].first; */ 
		}
		catch(...) // most likely a file not found error
		{
			throw DatabaseException("Unable to find database connection infomation");
		}
		return localDatabaseName;
	}
    
    void RAGroupMembersAccessFactory::addMemberToGroup( IRASubscribers& groupSubscriber,  IRASubscribers& memberSubscriber)
    {
        // check that group is a group, and member is a member
        TA_ASSERT(IRASubscribers::GROUP == groupSubscriber.getSubscriberType(), "groupSubscriber is not a group");
        TA_ASSERT(IRASubscribers::GROUP != memberSubscriber.getSubscriberType() &&
            IRASubscribers::PATCH_GROUP != memberSubscriber.getSubscriberType(), "memberSubscriber is not an individual");

        // attempt to add - don't worry about duplicates unique key on group/member will save us !
        // getTSI() may reload - this is a good sanity check for purity of data - fk constraint will get you even if we didn't reload.

        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Write);

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Writing RAGroupMember (Group, Member) = (%s, %s) to the database.",
            groupSubscriber.getTSI().c_str(), memberSubscriber.getTSI().c_str());
	 
       /* std::stringstream s;		//TD18317, zhanghongzhi
		s   << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
            << "insert into RA_GROUP_MEMBERS (GROUP_RASUBS_ID, MEMBER_RASUBS_ID) "
            << " values ( "
            << " ''" << groupSubscriber.getTSI() << "'', "
            << " ''" << memberSubscriber.getTSI() << "'' "
            << " )" 
			<< "'); END;"
            << std::ends;
        std::string sql(s.str()); */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_INSERT_34851,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_INSERT_34851,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_INSERT_34851,
			getLocalDatabaseName(), groupSubscriber.getTSI(), memberSubscriber.getTSI());

        // This will throw an exception upon failure - ready to catch??
        databaseConnection->executeModification(strSql);

    }

    void RAGroupMembersAccessFactory::deleteMemberFromGroup( IRASubscribers& groupSubscriber,  IRASubscribers& memberSubscriber)
    {
        // check that group is a group, and member is a member
        TA_ASSERT(IRASubscribers::GROUP == groupSubscriber.getSubscriberType(), "groupSubscriber is not a group");
        TA_ASSERT(IRASubscribers::GROUP != memberSubscriber.getSubscriberType() &&
            IRASubscribers::PATCH_GROUP != memberSubscriber.getSubscriberType(), "memberSubscriber is not an individual");

        // attempt to add - don't worry about duplicates unique key on group/member will save us !
        // getTSI() may reload - this is a good sanity check for purity of data - fk constraint will get you even if we didn't reload.

        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Write);

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Writing RAGroupMember (Group, Member) = (%s, %s) to the database.",
            groupSubscriber.getTSI().c_str(), memberSubscriber.getTSI().c_str());
	 
       /* std::stringstream s;		//TD18317, zhanghongzhi
		s   << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
            << "delete from RA_GROUP_MEMBERS where GROUP_RASUBS_ID = "
            << " ''" << groupSubscriber.getTSI() << "'' "
            << " and MEMBER_RASUBS_ID = ''" << memberSubscriber.getTSI() << "'' "
			<< "'); END;"
            << std::ends;
        std::string sql(s.str()); */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_DELETE_34951,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_DELETE_34951,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_DELETE_34951,
			getLocalDatabaseName(), groupSubscriber.getTSI(), memberSubscriber.getTSI());

        // This will throw an exception upon failure - ready to catch??
        databaseConnection->executeModification(strSql);

    }
    
} //end namespace TA_Base_Core



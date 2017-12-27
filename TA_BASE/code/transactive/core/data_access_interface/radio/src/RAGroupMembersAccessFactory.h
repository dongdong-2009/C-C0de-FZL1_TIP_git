/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RAGroupMembersAccessFactory.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * RAGroupMembersAccessFactory is a singleton that is used to retrieve RAGroupMembers objects either from the
  * database or newly created. All RAGroupMembers objects returned will adhear to the IRAGroupMembers interface.
  */



#if !defined(RAGroupMembersAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define RAGroupMembersAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <utility>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/radio/src/IRASubscribers.h"
//#include "core/data_access_interface/radio/src/IRAGroupMembers.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

	class RAGroupMembersAccessFactory
	{
    public:
        typedef std::pair<IRASubscribers*, std::vector<IRASubscribers*> >  GroupMembersType;

    public:
        virtual ~RAGroupMembersAccessFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a ComponentAccessFactory object.
         */
        static RAGroupMembersAccessFactory& getInstance();


        /**
         * getRAGroupMembers
         *
         * Retrieves the specified RAGroupMembers and returns the associated data as an 
         * object conforming to the IRAGroupMembers interface.
         *
         * @param RAGroupMembersKey The RASUBS_ID to the RAGroupMembers to retrieve
         *
         * @return A pointer to an object conforming to the IRAGroupMembers interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRAGroupMembers 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no Session
         *            matching the supplied PKEY, or if there is more than one Session
         *            matching the supplied PKEY. 
         */
//        IRAGroupMembers* getRAGroupMembers(unsigned long RAGroupMembersKey);


        /** 
         * getGroupSubscribers
         *
         * Retreives all Subscribers who are Groups
         * as a vector of objects conforming to the IRAGroupMembers 
		 * interface.
         *
         * @return A vector of pointers to objects conforming to the IRAGroupMembers interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISubscribers
         * objects
         *
         * @param readWrite  True if write access is required.
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         *
         */
		std::vector<IRASubscribers*> getGroupSubscribers(const bool readWrite = false) const;

        /** 
         * getGroupMembers
         *
         * Retreives all Members who are in a particular Group
         * as a vector of objects conforming to the IRAGroupMembers 
		 * interface.
         *
         * First element is the Group. // TODO: return a tuple instead to separate group/
         *
         * @return A vector of pointers to objects conforming to the IRAGroupMembers interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISubscribers
         * objects
         *
         * @param groupTSI
         * @param readWrite  True if write access is required.
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         *
         */
        GroupMembersType getGroupMembers(const std::string& groupTSI, const bool readWrite = false) const;


        
        void addMemberToGroup(IRASubscribers& groupSubscriber, IRASubscribers& memberSubscriber);

        void deleteMemberFromGroup(IRASubscribers& groupSubscriber, IRASubscribers& memberSubscriber);

    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        RAGroupMembersAccessFactory() { };
        RAGroupMembersAccessFactory& operator=(const RAGroupMembersAccessFactory &) { };
        RAGroupMembersAccessFactory( const RAGroupMembersAccessFactory& ) {};

        static RAGroupMembersAccessFactory* s_instance;
        static NonReEntrantThreadLockable m_singletonLock;	

        //TD18317, zhanghongzhi
		std::string getLocalDatabaseName();

    };

} // closes TA_Base_Core

#endif // !defined(RAGroupMembersAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASubscribersAccessFactory.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * RASubscribersAccessFactory is a singleton that is used to retrieve RASubscribers objects either from the
  * database or newly created. All RASubscribers objects returned will adhear to the IRASubscribers interface.
  */



#if !defined(RASubscribersAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define RASubscribersAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/radio/src/IRASubscribers.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

	class RASubscribersAccessFactory
	{

    public:
        virtual ~RASubscribersAccessFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a ComponentAccessFactory object.
         */
        static RASubscribersAccessFactory& getInstance();


        /**
         * getSubscriberDetails
         *
         * Retrieves the specified RASubscribers and returns the associated data as an 
         * object conforming to the IRASubscribers interface.
         *
         * @param RASubscribersKey The RASUBS_ID to the RASubscribers to retrieve
         *
         * @return A pointer to an object conforming to the IRASubscribers interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRASubscribers 
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
        IRASubscribers* getSubscriberDetails(std::string TSI);


        /** 
         * getIndividualSubscribers
         *
         * Retreives all Subscribers who are Individuals
         * as a vector of objects conforming to the IRASubscribers 
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
        std::vector<IRASubscribers*> getIndividualSubscribers(const bool readWrite = false) const;

        /** 
         * searchSubscribers
         *
         * Retreives all Subscribers who match the search condition
         * as a vector of objects conforming to the IRASubscribers 
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
        std::vector<IRASubscribers*> searchSubscribers(const std::string& searchText, const bool readWrite = false) const;

        IRASubscribers* createSubscriber(IRASubscribers * x);
        void deleteSubscriber(IRASubscribers* subscriber);

        /**
         * deleteGroup
         *
         * Deletes a group from the subscriber tables, and removes all references to it in the group members
         * table.
         *
         * @param groupITSI
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        void deleteGroup(const std::string& groupITSI);

        /**
         * deletePatch
         *
         * Deletes a group from the subscriber tables, and removes all references to it in the group members
         * table.
         *
         * @param groupITSI
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        void deletePatch(const std::string& patchITSI);

    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        RASubscribersAccessFactory() { };
        RASubscribersAccessFactory& operator=(const RASubscribersAccessFactory &) { };
        RASubscribersAccessFactory( const RASubscribersAccessFactory& ) {};

        static RASubscribersAccessFactory* s_instance;
        static NonReEntrantThreadLockable m_singletonLock;	

		//TD18317, zhanghongzhi
		std::string getLocalDatabaseName();
	};
} // closes TA_Base_Core

#endif // !defined(RASubscribersAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

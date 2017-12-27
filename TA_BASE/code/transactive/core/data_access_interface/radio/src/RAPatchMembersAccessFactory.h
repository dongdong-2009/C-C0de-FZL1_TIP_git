/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RAPatchMembersAccessFactory.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * RAPatchMembersAccessFactory is a singleton that is used to retrieve RAPatchMembers objects either from the
  * database or newly created. All RAPatchMembers objects returned will adhear to the IRAPatchMembers interface.
  */



#if !defined(RAPatchMembersAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define RAPatchMembersAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/radio/src/IRASubscribers.h"
//#include "core/data_access_interface/radio/src/IRAPatchMembers.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

	class RAPatchMembersAccessFactory
	{
    public:
        typedef std::pair<IRASubscribers*, std::vector<IRASubscribers*> >  PatchMembersType;

    public:
        virtual ~RAPatchMembersAccessFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a ComponentAccessFactory object.
         */
        static RAPatchMembersAccessFactory& getInstance();


        /**
         * getRAPatchMembers
         *
         * Retrieves the specified RAPatchMembers and returns the associated data as an 
         * object conforming to the IRAPatchMembers interface.
         *
         * @param RAPatchMembersKey The RASUBS_ID to the RAPatchMembers to retrieve
         *
         * @return A pointer to an object conforming to the IRAPatchMembers interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRAPatchMembers 
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
//        IRAPatchMembers* getRAPatchMembers(unsigned long RAPatchMembersKey);


        /** 
         * getPatchSubscribers
         *
         * Retreives all Subscribers who are Patchs
         * as a vector of objects conforming to the IRAPatchMembers 
		 * interface.
         *
         * @return A vector of pointers to objects conforming to the IRAPatchMembers interface.
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
		std::vector<IRASubscribers*> getPatchSubscribers(const bool readWrite = false) const;

        /** 
         * getPatchMembers
         *
         * Retreives all Members who are in a particular Patch
         * as a vector of objects conforming to the IRAPatchMembers 
		 * interface.
         *
         * First element is the Patch. // TODO: return a tuple instead to separate group/
         *
         * @return A vector of pointers to objects conforming to the IRAPatchMembers interface.
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
        PatchMembersType getPatchMembers(const std::string& groupTSI, const bool readWrite = false) const;

    
        void addGroupToPatch( IRASubscribers& patchSubscriber,  IRASubscribers& groupSubscriber);

        void deleteGroupFromPatch( IRASubscribers& patchSubscriber,  IRASubscribers& groupSubscriber);

    
    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        RAPatchMembersAccessFactory() { };
        RAPatchMembersAccessFactory& operator=(const RAPatchMembersAccessFactory &) { };
        RAPatchMembersAccessFactory( const RAPatchMembersAccessFactory& ) {};

        static RAPatchMembersAccessFactory* s_instance;
        static NonReEntrantThreadLockable m_singletonLock;	

        //TD18317, zhanghongzhi
		std::string getLocalDatabaseName();

};

} // closes TA_Base_Core

#endif // !defined(RAPatchMembersAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

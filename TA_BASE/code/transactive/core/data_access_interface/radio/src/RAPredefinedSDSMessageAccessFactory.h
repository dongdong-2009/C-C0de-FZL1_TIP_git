/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RAPredefinedSDSMessageAccessFactory.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * RAPredefinedSDSMessageAccessFactory is a singleton that is used to retrieve RAPredefinedSDSMessage objects either from the
  * database or newly created. All RAPredefinedSDSMessage objects returned will adhear to the IRAPredefinedSDSMessage interface.
  */



#if !defined(RAPredefinedSDSMessageAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define RAPredefinedSDSMessageAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/radio/src/IRAPredefinedSDSMessage.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

	class RAPredefinedSDSMessageAccessFactory
	{

    public:
        virtual ~RAPredefinedSDSMessageAccessFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a ComponentAccessFactory object.
         */
        static RAPredefinedSDSMessageAccessFactory& getInstance();


        /**
         *
         * Retrieves the specified RAPredefinedSDSMessage and returns the associated data as an 
         * object conforming to the IRAPredefinedSDSMessage interface.
         *
         * @param RAPredefinedSDSMessageKey The RASUBS_ID to the RAPredefinedSDSMessage to retrieve
         *
         * @return A pointer to an object conforming to the IRAPredefinedSDSMessage interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRAPredefinedSDSMessage 
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
        IRAPredefinedSDSMessage* getByShortName(std::string shortName);


        /** 
         *
         * Retreives all Subscribers who are Individuals
         * as a vector of objects conforming to the IRAPredefinedSDSMessage 
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
        std::vector<IRAPredefinedSDSMessage*> getList(const bool readWrite = false) const;


        IRAPredefinedSDSMessage* createMessage(IRAPredefinedSDSMessage * x);
        void deleteMessage(IRAPredefinedSDSMessage* subscriber);


    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        RAPredefinedSDSMessageAccessFactory() { };
        RAPredefinedSDSMessageAccessFactory& operator=(const RAPredefinedSDSMessageAccessFactory &) { };
        RAPredefinedSDSMessageAccessFactory( const RAPredefinedSDSMessageAccessFactory& ) {};

        static RAPredefinedSDSMessageAccessFactory* s_instance;
        static NonReEntrantThreadLockable m_singletonLock;	};

} // closes TA_Base_Core

#endif // !defined(RAPredefinedSDSMessageAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

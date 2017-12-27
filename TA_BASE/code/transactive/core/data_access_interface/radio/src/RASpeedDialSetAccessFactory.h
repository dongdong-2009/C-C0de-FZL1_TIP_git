/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASpeedDialSetAccessFactory.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * RASpeedDialSetAccessFactory is a singleton that is used to retrieve RASpeedDialSet objects either from the
  * database or newly created. All RASpeedDialSet objects returned will adhear to the IRASpeedDialSet interface.
  */



#if !defined(RASpeedDialSetAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define RASpeedDialSetAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/radio/src/RASpeedDialSet.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

	class RASpeedDialSetAccessFactory
	{

    public:
        virtual ~RASpeedDialSetAccessFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a ComponentAccessFactory object.
         */
        static RASpeedDialSetAccessFactory& getInstance();


        /**
         * getRASpeedDialSet
         *
         * Retrieves the specified RASpeedDialSet and returns the associated data as an 
         * object conforming to the IRASpeedDialSet interface.
         *
         * @param RASpeedDialSetKey The RASDSE_ID to the RASpeedDialSet to retrieve
         *
         * @return A pointer to an object conforming to the IRASpeedDialSet interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRASpeedDialSet 
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
        IRASpeedDialSet* getRASpeedDialSetDetails(unsigned long key);

        IRASpeedDialSet* createSpeedDialSet(IRASpeedDialSet* x);
        IRASpeedDialSet* getRASpeedDialSetByProfile(unsigned long profileID);
        void deleteSpeedDialSet(IRASpeedDialSet* x);

    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        RASpeedDialSetAccessFactory() { };
        RASpeedDialSetAccessFactory& operator=(const RASpeedDialSetAccessFactory &) { };
        RASpeedDialSetAccessFactory( const RASpeedDialSetAccessFactory& ) {};

        static RASpeedDialSetAccessFactory* s_instance;
        static NonReEntrantThreadLockable m_singletonLock;	};

} // closes TA_Base_Core

#endif // !defined(RASpeedDialSetAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

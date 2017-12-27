/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RACallStackHistoryAccessFactory.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * RACallStackHistoryAccessFactory is a singleton that is used to retrieve RACallStackHistory objects either from the
  * database or newly created. All RACallStackHistory objects returned will adhear to the IRACallStackHistory interface.
  */



#if !defined(RACallStackHistoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define RACallStackHistoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <utility>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/radio/src/IRACallStackHistory.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_Base_Core
{

	class RACallStackHistoryAccessFactory
	{
    public:
        typedef std::vector<IRACallStackHistory*> CallStackHistoryList;

    public:
        virtual ~RACallStackHistoryAccessFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a ComponentAccessFactory object.
         */
        static RACallStackHistoryAccessFactory& getInstance();


        /**
         * getRACallStackHistory
         *
         * Retrieves the specified RACallStackHistory and returns the associated data as an 
         * object conforming to the IRACallStackHistory interface.
         *
         * @param RACallStackHistoryKey The RASUBS_ID to the RACallStackHistory to retrieve
         *
         * @return A pointer to an object conforming to the IRACallStackHistory interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRACallStackHistory 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no Session
         *            matching the supplied PKEY, or if there is more than one Session
         *            matching the supplied PKEY. 
         */
//        IRACallStackHistory* getRACallStackHistory(unsigned long RACallStackHistoryKey);


        /** 
         * @return A vector of pointers to objects conforming to the IRACallStackHistory interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISubscribers
         * objects
         *
         * @param readWrite  True if write access is required.
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         *
         */
//		RACallStackHistoryList getList(const bool readWrite = false) const;
		CallStackHistoryList getList(const std::string & profile, const unsigned long & locationKey, const bool readWrite = false) const;

        
//        void addCallStackEntry(IRACallStackHistory& entry) const;
        void addCallStackEntry(
			//const unsigned long& key,
            const std::string& profile,
            const unsigned long& locationKey,
			const time_t& timestamp,
			IRACallStackHistory::OriginType originType,
            IRACallStackHistory::CallType callType,
            const std::string& identifier,
            const std::string& location,
			bool isEmergency
			);

    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        RACallStackHistoryAccessFactory() { };
        RACallStackHistoryAccessFactory& operator=(const RACallStackHistoryAccessFactory &) { };
        RACallStackHistoryAccessFactory( const RACallStackHistoryAccessFactory& ) {};

        static RACallStackHistoryAccessFactory* s_instance;
        static NonReEntrantThreadLockable m_singletonLock;	

        //TD18473, zhanghongzhi
		std::string getLocalDatabaseName();
    };

} // closes TA_Base_Core

#endif // !defined(RACallStackHistoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)

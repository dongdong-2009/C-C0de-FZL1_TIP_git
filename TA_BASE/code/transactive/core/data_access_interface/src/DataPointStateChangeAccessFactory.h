/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DataPointStateChangeAccessFactory.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * DataPointStateChangeAccessFactory is a singleton that is used to retrieve DataPointStateChangeData objects from the 
 * database.
 * It provides read-write objects already in the database, and creates new read-write objects for addition to the database.
 */

#if !defined(AFX_DATAPOINTSTATECHANGEACCESSFACTORY_H__2F662A5E_3A24_4888_9974_454A6828B6CB__INCLUDED_)
#define AFX_DATAPOINTSTATECHANGEACCESSFACTORY_H__2F662A5E_3A24_4888_9974_454A6828B6CB__INCLUDED_

#include <map>
#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabaseCallbackUser.h"
#include "core/data_access_interface/src/IApplicationObserver.h"
#include "core/database/src/CommonType.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    // Forward delaration to help the compiler avoid unnecessary compilation
    class IDataPointStateChangeData;
	class IDatabase;

    class DataPointStateChangeAccessFactory : public IApplicationObserver 
    {

    public:

        struct StateChangeFilter
        {
            std::string entityName;
            std::vector<std::string> value;
            std::vector<std::string> status;
            std::vector<unsigned long> subsystemIds;
            std::vector<unsigned long> locationIds;
        };


        virtual ~DataPointStateChangeAccessFactory() 
		{

		};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an DataPointStateChangeAccessFactory object.
         */
        static DataPointStateChangeAccessFactory& getInstance();

		

        /**
         * getDataPointStateChange
         *
         * Returns the Message associated with the specified key as a pointer
         * to an object conforming to the IDataPointStateChangeData interface.
         *
         * @param key The database key to the Message to retrieve
         *
         * @return A pointer to an object conforming to the IDataPointStateChangeData interface.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no Message 
         *            matching the supplied key, or if there is more than one Message 
         *            matching the supplied key. 
         */
        IDataPointStateChangeData* getDataPointStateChange(const std::string& key);
        

        /**
         * getDataPointStateChanges
         *
         * Returns the Message associated with the specified key as a pointer
         * to an object conforming to the IDataPointStateChangeData interface.
         *
         * @param key The database key to the Message to retrieve
         *
         * @return A pointer to an object conforming to the IDataPointStateChangeData interface.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no Message 
         *            matching the supplied key, or if there is more than one Message 
         *            matching the supplied key. 
         */
        std::vector<IDataPointStateChangeData*> getDataPointStateChanges();

        
        /**
         * getStateChangesBetweenTimes
         *
         * Returns all messages in the system. This method does not load the details of these messages, instead returning
         * the keys of the message.
         *
         * @param fromTime The time after which to retrieve the events
         * @param toTime The time until which to retreive events
         * @param maxEvents The maximum number of events that are to be selected retrieved for this query.
         * @param filter The filter object that specifies the filter criteria for this selection. NB: This object is deleted
         * by the access factory when used.
         * @param callbackUser The callback object to allow GUI updates during a long load process.
         *
         * @return A vector of pointers to objects conforming to the IDataPointStateChangeData interface.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no Message 
         *            matching the supplied key, or if there is more than one Message 
         *            matching the supplied key. 
         */
        std::vector<IDataPointStateChangeData*> getStateChangesBetweenTimes(const time_t fromTime,const time_t toTime = 0,
            const unsigned long maxEvents = 0, StateChangeFilter* filter = NULL, IDatabaseCallbackUser* callbackUser = NULL);

        /**
         * createMessage
         *
         * Returns a pointer to a new IDataPointStateChangeData instance.
         *
         * @return A pointer to an object conforming to the IDataPointStateChangeData interface
         * that has not been initialised.
         */
        IDataPointStateChangeData* createDataPointStateChange();

        /**
         * applicationTerminating
         *
         * If a loading method has been called, this method can be called to stop the load so it is
         * not necessary to wait for the load to finish before continuing on with execution.
         * NOTE: This method was added to fix TD1308.
         */
        void applicationTerminating();

		/**
         * applicationPausing
         *
         */
        void applicationPausing();

		/**
         * applicationResuming
         *
         */
        void applicationResuming();

		/**
         * getEarliestMessageTime
         *
         */
        time_t getEarliestMessageTime();

    private:
        DataPointStateChangeAccessFactory();
        DataPointStateChangeAccessFactory( const DataPointStateChangeAccessFactory& theDataPointStateChangeAccessFactory);
        DataPointStateChangeAccessFactory& operator=(const DataPointStateChangeAccessFactory &);
		
		void getDataPointStateChangeBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IDataPointStateChangeData*>& stateChangePointers);
		
    
        static DataPointStateChangeAccessFactory* s_instance;
		// protect singleton creation
		static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;

        bool volatile m_stopLoad;
        
        bool volatile m_pauseLoad;

        std::map< std::string, unsigned long > m_alarmMessageDataPointNameToPKeyMap;
    };
}

#endif // !defined(AFX_DATAPOINTSTATECHANGEACCESSFACTORY_H__2F662A5E_3A24_4888_9974_454A6828B6CB__INCLUDED_)

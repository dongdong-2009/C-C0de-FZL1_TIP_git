/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/MessageAccessFactory.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * MessageAccessFactory is a singleton that is used to retrieve MessageData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */


#if !defined(MessageAccessFactory_E17A1C95_63D1_4d61_A546_CCF8D0E8A633__INCLUDED_)
#define MessageAccessFactory_E17A1C95_63D1_4d61_A546_CCF8D0E8A633__INCLUDED_

#include <vector>
#include "core/data_access_interface/src/IApplicationObserver.h"


namespace TA_Base_Core
{

    // Forward delarations to help the compiler avoid unnecessary compilation
    class IData;
    class IMessageData;
    class IDatabaseCallbackUser;

    class MessageAccessFactory : public IApplicationObserver
    {

    public:
        
        struct MessageFilter
        {
            std::string description;
            std::vector<unsigned long> eventTypes;
            std::string assetName;       // If "", filter on entity name instead.
            std::vector<unsigned long> operatorIds;
            std::vector<unsigned long> severityIds;
            std::vector<unsigned long> subsystemIds;
            std::vector<unsigned long> locationIds;
            std::vector<unsigned long> alarmTypes;
            std::string alarmId;
        };

		static const std::string SESSION_STARTED_EVENT_NAME;
		static const std::string SESSION_ENDED_EVENT_NAME;
		static const std::string OVERRIDE_STARTED_EVENT_NAME;
		static const std::string OVERRIDE_ENDED_EVENT_NAME;

        static const unsigned long INVALID_SEQUENCE_NUMBER;
        static const unsigned long MIN_SEQUENCE_NUMBER;
        static const unsigned long MAX_SEQUENCE_NUMBER;
        static const unsigned long MAX_ROWS;


        virtual ~MessageAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an MessageAccessFactory object.
         */
        static MessageAccessFactory& getInstance();

        /**
         * getMessage
         *
         * Returns the Message associated with the specified key as a pointer
         * to an object conforming to the IMessageData interface.
         *
         * @param key The database key to the Message to retrieve
         *
         * @return A pointer to an object conforming to the IMessageData interface.
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
        IMessageData* getMessage(const std::string& key);

        
        /**
         * getAllMessages
         *
         * Returns all messages in the system. This method does not load the details of these messages, instead returning
         * the keys of the message.
         *
         * @return A vector of pointers to objects conforming to the IMessageData interface.
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
        std::vector<IMessageData*> getAllMessages();

        
        /**
         * getMessagesBetweenTimes
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
         * @return A vector of pointers to objects conforming to the IMessageData interface.
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
//         std::vector<IMessageData*> getMessagesBetweenTimes(const time_t fromTime,const time_t toTime = 0,
//             const unsigned long maxEvents = 0, MessageFilter* filter = NULL, IDatabaseCallbackUser* callbackUser = NULL, bool eventOnly = false);

        /**
         * getAuthMessagesForSession
         *
         * Returns all authentication related messages for a given session.
         *
         * @return A vector of pointers to objects conforming to the IMessageData interface.
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
		std::vector<IMessageData*> getAuthMessagesForSession( const std::string& session );
		
        /**
         * getMessagesFromView
         *
         * Returns up to max messages from the specified view with a pkey newer than the specified message pkey. 
         * Message data contains values for the specified view columns.
         *
         * @param viewName The name of the message view. The view must have a PKEY column.
         * @param columnNames The names of the columns to be extracted.
         * @param precedingMsgPkey Extract messages with PKEY greater than this.
         * @param maxMsgs Extract no more than this number of messages.
         * @return Vector of non NULL pointers to objects conforming to the IData interface.
         *         Can be empty if there are no matching messages.
         *         NB: It is the caller's responsibility to destroy the returned data objects.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         */
//         std::vector<IData*> getMessagesFromView(const std::string& viewName, const std::vector<std::string>& columnNames,
//                                                 unsigned long precedingMsgPkey, unsigned long maxMsgs);


        /**
         * getNextMessageSequenceNumber
         *
         * Returns the next value of the sequence which the event message PKEYs are based on.
         * 
         * @return Next PKEY sequence value.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         */
//         unsigned long getNextMessageSequenceNumber();

        /**
         * createMessage
         *
         * Returns a pointer to a new message instance.
         *
         * @return A pointer to an object conforming to the IMessageData interface
         * that has not been initialised.
         */
        IMessageData* createMessage();

        /**
         * applicationTerminating
         *
         * If a loading method has been called, this method can be called to stop the load so it is
         * not necessary to wait for the load to finish before continuing on with execution.
         * NOTE: This method was added to fix TD1308.
         */
        void applicationTerminating();

        void applicationPausing();

        void applicationResuming();

        void resetStopFlag();

        /*time_t getEarliestMessageTime();*/


        /**
         * getMessageOfEntityType
         *
         * Returns all messages that have been sent by the specified entity type.  
         * If the to and from times are not 0, it will only fetch the events that 
         * have occurred between those 2 times.  If maxEvent is not set to 0, it will
         * fetch up to 'maxEvent' of the latest events from the database; if it is 0, it will
         * fetch up to 20,000 of the latest events from the database; if 
         * it is set to MAX_ULONG, it will fetch any number of entries from the database.
         *
         * If fromTime is specified by toTime is not, it will assume that it will 
         * get events from the specified fromTime to the current time.
         *
         * @param entityType The entity type that this method is going to return
         * @param fromTime The time after which to retrieve the events
         * @param toTime The time until which to retreive events
         * @param maxEvents The maximum number of events that are to be selected retrieved for this query.
         *          If this is set to ULONG_MAX, then it will get any number of rows from the database with no limit
         *
         * @return A vector of pointers to objects conforming to the IMessageData interface.
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
//         std::vector<IMessageData*> getMessageOfEntityType(const unsigned long entityType, 
//                                                             const time_t fromTime = 0,
//                                                             const time_t toTime = 0,
//                                                             const unsigned long maxEvents = 0);
		/**
         * getMessageOfEntity
         *
         * Returns all messages that have been sent by the specified entity.  
         * If the to and from times are not 0, it will only fetch the events that 
         * have occurred between those 2 times.  Otherwise it will fetch all rows.
         * the keys of the message.
         *
         * If fromTime is specified by toTime is not, it will assume that it will 
         * get events from the specified fromTime to the current time.
         *
         * @param entityKey The entity key that this method is going to return
         * @param fromTime The time after which to retrieve the events
         * @param toTime The time until which to retreive events
         * @param maxEvents The maximum number of events that are to be selected retrieved for this query.
         *          If this is set to ULONG_MAX, then it will get any number of rows from the database with no limit
         *
         * @return A vector of pointers to objects conforming to the IMessageData interface.
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
		//TD 15056
		//zhou yuan++		
// 		std::vector<IMessageData*> getMessageOfEntity(const unsigned long entityKey,
// 													  const bool isGetMessageOfOneDay = false,
// 													  const time_t fromTime = 0, 
// 													  const time_t toTime = 0, 
// 													  const unsigned long maxEvents = 0);
		//++zhou yuan

    private:
        MessageAccessFactory();
        MessageAccessFactory( const MessageAccessFactory& theMessageAccessFactory);
        MessageAccessFactory& operator=(const MessageAccessFactory &);
    
        static MessageAccessFactory* m_instance;

        bool volatile m_stopLoad;

        bool volatile m_pauseLoad;

        std::string m_operatorName;
    };
} // closes TA_Base_Core

#endif // !defined(MessageAccessFactory_E17A1C95_63D1_4d61_A546_CCF8D0E8A633__INCLUDED_)

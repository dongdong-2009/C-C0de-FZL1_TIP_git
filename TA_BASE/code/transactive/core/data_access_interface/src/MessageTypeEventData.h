/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/MessageTypeEventData.h $
 * @author:  Lu Xiangmei
 * @version: 
 *
 * Last modification: $DateTime: 
 * Last modified by:  $Author: 
 * 
 * MessageTypeEventData is an implementation of IMessageTypeData. It holds the data specific to a message
 * type entry (for event viewer, i.e., has an related action_id in se_action table)in the database.
 */

#if !defined(MessageTypeEventData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_)
#define MessageTypeEventData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IMessageTypeData.h"

namespace TA_Base_Core
{

	class MessageTypeEventData : public IMessageTypeData
	{

	public:

// 		/**
// 		 * MessageTypeEventData (constructor)
// 		 *
// 		 * Construct a MessageTypeData class for the entry represented by the specified key.
// 		 *
// 		 * @param key The key of this message type in the database
// 		 */
// 		MessageTypeData(const std::string& key);

        /**
		 * MessageTypeEventData (constructor)
		 *
		 * Construct a MessageTypeEventData class for the entry represented by the specified parameters.
		 *
		 * @param key The key of this message type in the database
		 */
		MessageTypeEventData(unsigned long& key,
			const std::string& name,
			const std::string& description,
			int priority,
			bool toBePrinted,
			bool toBeLogged,
			bool toBePersisted,
			unsigned int viewLevel,
			unsigned long actionId,
			bool isEnableRealTimeEvent,
			time_t dateCreated,
			time_t dateModified);


		MessageTypeEventData();
		
		/**
		 * Constructor
		 *
		 * Construct a MessageTypeEventData class based around dataset
		 */
		//MessageTypeEventData(const unsigned long row, TA_Base_Core::IData& data);


		virtual ~MessageTypeEventData()
		{
		};
	
		/**
         * getKey
         *
         * Returns the key for this MessageType.
         *
         * @return The key for this MessageType as an unsigned long.
         */
		unsigned long getKey();


		/**
         * getName
         *
         * Returns the name of this MessageType. If this is the first time data for this MessageType
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this MessageType as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
         */
		std::string getName();


		/**
         * getDescription
         *
         * Returns the description for this MessageType.
         *
         * @return The description for this MessageType as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
         */
		std::string getDescription();


		/**
		 * getPriority
		 *
		 * Returns the prioirity level of this message type.
		 *
		 * @return The priority level of the message type as an unsigned int. This value will be between 0 and 9.
		 * any other value will cause an exception.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved. Additionally, 
		 *            a check is performed to ensure the return value is between 0 and 9.
		 *            If it is not, a DataException will be thrown.
		 *
		 * post: 0 <= return value <= 9
		 */
		unsigned int getPriority();


		/**
		 * toBePrinted
		 *
		 * Returns whether or not this message type should be printed.
		 *
		 * @return true if this message type should be printed.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 */
		bool toBePrinted();


		/**
		 * toBeLogged
		 *
		 * Returns whether or not this message type should be written to the database.
		 *
		 * @return true if this message type should be written to the database.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 */
		bool toBeLogged();


		/**
		 * toBePersisted
		 *
		 * Returns whether or not this message type should be added to a persistent data structure
		 *
		 * @returns true if this message type should be stored in a persistent data structure
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 */
		bool toBePersisted();

        /**
         * getDateCreated
         *
         * Returns the time at which the message type was created.
         *
         * @return time_t The creation time.
         *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved. Additionally, 
		 *            a check is performed to ensure the return value is between 0 and 9.
		 *            If it is not, a DataException will be thrown.
         */
        time_t getDateCreated();

        /**
         * getDateModified
         *
         * Returns the time at which the message type was last modified.
         *
         * @return time_t The modification time.
         *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved. Additionally, 
		 *            a check is performed to ensure the return value is between 0 and 9.
		 *            If it is not, a DataException will be thrown.
         */
        time_t getDateModified();

        /**
         * getViewLevel
         *
         * Returns the view level required to see this message in the event viewer.
         *
         * @return unsigned int The view level.
         *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved. Additionally, 
		 *            a check is performed to ensure the return value is between 0 and 9.
		 *            If it is not, a DataException will be thrown.
         */
        unsigned int getViewLevel();

		unsigned long getActionId();

		bool getIsEnableRealTimeEvent();

		void invalidate(){};

	private:
		//MessageTypeEventData& operator=(const MessageTypeData &);
		//MessageTypeData( const MessageTypeData& theMessageTypeData);

		unsigned long m_key;
		std::string m_name;
		std::string m_description;
		int m_priority;
		bool m_toBePrinted;
		bool m_toBeLogged;
		bool m_toBePersisted;
		unsigned int m_viewLevel;
		unsigned long m_actionId;
		bool m_isEnableRealTimeEvent;
		time_t m_dateCreated;
		time_t m_dateModified;

	};
} // closes TA_Base_Core

#endif // !defined(MessageTypeEventData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_)

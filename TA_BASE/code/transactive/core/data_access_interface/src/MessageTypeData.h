/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/MessageTypeData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/26 14:07:36 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * MessageTypeData is an implementation of IMessageTypeData. It holds the data specific to a message
 * type entry in the database.
 */

#if !defined(MessageTypeData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_)
#define MessageTypeData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IMessageTypeData.h"
#include "core/data_access_interface/src/MessageTypeHelper.h"

namespace TA_Base_Core
{

	class MessageTypeData : public IMessageTypeData
	{

	public:
		/**
		 * MessageTypeData (constructor)
		 *
		 * Construct a MessageTypeData class for the entry represented by the specified key.
		 *
		 * @param key The key of this message type in the database
		 */
		MessageTypeData(const std::string& key);

        /**
		 * MessageTypeData (constructor)
		 *
		 * Construct a MessageTypeData class for the entry represented by the specified parameters.
		 *
		 * @param key The key of this message type in the database
		 */
		MessageTypeData(const std::string& key,
                        const std::string& name,
                        const std::string& description,
                        int priority,
                        bool toBePrinted,
                        bool toBeLogged,
                        bool toBePersisted,
                        unsigned int viewLevel,
						bool isRealTimeEvent,
						unsigned long actionID);

		
		/**
		 * Constructor
		 *
		 * Construct a MessageTypeData class based around dataset
		 */
		MessageTypeData(const unsigned long row, TA_Base_Core::IData& data);


		virtual ~MessageTypeData()
		{
			if (NULL != m_messageHelper)
			{
				delete m_messageHelper;
				m_messageHelper = NULL;
			}
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
		virtual std::string getName();


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

		        /**
         * getActionId
         *
         * Returns the ActionID of this message(join with table se_action), if no record find in se_action table,return 0
         *
         * @return unsigned long The action ID.
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
		virtual unsigned long getActionId() ;


		/**
		 * getIsEnableRealTimeEvent
		 *
		 * Returns whether or not this message type is a real time message.
		 *
		 * @return true if this message type is a real-time message.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
		 */
		virtual bool getIsEnableRealTimeEvent() ;

		


		/**
         * invalidate
         *
         * Mark the data contained by this message type as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

		std::string getDisplayName();

	private:
		MessageTypeData& operator=(const MessageTypeData &);
		MessageTypeData( const MessageTypeData& theMessageTypeData);


		MessageTypeHelper* m_messageHelper;

		std::string m_strKey;
	};
} // closes TA_Base_Core

#endif // !defined(MessageTypeData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_)

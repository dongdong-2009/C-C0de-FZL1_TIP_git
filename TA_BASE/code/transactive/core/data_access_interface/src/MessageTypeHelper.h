 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/MessageTypeHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/01/26 14:07:36 $
  * Last modified by:  $Author: huirong.luo $
  * 
  * MessageTypeHelper is an object that is held by MessageTypeData and ConfigMessageType objects. 
  * It contains all data used by message types, and manipulation
  * methods for the data. It helps avoid re-writing code for both MessageTypeData and ConfigMessageType.
  */


#if !defined(MessageTypeHelper_69D71528_B22E_4881_BF9C_1FDB0D9CC991__INCLUDED_)
#define MessageTypeHelper_69D71528_B22E_4881_BF9C_1FDB0D9CC991__INCLUDED_

#include <string>
#include <sstream>
#include <vector>

namespace TA_Base_Core
{
	// forward declarations
	class IData;

	class MessageTypeHelper
	{

	public:
		/**
		 * MessageTypeHelper
		 *
		 * Constructs a new message helper for the message type associated with the specified key
		 *
		 * @param key The key into the me_message_type table for this message type.
		 */
		MessageTypeHelper(const std::string& key): m_isValidData(false),m_key(key),m_name(""), m_displayname(""), m_description(""),m_priority(10),
            m_toBePrinted(false),m_toBeLogged(false),m_toBePersisted(false), m_dateCreated(0), m_dateModified(0), m_viewLevel(0),
			m_isRealTimeEvent(false), m_actionID(0){};

        /**
		 * MessageTypeHelper
		 *
		 * Constructs a new message helper for the message type associated with the specified key
		 *
		 * @param key The key into the me_message_type table for this message type.
		 */
		MessageTypeHelper(const std::string& key,
                          const std::string& name,
                          const std::string& description,
                          int priority,
                          bool toBePrinted,
                          bool toBeLogged,
                          bool toBePersisted,
                          unsigned int viewLevel,
						  bool isRealTimeEvent=false,
						  unsigned long actionID=0)
            : m_isValidData(true),m_key(key),m_name(name), m_displayname(""),m_description(description),m_priority(priority),
            m_toBePrinted(toBePrinted),m_toBeLogged(toBeLogged),m_toBePersisted(toBePersisted), m_dateCreated(0),
            m_dateModified(0), m_viewLevel(viewLevel),m_isRealTimeEvent(isRealTimeEvent), m_actionID(actionID) {};

	    /**
	      * MessageTypeHelper
	      * 
	      * This constructor creates a new object using the input data
          * which is representative of a row returned from SQL statement
	      * 
	      * @param : const unsigned long row
          *          - the row of data in the data object that we should collect data from
          *          in order to construct ourselves
	      * @param : TA_Base_Core::IData& data
          *          - the IData interface that should have been obtained using the 
          *          getBasicQueryData function
	      * 
	      * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
	      */		
		MessageTypeHelper(const unsigned long row, TA_Base_Core::IData& data);		

        virtual ~MessageTypeHelper() {};


		/**
         * getKey
         *
         * Returns the key for this MessageType.
         *
         * @return The key for this MessageType as an unsigned long.
         *
         * pre: This MessageType has not been deleted
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
         *
         * pre: This MessageType has not been deleted
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
         *
         * pre: This MessageType has not been deleted
         */
		std::string getDescription();

        /**
         * setName
         *
         * Sets the name of this item locally.
         *
         * @param name The name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        void setName(const std::string& name);

		/**
         * setDescription
         *
         * Sets the description of this message type. This should only be used by the ConfigMessageType class
         *
         * @param description The description to give this message type
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int), or if the wrong amount of data is retrieved. Additionally, 
		 *            a check is performed to ensure the return value is between 0 and 9.
		 *            If it is not, a DataException will be thrown.
         *
         * pre: This MessageType has not been deleted
         */
        void setDescription(const std::string& description);

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
         * pre: This MessageType has not been deleted
		 * post: 0 <= return value <= 9
		 */
		unsigned int getPriority();

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
		 * setPriority
		 *
		 * Sets the prioirity level of this message type. This should only be used by the ConfigMessageType class
		 *
		 * @param priority The priority level to give this message type. This MUST be between 0 and 9 - this is tested
		 * by an assert.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
         *
         * pre: - This MessageType has not been deleted
		 *		- 0 <= priority <= 9
		 */
		 void setPriority(const unsigned int priority);

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
         *
         * pre: This MessageType has not been deleted
		 */
		bool toBePrinted();

		/**
		 * setPrintMessageStatus
		 *
		 * Locally sets whether or not this message type should be printed.
		 *
		 * @param printMessage Set to true if this message type should be printed.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
         *
         * pre: This MessageType has not been deleted
		 */
		void setPrintMessageStatus(const bool printMessageStatus);

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
         *
         * pre: This MessageType has not been deleted
		 */
		bool toBeLogged();

		/**
		 * setLogMessageStatus
		 *
		 * Locally sets whether or not this message type should be written to the database.
		 *
		 * @param logMessageStatus Set to true if this message type should be written to the database.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
         *
         * pre: This MessageType has not been deleted
		 */
		void setLogMessageStatus(const bool logMessageStatus);

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
         *
         * pre: This MessageType has not been deleted
		 */
		bool toBePersisted();

		/**
		 * persistMessageStatus
		 *
		 * Locally sets whether or not this message type should be added to a persistent data structure
		 *
		 * @param persistMessageStatus Set to true if this message type should be stored in a persistent data structure
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. priority should be an 
         *            int between 0 and 9), or if the wrong amount of data is retrieved.
         *
         * pre: This MessageType has not been deleted
		 */
		void setPersistMessageStatus(const bool persistMessageStatus);

		/**
         * invalidate
         *
         * Mark the data contained by this message type as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        /**
         * writeMessageTypeData
         * 
         * Write this message type to the database.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the message type object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         */
        void writeMessageTypeData();

        /**
         * deleteMessageType
         *
         * Remove this message type from the database. Once this method has been executed, the
         * MessageTypeHelper object should be destroyed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem removing the data from the database.
         */
        void deleteMessageType();

        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        bool isNew();

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
         * setViewLevel
         *
         * Sets the view level required to see this message in the event viewer.
         *
         * @param viewLevel The view level.
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
        void setViewLevel( const unsigned int viewLevel );

		std::string getDisplayName();
		void setDisplayName(const std::string& displayname);

	private:

		MessageTypeHelper& operator=(const MessageTypeHelper &);
		MessageTypeHelper( const MessageTypeHelper& theMessageTypeHelper);

		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this action was initially loaded from the database
         *      OR     - writeProfile() has already been called
         */		
		void reload();

        /**
          * reloadUsing
          *
          * Fills out the local members with data contained in the input data object
          *
          * @param : const unsigned long row - the row to query in the data object
          * @param : TA_Base_Core::IData& data - the IData interface that should 
          *          have been obtained using the getBasicQueryData function
          *         
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
          */
        void reloadUsing(const unsigned long row, TA_Base_Core::IData& data);
		bool MessageTypeHelper::getIsRealTimeEvent();

		bool m_isValidData;
		std::string m_key;
        std::string m_name;
		std::string m_displayname;
        std::string m_description;
        unsigned int m_priority;
		bool m_toBePrinted;
		bool m_toBeLogged;
		bool m_toBePersisted;
        time_t m_dateCreated;
        time_t m_dateModified;
        unsigned int m_viewLevel;

		//Note: these 2 members are currently just added for MessageTypeData, not for ConfigMessageType.
		//so, they have get*** function, but no saving function
		//(we haven't use them in function deleteMessageType() and writeMessageTypeData(), and no set*** function)  
		unsigned long m_actionID;
		bool m_isRealTimeEvent;
	};
} // closes TA_Base_Core

#endif // !defined(MessageTypeHelper_69D71528_B22E_4881_BF9C_1FDB0D9CC991__INCLUDED_)

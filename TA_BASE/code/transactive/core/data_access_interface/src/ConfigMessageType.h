/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigMessageType.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/26 14:07:36 $
  * Last modified by:  $Author: huirong.luo $
  *
  * <description>
  *
  */


#if !defined(ConfigMessage_4ADB69AA_36D4_4c3c_A980_D90637DDF888__INCLUDED_)
#define ConfigMessage_4ADB69AA_36D4_4c3c_A980_D90637DDF888__INCLUDED_

#include "core/data_access_interface/src/IConfigMessageType.h"
#include "core/data_access_interface/src/MessageTypeHelper.h"

namespace TA_Base_Core
{

    class ConfigMessageType : public IConfigMessageType
    {
    public:

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
		 * pre: 0 <= priority <= 9
		 */
		void setPriority(const unsigned int priority);

		/**
		 * setPrintMessageStaus
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
		 */
		void setPrintMessageStatus(const bool printMessageStatus);

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
		 */
		void setLogMessageStatus(const bool logMessageStatus);

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
		 */
		void setPersistMessageStatus(const bool persistMessageStatus);

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
         * deleteThisObject
         *
         * Removes this message type from the database. 
         * The calling application MUST then delete this message type object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: - The writeMessageTypeData() method has already been called
         *      - This MessageType has not been deleted
         */
         void deleteThisObject();

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the MessageType object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This MessageType has not been deleted
         */
         void applyChanges();

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


		/**
         * invalidate
         *
         * Mark the data contained by this message type as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();


        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        unsigned long getUniqueIdentifier();

        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        ItemChanges getAllItemChanges();

        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        bool hasChanged();


        /**
         *  Gets a boolean as a string. Use getBoolFromString to convert
         *  this value back to a bool.
         */
        std::string getBoolAsString(bool theBool);

        /**
         *  Gets a boolean from a string created using getBoolAsString.
         */
        bool getBoolFromString(const std::string& theString);

        /**
         *  Gets an unsigned long as a string. Use getUnsignedLongFromString
         *  to convert this value back to an unsigned long.
         */
        std::string getUnsignedLongAsString(unsigned long number);

        /**
         *  Gets an unsigned long from a string created using
         *  getUnsignedLongAsString.
         */
        unsigned long getUnsignedLongFromString(const std::string& theString);

		std::string getDisplayName();
		

    public:

        ConfigMessageType(const std::string& key);

		/**
         * Constructor
         *
         * Construct a ConfigMessageType class based around dataset
         */
		ConfigMessageType(const unsigned long row, TA_Base_Core::IData& data);

        virtual ~ConfigMessageType();

    private:

        ConfigMessageType();
        ConfigMessageType( const ConfigMessageType& theConfigMessageType) {};



        /**
         * updateMessageTypeChanges
         *
         * This updates the map recording the changes to the message type.
         * Whenever a call to set is made in this class then this method
         * must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        void ConfigMessageType::updateMessageTypeChanges(
            const std::string& name,
            const std::string& oldValue,
            const std::string& newValue);


        /** The class that does the real database work */
        MessageTypeHelper* m_helper;

        /**
         *  We cannot use keys for identification of locations in the
         *  Configuration Editor because new locations do not have keys.
         *  Therefore we use this method for identification.
         */
        unsigned long m_uniqueIdentifier;

        /** The next unique identifier available */
        static long s_nextAvailableIdentifier;

        /**
         *  This is a map of all changes made to the message type so that they
         *  are recorded and can be sent in audit or online update messages.
         */
        ItemChanges m_messageTypeChanges;


        // Constants used as keys for the ItemChanges made to this object.
        static const std::string ConfigMessageType::NAME;
		static const std::string ConfigMessageType::DISPLAY_NAME;
        static const std::string ConfigMessageType::DESCRIPTION;
        static const std::string ConfigMessageType::PRINT_MESSAGE_STATUS;
        static const std::string ConfigMessageType::LOG_MESSAGE_STATUS;
        static const std::string ConfigMessageType::PERSIST_MESSAGE_STATUS;
        static const std::string ConfigMessageType::PRIORITY;
        static const std::string ConfigMessageType::VIEW_LEVEL;

    };

} //close namespace TA_Base_Core

#endif // !defined(ConfigMessage_4ADB69AA_36D4_4c3c_A980_D90637DDF888__INCLUDED_)

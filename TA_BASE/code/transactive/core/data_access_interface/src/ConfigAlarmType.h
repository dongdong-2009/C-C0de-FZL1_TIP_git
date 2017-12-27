/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigAlarmType.h $
 * @author:  Nick Jardine
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/21 00:07:03 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * ConfigAlarmType is a read/write interface to the database for Alarm Types.
 */



#if !defined(ConfigAlarm_4ADB69AA_36D4_4c3c_A980_D90637DDF888__INCLUDED_)
#define ConfigAlarm_4ADB69AA_36D4_4c3c_A980_D90637DDF888__INCLUDED_

#include "core/data_access_interface/src/IConfigAlarmType.h"
#include "core/data_access_interface/src/AlarmTypeHelper.h"

namespace TA_Base_Core
{

    class ConfigAlarmType : public IConfigAlarmType
    {
    public:

        /**
         * setDescription
         *
         * Sets the description of this Alarm type. This should only be used by the ConfigAlarmType class
         *
         * @param description The description to give this Alarm type
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved. 
         *
         * pre: This AlarmType has not been deleted
         */
        void setDescription(const std::string& description);

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
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        bool isNew();

        
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
         * setSeverityKey
         *
         * Sets the severity level of this Alarm type. This should only be used by the ConfigAlarmType class
         *
         * @param severityKey The key to the severty level to give this Alarm type. 
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        void setSeverityKey(const unsigned long severityKey);


        /**
         * setPersistAlarmStatus
         *
         * Locally sets whether or not this Alarm type should be persisted.
         *
         * @param persistAlarm Set to true if this Alarm type should be persisted.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        void setPersistAlarmStatus(const bool persistAlarmStatus);

        /**
         * setPrintAlarmStaus
         *
         * Locally sets whether or not this Alarm type should be printed.
         *
         * @param printAlarm Set to true if this Alarm type should be printed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        void setPrintAlarmStatus(const bool printAlarmStatus);

        /**
         * setSystemAlarmStatus
         *
         * Locally sets whether or not this Alarm type is System Alarm type.
         *
         * @param systemAlarmStatus Set to true if this Alarm type is System Alarm type.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        void setSystemAlarmStatus(const bool systemAlarmStatus);

		// To get all the alarm-param names
		std::vector<std::string> getParameterNames();
		std::string getParameterValue(const std::string& paramName);
		void setParameterValue( const std::string& value, const std::string& paramName );

		// parameter values for system alarm
		// return values will be empty for non-system alarm
		std::string getAlarmValue();
		std::string getNormalValue();
		
		// To set the alarm values for system alarm
		void setAlarmValue(const std::string& alarmValue);
		void setNormalValue(const std::string& normalValue);

        /**
         * deleteThisObject
         *
         * Removes this Alarm type from the database. 
         * The calling application MUST then delete this Alarm type object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DataException A DataException will be thrown if deleting
         *            this AlarmType is not permitted
         *            
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: - The writeAlarmTypeData() method has already been called
         *      - This AlarmType has not been deleted
         */
         void deleteThisObject();

        /**
         *  Indicates whether it is legal to delete this AlarmType.
         *  (It is illegal if the AlarmType is a system alarm, ie not a new
         *  alarm)
         */
        bool canDelete();

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the AlarmType object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This AlarmType has not been deleted
         */
         void applyChanges();

         /**
         * getKey
         *
         * Returns the key for this AlarmType.
         *
         * @return The key for this AlarmType as an unsigned long.
         */
		 unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of this AlarmType. If this is the first time data for this AlarmType
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this AlarmType as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        std::string getName();

        /**
         * getDescription
         *
         * Returns the description for this AlarmType.
         *
         * @return The description for this AlarmType as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        std::string getDescription();

        /**
         * getSeverityKey
         *
         * Returns the severity level of this Alarm type.
         *
         * @return The key to the severity level of the Alarm type as an unsigned long. 
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        unsigned long getSeverityKey();

		/**
		 * getSeverityName
		 *
		 * Returns the name associated with the severity level of this Alarm type.
		 *
		 * @return The severity level of the Alarm type as a string.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved. 
		 */
        std::string getSeverityName();

		/**
		 * getContextKey
		 *
		 * @return The key of the context for the Alarm type as an unsigned long. 
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved. 
		 */
		unsigned long getContextKey();

		/**
		 * getContextName
		 *
		 * @return The name of the context for the Alarm type as a string.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved. 
		 */
        std::string getContextName();


        /**
         * toBePrinted
         *
         * Returns whether or not this Alarm type should be printed.
         *
         * @return true if this Alarm type should be printed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        bool toBePrinted();

        /**
         * isSystemAlarm
         *
         * Returns whether or not this Alarm type is system alarm.
         *
         * @return true if this Alarm type is system alarm.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        bool isSystemAlarm();

        /**
         * isPhysicalAlarm
         *
         * Returns whether or not this Alarm type is physical subsystem alarm.
         *
         * @return true if this Alarm type is physical subsystem alarm.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        bool isPhysicalAlarm(); //20449++
		/*
         * toBePersisted
         *
         * Returns whether or not this Alarm type should be added to a persistent data structure
         *
         * @returns true if this Alarm type should be stored in a persistent data structure
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        bool toBePersisted();

		// TD15195
		/* This is to indicate whether this alarm type is to be used for the project
		 *
		 * If isVisible == false, this alarm type will not be displayed in the CE or allowed to be submitted as alarm
		 * If isVisible == true, this alarm type will be used as norm.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
		 */
		virtual bool isVisible();

        /**
         * invalidate
         *
         * Mark the data contained by this Alarm type as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        /**
         *  Gets a boolean as a string. Use getBoolFromString to convert
         *  this value back to a bool.
         */
        std::string getBoolAsString(bool theBool);

        /**
         *  Gets a boolean from a string created using getBoolAsString.
         */
        bool getBoolFromString(
            const std::string& theString);

        /**
         *  Gets an unsigned long as a string. Use getUnsignedLongFromString
         *  to convert this value back to an unsigned long.
         */
        std::string getUnsignedLongAsString(
            unsigned long number);

        /**
         *  Gets an unsigned long from a string created using
         *  getUnsignedLongAsString.
         */
        unsigned long getUnsignedLongFromString(
            const std::string& theString);


        /**
         *  Creates a ConfigAlarmType using the given database key.
         */
        ConfigAlarmType(unsigned long key);


        /**
         *  Creates a new ConfigAlarmType, and creates a row in the database
         *  for it.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        ConfigAlarmType(
            const std::string& name,
            const std::string& description,
            unsigned long severityKey,
	    	bool toBePrinted,
		    bool toBePersisted,
			bool isSystemAlarm,
			bool isVisible = true);

         /**
         * Constructor
         *
         * Construct a ConfigAlarmType class based on dataset
         */
		ConfigAlarmType(const unsigned long row, TA_Base_Core::IData& data);


		/**
         * Constructor
         *
         * Construct a AlarmType class based around an existing location. This will
		 * copy all the paramters of the existing location
         *
         * @param theAlarmType The location to copy
         */
        ConfigAlarmType::ConfigAlarmType( const ConfigAlarmType& theAlarmType);


        /**
         *  Destructor.
         */
        virtual ~ConfigAlarmType();


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

        //Mintao++ TD15302
	    /**
	      * createMMSRule
	      * 
	      * TD17628 - save the MMMS state type when the associate MMS rule is created
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void createMMSRule(IAlarmData::EMmsState mmsStateType); //TD17628  
	    /**
	      * deleteMMSRule
	      * 
	      * <description>
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void deleteMMSRule();  
	    /**
	      *   isMMSRuleDeleted
	      * 
	      * <description>
	      * 
	      * @return bool  
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual bool    isMMSRuleDeleted();   
        /**
	      *   isMMSEnabled
	      * 
	      *  This function check the Alarm Rule table via the Alarm Rule Access Factory 
          *  for the MMS Rule with the same alarm type key and return m_mmsRule is not null
	      * 
	      * @return bool  
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual bool    isMMSEnabled();
	    /**
	      * getRelatedMMSRule
	      * 
	      * <description>
	      * 
	      * @return MMSRuleData* 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual MMSRuleData* getRelatedMMSRule();  
        //Mintao++ TD15302

		/**
		  * TD17628
		  *
	      * getRelatedMMSType
	      * 
	      * Get the set mms type
	      * 
	      * @return IAlarmData::TYPE_NONE default
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual IAlarmData::EMmsState getRelatedMMSType();  
		/**
		  * TD17628
		  *
	      * setRelatedMMSType
	      * 
	      * 
	      * set the corresponding mms type 
	      * 
	      * 
	      * 
	      */

		virtual void setRelatedMMSType(IAlarmData::EMmsState mmsState);
		
		virtual std::string getDisplayName();
		virtual void setDisplayName(std::string displayname);
		
    private:
        ConfigAlarmType();
        const ConfigAlarmType& operator=(const ConfigAlarmType&);


        /**
         *  getSeverityName
         *
         *  Gets the name of the alarm severity with the given pkey.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        std::string getSeverityName(const unsigned long key);


        /**
         * updateAlarmTypeChanges
         *
         * This updates the map recording the changes to the alarm type.
         * Whenever a call to set is made in this class then this method
         * must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        void updateAlarmTypeChanges(
            const std::string& name,
            const std::string& oldValue,
            const std::string& newValue);


        /** The class that does the real database work */
        AlarmTypeHelper* m_helper;

        /**
         *  We cannot use keys for identification of locations in the
         *  Configuration Editor because new locations do not have keys.
         *  Therefore we use this method for identification.
         */
        unsigned long m_uniqueIdentifier;

        /** The next unique identifier available */
        static long s_nextAvailableIdentifier;

        /**
         *  This is a map of all changes made to the alarm type so that they
         *  are recorded and can be sent in audit or online update messages.
         */
        ItemChanges m_alarmTypeChanges;


        // Constants used as keys for the ItemChanges made to this object.
        static const std::string ConfigAlarmType::NAME;
		static const std::string ConfigAlarmType::DISPLAY_NAME;
        static const std::string ConfigAlarmType::DESCRIPTION;
        static const std::string ConfigAlarmType::SEVERITY;
        static const std::string ConfigAlarmType::PRINT_ALARM_STATUS;
		static const std::string ConfigAlarmType::SYSTEM_ALARM_STATUS;
        static const std::string ConfigAlarmType::PERSIST_ALARM_STATUS;
		//TD17628 marc_bugfix support mms state
		static const std::string ConfigAlarmType::MMS_STATE;

		static const std::string ConfigAlarmType::PARAM_ALARM_VALUE;
		static const std::string ConfigAlarmType::PARAM_NORMAL_VALUE;
    };

} //close namespace TA_Base_Core

#endif // !defined(ConfigAlarm_4ADB69AA_36D4_4c3c_A980_D90637DDF888__INCLUDED_)

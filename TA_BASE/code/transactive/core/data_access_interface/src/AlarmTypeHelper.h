 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmTypeHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 00:07:03 $
  * Last modified by:  $Author: huirong.luo $
  * 
  * AlarmTypeHelper is an object that is held by AlarmTypeData and ConfigAlarmType objects. 
  * It contains all data used by Alarm types, and manipulation
  * methods for the data. It helps avoid re-writing code for both AlarmTypeData and ConfigAlarmType.
  */


#if !defined(AlarmTypeHelper_69D71528_B22E_4881_BF9C_1FDB0D9CC991__INCLUDED_)
#define AlarmTypeHelper_69D71528_B22E_4881_BF9C_1FDB0D9CC991__INCLUDED_

#include <string>
#include <sstream>
#include "core/data_access_interface/alarm_rule/src/MMSRuleData.h"
#include "core/data_access_interface/src/IAlarmData.h"

namespace TA_Base_Core
{
	// forward declarations
	class IData;

	class AlarmTypeHelper
	{

	public:

        /**
		 * AlarmTypeHelper
		 *
		 * Constructs a new Alarm helper for the Alarm type associated with the specified key
		 *
		 * @param key The key into the me_Alarm_type table for this Alarm type.
		 */
		AlarmTypeHelper(unsigned long key);

        /**
		 * AlarmTypeHelper
		 *
		 * Constructs a new Alarm helper for the Alarm type associated with the specified parameters
		 *
		 * @param key The key into the me_Alarm_type table for this Alarm type.
		 */
        AlarmTypeHelper(
            unsigned long key,
            const std::string& name,
            const std::string& description,
            unsigned long severityKey,
	    	bool toBePrinted,
		    bool toBePersisted,
			bool isSystemAlarm,
			bool isVisible);

        /**
         * Constructor
         *
         * Construct a AlarmTypeHelper class based around an existing location. This will
		 * copy all the paramters of the existing location
         *
         * @param theAlarmType The location to copy
         */
		AlarmTypeHelper( const AlarmTypeHelper& theAlarmType);

		/**
		 * AlarmTypeHelper
		 *
		 * Constructs a new Alarm helper and creates an entry in the
         * database for it.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
		 */
        AlarmTypeHelper(
            const std::string& name,
            const std::string& description,
            unsigned long severityKey,
	    	bool toBePrinted,
		    bool toBePersisted,
			bool isSystemAlarm,
			bool isVisible);


		/**
	      * AlarmTypeHelper
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
		AlarmTypeHelper(const unsigned long row, TA_Base_Core::IData& data);

        /**
		 * AlarmTypeHelper (deconstructor)
		 *
		 * Deconstruct a AlarmTypeHelper class.
		 *
		 */
		virtual ~AlarmTypeHelper();

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

		void setDisplayName(const std::string& displayname);

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
         * setSystemAlarmStaus
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

		/**
         * getKey
         *
         * Returns the key for this AlarmType.
         *
         * @return The key for this AlarmType as an unsigned long.
         *
         * pre: This AlarmType has not been deleted
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
         *
         * pre: This AlarmType has not been deleted
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
         *
         * pre: This AlarmType has not been deleted
         */
		std::string getDescription();

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
		 * getSeverityKey
		 *
		 * Returns the severity level of this Alarm type.
		 *
		 * @return The severity level of the Alarm type as an unsigned long.
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
		 * @return The context key of the Alarm type as an unsigned long.
		 *
		 * @exception   DatabaseException A DatabaseException will be thrown
         *              if there is a problem establishing a connection with
         *              the database, or if an error is encountered while
         *              retrieving data.
         * @exception   DataException A DataException will be thrown if the
         *              data cannot be converted to the required format
         *              (e.g. persist_message should be either 0 or 1), or
         *              if the wrong amount of data is retrieved. 
		 */
        unsigned long getContextKey();

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
         *
         * pre: This AlarmType has not been deleted
		 */
		bool toBePrinted();

        /**
		 * isSystemAlarm
		 *
		 * Returns whether or not this Alarm type is a system alarm type.
		 *
		 * @return true if this Alarm type is a system alarm type, otherwise false
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
		 * Returns whether or not this Alarm type is a physical subsystem alarm type.
		 *
		 * @return true if this Alarm type is a physical subsystem alarm type, otherwise false
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
		 */
		bool isPhysicalAlarm(); //20449++
		/**
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
         *
         * pre: This AlarmType has not been deleted
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
		bool isVisible();
		
		/**
		 * persistAlarmStatus
		 *
		 * Locally sets whether or not this Alarm type should be added to a persistent data structure
		 *
		 * @param persistAlarmStatus Set to true if this Alarm type should be stored in a persistent data structure
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
		void setPersistAlarmStatus(const bool persistAlarmStatus);

		// Parameters for system alarm type
		// Alarm Value if system alarm is raised
		// Normalised Value if system alarm is closed
		std::string getAlarmValue();
		std::string getNormalValue();
		void setAlarmValue(const std::string& alarmValue);
		void setNormalValue(const std::string& normalValue);

		/**
         * invalidate
         *
         * Mark the data contained by this Alarm type as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        /**
         * writeAlarmTypeData
         * 
         * Write this Alarm type to the database.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Alarm type object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         */
        void writeAlarmTypeData();

        /**
         * deleteAlarmType
         *
         * Remove this Alarm type from the database. Once this method has been executed, the
         * AlarmTypeHelper object should be destroyed.
         *
         * @exception DataException A DataException will be thrown if deleting
         *            this AlarmType is not permitted
         *            
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem removing the data from the database.
         */
        void deleteAlarmType();


        /**
         *  Indicates whether it is legal to delete this AlarmType.
         *  (It is illegal if the AlarmType is a system alarm, ie not a new
         *  alarm)
         */
        bool canDelete();


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
	      * <description>
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        void createMMSRule(IAlarmData::EMmsState mmsStateType);   
	    /**
	      * deleteMMSRule
	      * 
	      * <description>
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        void deleteMMSRule();  
	    /**
	      *   isMMSRuleDeleted
	      * 
	      * <description>
	      * 
	      * @return bool  
	      * 
	      * @exception <exceptions> Optional
	      */
        bool    isMMSRuleDeleted();   
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
        bool    isMMSEnabled();

	    /**
	      * getRelatedMMSRule
	      * 
	      * <description>
	      * 
	      * @return MMSRuleData* 
	      * 
	      * @exception <exceptions> Optional
	      */
        MMSRuleData* getRelatedMMSRule();  
        //Mintao++ TD15302

	    /**
		  * TD17628
		  *
	      * getRelatedMMSType
	      * 
	      * Get the currently set MMS type (None, Automatic, Semi-Automatic)
	      * 
	      * @return IAlarmData::TYPE_NONE default
	      * 
	      * @exception <exceptions> Optional
	      */
        IAlarmData::EMmsState getRelatedMMSType();  

	    /**
		  * TD17628
		  *
	      * getRelatedMMSType
	      * 
	      * Set the mms type specified by user 
	      * 
	      * @return IAlarmData::TYPE_NONE default
	      * 
	      * @exception <exceptions> Optional
	      */

		void setRelatedMMSType(IAlarmData::EMmsState mmsState);
		
		std::string getDisplayName();

	private:

        // Alarm Types cannot be created, so this constructor is disabled.
        AlarmTypeHelper();
		AlarmTypeHelper& operator=(const AlarmTypeHelper &);
		
		bool m_isValidData;
        unsigned long m_key;
        std::string m_name;
		std::string m_displayname;
        std::string m_description;
        unsigned long m_severityKey;
		bool m_toBePrinted;
		bool m_isSystemAlarm;
		bool m_toBePersisted;
        bool m_isDeleted;
        std::string m_contextName;
        unsigned long m_contextKey;
        time_t m_dateCreated;
        time_t m_dateModified;
		std::string m_alarmValue;
		std::string m_normalValue;

		bool m_isVisible;

        //TD15302 Mintao++
        MMSRuleData*  m_MmsRule;
        bool m_isMmsRuleDeleted;
		//TD17628
		IAlarmData::EMmsState m_MmsStateType;

		bool m_isPhysicalAlarm; //20449++
		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... or set... method
         * is called and the data state is not valid.
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

        /**
         *  createNewRowInDatabase
         *
         *  Called when a new object is created without a key, the first time
         *  an attempt is made to read or write the object.
         */
        void AlarmTypeHelper::createNewRowInDatabase();
    
        // TODO: this will eventually be replaced by a column in the table
        // indicating if a row is deletable.
        /** The pkey of the row at which new (deletable) alarm types start */
        static const unsigned long AlarmTypeHelper::NEW_ALARM_TYPES_STARTING_PKEY;

		

    };
} // closes TA_Base_Core

#endif // !defined(AlarmTypeHelper_69D71528_B22E_4881_BF9C_1FDB0D9CC991__INCLUDED_)

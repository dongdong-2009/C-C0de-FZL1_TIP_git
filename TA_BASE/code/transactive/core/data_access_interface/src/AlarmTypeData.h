/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmTypeData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/21 00:07:03 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * AlarmTypeData is an implementation of IAlarmTypeData. It holds the data specific to a Alarm
 * type entry in the database.
 */

#if !defined(AlarmTypeData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_)
#define AlarmTypeData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeHelper.h"

namespace TA_Base_Core
{

	class AlarmTypeData : public IAlarmTypeData
	{

	public:
		/**
		 * AlarmTypeData (constructor)
		 *
		 * Construct a AlarmTypeData class for the entry represented by the specified key.
		 *
		 * @param key The key of this Alarm type in the database
		 */
		AlarmTypeData(unsigned long key);

        /**
		 * AlarmTypeData (constructor)
		 *
		 * Construct a AlarmTypeData class for the entry represented by the specified parameters.
		 *
		 * @param key The key of this Alarm type in the database
		 */
		AlarmTypeData( unsigned long key,
            const std::string& name,
            const std::string& description,
            unsigned long severityKey,
	    	bool toBePrinted,
		    bool toBePersisted,
			bool isSystemAlarm,
			const std::string& alarmValue = "",
			const std::string& normalValue = "",
			bool isVisible = true);


         /**
         * Constructor
         *
         * Construct a AlarmTypeData class based on round dataset
         */
		AlarmTypeData(const unsigned long row, TA_Base_Core::IData& data);

        /**
		 * AlarmTypeData (deconstructor)
		 *
		 * Deconstruct a AlarmTypeData class.
		 *
		 */
		virtual ~AlarmTypeData();

	
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
		// parameter values for system alarm
		// return values will be empty for non-system alarm
		std::string getAlarmValue();
		std::string getNormalValue();

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

        //TD15302 Mintao++
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
        //TD15302 Mintao++

	    /**
		  * TD17628
		  *
	      * getRelatedMMSType
	      * 
	      * <description>
	      * 
	      * @return IAlarmData::TYPE_NONE default
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual IAlarmData::EMmsState getRelatedMMSType();  

		virtual std::string getDisplayName();
		virtual void setDisplayName(std::string displayname);
	private:
		AlarmTypeData& operator=(const AlarmTypeData &);
		AlarmTypeData( const AlarmTypeData& theAlarmTypeData);


		AlarmTypeHelper* m_AlarmHelper;
	};
} // closes TA_Base_Core

#endif // !defined(AlarmTypeData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A2__INCLUDED_)

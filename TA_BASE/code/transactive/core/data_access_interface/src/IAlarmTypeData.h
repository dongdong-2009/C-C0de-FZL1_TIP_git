/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IAlarmTypeData.h $
 * @author Nick Jardine
 * @version $Revision: #2 $
 * Last modification: $DateTime: 2015/01/21 00:07:03 $
 * Last modified by: $Author: huirong.luo $
 * 
 * IAlarmTypeData is an interface to a Alarm type object. It allows the Alarm type object implementation
 * to be changed without changing the code that uses this interface.
 */


#if !defined(IAlarmTypeData_412CCE29_A960_4da6_9EA9_D2418C31B5C2__INCLUDED_)
#define IAlarmTypeData_412CCE29_A960_4da6_9EA9_D2418C31B5C2__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IItem.h"
#include "core/data_access_interface/alarm_rule/src/MMSRuleData.h"
#include "core/data_access_interface/src/IAlarmData.h"

namespace TA_Base_Core
{

    class IAlarmTypeData : public IItem
	{
	public:

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
        virtual std::string getDescription() =0;

		/**
		 * getSeverityKey
		 *
		 * Returns the severity level of this Alarm type.
		 *
		 * @return The severity level of the Alarm type as an unsigned int. 
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved. 
		 */
		virtual unsigned long getSeverityKey() =0;

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
        virtual std::string getSeverityName() =0;

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
		virtual unsigned long getContextKey() = 0;


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
        virtual std::string getContextName() = 0;

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
		virtual bool toBePrinted() =0;

		/**
		 * isSystemAlarm
		 *
		 * Returns whether or not this Alarm type is system alarm type.
		 *
		 * @return true if this Alarm type is system alarm type.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
		 */
		virtual bool isSystemAlarm() =0;

		/**
		 * isPhysicalAlarm
		 *
		 * Returns whether or not this Alarm type is physical subsystem alarm type.
		 *
		 * @return true if this Alarm type is physical subsystem alarm type.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
		 */
		virtual bool isPhysicalAlarm() =0; //20449++
		// parameter values for system alarm
		// return values will be empty for non-system alarm
		virtual std::string getAlarmValue() =0;
		virtual std::string getNormalValue() =0;
		
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
		virtual bool toBePersisted() =0;

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
		virtual bool isVisible() =0;


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
        virtual time_t getDateCreated() =0;

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
        virtual time_t getDateModified() =0;

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
        virtual bool    isMMSEnabled() = 0;

	    /**
	      * getRelatedMMSRule
	      * 
	      * <description>
	      * 
	      * @return MMSRuleData* 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual MMSRuleData* getRelatedMMSRule() = 0;  
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
        virtual IAlarmData::EMmsState getRelatedMMSType() = 0;  

		virtual std::string getDisplayName() = 0;

        virtual ~IAlarmTypeData() {};
	};

} //close namespace TA_Base_Core

#endif // !defined(IAlarmTypeData_412CCE29_A960_4da6_9EA9_D2418C31B5C2__INCLUDED_)

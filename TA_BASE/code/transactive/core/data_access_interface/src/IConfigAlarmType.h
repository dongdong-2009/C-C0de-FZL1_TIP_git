/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigAlarmType.h $
 * @author Nick Jardine
 * @version $Revision: #1 $
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by: $Author: CM $
 * 
 * IConfigAlarmType is an interface to a Alarm type object that supports changing the data.
 * It provides methods for retrieving, setting, and writing data.
 * The set methods in this interface do not set the values in the database
 * straight away. They only set the values in the object after validating that
 * they are correct. The data must be explicitly written to the database.
 */


#if !defined(IConfigAlarmType_24660863_93AA_444a_9EF9_E18CA21CE00A__INCLUDED_)
#define IConfigAlarmType_24660863_93AA_444a_9EF9_E18CA21CE00A__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"

namespace TA_Base_Core
{

	class IConfigAlarmType : public IAlarmTypeData, virtual public IConfigItem
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
        virtual void setDescription(const std::string& description) =0;

		/**
		 * setSeverityKey
		 *
		 * Sets the severity level of this Alarm type. This should only be used by the ConfigAlarmType class
		 *
		 * @param priority The severity level to give this Alarm type. 
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. persist_message should be 
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
		virtual void setSeverityKey(const unsigned long severityKey) =0;

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
		virtual void setPrintAlarmStatus(const bool printAlarmStatus) =0;

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
        virtual void setSystemAlarmStatus(const bool systemAlarmStatus) =0;

		// To get all the alarm-param names
		virtual std::vector<std::string> getParameterNames() =0;
		virtual std::string getParameterValue(const std::string& paramName) =0;
		virtual void setParameterValue( const std::string& value, const std::string& paramName ) =0;

		// To set the alarm-param values for system alarm
		virtual void setAlarmValue(const std::string& alarmValue) =0;
		virtual void setNormalValue(const std::string& normalValue) =0;

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
         */
		virtual void setPersistAlarmStatus(const bool persistAlarmStatus) =0;

        /**
         * deleteThisObject
         *
         * Removes this Alarm type from the database. 
         * The calling application MUST then delete this Alarm type object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: - The writeAlarmTypeData() method has already been called
         *      - This AlarmType has not been deleted
         */
        virtual void deleteThisObject() =0;

        /**
         *  Indicates whether it is legal to delete this AlarmType.
         *  (It is illegal if the AlarmType is a system alarm, ie not a new
         *  alarm)
         */
        virtual bool canDelete() =0;

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
        virtual void createMMSRule(IAlarmData::EMmsState mmsStateType) = 0;   //TD17628
	    /**
	      * deleteMMSRule
	      * 
	      * <description>
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void deleteMMSRule() = 0;  
	    /**
	      *   isMMSRuleDeleted
	      * 
	      * <description>
	      * 
	      * @return bool  
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual bool    isMMSRuleDeleted() = 0;   
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
        //Mintao++ TD15302
        
	    /**
		  * TD17628
		  *
	      * getRelatedMMSType
	      * 
	      * Get the saved MMS type 
	      * 
	      * @return IAlarmData::TYPE_NONE default
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual IAlarmData::EMmsState getRelatedMMSType() = 0;
		

	    /**
		  * TD17628
		  *
	      * setRelatedMMSType
	      * 
	      * save the set mms type 
	      * 
	      * 
	      * 
	      * 
	      */
        virtual void  setRelatedMMSType(IAlarmData::EMmsState mmsState) = 0;


        virtual ~IConfigAlarmType() {};
	};

} //close namespace TA_Base_Core

#endif // !defined(IConfigAlarmType_24660863_93AA_444a_9EF9_E18CA21CE00A__INCLUDED_)

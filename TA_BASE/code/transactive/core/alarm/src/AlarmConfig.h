#ifndef ALARM_CONFIG_H
#define ALARM_CONFIG_H

/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmConfig.h $
* @author:  Cameron Rochester
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
* 
* AlarmTypeTableAccessor provides read-only access to a subset of the columns in the 
* ME_ALARM_TYPE table.  A cache is maintained for records that have been accessed.
*/

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

//#include "IOnlineUpdatable.h"
#include <string>
#include <map>
#include "core/message/types/MessageTypes.h"
#include "core/message/src/NameValuePair.h"

//TD17994,jianghp
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
//TD17994,jianghp
//TD17862 marc_bugfix 
#include "core/data_access_interface/src/IAlarmData.h"

#include "core/alarm/src/AlarmItem.h"

namespace TA_Base_Core
{
	class IAlarmTypeData;

	class IAlarmData;
	
	
	class AlarmTypeTableAccessor //: public IOnlineUpdatable
	{
	public:
		std::string GetAlarmDescriptionModel(unsigned long messageTypeKey);
		
		static const std::string DEFAULT_SYSTEM_ALARM_ALARM_VALUE;
		static const std::string DEFAULT_SYSTEM_ALARM_NORMAL_VALUE;
		
		/**
		* destructor
		*/
		virtual ~AlarmTypeTableAccessor();
		
		/**
		* Returns a reference to this object.
		*/
		static AlarmTypeTableAccessor& getInstance();
		/**
		* Remove the instance.
		*/
		static void removeInstance();


		/**
		* Load all the alarm types.
		*/
		void loadAllAlarmTypes();
		
		/**
		* isLoggable
		*
		* Check to see if a particular alarm Type should be logged
		*
		* @param    messageType    The alarm type to check if logging is required
		* @return	bool	true if the alarm is loggable, false otherwise
		*/
		bool isLoggable(const TA_Base_Core::MessageType& messageType);
		
		/**
		* getAlarmTypeName
		*
		* Check to obtain the alarm type name in string
		*
		* @param    messageType    The alarm type to check if logging is required
		* @return	string	String of the alarm type name
		*/
		std::string getAlarmTypeName(const TA_Base_Core::MessageType& messageType);
		
		/**
		* isPrintable
		*
		* Check to see if a particular alarm should be logged
		*
		* @param    messageType    The alarm type to check if printing is required
		* @return	bool	true if the message is printable, false otherwise
		*/
		bool isPrintable(const TA_Base_Core::MessageType& messageType);
		
		/**
		* isPersistent
		*
		* Check to see if a particular alarm Type should be persisted
		*
		* @param    messageType    The alarm type to check if persistence is required
		* @return	bool	true if the message is persistent, false otherwise
		*/
		bool isPersistent(const TA_Base_Core::MessageType& messageType);
		
		/**
		* isSystemAlarm
		*
		* Check to see if a particular alarm Type is a system alarm type
		*
		* @param    messageType    The alarm type to check
		* @return	bool	true if the message is system alarm type, false otherwise
		*/
		bool isSystemAlarm(const TA_Base_Core::MessageType& messageType);
		
		/**
		* isVisible
		*
		* Check to see if a particular alarm Type is to be used/displayed
		* Discard this alarm submission if isVisible = false
		*
		* @param    messageType    The alarm type to check
		* @return	bool	true if the message/alarm type is visible, false otherwise
		*/
		bool isVisible(const TA_Base_Core::MessageType& messageType);
		
		/**
		* getPriority
		*
		* Get the configured priority of the message
		*
		* @param    messageType    The message Type to lookup
		* @return	short	the message priority
		*/
		short getPriority(const TA_Base_Core::MessageType& messageType);
		
		/**
		* getSeverityKey
		*
		* Get the configured priority of the message
		*
		* @param    messageType    The message Type to lookup
		* @return	the alarm severity key
		*/
		unsigned long getSeverityKey(const TA_Base_Core::MessageType& messageType);
		
		/**
		* getAlarmValue
		*
		* Get the configurable alarm value for the message
		*
		* @param    messageType    The message Type to lookup
		* @return	the alarm value
		*/
		std::string getAlarmValue(const TA_Base_Core::MessageType& messageType);
		std::string getNormalValue(const TA_Base_Core::MessageType& messageType);
		
		/**
		* getExpandedDescription
		*
		* Get the description field as stored in the MessageTypeTable
		*
		* @param    messageType    The message Type to lookup
		* @param	DescriptionParameters	NameValuePairList of parameters to replace
		* @return	the formatted message description
		*/
		std::string getExpandedDescription(const TA_Base_Core::MessageType& messageType,
			const TA_Base_Core::DescriptionParameters& parameters);
		
			/**
			* getExpandedDescription
			*
			* Get the description field as stored in the MessageTypeTable
			*
			* @param    messageTypeKey    The message Type to lookup
			* @param	DescriptionParameters	NameValuePairList of parameters to replace
			* @return	the formatted message description
		*/
		std::string getExpandedDescription( unsigned long messageTypeKey,
			const std::string& parametersStr);
		
			/*
			virtual void processUpdate(ConfigUpdateDetails updateEvent);
		*/
        //Mintao++ TD15302
		/**
		* isMMSEnabled
		* 
		* Get the mms enabled status for the particular alarmtype
		* 
		* @return bool 
		* @param : const TA_Base_Core::MessageType& messageType
		* 
		* @exception <exceptions> Optional
		*/
		bool isMMSEnabled(const TA_Base_Core::MessageType& messageType);

		//TD17862 marc_bugfix 
		/**
		* getRelatedMMSType
		* 
		* Get the mms type for the particular alarm type 
		* 
		* @return bool 
		* @param : const TA_Base_Core::MessageType& messageType
		* 
		* @exception <exceptions> Optional
		*/

		TA_Base_Core::IAlarmData::EMmsState getRelatedMMSType(const TA_Base_Core::MessageType& messageType);
		

		
		/**
		* getAlarmTypeData
		*
		* Used to retrieve the alarmTypeData object, or create a new one if it does not
		* exist
		*
		* @param	typeKey	the typekey of the AlarmType object to load
		* @return	pointer to the AlarmTypeData object
			*/
		TA_Base_Core::IAlarmTypeData* getAlarmTypeRecord(const std::string& typeKey);
		
		/**
		* getAlarmTypeData
		*
		* Used to retrieve the alarmTypeData object, or create a new one if it does not
		* exist
		*
		* @param	typeKey	the typekey of the AlarmType object to load
		* @return	pointer to the AlarmTypeData object
			*/
		TA_Base_Core::IAlarmTypeData* getAlarmTypeRecord(unsigned long typeKey);

	private:		
		//TD17994,jianghp		
		/**
		* constructor
		*
		*private as this is a singleton
		*/
		AlarmTypeTableAccessor(){};

		AlarmTypeTableAccessor(const AlarmTypeTableAccessor &);
		AlarmTypeTableAccessor& operator =(const AlarmTypeTableAccessor &);
		//TD17994,jianghp		

	private:
		
		/**
		* Store all the MessageType data info in this map
		* indexed by MessageTypeKey
			*/
		typedef std::map<unsigned long, TA_Base_Core::IAlarmTypeData*> AlarmTypeMap;
		AlarmTypeMap m_alarmTypeMap;
		/**
		 * AlarmTypeMap lock to protect updating
		 */
		TA_Base_Core::ReEntrantThreadLockable    m_mapLock;
		
		//TD17994,jianghp
		/**
		* protect singleton creation
		*/
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		static AlarmTypeTableAccessor * m_theAlarmConfig;
		//TD17994,jianghp
	};
	
	
}; // TA_Base_Core
#endif // ifndef ALARM_CONFIG_H

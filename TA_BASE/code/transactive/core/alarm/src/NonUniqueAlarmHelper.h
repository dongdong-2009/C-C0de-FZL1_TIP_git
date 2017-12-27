#if !defined(NonUniqueAlarmHelper_B5F29856_8C7D_4f4c_84B9_6FC4D80B8485__INCLUDED_)
#define NonUniqueAlarmHelper_B5F29856_8C7D_4f4c_84B9_6FC4D80B8485__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/NonUniqueAlarmHelper.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * description
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/NameValuePair.h"
#include "core/alarm/src/AlarmConstants.h"
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef;
	class MessageType;
	class AlarmHelperBase;

	class NonUniqueAlarmHelper
	{
	public:
		/**
		* constructor
		*
		* @param	alarmHelperBase		the alarmHelperBase to use
		*/
		explicit NonUniqueAlarmHelper(AlarmHelperBase& alarmHelperBase);

		/**
		* destructor
		*/
		virtual ~NonUniqueAlarmHelper();

		/**
		* submitAlarm
		*
		* Submit a new alarm with the given details. This alarm does not need
		* to be unique, in fact it will always generate a duplicate alarm if this 
		* function is called and an identical alarm exists in the system already. 
		* It will return the AlarmID, this is necessary to close alarms.
		*
		* @return	std::string		the unique id of this alarm
		*
		* @param    alarmType    The specific type of the Alarm being generated
		* @param    entityKey    The entity that is raising the alarm
		* @param    entityTypeKey    The entityType of the entity that has raised the alarm
		* @param    description    The parameters for the description field
		* @param    entityName    The name of the entity
		* @param    locationKey    The location the entity belongs to
		* @param    subsystemKey    The subsystem the entity belongs to
		* @param	sourceTime	The time the alarm was first detected.
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		std::string submitAlarm(const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName,
			unsigned long locationKey, 
			unsigned long subsystemKey,
			timeb sourceTime = AlarmConstants::defaultTime);

		/**
		* submitAlarmWithSeverity
		*
		* Use this to submit an alarm that is unique in the system, with a specific severity
		*
		* @return	std::string		the unique id of this alarm
		*
		* @param    alarmType    The type of alarm as defined in the specific Message Type header file
		* @param    entityKey    The entity that has raised the alarm
		* @param    entityTypeKey    The entityType of the entity that has raised the alarm
		* @param    description    The parameters to be inserted into the description as defined in the AlarmType table
		* @param    entityName    The name of the entity
		* @param    locationKey    The location the entity belongs to
		* @param    subsystemKey    The subsystem the entity belongs to
		* @param    alarmSeverityKey    The severity to associate with this Alarm
		* @param	sourceTime	The time the alarm was first detected.
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		std::string submitAlarmWithSeverity(const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName, 
			unsigned long locationKey, 
			unsigned long subsystemKey,
			unsigned long alarmSeverityKey,
			timeb sourceTime = AlarmConstants::defaultTime);
		/**
		* submitAlarmWithAsset
		*
		* Submit a new alarm with the given details. This alarm does not need
		* to be unique, in fact it will always generate a duplicate alarm if this 
		* function is called and an identical alarm exists in the system already. 
		* It will return the AlarmID, this is necessary to close alarms.
		*
		* @return	std::string		the unique id of this alarm
		*
		* @param    alarmType    The specific type of the Alarm being generated
		* @param    entityKey    The entity that is raising the alarm
		* @param    entityTypeKey    The entityType of the entity that has raised the alarm
		* @param    description    The parameters for the description field
		* @param    entityName    The name of the entity
		* @param    locationKey    The location the entity belongs to
		* @param    subsystemKey    The subsystem the entity belongs to
		* @param    assetName   Asset name
		* @param	sourceTime	The time the alarm was first detected.
		* @param	mmsState	The mms state of the particular alarm.
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		std::string submitAlarmWithAsset(const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName,
			unsigned long locationKey, 
			unsigned long subsystemKey,
			const std::string& assetName,
			timeb sourceTime = AlarmConstants::defaultTime,
            TA_Base_Core::MmsStateType mmsState = MMS_NONE);

		/**
		* submitAlarmWithSeverityAndAsset
		*
		* Use this to submit an alarm that is unique in the system, with a specific severity
		*
		* @return	std::string		the unique id of this alarm
		*
		* @param    alarmType    The type of alarm as defined in the specific Message Type header file
		* @param    entityKey    The entity that has raised the alarm
		* @param    entityTypeKey    The entityType of the entity that has raised the alarm
		* @param    description    The parameters to be inserted into the description as defined in the AlarmType table
		* @param    entityName    The name of the entity
		* @param    locationKey    The location the entity belongs to
		* @param    subsystemKey    The subsystem the entity belongs to
		* @param    alarmSeverityKey    The severity to associate with this Alarm
		* @param    assetName   Asset name
		* @param	sourceTime	The time the alarm was first detected.
		* @param	mmsState	The mms state of the particular alarm.
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		std::string submitAlarmWithSeverityAndAsset(const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName, 
			unsigned long locationKey, 
			unsigned long subsystemKey,
			unsigned long alarmSeverityKey,
			const std::string& assetName,
			timeb sourceTime = AlarmConstants::defaultTime,
            TA_Base_Core::MmsStateType mmsState = MMS_NONE);

		/**
		* closeAlarm
		*
		* Close a given alarm, identified by the AlarmID
		*
		* @param    alarmID    The ID of the alarm to be closed
		*/
		void closeAlarm(const std::string& alarmID,
			const std::string &closeValue = "",
			const timeb closeTime = AlarmConstants::defaultTime);

		/**
		* closeAlarmAtLocation
		*
		* Close a given alarm, identified by the AlarmID
		*
		* @param    alarmID    The ID of the alarm to be closed
		*/
		void closeAlarmAtLocation(const std::string& alarmID, 
			unsigned long locationKey,
			const std::string &closeValue = "",
			const timeb closeTime = AlarmConstants::defaultTime);

		/**
		* getAlarmAtLocation
		*
		* @return AlarmDetailCorbaDef* of the alarm with the specified UID
		*
		* @param alarmID: The UID of the alarm, PRECONDITION: alarmID must not be empty
		* @param locationKey: The key of the location where the alarm was raised
		*
        * @exception NoAlarmAgentException raised if unable to locate an agent at the specified location
		* or communications fail
	    * @exception AlarmException if the specified alarm does not exist.
		*/
		AlarmDetailCorbaDef* getAlarmAtLocation(const std::string& alarmID,unsigned long locationKey);

		/** 
		  * getMostRecentActiveAlarmForEntity
		  *
		  * This function will retrieve the most recent alarm for an entity by querying
		  * the AlarmAgent assigned to the entities location
		  *
		  * @return AlarmDetailCorbaDef* the alarm
		  *
		  * @param  unsigned long entityKey - the entitykey of the entity
		  * @param unsigned long locationKey - the locationkey of the entity
          *
          * @exception AlarmAgentException raised if no alarms exist for the entity.
		  * @exception ObjectResolutionException if unable to contact the Agent
		  * @exception NoAlarmAgentExcption if unable to location an agent at the specified location
		  */
		AlarmDetailCorbaDef* getMostRecentActiveAlarmForEntity( unsigned long entityKey, 
			unsigned long locationKey );


	private:
		/**
		* disable default constructor
		*/
		NonUniqueAlarmHelper();
		
		/**
		* disable default copy constructor
		*/
		NonUniqueAlarmHelper( const NonUniqueAlarmHelper& theNonUniqueAlarmHelper);
		NonUniqueAlarmHelper& operator = ( const NonUniqueAlarmHelper& theNonUniqueAlarmHelper);

	private:
		/**
		* store a reference to the AlarmHelperBase class
		*/
		AlarmHelperBase& m_alarmHelperBase;

	public:
		static const std::string NOT_VISIBLE_DEFAULT_STRING;
	};


}; // namespace TA_Base_Core

#endif // !defined(NonUniqueAlarmHelper_B5F29856_8C7D_4f4c_84B9_6FC4D80B8485__INCLUDED_)

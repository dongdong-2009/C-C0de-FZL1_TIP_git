#if !defined(AlarmHelper_9D219F88_2F5D_412e_9B29_DCFF1A374D2F__INCLUDED_)
#define AlarmHelper_9D219F88_2F5D_412e_9B29_DCFF1A374D2F__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmHelper.h $
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
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"

#include "core/alarm/src/AlarmConstants.h"

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef;
	class MessageType;
	class AlarmHelperBase;

	class AlarmHelper
	{

	public:
		/**
		* constructor
		*
		* @param	alarmHelperBase		the alarmHelperBase to use
		*/
		AlarmHelper( AlarmHelperBase& alarmHelperBase );

		/**
		* destructor
		*/
		virtual ~AlarmHelper();

		/**
		* submitAlarm
		*
		* Use this to submit an alarm that is unique in the system
		*
		* @param    alarmType    The type of alarm as defined in the specific Message Type header file
		* @param    entityKey    The entity that has raised the alarm
		* @param    entityTypeKey    The entityType of the entity that has raised the alarm
		* @param    description    The parameters to be inserted into the description as defined in the AlarmType table
		* @param    entityName    The name of the entity
		* @param    locationKey    The location the entity belongs to
		* @param    subsystemKey    The subsystem the entity belongs to
		* @param	sourceTime	The time the alarm was first detected.
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		void submitAlarm(const TA_Base_Core::MessageType& alarmType,
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
		void submitAlarmWithSeverity(const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName, 
			unsigned long locationKey, 
			unsigned long subsystemKey,
			unsigned long alarmSeverityKey,
            timeb sourceTime = AlarmConstants::defaultTime);


		/**
		* submitAlarmWithSeverityAndAsset
		*
		* Use this to submit an alarm that is unique in the system, with a specific severity
		*
		* @param    alarmType    The type of alarm as defined in the specific Message Type header file
		* @param    entityKey    The entity that has raised the alarm
		* @param    entityTypeKey    The entityType of the entity that has raised the alarm
		* @param    description    The parameters to be inserted into the description as defined in the AlarmType table
		* @param    entityName    The name of the entity
		* @param    locationKey    The location the entity belongs to
		* @param    subsystemKey    The subsystem the entity belongs to
		* @param    alarmSeverityKey    The severity to associate with this Alarm
		* @param    assetName   Name of asset
		* @param	sourceTime	The time the alarm was first detected.
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		void AlarmHelper::submitAlarmWithSeverityAndAsset(const TA_Base_Core::MessageType& alarmType,
				unsigned long entityKey,
				unsigned long entityTypeKey,
				const TA_Base_Core::DescriptionParameters& description,
				const std::string& entityName, 
				unsigned long locationKey, 
				unsigned long subsystemKey,
				unsigned long alarmSeverityKey,
				const std::string&  assetName,
				timeb sourceTime = AlarmConstants::defaultTime,
                TA_Base_Core::MmsStateType mmsState = MMS_NONE);

				
		/**
		* submitAlarmWithAsset
		*
		* Use this to submit an alarm that is unique in the system, with a specific severity
		*
		* @param    alarmType    The type of alarm as defined in the specific Message Type header file
		* @param    entityKey    The entity that has raised the alarm
		* @param    entityTypeKey    The entityType of the entity that has raised the alarm
		* @param    description    The parameters to be inserted into the description as defined in the AlarmType table
		* @param    entityName    The name of the entity
		* @param    locationKey    The location the entity belongs to
		* @param    subsystemKey    The subsystem the entity belongs to
		* @param    assetName   Name of asset
		* @param	sourceTime	The time the alarm was first detected.
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		void AlarmHelper::submitAlarmWithAsset(const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName, 
			unsigned long locationKey, 
			unsigned long subsystemKey,
			const std::string& assetName,
			timeb sourceTime /*= 0*/ );

			
		/**
		* closeAlarm
		*
		* Close an alarm that is unique in the system. Note: If there are multiple alarms of the
		* same type, and entitykey, in the system, they will all be closed!
		*
		* @param    alarmType    The type of alarm as defined in the specific Message Type header file
		* @param    entityKey    The entity that has raised the alarm, that now wants to close it
		*/
		void closeAlarm(const TA_Base_Core::MessageType& alarmType, 
			unsigned long entityKey,
			const std::string &closeValue = "",
			const timeb closeTime = AlarmConstants::defaultTime);


		/**
		* closeAlarmAtLocation
		*
		* Close an alarm at a location that is unique in the system. Note: If there are multiple alarms of the
		* same type, and entitykey, in the system, they will all be closed!
		*
		* @param    alarmType    The type of alarm as defined in the specific Message Type header file
		* @param    entityKey    The entity that has raised the alarm, that now wants to close it
		*/
		void closeAlarmAtLocation(const TA_Base_Core::MessageType& alarmType, 
			unsigned long entityKey,
			unsigned long location,
			const std::string &closeValue = "",
			const timeb closeTime = AlarmConstants::defaultTime);


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
		  * @exception NoAlarmAgentException if unable to location an agent at the specified location
		  */
		AlarmDetailCorbaDef* getMostRecentActiveAlarmForEntity( unsigned long entityKey, 
			unsigned long locationKey );


		/** 
		  * getActiveAlarmForEntityOfType
		  *
		  * This function will retrieve the unack/open alarm for an entity of a defined 
		  * alarm type by querying the AlarmAgent assigned to the entities location.
		  * NOTE: Returned AlarmDetailCorbaDef* is a new deep copy and should be deleted by requester after use
		  *
		  * @return AlarmDetailCorbaDef* the alarm. If no such alarm, NULL will be returned.
		  *
		  *
		  * @param alarmType - The type of alarm as defined in the specific Message Type header file
		  * @param unsigned long entityKey - the entitykey of the entity
		  * @param unsigned long locationKey - the locationkey of the entity
          *
          * @exception AlarmAgentException raised if encounter problem in locating alarm for the entity.
		  * @exception ObjectResolutionException if unable to contact the Agent
		  * @exception NoAlarmAgentException if unable to location an agent at the specified location
		  */
		AlarmDetailCorbaDef* getActiveAlarmForEntityOfType( const TA_Base_Core::MessageType& alarmType, 
			unsigned long entityKey, unsigned long locationKey );


		/** 
		* getActiveAlarms
		*
		* This function will retrieve all of the active alarms from the alarmagent in this location 
		*
		* @return AlarmSequence* the alarm. 
		*
		*
		* @param unsigned long locationKey - the locationkey of the entity
		*
		* @exception AlarmAgentException raised if encounter problem in locating alarm for the entity.
		* @exception ObjectResolutionException if unable to contact the Agent
		* @exception NoAlarmAgentException if unable to location an agent at the specified location
		* @exception TransactiveException  if retreve alarm from alarm agent failed
		*/

		AlarmSequence* getActiveAlarms( unsigned long locationKey );

	private:
		/**
		* disable default constructor
		*/
		AlarmHelper();

		/**
		* disable default copy constructor
		*/
		AlarmHelper( const AlarmHelper& theAlarmHelper);
		AlarmHelper& operator = ( const AlarmHelper& theAlarmHelper);

	private:
	/**
		* store a reference to the AlarmHelperBase class
		*/
		AlarmHelperBase& m_alarmHelperBase;
	};


}; // namespace TA_Base_Core

#endif // !defined(AlarmHelper_9D219F88_2F5D_412e_9B29_DCFF1A374D2F__INCLUDED_)

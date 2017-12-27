#if !defined(ItaAlarms_372F836D_40B6_4a9c_93B8_86BB3EB55E0C__INCLUDED_)
#define ItaAlarms_372F836D_40B6_4a9c_93B8_86BB3EB55E0C__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/public_interfaces/ItaAlarms.h $
  * @author:  F. Stuart
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class provides an interface to the TransActive Alarms component
  * Parameter values are shorthand for:
  * entityKey = ItaRuntime::getMyEntityKey();
  * entityTypeKey = ItaRuntime::getMyEntityTypeKey();
  * entityName = ItaRuntime::getMyEntityName();
  * locationKey = ItaRuntime::getMyEntityLocationKey();
  * subsystemKey = ItaRuntime::getMyEntitySubsystemKey();
  * assetName = entityName;
  */

#include <string>

#include "core/types/public_types/TypeDefs.h"
#include "core/types/public_types/Constants.h"
#include "core/message/src/NameValuePair.h"
#include "core/alarm/src/AlarmConstants.h"

namespace TA_Base_Core
{
    class MessageType;       // Forward declaration. Found in core/message/types/MessageTypes.h
    struct AlarmDetailCorbaDef;

    class ItaAlarms
    {

    public:

	    /**
	     * raiseEntityUniqueAlarm
         *
         * Use this to submit an alarm that is unique in the
	     * system for the specified entity
         *
	     * @param alarmType    The type of alarm as defined in the specific Message Type
	     *                     header file
	     * @param entityKey    The entity that has raised the alarm
	     * @param entityTypeKey    The entityType of the entity that has raised the alarm
	     * @param description    The parameters to be inserted into the description as
	     *                       defined in the AlarmType table
	     * @param entityName    The name of the entity
	     * @param locationKey    The location the entity belongs to
	     * @param subsystemKey    The subsystem the entity belongs to
	     * @param assetName
	     * @param sourceTime    The time the alarm was first detected. The default means
	     *                      the time will be filled in by the alarm subsystem
	     */
	    virtual void raiseEntityUniqueAlarm(const TA_Base_Core::MessageType& alarmType,
                                            unsigned long entityKey,
                                            unsigned long entityTypeKey,
                                            const TA_Base_Core::NameValuePairs& description,
                                            const std::string& entityName,
                                            unsigned long locationKey,
                                            unsigned long subsystemKey,
                                            const std::string& assetName,
                                            const timeb& sourceTime = defaultTime) =0;

        
        /**
	     * raiseSystemUniqueAlarm
         *
         * Use this to submit an alarm that is unique in the
	     * system.
         *
	     * @param alarmType    The type of alarm as defined in the specific Message Type
	     *                     header file
	     * @param description    The parameters to be inserted into the
	     * @param sourceTime    The time the alarm was first detected. The default means
	     *                      the time will be filled in by the alarm subsystem
	     */
	    virtual void raiseSystemUniqueAlarm(const TA_Base_Core::MessageType& alarmType,
                                            const TA_Base_Core::NameValuePairs& description,
                                            const timeb& sourceTime = defaultTime) =0;


	    /**
	     * closeEntityUniqueAlarm
         *
         * Close an alarm that is unique in the system. Note: If
	     * there are multiple alarms of the same type, and entitykey, in the system, they
	     * will all be closed!
         *
	     * @param    alarmType    The type of alarm as defined in the specific Message
	     *                        Type header file
	     * @param    entityKey    The entity that has raised the alarm, that now wants to
	     *                        close it
	     */
	    virtual void closeEntityUniqueAlarm(const TA_Base_Core::MessageType& alarmType, 
			                                unsigned long entityKey) =0;

        /**
	     * closeEntityUniqueAlarm (2) - The prefered method
         *
         * Close an alarm that is unique in the system. Note: If
	     * there are multiple alarms of the same type, and entitykey, in the system, they
	     * will all be closed!
         *
	     * @param    alarmType    The type of alarm as defined in the specific Message
	     *                        Type header file
	     * @param    entityKey    The entity that has raised the alarm, that now wants to
	     *                        close it
         * @param    locationKey  The location the entity belongs to
	     */
	    virtual void closeEntityUniqueAlarm(const TA_Base_Core::MessageType& alarmType, 
			                                unsigned long entityKey,
                                            unsigned long locationKey) =0;

        /**
	     * closeSystemUniqueAlarm
         *
         * Close an alarm that is unique in the system
         *
	     * @param    alarmType    The type of alarm as defined in the specific Message
	     *                        Type header file
	     *
	     */
	    virtual void closeSystemUniqueAlarm(const TA_Base_Core::MessageType& alarmType) =0;


       /**
		* submitNonUniqueAlarm
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
		virtual std::string submitNonUniqueAlarm(const TA_Base_Core::MessageType& alarmType,
                                                 unsigned long entityKey,
                                                 unsigned long entityTypeKey,
                                                 const TA_Base_Core::DescriptionParameters& description,
                                                 const std::string &entityName,
                                                 unsigned long locationKey, 
                                                 unsigned long subsystemKey,
                                                 timeb sourceTime = AlarmConstants::defaultTime) = 0;

       /**
		* submitNonUniqueAlarm
		*
		* Submit a new alarm with the given details. This alarm does not need
		* to be unique, in fact it will always generate a duplicate alarm if this 
		* function is called and an identical alarm exists in the system already. 
		* It will return the AlarmID, this is necessary to close alarms.
		*
		* @return	std::string		the unique id of this alarm
		*
		* @param    alarmType    The specific type of the Alarm being generated
		* @param    description    The parameters for the description field
		* @param	sourceTime	The time the alarm was first detected.
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		virtual std::string submitNonUniqueAlarm(const TA_Base_Core::MessageType& alarmType,
                                                 const TA_Base_Core::DescriptionParameters& description,
                                                 timeb sourceTime = AlarmConstants::defaultTime) = 0;
        /**
		* submitNonUniqueAlarmWithSeverity
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
		virtual std::string submitNonUniqueAlarmWithSeverity(const TA_Base_Core::MessageType& alarmType,
                                                             unsigned long entityKey,
                                                             unsigned long entityTypeKey,
                                                             const TA_Base_Core::DescriptionParameters& description,
                                                             const std::string& entityName, 
                                                             unsigned long locationKey, 
                                                             unsigned long subsystemKey,
                                                             unsigned long alarmSeverityKey,
                                                             timeb sourceTime = AlarmConstants::defaultTime) = 0;
        /**
		* submitNonUniqueAlarmWithSeverity
		*
		* Use this to submit an alarm that is unique in the system, with a specific severity
		*
		* @return	std::string		the unique id of this alarm
		*
		* @param    alarmType    The type of alarm as defined in the specific Message Type header file
		* @param    description    The parameters to be inserted into the description as defined in the AlarmType table
		* @param    alarmSeverityKey    The severity to associate with this Alarm
		* @param	sourceTime	The time the alarm was first detected.
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		virtual std::string submitNonUniqueAlarmWithSeverity(const TA_Base_Core::MessageType& alarmType,
                                                             const TA_Base_Core::DescriptionParameters& description,
                                                             unsigned long alarmSeverityKey,
                                                             timeb sourceTime = AlarmConstants::defaultTime) = 0;
		
        /**
		* submitNonUniqueAlarmWithAsset
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
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		virtual std::string submitNonUniqueAlarmWithAsset(const TA_Base_Core::MessageType& alarmType,
                                                          unsigned long entityKey,
                                                          unsigned long entityTypeKey,
                                                          const TA_Base_Core::DescriptionParameters& description,
                                                          const std::string &entityName,
                                                          unsigned long locationKey, 
                                                          unsigned long subsystemKey,
                                                          const std::string& assetName,
                                                          timeb sourceTime = AlarmConstants::defaultTime) = 0;
        /**
		* submitNonUniqueAlarmWithAsset
		*
		* Submit a new alarm with the given details. This alarm does not need
		* to be unique, in fact it will always generate a duplicate alarm if this 
		* function is called and an identical alarm exists in the system already. 
		* It will return the AlarmID, this is necessary to close alarms.
		*
		* @return	std::string		the unique id of this alarm
		*
		* @param    alarmType    The specific type of the Alarm being generated
		* @param    description    The parameters for the description field
		* @param    assetName   Asset name
		* @param	sourceTime	The time the alarm was first detected.
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		virtual std::string submitNonUniqueAlarmWithAsset(const TA_Base_Core::MessageType& alarmType,
                                                          const TA_Base_Core::DescriptionParameters& description,
                                                          const std::string& assetName,
                                                          timeb sourceTime = AlarmConstants::defaultTime) = 0;

		/**
		* submitNonUniqueAlarmWithSeverityAndAsset
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
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		virtual std::string submitNonUniqueAlarmWithSeverityAndAsset(const TA_Base_Core::MessageType& alarmType,
			                                                         unsigned long entityKey,
                                                                     unsigned long entityTypeKey,
                                                                     const TA_Base_Core::DescriptionParameters& description,
                                                                     const std::string& entityName,
                                                                     unsigned long locationKey, 
                                                                     unsigned long subsystemKey,
                                                                     unsigned long alarmSeverityKey,
                                                                     const std::string& assetName,
                                                                     timeb sourceTime = AlarmConstants::defaultTime) = 0;
		/**
		* submitNonUniqueAlarmWithSeverityAndAsset
		*
		* Use this to submit an alarm that is unique in the system, with a specific severity
		*
		* @return	std::string		the unique id of this alarm
		*
		* @param    alarmType    The type of alarm as defined in the specific Message Type header file
		* @param    description    The parameters to be inserted into the description as defined in the AlarmType table
		* @param    alarmSeverityKey    The severity to associate with this Alarm
		* @param    assetName   Asset name
		* @param	sourceTime	The time the alarm was first detected.
		*									0 (default) means filled in by AlarmHelper using time( NULL )
		*/
		virtual std::string submitNonUniqueAlarmWithSeverityAndAsset(const TA_Base_Core::MessageType& alarmType,
                                                                     const TA_Base_Core::DescriptionParameters& description,
                                                                     unsigned long alarmSeverityKey,
                                                                     const std::string& assetName,
                                                                     timeb sourceTime = AlarmConstants::defaultTime) = 0;

		/**
		* closeNonUniqueAlarm
		*
		* Close a given alarm, identified by the AlarmID
		*
		* @param    alarmID    The ID of the alarm to be closed
		*/
		virtual void closeNonUniqueAlarm(const std::string& alarmID) = 0;

		/**
		* closeNonUniqueAlarmAtLocation
		*
		* Close a given alarm, identified by the AlarmID
		*
		* @param    alarmID    The ID of the alarm to be closed
		*/
		virtual void closeNonUniqueAlarmAtLocation(const std::string& alarmID,unsigned long locationKey) = 0;

		/**
		* getNonUniqueAlarmAtLocation
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
		virtual const AlarmDetailCorbaDef* getNonUniqueAlarmAtLocation(const std::string& alarmID,unsigned long locationKey) = 0;

		/** 
		  * getMostRecentActiveNonUniqueAlarmForEntity
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
		virtual const AlarmDetailCorbaDef* getMostRecentActiveNonUniqueAlarmForEntity( unsigned long entityKey, 
			                                                                     unsigned long locationKey ) = 0;

        virtual ~ItaAlarms() { };

    };
}
#endif // !defined(ItaAlarms_372F836D_40B6_4a9c_93B8_86BB3EB55E0C__INCLUDED_)

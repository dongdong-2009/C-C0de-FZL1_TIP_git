#if !defined(CtaAlarms_0B911420_DA2B_47f7_8561_CF92C5901A8B__INCLUDED_)
#define CtaAlarms_0B911420_DA2B_47f7_8561_CF92C5901A8B__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/CtaAlarms.h $
  * @author:  K. Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  */

#include <memory>

#include "core/alarm/public_interfaces/ItaAlarms.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"


namespace TA_Base_Core
{
    // Forward Declaration
    class AlarmHelper;  
    class NonUniqueAlarmHelper;

    class CtaAlarms : public ItaAlarms 
    {
    public:

        /**
         * Constructor
         *
         * @param entityData - The entity data object for the application (ie agent) that is
         *                     raising alarms.
         *
         * @exception DataException - If the application data cannot be loaded from the entity passed
         *                            in then this exception will be thrown.
         * @exception DatabaseException - If the application data cannot be loaded from the database
         *                                then this exception will be thrown.
         */
	    explicit CtaAlarms(IEntityDataPtr agentConfiguration);
	    
        
        /**
         * Destructor
         */
        virtual ~CtaAlarms();


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
                                            const timeb& sourceTime = defaultTime);


        /**
	     * raiseSystemUniqueAlarm
         *
         * Use this to submit an alarm that is unique in the
	     * system.
         *
	     * @param alarmType    The type of alarm as defined in the specific Message Type
	     *                     header file
	     * @param description    The parameters to be inserted into the description as
	     *                       defined in the AlarmType table
	     * @param sourceTime    The time the alarm was first detected. The default means
	     *                      the time will be filled in by the alarm subsystem
	     */
	    virtual void raiseSystemUniqueAlarm(const TA_Base_Core::MessageType& alarmType,
                                            const TA_Base_Core::NameValuePairs& description,
                                            const timeb& sourceTime = defaultTime);

        
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
			                                unsigned long entityKey);

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
                                            unsigned long locationKey);

        /**
	     * closeSystemUniqueAlarm
         *
         * Close an alarm that is unique in the system
         *
	     * @param    alarmType    The type of alarm as defined in the specific Message
	     *                        Type header file
	     *
	     */
	    virtual void closeSystemUniqueAlarm(const TA_Base_Core::MessageType& alarmType);

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
                                                 timeb sourceTime = AlarmConstants::defaultTime);
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
                                                 timeb sourceTime = AlarmConstants::defaultTime);

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
                                                             timeb sourceTime = AlarmConstants::defaultTime);
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
                                                             timeb sourceTime = AlarmConstants::defaultTime);
		
		
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
                                                          const std::string& entityName,
                                                          unsigned long locationKey, 
                                                          unsigned long subsystemKey,
                                                          const std::string& assetName,
                                                          timeb sourceTime = AlarmConstants::defaultTime);
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
                                                          timeb sourceTime = AlarmConstants::defaultTime);

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
                                                                     timeb sourceTime = AlarmConstants::defaultTime);

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
                                                                     timeb sourceTime = AlarmConstants::defaultTime);

		/**
		* closeNonUniqueAlarm
		*
		* Close a given alarm, identified by the AlarmID
		*
		* @param    alarmID    The ID of the alarm to be closed
		*/
		virtual void closeNonUniqueAlarm(const std::string& alarmID);

		/**
		* closeNonUniqueAlarmAtLocation
		*
		* Close a given alarm, identified by the AlarmID
		*
		* @param    alarmID    The ID of the alarm to be closed
		*/
		virtual void closeNonUniqueAlarmAtLocation(const std::string& alarmID,unsigned long locationKey);

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
		virtual const AlarmDetailCorbaDef* getNonUniqueAlarmAtLocation(const std::string& alarmID,unsigned long locationKey);

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
			                                                                     unsigned long locationKey );


    private:

        /**
         * Disable copy Constructor.
         */
        CtaAlarms(const CtaAlarms& rhs);

        /**
         * Disable assignment operator
         */
        const CtaAlarms& operator=(const CtaAlarms& rhs);


        // The object that will raise and close the alarms.
		AlarmHelper& m_alarmHelper;

        // The object that will raise and close the alarms.
        NonUniqueAlarmHelper& m_nonUniqueAlarmHelper;

        // The configuration data of the agent. This information is used for system alarms.
        unsigned long m_entityKey;
        unsigned long m_entityTypeKey;
        std::string m_entityName;
        unsigned long m_entityLocationKey;
        unsigned long m_entitySubsystemKey;
    };
}
#endif // !defined(CtaAlarms_0B911420_DA2B_47f7_8561_CF92C5901A8B__INCLUDED_)

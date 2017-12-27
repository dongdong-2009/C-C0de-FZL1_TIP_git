/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ross Tucker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */

#ifdef _MSC_VER
  #pragma warning(disable: 4786)
  #pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "app/alarm/alarm_agent/src/AutoAcknowledgeRule.h"
#include "app/alarm/alarm_agent/src/AlarmCache.h"
#include "app/alarm/alarm_agent/src/LocalRunParamDefinitions.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

using TA_Base_Core::RunParams;


namespace TA_Base_App
{

    //
    // Constructor
    //
    AutoAcknowledgeRule::AutoAcknowledgeRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmTypeKey,
                                             TA_Base_App::AbstractRule::ETriggerType trigger, unsigned long key )
    : AbstractRule(entityKey, entityType, alarmTypeKey, trigger, key)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Entity key is %lu", entityKey );
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Entity type is %lu", entityType );

        if( alarmTypeKey == 0 )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Alarm type is empty" );
        }
        else
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Alarm type is %u", alarmTypeKey );
        }
		//TD17851 marc_bugfix begin
		m_entityKey = entityKey;
		m_entityType = entityType;
		m_alarmTypeKey = alarmTypeKey;
		m_trigger = trigger;
		m_key = key;
		//TD17851 marc_bugfix end
    }


    //
    // Destructor
    //
    AutoAcknowledgeRule::~AutoAcknowledgeRule()
    {}


    //
    // process
    //
    void AutoAcknowledgeRule::process( const TA_Base_Core::AlarmDetailCorbaDef& message,
                                       TA_Base_App::AbstractRule::ETriggerType trigger)
    {
		bool bSpecialCase = false;

        if( doesRuleApply(message, trigger) && message.isUniqueAlarm == true ) // This check for the correct message type and trigger, and alarm is unique
        {
			//TD17851 marc_bugfix begin
			if (m_entityKey == 0 && m_entityType == 0 && m_alarmTypeKey == 0)
			{
				//This checking is necessary to determine if the current autoacknowledge rule
				//should only be applied to past alarms 
				bSpecialCase = true;
			}
			//TD17851 marc_bugfix end

            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Auto ack rule applies to alarm" );

            TA_Base_Core::AlarmSequence* alarms = 0;
            alarms = AlarmCache::getInstance()->getByEntityAndAlarmType(message.assocEntityKey, message.messageTypeKey);
            for( unsigned int i = 0; i < alarms->length(); i++ )
            {
		      if (bSpecialCase)
			  {
				  if( (*alarms)[i].state != TA_Base_Core::AlarmOpen )
				  {
						LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
							 "Acknowledgin alarm with ID %s, Type %u, Entity key %d", (*alarms)[i].alarmID.in(),
																					  (*alarms)[i].messageTypeKey,
																					  (*alarms)[i].assocEntityKey);
 
						if( trigger == TA_Base_App::AbstractRule::SUBMIT )
						{
							AlarmCache::getInstance()->updateAlarmAcknowledgeStateNonReentrant(std::string((*alarms)[i].alarmID), 
								RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::OPERATOR_NAME.c_str()),
							RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str()));
						}
						else
						{
							AlarmCache::getInstance()->updateAlarmAcknowledgeStateReentrant(std::string((*alarms)[i].alarmID), 
								RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::OPERATOR_NAME.c_str()),
								RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str()));
						}
				  }

			  }
			  else
			  {
                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                         "Acknowledgin alarm with ID %s, Type %u, Entity key %d", (*alarms)[i].alarmID.in(),
                                                                                  (*alarms)[i].messageTypeKey,
                                                                                (*alarms)[i].assocEntityKey);
					
                    if( trigger == TA_Base_App::AbstractRule::SUBMIT )
                    {
                        AlarmCache::getInstance()->updateAlarmAcknowledgeStateNonReentrant(std::string((*alarms)[i].alarmID), 
                            RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::OPERATOR_NAME.c_str()),
                        RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str()));
                    }
                    else
                    {
                        AlarmCache::getInstance()->updateAlarmAcknowledgeStateReentrant(std::string((*alarms)[i].alarmID), 
                            RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::OPERATOR_NAME.c_str()),
                            RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str()));
                    }

			  }
            }
            delete alarms;
        }

    }
	//TD17851 marc_bugfix begin
    // doesRuleApply
    //
    bool AutoAcknowledgeRule::doesRuleApply( const TA_Base_Core::AlarmDetailCorbaDef& message,
                                      const ETriggerType& trigger )
    {
		//Check if trigger matches
        if( m_trigger != trigger )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Rule trigger does not match alarm trigger" );
            return false;
        }

        if( m_entityKey != 0 )
        {
			//Check if entity keys matches
            if( message.assocEntityKey == m_entityKey )
            {
                if( m_alarmTypeKey != 0 )
                { 
                    if( m_alarmTypeKey == message.messageTypeKey )
                    {
                        return true;
                    }
                    else
                    {
                        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                             "Entity keys match but Rule Message type %u does not match rule alarm message type %u", 
                             m_alarmTypeKey, message.messageTypeKey);

                        return false;
                    }
                }
                else
                {
                    return true;
                }
            }
			else
			{
	            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
		             "Rule Entity key %li does not match alarm entity key %li", m_entityKey, message.assocEntityKey );
	
		        return false;

			}
        }
        else if( m_entityType != 0 )
        {
			//Check if entity type matches
            if( m_entityType == message.assocEntityTypeKey )
            {
                if( m_alarmTypeKey != 0 )
                { 
                    if( m_alarmTypeKey == message.messageTypeKey)
                    {
                        return true;
                    }
                    else
                    {
                        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                             "Entity types match but Rule Message type %u does not match rule alarm message type %u", 
                             m_alarmTypeKey, message.messageTypeKey );

                        return false;
                    }
                }
                else
                {
                    return true;
                }
            }
            else
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                     "Rule Entity type %li does not match alarm entity type %li", m_entityType, message.assocEntityTypeKey );

                return false;
            }
        }

		//Check if alarm types matches
        if( m_alarmTypeKey != 0 )
        {
            if( m_alarmTypeKey == message.messageTypeKey )
            {
				return true;
            }
			else
			{
	            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                 "Rule Alarm type %u does not match alarm alarm key type %u",
                 m_alarmTypeKey, message.messageTypeKey );
	
		        return false;
			}
        }
		return true;

    }
	//TD17851 marc_bugfix end
};

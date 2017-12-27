/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AbstractRule.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "app/alarm/alarm_agent/src/AbstractRule.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    //
    // Constructor
    //
    AbstractRule::AbstractRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmTypeKey,
                                   ETriggerType trigger, unsigned long key )
    : m_entityKey(entityKey), m_entityType(entityType), m_alarmTypeKey(alarmTypeKey), m_trigger(trigger), m_key(key)
    {}


    //
    // Destructor
    //
    AbstractRule::~AbstractRule()
    {}


    //
    // alarmShouldBeSuppressed
    //
    bool AbstractRule::alarmShouldBeSuppressed( const TA_Base_Core::AlarmDetailCorbaDef& message )
    {
        return false;
    }


    //
    // alarmIsAnAvalancheHead
    //
    bool AbstractRule::alarmIsAnAvalancheHead( const TA_Base_Core::AlarmDetailCorbaDef& message )
    {
        return false;
    }


    //
    // Comparison operator
    //
    bool AbstractRule::operator ==( const AbstractRule& rule )
    {
        if(m_key == rule.m_key)
        {
            return true;
        }
        return false;
    }


    //
    // getEntityKey
    //
    const unsigned long AbstractRule::getEntityKey()
    {
        return m_entityKey;
    }


    //
    // getEntityType
    //
    const unsigned long AbstractRule::getEntityType()
    {
        return m_entityType;
    }


    //
    // getAlarmType
    //
    const unsigned long AbstractRule::getAlarmTypeKey()
    {
        return m_alarmTypeKey;
    }


    //
    // getTrigger
    //
    const TA_Base_App::AbstractRule::ETriggerType AbstractRule::getTrigger()
    {
        return m_trigger;
    }


    //
    // getKey
    //
    const unsigned long AbstractRule::getKey()
    {
        return m_key;
    }


    //
    // doesRuleApply
    //
    bool AbstractRule::doesRuleApply( const TA_Base_Core::AlarmDetailCorbaDef& message,
                                      const ETriggerType& trigger )
    {
        if( m_trigger != trigger )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugTrace, "Rule trigger does not match alarm trigger" );
            return false;
        }

        if( m_entityKey != 0 )
        {
            if( message.assocEntityKey == m_entityKey )
            {
                if( m_alarmTypeKey != 0)
                { 
                    if( m_alarmTypeKey == message.messageTypeKey )
                    {
                        return true;
                    }
                    else
                    {
                        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugTrace, 
                             "Entity keys match but Rule Message type %u does not match rule alarm message type %u", 
                             m_alarmTypeKey, message.messageTypeKey );

                        return false;
                    }
                }
                else
                {
                    return true;
                }
            }

            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugTrace, 
                 "Rule Entity key %li does not match alarm entity key %li", m_entityKey, message.assocEntityKey );

            return false;
        }

        if( m_alarmTypeKey != 0 )
        {
            if( m_alarmTypeKey == message.messageTypeKey )
            {
                if( m_entityType != 0 )
                {
                    if( m_entityType == message.assocEntityTypeKey )
                    {
                        return true;
                    }
                    else
                    {
                        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugTrace, 
                             "Alarm types match but Rule Entity type %li does not match rule alarm entity type %li", 
                             m_entityType, message.assocEntityTypeKey );

                        return false;
                    }
                }
                else
                {
                    return true;
                }
            }

            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugTrace, 
                 "Rule Alarm type %u does not match alarm alarm key type %u",
                 m_alarmTypeKey, message.messageTypeKey );

            return false;
        }

        if( m_entityType != 0 )
        {
            if( m_entityType == message.assocEntityTypeKey )
            {
                return true;
            }
            else
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugTrace, 
                     "Rule Entity type %li does not match alarm entity type %li", m_entityType, message.assocEntityTypeKey );

                return false;
            }
        }

        return true;
    }
};

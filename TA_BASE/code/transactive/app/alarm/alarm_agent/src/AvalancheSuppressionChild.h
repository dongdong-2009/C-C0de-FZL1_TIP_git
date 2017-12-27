/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AvalancheSuppressionChild.h $
  * @author:  Ross Tucker
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AVALANCHESUPPRESSIONCHILD)
#define AVALANCHESUPPRESSIONCHILD

#include "app/alarm/alarm_agent/src/AvalancheSuppressionRule.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	class AvalancheSuppressionRule;
	
    class AvalancheSuppressionChild
    {
    public:
        
        /** 
          * Default constructor
          */
        AvalancheSuppressionChild() : m_entityType(0), m_entityKey(0), m_alarmType(0), m_pAvalancheHead(NULL),m_IsAvalancheHead(false)
        {}

        /** 
          * Class constructor
          *
          * @param Entity type key
          * @param Entity key
          * @param Alarm type
          */
        AvalancheSuppressionChild( unsigned long p_entityType, unsigned long p_entityKey, unsigned long p_alarmType )
        : m_entityType(p_entityType), m_entityKey(p_entityKey), m_alarmType(p_alarmType)
        {
			m_pAvalancheHead = NULL;
			m_IsAvalancheHead = false;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                 "Avalanche child details : entity key - %lu entity type - %lu alarm type - %u", p_entityKey, p_entityType, p_alarmType );
        }

        /** 
          * Destructor
          */
        ~AvalancheSuppressionChild(){}

        /** 
          * getEntityType
          *
          * @return The child entity type 
          */
        const unsigned long getEntityType() 
        { 
            return m_entityType;
        }

        /** 
          * setEntityType
          *
          * @param The child entity type
          */
        void setEntityType( unsigned long p_entityType )
        {
            m_entityType = p_entityType;
        }

        /** 
          * getEntityKey
          *
          * @return The child entity key 
          */
        const unsigned long getEntityKey()
        {
            return m_entityKey;
        }

        /** 
          * setEntityKey
          *
          * @param The child entity key
          */
        void setEntityKey( unsigned long p_entityKey )
        {
            m_entityKey = p_entityKey;
        }

        /** 
          * getAlarmType
          *
          * @return The child alarm type 
          */
        unsigned long getAlarmType()
        {
            return m_alarmType;
        }

        /** 
          * setAlarmType
          *
          * @param The child alarm type
          */
        void setAlarmType( unsigned long p_alarmType )
        {
			m_alarmType = p_alarmType;
        }


        /** 
          * compareToAlarm
          *
          * This method takes an alarm and returns true if the alarm matches the child.
          * That is, true if the alarm is subject to the suppression rule
          *
          * @param The alarm
          *
          * @return TRUE if the alarm matches the child description
          */
        const bool compareToAlarm( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm )
        {
            if( m_entityType != 0 )
            {
                if( m_entityType != p_alarm.assocEntityTypeKey )
                {
                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                         "Entity type did not match %lu and %lu", m_entityType, p_alarm.assocEntityTypeKey );

                    return false;
                }
            }

            if( m_entityKey != 0 )
            {
                if( m_entityKey != p_alarm.assocEntityKey )
                {
                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                         "Entity key did not match %lu and %lu", m_entityKey, p_alarm.assocEntityKey );

                    return false;
                }
            }

            if( m_alarmType != 0 )
            {
				// CL 15035 
				// change "==" to "!="
                if( m_alarmType != p_alarm.messageTypeKey )
                {
                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                         "Alarm type did not match %u and %u", m_alarmType, 
                                                               p_alarm.messageTypeKey);

                    return false;
                }
            }

            return true;
        }

        /**
          * Comparison operator
          *
          * @param The AvalancheSuppresionChild to compare
          *
          * @return TRUE if the entity type, entity key, and alarm type all match
          */
        bool operator== (const AvalancheSuppressionChild& child)
        {
            if( (m_entityType == child.m_entityType) &&
                (m_entityKey == child.m_entityKey) &&
                (m_alarmType == child.m_alarmType) )
            {
                return true;
            }
            return false;
        }

		AvalancheSuppressionRule* getAvalancheHead()
		{
			return m_pAvalancheHead;
		}

		void setAvalancheHead(AvalancheSuppressionRule* AvalancheHead=NULL)
		{
			if (AvalancheHead != NULL)
			{
				m_IsAvalancheHead=true;
			}
			m_pAvalancheHead = AvalancheHead;
		}
		void delAvalancheHead()
		{
			m_pAvalancheHead = NULL;
			m_IsAvalancheHead=false;
		}
		bool isSetAvalancheHead()
		{
			return m_IsAvalancheHead;
		}

    private:

        unsigned long m_entityType;

        unsigned long m_entityKey;

        unsigned long m_alarmType;

		AvalancheSuppressionRule* m_pAvalancheHead;

		bool m_IsAvalancheHead;

    };
}

#endif // !defined(AVALANCHESUPPRESSIONCHILD)

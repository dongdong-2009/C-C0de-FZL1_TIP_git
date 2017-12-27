/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/CtaAlarms.cpp $
  * @author:  K. Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "CtaAlarms.h"

#include "core/message/types/MessageTypes.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    CtaAlarms::CtaAlarms(IEntityDataPtr agentConfiguration)
	:
	m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper()),
	m_nonUniqueAlarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper())
    {
        // The calls below can throw Data or Database exceptions
        m_entityKey = agentConfiguration->getKey();
        m_entityTypeKey = agentConfiguration->getTypeKey();
        m_entityName = agentConfiguration->getName();
        m_entityLocationKey = agentConfiguration->getLocation();
        m_entitySubsystemKey = agentConfiguration->getSubsystem();
    }


    CtaAlarms::~CtaAlarms()
    {
        // Empty
    }


    void CtaAlarms::raiseEntityUniqueAlarm(const TA_Base_Core::MessageType& alarmType,
                                           unsigned long entityKey,
                                           unsigned long entityTypeKey, 
                                           const TA_Base_Core::NameValuePairs& description, 
                                           const std::string& entityName, 
                                           unsigned long locationKey, 
                                           unsigned long subsystemKey, 
                                           const std::string& assetName, 
                                           const timeb& sourceTime)
    {
		////TD17994,jianghp
        //TA_ASSERT(m_alarmHelper.get()!=0, "Alarm Helper has not been constructed!");

        // Take the named pairs passed in and convert them into a set of description parameters
        DescriptionParameters dp;
        for(NameValuePairs::const_iterator iter = description.begin(); iter != description.end(); ++iter)
        {
			dp.push_back( new NameValuePair(iter->first,iter->second) );
        }

        // Submit the alarm
        m_alarmHelper.submitAlarmWithAsset(alarmType, entityKey, entityTypeKey, dp, entityName,
                                            locationKey, subsystemKey, assetName, sourceTime);
                           
        // Now clean up the description parameters
        for(DescriptionParameters::iterator it = dp.begin(); it != dp.end(); ++it)
        {
            if( (*it) != NULL )
            {
                delete *it;
                *it = NULL;
            }
        }
    }


    void CtaAlarms::raiseSystemUniqueAlarm(const TA_Base_Core::MessageType& alarmType, 
                                           const TA_Base_Core::NameValuePairs& description,
                                           const timeb& sourceTime)
    {
		//TD17994,jianghp
        //TA_ASSERT(m_alarmHelper.get()!=0, "Alarm Helper has not been constructed!");

        // Take the named pairs passed in and convert them into a set of description parameters
        DescriptionParameters dp;
        for(NameValuePairs::const_iterator iter = description.begin(); iter != description.end(); ++iter)
        {
			dp.push_back( new NameValuePair(iter->first,iter->second) );
        }

        // Submit alarm
        m_alarmHelper.submitAlarm(alarmType,
                                   m_entityKey,
                                   m_entityTypeKey,
                                   dp,
                                   m_entityName,
                                   m_entityLocationKey,
                                   m_entitySubsystemKey,
                                   sourceTime);

        // Now clean up the description parameters
        for(DescriptionParameters::iterator it = dp.begin(); it != dp.end(); ++it)
        {
            if( (*it) != NULL )
            {
                delete *it;
                *it = NULL;
            }
        }
    }


    void CtaAlarms::closeEntityUniqueAlarm(const TA_Base_Core::MessageType& alarmType, 
			                               unsigned long entityKey)
    {
		//TD17994,jianghp
        //TA_ASSERT(m_alarmHelper.get()!=0, "Alarm Helper has not been constructed!");
		//TD17994,jianghp

        //m_alarmHelper.closeAlarm(alarmType, entityKey);
		m_alarmHelper.closeAlarmAtLocation(alarmType, entityKey, m_entityLocationKey);
    }

    void CtaAlarms::closeEntityUniqueAlarm(const TA_Base_Core::MessageType& alarmType, 
			                               unsigned long entityKey,
                                           unsigned long locationKey)
    {
		//TD17994,jianghp
        //TA_ASSERT(m_alarmHelper.get()!=0, "Alarm Helper has not been constructed!");
        m_alarmHelper.closeAlarmAtLocation(alarmType, entityKey, locationKey);
    }

    void CtaAlarms::closeSystemUniqueAlarm(const TA_Base_Core::MessageType& alarmType)
    {
		//TD17994,jianghp
        //TA_ASSERT(m_alarmHelper.get()!=0, "Alarm Helper has not been constructed!");
        m_alarmHelper.closeAlarmAtLocation(alarmType, m_entityKey, m_entityLocationKey);
    }

	std::string CtaAlarms::submitNonUniqueAlarm(const TA_Base_Core::MessageType& alarmType,
                                                unsigned long entityKey,
                                                unsigned long entityTypeKey,
                                                const TA_Base_Core::DescriptionParameters& description,
                                                const std::string& entityName,
                                                unsigned long locationKey, 
                                                unsigned long subsystemKey,
                                                timeb sourceTime)
    {
		//TD17994,jianghp
        //TA_ASSERT(m_nonUniqueAlarmHelper.get()!=0, "Non Unique Alarm Helper has not been constructed!");
		//TD17994,jianghp

        TA_ASSERT(entityName.empty() == false, "entity name is empty");

        return m_nonUniqueAlarmHelper.submitAlarm(alarmType,
                                                   entityKey,
                                                   entityTypeKey,
                                                   description,
                                                   entityName,
                                                   locationKey, 
                                                   subsystemKey,
                                                   sourceTime);
    }

    std::string CtaAlarms::submitNonUniqueAlarm(const TA_Base_Core::MessageType& alarmType,
                                                const TA_Base_Core::DescriptionParameters& description,
                                                timeb sourceTime)
    {
		//TD17994,jianghp
        //TA_ASSERT(m_nonUniqueAlarmHelper.get()!=0, "Non Unique Alarm Helper has not been constructed!");
		//TD17994,jianghp

        return m_nonUniqueAlarmHelper.submitAlarm(alarmType,
                                                   m_entityKey,
                                                   m_entityTypeKey,
                                                   description,
                                                   m_entityName,
                                                   m_entityLocationKey, 
                                                   m_entitySubsystemKey,
                                                   sourceTime);
    }

	std::string CtaAlarms::submitNonUniqueAlarmWithSeverity(const TA_Base_Core::MessageType& alarmType,
			                                                unsigned long entityKey,
                                                            unsigned long entityTypeKey,
                                                            const TA_Base_Core::DescriptionParameters& description,
                                                            const std::string& entityName, 
                                                            unsigned long locationKey, 
                                                            unsigned long subsystemKey,
                                                            unsigned long alarmSeverityKey,
                                                            timeb sourceTime)
    {
		//TD17994,jianghp
        //TA_ASSERT(m_nonUniqueAlarmHelper.get()!=0, "Non Unique Alarm Helper has not been constructed!");
		//TD17994,jianghp

        TA_ASSERT(entityName.empty() == false, "entity name is empty");

        return m_nonUniqueAlarmHelper.submitAlarmWithSeverity(alarmType,
			                                                   entityKey,
                                                               entityTypeKey,
                                                               description,
                                                               entityName, 
                                                               locationKey, 
                                                               subsystemKey,
                                                               alarmSeverityKey,
                                                               sourceTime);
    }

	std::string CtaAlarms::submitNonUniqueAlarmWithSeverity(const TA_Base_Core::MessageType& alarmType,
                                                            const TA_Base_Core::DescriptionParameters& description,
                                                            unsigned long alarmSeverityKey,
                                                            timeb sourceTime)
    {
		//TD17994,jianghp
        //TA_ASSERT(m_nonUniqueAlarmHelper.get()!=0, "Non Unique Alarm Helper has not been constructed!");
		//TD17994,jianghp

        return m_nonUniqueAlarmHelper.submitAlarmWithSeverity(alarmType,
			                                                   m_entityKey,
                                                               m_entityTypeKey,
                                                               description,
                                                               m_entityName, 
                                                               m_entityLocationKey, 
                                                               m_entitySubsystemKey,
                                                               alarmSeverityKey,
                                                               sourceTime);
    }
    
	std::string CtaAlarms::submitNonUniqueAlarmWithAsset(const TA_Base_Core::MessageType& alarmType,
                                                         unsigned long entityKey,
                                                         unsigned long entityTypeKey,
                                                         const TA_Base_Core::DescriptionParameters& description,
                                                         const std::string& entityName,
                                                         unsigned long locationKey, 
                                                         unsigned long subsystemKey,
                                                         const std::string& assetName,
                                                         timeb sourceTime)
    {
		//TD17994,jianghp
        TA_ASSERT(entityName.empty() == false, "entity name is empty");
        TA_ASSERT(assetName.empty() == false, "asset name is empty");

        return m_nonUniqueAlarmHelper.submitAlarmWithAsset(alarmType,
                                                            entityKey,
                                                            entityTypeKey,
                                                            description,
                                                            entityName,
                                                            locationKey, 
                                                            subsystemKey,
                                                            assetName,
                                                            sourceTime);
    }

    std::string CtaAlarms::submitNonUniqueAlarmWithAsset(const TA_Base_Core::MessageType& alarmType,
                                                         const TA_Base_Core::DescriptionParameters& description,
                                                         const std::string& assetName,
                                                         timeb sourceTime)
    {
		//TD17994,jianghp
        //TA_ASSERT(m_nonUniqueAlarmHelper.get()!=0, "Non Unique Alarm Helper has not been constructed!");
		//TD17994,jianghp

        TA_ASSERT(assetName.empty() == false, "asset name is empty");

        return m_nonUniqueAlarmHelper.submitAlarmWithAsset(alarmType,
                                                            m_entityKey,
                                                            m_entityTypeKey,
                                                            description,
                                                            m_entityName,
                                                            m_entityLocationKey, 
                                                            m_entitySubsystemKey,
                                                            assetName,
                                                            sourceTime);
    }


	std::string CtaAlarms::submitNonUniqueAlarmWithSeverityAndAsset(const TA_Base_Core::MessageType& alarmType,
                                                                    unsigned long entityKey,
                                                                    unsigned long entityTypeKey,
                                                                    const TA_Base_Core::DescriptionParameters& description,
                                                                    const std::string& entityName, 
                                                                    unsigned long locationKey, 
                                                                    unsigned long subsystemKey,
                                                                    unsigned long alarmSeverityKey,
                                                                    const std::string& assetName,
                                                                    timeb sourceTime)
    {
        //TA_ASSERT(m_nonUniqueAlarmHelper.get()!=0, "Non Unique Alarm Helper has not been constructed!");
        TA_ASSERT(entityName.empty() == false, "entity name is empty");
        TA_ASSERT(assetName.empty() == false, "asset name is empty");

        return m_nonUniqueAlarmHelper.submitAlarmWithSeverityAndAsset(alarmType,
                                                                       entityKey,
                                                                       entityTypeKey,
                                                                       description,
                                                                       entityName, 
                                                                       locationKey, 
                                                                       subsystemKey,
                                                                       alarmSeverityKey,
                                                                       assetName,
                                                                       sourceTime);
    }

	std::string CtaAlarms::submitNonUniqueAlarmWithSeverityAndAsset(const TA_Base_Core::MessageType& alarmType,
                                                                    const TA_Base_Core::DescriptionParameters& description,
                                                                    unsigned long alarmSeverityKey,
                                                                    const std::string& assetName,
                                                                    timeb sourceTime)
    {

        //TA_ASSERT(m_nonUniqueAlarmHelper.get()!=0, "Non Unique Alarm Helper has not been constructed!");
        TA_ASSERT(assetName.empty() == false, "asset name is empty");

        return m_nonUniqueAlarmHelper.submitAlarmWithSeverityAndAsset(alarmType,
                                                                       m_entityKey,
                                                                       m_entityTypeKey,
                                                                       description,
                                                                       m_entityName, 
                                                                       m_entityLocationKey, 
                                                                       m_entitySubsystemKey,
                                                                       alarmSeverityKey,
                                                                       assetName,
                                                                       sourceTime);
    }

	void CtaAlarms::closeNonUniqueAlarm(const std::string& alarmID)
    {
        //TA_ASSERT(m_nonUniqueAlarmHelper.get()!=0, "Non Unique Alarm Helper has not been constructed!");
        TA_ASSERT(alarmID.empty() == false, "alarm id is empty");

        //m_nonUniqueAlarmHelper.closeAlarm(alarmID);
		m_nonUniqueAlarmHelper.closeAlarmAtLocation(alarmID, m_entityLocationKey);

    }

	void CtaAlarms::closeNonUniqueAlarmAtLocation(const std::string& alarmID,unsigned long locationKey)
    {
        //TA_ASSERT(m_nonUniqueAlarmHelper.get()!=0, "Non Unique Alarm Helper has not been constructed!");
        TA_ASSERT(alarmID.empty() == false, "alarm id is empty");

        m_nonUniqueAlarmHelper.closeAlarmAtLocation(alarmID,locationKey);
    }

	const AlarmDetailCorbaDef* CtaAlarms::getNonUniqueAlarmAtLocation(const std::string& alarmID,unsigned long locationKey)
    {
        //TA_ASSERT(m_nonUniqueAlarmHelper.get()!=0, "Non Unique Alarm Helper has not been constructed!");
        TA_ASSERT(alarmID.empty() == false, "alarm id is empty");

        return m_nonUniqueAlarmHelper.getAlarmAtLocation(alarmID,locationKey);
    }

	const AlarmDetailCorbaDef* CtaAlarms::getMostRecentActiveNonUniqueAlarmForEntity( unsigned long entityKey,
                                                                                unsigned long locationKey )
    {
        //TA_ASSERT(m_nonUniqueAlarmHelper.get()!=0, "Non Unique Alarm Helper has not been constructed!");
        return m_nonUniqueAlarmHelper.getMostRecentActiveAlarmForEntity( entityKey,locationKey );
    }
}

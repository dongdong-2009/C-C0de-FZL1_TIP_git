/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/ConfigConfiguredDerivedState.cpp $
  * @author Karen Graham
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * ConfigConfiguredDerivedState provides read-write parameters that are specific
  * to a Derived DataPoint derived state in the database
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <sstream>

#include "core/data_access_interface/derived_datapoints/src/ConfigConfiguredDerivedState.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigDerivedDataPoint.h"

#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Core
{
    const std::string ConfigConfiguredDerivedState::STATE = "State ";

    const std::string ConfigConfiguredDerivedState::IS_ALARM_ENABLED  = " Alarm Enabled";
    const std::string ConfigConfiguredDerivedState::ALARM_DELAY       = " Alarm Delay";
    const std::string ConfigConfiguredDerivedState::ALARM_MESSAGE     = " Alarm Message";
	const std::string ConfigConfiguredDerivedState::ALARM_MMS		  = " Alarm MMS";
    const std::string ConfigConfiguredDerivedState::ALARM_SEVERITY    = " Alarm Severity";
    const std::string ConfigConfiguredDerivedState::STATE_DESCRIPTION = " State Description";

    const std::string ConfigConfiguredDerivedState::ENABLED  = "Enabled";
    const std::string ConfigConfiguredDerivedState::DISABLED = "Disabled";

    ConfigConfiguredDerivedState::ConfigConfiguredDerivedState( ConfigDerivedDataPoint& parent,
                                                                unsigned long key,
                                                                int stateValue,
                                                                const std::string & stateDescription,
                                                                bool alarmEnabled,
                                                                int alarmDelay,
                                                                int alarmSeverity,
                                                                const std::string & alarmMessage)
		: ConfiguredDerivedState( 0, stateValue, stateDescription, alarmEnabled, alarmDelay,
                                  alarmSeverity, alarmMessage ),
          m_parent(parent), m_key(key)
	{
	}


    ConfigConfiguredDerivedState::ConfigConfiguredDerivedState(ConfigDerivedDataPoint& parent, int stateValue)
        : ConfiguredDerivedState( 0,stateValue,"",false,0,0,""),
          m_parent(parent), m_key(0)
    {
    }


    ConfigConfiguredDerivedState::ConfigConfiguredDerivedState( const ConfigConfiguredDerivedState& obj)
        : ConfiguredDerivedState(0, obj.m_stateValue, obj.m_stateDescription, obj.m_alarmEnabled,
                                 obj.m_alarmDelay, obj.m_alarmSeverity, obj.m_alarmMessage),
          m_parent(obj.m_parent), m_key(0)
    {
    }


    ConfigConfiguredDerivedState::~ConfigConfiguredDerivedState()
	{
        try
        {
            // Update the changes to show that all these state settings have been set to nothing.
            // ie the state has been removed. We need to do this in case the state is added back in.
            std::ostringstream parameter;
            parameter << STATE << m_stateValue << ALARM_SEVERITY;
            setAlarmSeverity(0);

            parameter.str("");
            parameter << STATE << m_stateValue << ALARM_DELAY;
            std::ostringstream oldValue;
            oldValue << m_alarmDelay;
            m_parent.updateChanges(parameter.str(), oldValue.str(), "");

            parameter.str("");
            parameter << STATE << m_stateValue << IS_ALARM_ENABLED;
            std::string oldValue2 = ENABLED;
            if (!m_alarmEnabled)
            {
                oldValue2 = DISABLED;
            }
            m_parent.updateChanges(parameter.str(), oldValue2, DISABLED);

            parameter.str("");
            parameter << STATE << m_stateValue << STATE_DESCRIPTION;
            m_parent.updateChanges(parameter.str(), m_stateDescription, "");

            parameter.str("");
            parameter << STATE << m_stateValue << ALARM_MESSAGE;
            m_parent.updateChanges(parameter.str(), m_alarmMessage, "");

			parameter.str("");
			parameter << STATE << m_stateValue << ALARM_MMS;
			m_parent.updateChanges(parameter.str(), m_alarmMMS, "");
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception caught in the destructor");
        }
	}


    std::string ConfigConfiguredDerivedState::getAlarmSeverityName()
    {
        return getAlarmSeverityName(m_alarmSeverity);
    }


    std::string ConfigConfiguredDerivedState::getAlarmSeverityName(unsigned long severity)
    {
        IAlarmSeverityData* data = NULL;
        std::string name = "";
        try
        {
            data = AlarmSeverityAccessFactory::getInstance().getSeverityByKey( severity );
            name = data->getName();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not retrieve an alarm severity name for the given alarm severity key");
        }

        if (data != NULL)
        {
            delete data;
            data = NULL;
        }

        return name;
    }

    void ConfigConfiguredDerivedState::setIsAlarmEnabled(bool isAlarmEnabled)
    {
        std::string oldValue = ENABLED;
        std::string newValue = ENABLED;
        if (!m_alarmEnabled)
        {
            oldValue = DISABLED;
        }
        if (!isAlarmEnabled)
        {
            newValue = DISABLED;
        }

        std::ostringstream parameter;
        parameter << STATE << m_stateValue << IS_ALARM_ENABLED;

        m_parent.updateChanges(parameter.str(),oldValue,newValue);

        m_alarmEnabled = isAlarmEnabled;
    }


    void ConfigConfiguredDerivedState::setAlarmDelay(int alarmDelay)
    {
        std::ostringstream oldValue;
        std::ostringstream newValue;
        oldValue << m_alarmDelay;
        newValue << alarmDelay;

        std::ostringstream parameter;
        parameter << STATE << m_stateValue << ALARM_DELAY;

        m_parent.updateChanges(parameter.str(), oldValue.str(), newValue.str());

        m_alarmDelay = alarmDelay;
    }


    void ConfigConfiguredDerivedState::setAlarmSeverity(int alarmSeverity)
    {
        std::string newValue = getAlarmSeverityName( alarmSeverity );
        std::string oldValue = getAlarmSeverityName( m_alarmSeverity );

        std::ostringstream parameter;
        parameter << STATE << m_stateValue << ALARM_SEVERITY;

        m_parent.updateChanges( parameter.str(), oldValue, newValue);

        m_alarmSeverity = alarmSeverity;
    }


    void ConfigConfiguredDerivedState::setAlarmMessage(const std::string& message)
    {
        std::ostringstream parameter;
        parameter << STATE << m_stateValue << ALARM_MESSAGE;

        m_parent.updateChanges(parameter.str(), m_alarmMessage, message);

        m_alarmMessage = message;
    }

	
    void ConfigConfiguredDerivedState::setAlarmMMS(const std::string& newCondition)
    {
        std::ostringstream parameter;
        parameter << STATE << m_stateValue << ALARM_MMS;

        m_parent.updateChanges(parameter.str(), m_alarmMMS, newCondition);

        m_alarmMMS = newCondition;
    }


    void ConfigConfiguredDerivedState::setStateDescription(const std::string& description)
    {
        std::ostringstream parameter;
        parameter << STATE << m_stateValue << STATE_DESCRIPTION;

        m_parent.updateChanges(parameter.str(), m_stateDescription, description);

        m_stateDescription = description;
    }
}


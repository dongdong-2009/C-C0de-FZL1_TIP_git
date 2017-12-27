/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/ConfigConfiguredDerivedState.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * ConfigConfiguredDerivedState provides read-write parameters that are specific
  * to a Derived DataPoint derived state in the database
  */


#ifndef CONFIG_CONFIGURED_DERIVED_STATE_H
#define CONFIG_CONFIGURED_DERIVED_STATE_H

#include <string>

#include "core/data_access_interface/derived_datapoints/src/ConfiguredDerivedState.h"

namespace TA_Base_Core
{
    class ConfigDerivedDataPoint;

    class ConfigConfiguredDerivedState : public ConfiguredDerivedState
	{
	public:

        /**
         * Constructor
         *
         * @param ConfigDerivedDataPoint& - This is the derived data point class that we can use to
         *                                  record any changes made to our members.
         * @param unsigned long - The key from the database for this object. This will be 0 if no key is known
         * @param int - The value for this state
         * @param string - The description for this state
         * @param bool - Indicates whether this state has it's alarm enabled
         * @param int - Indicates the alarm delay for this state
         * @param int - Indicates the alarm severity for this state. 0 means no severity
         * @param string - The alarm message for this state
         */
		ConfigConfiguredDerivedState ( ConfigDerivedDataPoint& parent,
                                       unsigned long key,
                                       int stateValue,
 					                   const std::string & stateDescription,
					                   bool alarmEnabled,
					                   int alarmDelay,
					                   int alarmSeverity,
					                   const std::string & alarmMessage);


        /**
         * Constructor
         *
         * Create a new derived state
         *
         * @param ConfigDerivedDataPoint& - This is the derived data point class that we can use to
         *                                  record any changes made to our members.
         * @param int - The value for this state
         */
        ConfigConfiguredDerivedState(ConfigDerivedDataPoint& parent, int stateValue);


        /**
         * Copy Constructor
         */
        ConfigConfiguredDerivedState( const ConfigConfiguredDerivedState& obj );


        /**
         * Destructor
         */
		virtual ~ConfigConfiguredDerivedState();


        /**
         * getKey
         *
         * This retrieves the primary key for this input association
         *
         * @return unsigned long - The primary key. 0 if one has not been set
         */
        unsigned long getKey() const
        {
            return m_key;
        }

        
        /**
         * getAlarmSeverityName
         *
         * This will return the alarm severity name for the current alarm severity
         *
         * @return string - The name of the alarm severity
         */
        std::string getAlarmSeverityName();


        /**
         * getAlarmSeverityName
         *
         * This will return the alarm severity name for the current alarm severity
         *
         * @param unsigned long - The severity to retrieve the name for
         *
         * @return string - The name of the alarm severity
         */
        std::string getAlarmSeverityName(unsigned long severity);


        /**
         * setIsAlarmEnabled
         *
         * This sets the indicator that determines if the alarm is enabled for the derived state.
         *
         * @param bool - the new value for m_alarmEnabled
         */
        void setIsAlarmEnabled(bool isAlarmEnabled);

        
        /**
         * setAlarmDelay
         *
         * Sets the alarm delay of the derived datapoint.
         *
         * @param int - The alarm delay value
         */
        void setAlarmDelay(int alarmDelay);


        /**
         * setAlarmSeverity
         *
         * This sets the alarm severity associated with the dervived datapoint
         *
         * @param int - The key of the alarm severity from the database
         */
        void setAlarmSeverity(int alarmSeverity);


        /**
         * setAlarmMessage
         *
         * This sets the alarm message associated with the dervived datapoint
         *
         * @param string - The new alarm message
         */
        void setAlarmMessage(const std::string& message);


		/*
		 * setAlarmMMS
		 * 
		 * @param newCondition 	the new MMS alarm condition associated with this DerivedState
		 *
		 */
		virtual void setAlarmMMS ( const std::string & newCondition );

        
        /**
         * setStateDescription
         *
         * This sets the description for this derived state
         *
         * @param string - The new description for the state
         */
        void setStateDescription(const std::string& description);


        /**
         * setKey
         *
         * This sets the database primary key for this input association
         *
         * @param unsigned long - The primary key
         */
        void setKey(unsigned long key)
        {
            m_key = key;
        }

        static const std::string STATE;
        static const std::string IS_ALARM_ENABLED;
        static const std::string ALARM_DELAY;
        static const std::string ALARM_MESSAGE;
        static const std::string ALARM_MMS;
        static const std::string ALARM_SEVERITY;
        static const std::string STATE_DESCRIPTION;

        static const std::string ENABLED;
        static const std::string DISABLED;

    private:
        // Hidden as these are not used
        ConfigConfiguredDerivedState& operator=(const ConfigConfiguredDerivedState&);


        ConfigDerivedDataPoint& m_parent;
        unsigned long m_key;
	};


}

#endif //CONFIG_CONFIGURED_DERIVED_STATE_H


/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/AnalogueAlarmDetails.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * a new class to support the min/max limit value of a alarm.
  */

#ifndef AnalogueAlarmDetails_H
#define AnalogueAlarmDetails_H

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datapoint_library/src/DataPointAlarmDetails.h"
#include "bus/scada/datapoint_library/src/DataPointState.h"

namespace TA_Base_Bus
{
    class AnalogueAlarmDetails : public DataPointAlarmDetails
    {
    public:
        /**
         * AnalogueAlarmDetails::AnalogueAlarmDetails()
         *
         * @param   dpEntityKey     the pkey for the DataPoint entity that this
         *                          alarm pertains to
         *
         * @param   almEnabled      whether or not to process this alarm
         *
         * @param   almDelay        the alarm state must persist for this period
         *                          of time (in seconds) before an alarm is
         *                          generated
         *
         * @param   almSeverity     the severity of alarms generated as a result
         *                          of this alarm condition
         *
         * @param   almMessage      the message of alarms generated as a result
         *                          of this alarm condition
         *
         * @param   almValue        the value that defines this alarm condition
         *
         */
        AnalogueAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
                              const TA_Base_Core::MessageType& alarmType,
                              unsigned long dpParentEntityKey,
                              unsigned long dpEntityKey,
                              unsigned long dpEntityTypeKey,
                              unsigned long regionKey,
                              unsigned long subsystemKey,
                              const std::string & entityName,
                              bool almEnabled,
                              int almDelay,
                              int almSeverity,
                              const std::string & almMessage,
                              const std::string & MMSConfiguration,
                              const TA_Base_Bus::EDataPointAlarms & almType,
                              const DpValue& almValue,
                              MmsScadaUtility & mmsScadaUtility )
        :
        DataPointAlarmDetails( dataPointEntityAccess, alarmType, dpParentEntityKey, dpEntityKey, 
                               dpEntityTypeKey, regionKey, subsystemKey, entityName, almEnabled,
                               almDelay, almSeverity, almMessage,
                               MMSConfiguration, almType, almValue, mmsScadaUtility ),
        
        m_pOtherLimitValue(NULL),
        m_hasOtherLimit( false )
        {
            //setValue( almValue );
            m_pOtherLimitValue = new DpValue(almValue);
        }


        virtual ~AnalogueAlarmDetails() {  delete m_pOtherLimitValue; m_pOtherLimitValue = NULL; }


        /*
         * DataPointAlarmDetails::getOtherLimitValue()
         *
         * @return the value for the alarm rule
         *
         */
        virtual const DpValue & getOtherLimitValue() const { return *m_pOtherLimitValue; }


        /*
         * DataPointAlarmDetails::setOtherLimitValue()
         *
         * @param newValue  the value used as another limit end in future evaluation of alarm
         *                  condition
         *
         */
        virtual void setOtherLimitValue( const DpValue & newValue ) { (* m_pOtherLimitValue) = newValue; m_hasOtherLimit = true; }

    
    protected:

        DpValue*                    m_pOtherLimitValue;
        bool                        m_hasOtherLimit;
    };
}

#endif

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/FALSEAlarmDetails.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	Represents an alarm condition for a boolean false value
  *
  */

#ifndef FALSEALARMDETAILS_H
#define FALSEALARMDETAILS_H

#include "bus/scada/datapoint_library/src/DataPointAlarmDetails.h"
#include "bus/scada/datapoint_library/src/DataPointState.h"

namespace TA_Base_Bus
{

	class FALSEAlarmDetails : public DataPointAlarmDetails
	{
	public:

		/**
		 * FALSEAlarmDetails::FALSEAlarmDetails()
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
		 */
		FALSEAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
                           unsigned long        dpParentEntityKey,
						   unsigned long        dpEntityKey,
						   unsigned long        dpEntityTypeKey,
						   unsigned long	    regionKey,
						   unsigned long	    subsystemKey,
						   const std::string &  entityName,
						   bool                 almEnabled,
						   int                  almDelay,
						   int                  almSeverity,
                           const std::string &  almMessage,
						   const std::string &	MMSConfiguration,
						   const DpValue &      almValue,
                           MmsScadaUtility &    mmsScadaUtility );
        
		
        virtual ~FALSEAlarmDetails();


		virtual std::string getAlarmTypeStr();

	
	protected:

		/*
		 * FALSEAlarmDetails::inAlarm()
		 *
		 * Determines whether a DataPointState constitutes an alarm
		 * state or not.
		 *
		 * @param state     the state to be tested
		 *
		 */
		virtual bool inAlarm( const DataPointState & state );
	};
}

#endif	

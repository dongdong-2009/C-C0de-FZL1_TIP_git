/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/GeneralAlarmDetails.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	Represents an alarm condition for a general alarm
  *
  */

#ifndef GENERAL_ALARM_DETAILS_H
#define GENERAL_ALARM_DETAILS_H

#include "bus/scada/datapoint_library/src/DataPointAlarmDetails.h"
#include "bus/scada/datapoint_library/src/DataPointState.h"

namespace TA_Base_Bus
{

	class GeneralAlarmDetails : public DataPointAlarmDetails
	{
	public:

		GeneralAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
                             unsigned long			dpParentEntityKey,
							 unsigned long			dpEntityKey,
							 unsigned long			dpEntityTypeKey,
							 unsigned long			regionKey,
							 unsigned long			subsystemKey,
							 const std::string &	entityName,
							 bool					almEnabled,
							 int					almDelay,
							 int					almSeverity,
							 const std::string &	almMessage,
							 const std::string &	MMSConfiguration,
							 const DpValue &	    almValue,
                             MmsScadaUtility&       mmsScadaUtility);

		virtual ~GeneralAlarmDetails();

		virtual std::string getAlarmTypeStr();
	
	protected:

	   /*
		* inAlarm()
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

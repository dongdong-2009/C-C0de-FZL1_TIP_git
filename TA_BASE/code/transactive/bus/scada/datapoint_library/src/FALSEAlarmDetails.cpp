/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/FALSEAlarmDetails.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "bus/scada/datapoint_library/src/FALSEAlarmDetails.h"

#include "core/message/types/DataPointAlarms_MessageTypes.h"

namespace TA_Base_Bus
{
    FALSEAlarmDetails::FALSEAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
        unsigned long  dpParentEntityKey,
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
        MmsScadaUtility &    mmsScadaUtility )
        :
    DataPointAlarmDetails( dataPointEntityAccess, TA_Base_Core::DataPointAlarms::DpFalse, dpParentEntityKey, 
        dpEntityKey, dpEntityTypeKey, regionKey,
        subsystemKey, entityName, almEnabled, almDelay, almSeverity, 
        almMessage, MMSConfiguration,
        TA_Base_Bus::DptBoolFalse, almValue, mmsScadaUtility )
    {
        //setValue( almValue );
    }
    

    FALSEAlarmDetails::~FALSEAlarmDetails() 
    {
    }
    
    
    bool FALSEAlarmDetails::inAlarm( const DataPointState & state )
    {
        return ( ! state.getValue().getBoolean() );
    }
    

    std::string FALSEAlarmDetails::getAlarmTypeStr()
	{
		std::string alarmTypeStr = DPTALM_FALSE_SHORT;
		
		return alarmTypeStr;
	}

}

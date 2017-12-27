/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPointAlarmDetailsFactory.cpp $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  * Class for creating and modifying the various types
  * of DataPointAlarmDetails.
  *
  */

#include "bus/scada/datapoint_library/src/DataPointAlarmDetailsFactory.h"
#include "bus/scada/datapoint_library/src/DataPointAlarmDetails.h"
#include "bus/scada/datapoint_library/src/OverScaleHiAlarmDetails.h"
#include "bus/scada/datapoint_library/src/HIHIAlarmDetails.h"
#include "bus/scada/datapoint_library/src/HIAlarmDetails.h"
#include "bus/scada/datapoint_library/src/LOAlarmDetails.h"
#include "bus/scada/datapoint_library/src/LOLOAlarmDetails.h"
#include "bus/scada/datapoint_library/src/OverScaleLoAlarmDetails.h"
#include "bus/scada/datapoint_library/src/TRUEAlarmDetails.h"
#include "bus/scada/datapoint_library/src/FALSEAlarmDetails.h"
#include "bus/scada/datapoint_library/src/GeneralAlarmDetails.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

	DataPointAlarmDetails *
	DataPointAlarmDetailsFactory::createAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
                                                      const TA_Base_Bus::EDataPointAlarms& type,
													  bool almEnabled,
													  int almDelay,
													  int almSeverity,
													  const std::string & msg,
													  const DpValue & almValue,
													  const std::string & entityName,
													  const std::string & MMSConfiguration,
													  unsigned long dpParentEntityKey,
													  unsigned long entityKey,
													  unsigned long entityTypeKey,
													  unsigned long regionKey,
													  unsigned long subsystemKey,
													  double alarmThresholdConstant,
                                                      MmsScadaUtility& mmsScadaUtility
													  )
	{
        FUNCTION_ENTRY("createAlarmDetails");
		DataPointAlarmDetails * newAlarmDetails = NULL;

		switch( type )
		{
        case TA_Base_Bus::DptOverScaleHi:
            newAlarmDetails = new OverScaleHiAlarmDetails( dataPointEntityAccess, dpParentEntityKey, entityKey, entityTypeKey, regionKey, subsystemKey, entityName,
                almEnabled, almDelay, almSeverity, msg, MMSConfiguration, almValue, mmsScadaUtility );
            break;
        
        case TA_Base_Bus::DptHiHi:
            newAlarmDetails = new HIHIAlarmDetails( dataPointEntityAccess, dpParentEntityKey, entityKey, entityTypeKey, regionKey, subsystemKey, entityName,
                almEnabled, almDelay, almSeverity, msg, MMSConfiguration, almValue, mmsScadaUtility );
            break;
        
        case TA_Base_Bus::DptHi:
            newAlarmDetails = new HIAlarmDetails( dataPointEntityAccess, dpParentEntityKey, entityKey, entityTypeKey, regionKey, subsystemKey, entityName,
                almEnabled, almDelay, almSeverity, msg, MMSConfiguration, almValue, mmsScadaUtility );
            break;
        
        case TA_Base_Bus::DptLo:
            newAlarmDetails = new LOAlarmDetails( dataPointEntityAccess, dpParentEntityKey, entityKey, entityTypeKey, regionKey, subsystemKey, entityName,
                almEnabled, almDelay, almSeverity, msg, MMSConfiguration, almValue, mmsScadaUtility );
            break;
        
        case TA_Base_Bus::DptLoLo:
            newAlarmDetails = new LOLOAlarmDetails( dataPointEntityAccess, dpParentEntityKey, entityKey, entityTypeKey, regionKey, subsystemKey, entityName,
                almEnabled, almDelay, almSeverity, msg, MMSConfiguration, almValue, mmsScadaUtility );
            break;
        
        case TA_Base_Bus::DptOverScaleLo:
            newAlarmDetails = new OverScaleLoAlarmDetails( dataPointEntityAccess, dpParentEntityKey, entityKey, entityTypeKey, regionKey, subsystemKey, entityName,
                almEnabled, almDelay, almSeverity, msg, MMSConfiguration, almValue, mmsScadaUtility );
            break;
        
        case TA_Base_Bus::DptBoolTrue:
            newAlarmDetails = new TRUEAlarmDetails( dataPointEntityAccess, dpParentEntityKey, entityKey, entityTypeKey, regionKey, subsystemKey, entityName,
                almEnabled, almDelay, almSeverity, msg, MMSConfiguration, almValue, mmsScadaUtility );
            break;
        
        case TA_Base_Bus::DptBoolFalse:
            newAlarmDetails = new FALSEAlarmDetails( dataPointEntityAccess, dpParentEntityKey, entityKey, entityTypeKey, regionKey, subsystemKey, entityName,
                almEnabled, almDelay, almSeverity, msg, MMSConfiguration, almValue, mmsScadaUtility );
            break;
        
        case TA_Base_Bus::DptGeneral:
            newAlarmDetails = new GeneralAlarmDetails ( dataPointEntityAccess, dpParentEntityKey, entityKey, entityTypeKey, regionKey, subsystemKey, entityName,
                almEnabled, almDelay, almSeverity, msg, MMSConfiguration, almValue, mmsScadaUtility );
            break;
        
        default:
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "[DataPointAlarmDetailsFactory::createAlarmDetails] Datapoint (%s), trying to create unknown alarm type (%d)",
                entityName.c_str(), type );
            break;
        }

		if ( NULL != newAlarmDetails )
		{
			newAlarmDetails->setAlarmThresholdConstant ( alarmThresholdConstant );
		}

		FUNCTION_EXIT;
		return newAlarmDetails;
	}

	void DataPointAlarmDetailsFactory::updateAlarmDetails( const std::string OMFlag,
													DataPointAlarmDetails * alm,
													bool almEnabled,
													int almDelay,
													int almSeverity,
													const std::string & almMessage,
													const std::string & MMSConfiguration,
													const DpValue & almValue,
													const DataPointState & almState,
													bool alarmInhibited,
													double alarmThresholdConstant
													)
	{
        FUNCTION_ENTRY("updateAlarmDetails");
		if ( alm != NULL )
		{
			// close the alarm just in case the new configuration might
			// change the alarm condition
			// TD15019, use current system as the close alarm timestamp
			struct timeb currentSystemTime;
			ftime ( &currentSystemTime );
			alm->closeAlarm(almState.getValue(), currentSystemTime);

			// update the alarm configurations
			alm->setEnabled( almEnabled );
			alm->setDelay( almDelay );
			alm->setSeverity( almSeverity );
			alm->setMessage( almMessage );
			alm->setMMSConfiguration ( MMSConfiguration );
			alm->setValue( almValue );
			alm->setAlarmThresholdConstant ( alarmThresholdConstant );

			// re-evaluate the alarm status with the current state and configuration
			// alm->updateState( OMFlag, almState, alarmInhibited );
		}
		FUNCTION_EXIT;
	}

}



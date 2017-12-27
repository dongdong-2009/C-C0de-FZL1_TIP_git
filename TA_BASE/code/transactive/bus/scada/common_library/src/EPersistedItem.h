/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/EPersistedItem.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 */

#if !defined(EPersistedItem_C2177E5E_3E27_4cdc_9B96_E40F49E1C50A__INCLUDED_)
#define EPersistedItem_C2177E5E_3E27_4cdc_9B96_E40F49E1C50A__INCLUDED_

#include <string>

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{
    enum EPersistedItem
    {
        //
        // DataPoint persisted values
        //
        DP_CURRENT_VALUE,
        DP_MANUALLY_DRESSED,
        DP_MANUALLY_DRESSED_VALUE,
        DP_HI_ALARM_ENABLED,
        DP_HIHI_ALARM_ENABLED,
        DP_OHI_ALARM_ENABLED,
        DP_LO_ALARM_ENABLED,
        DP_LOLO_ALARM_ENABLED,
        DP_OLO_ALARM_ENABLED,
        DP_TRUE_ALARM_ENABLED,
        DP_FALSE_ALARM_ENABLED,
        DP_CONTROL_STATUS,
        DP_CONTROL_INHIBIT,
        DP_ALARM_INHIBIT,
        DP_MMS_INHIBIT,
        DP_MMS_PREVIOUS_INHIBIT,
        DP_SERVICE_STATE,
        DP_LAUNCH_RESULT_STRING,
        DP_CURRENT_CONTROL_REQUEST,
        DP_LAST_OPERATOR_SESSION_ID,
        DP_LAST_OPERATOR_TIMESTAMP,
        DP_LAST_INPUT_VALUE,
        DP_ACCUMULATED_INPUT_VALUE,
        DP_CORRECTION_VALUE,

		//TD15446
		DP_HI_ALARM_VALUE,
		DP_HIHI_ALARM_VALUE,
		DP_OHI_ALARM_VALUE,
		DP_LO_ALARM_VALUE,
		DP_LOLO_ALARM_VALUE,
		DP_OLO_ALARM_VALUE,
		DP_DS_ALARM_ENABLED,
        //
        // DataNode persisted values
        //
        DN_TAG_STATUS,
        DN_TAG_OUTPUT_INHIBIT,
        DN_TAG_INPUT_INHIBIT,
        DN_TAG_PTW,
        DN_PTW_COMMENTS,
        DN_LAST_OPERATOR_SESSION_ID,
        DN_LAST_OPERATOR_TIMESTAMP,
        DN_NOTES,
        DN_MMS_INHIBIT_STATE
    };

    namespace
    {
        //
        // This function must be kept in sync with the DR_LIVE_PROPERTY table
        //
        static std::string mapEPersistedItem2DataRecordingId( EPersistedItem item ) 
        {
            switch( item )
            {
                case DP_CURRENT_VALUE :             return "DATAPOINT"; break;
                case DP_MANUALLY_DRESSED :          return "DRDPS_01"; break;
                case DP_MANUALLY_DRESSED_VALUE :    return "DRDPS_02"; break;
                case DP_OHI_ALARM_ENABLED :         return "DRDPS_03"; break;
                case DP_HIHI_ALARM_ENABLED :        return "DRDPS_04"; break;
                case DP_HI_ALARM_ENABLED :          return "DRDPS_05"; break;
                case DP_LO_ALARM_ENABLED :          return "DRDPS_06"; break;
                case DP_LOLO_ALARM_ENABLED :        return "DRDPS_07"; break;
                case DP_OLO_ALARM_ENABLED :         return "DRDPS_08"; break;
                case DP_TRUE_ALARM_ENABLED :        return "DRDPS_09"; break;
                case DP_FALSE_ALARM_ENABLED :       return "DRDPS_10"; break;
                case DP_CONTROL_STATUS :            return "DRDPS_11"; break;
                case DP_CONTROL_INHIBIT :           return "DRDPS_12"; break;
                case DP_ALARM_INHIBIT :             return "DRDPS_13"; break;
                case DP_MMS_INHIBIT :               return "DRDPS_14"; break;
                case DP_MMS_PREVIOUS_INHIBIT :      return "DRDPS_15"; break;
                case DP_SERVICE_STATE :             return "DRDPS_16"; break;
                case DP_LAUNCH_RESULT_STRING :      return "DRDPS_17"; break;
                case DP_CURRENT_CONTROL_REQUEST :   return "DRDPS_18"; break;
                case DP_LAST_OPERATOR_SESSION_ID :  return "DRDPS_19"; break;
                case DP_LAST_OPERATOR_TIMESTAMP :   return "DRDPS_20"; break;
                case DP_LAST_INPUT_VALUE :          return "DRDPS_21"; break;
                case DP_ACCUMULATED_INPUT_VALUE :   return "DRDPS_22"; break;
                case DP_CORRECTION_VALUE :          return "DRDPS_23"; break;
				//TD15446
				case DP_HI_ALARM_VALUE :			return "DRDPS_24"; break;
				case DP_HIHI_ALARM_VALUE :          return "DRDPS_25"; break;
				case DP_OHI_ALARM_VALUE :			return "DRDPS_26"; break;
				case DP_LO_ALARM_VALUE :			return "DRDPS_27"; break;
				case DP_LOLO_ALARM_VALUE :          return "DRDPS_28"; break;
				case DP_OLO_ALARM_VALUE :			return "DRDPS_29"; break;
				case DP_DS_ALARM_ENABLED :          return "DRDPS_30"; break;
					
                case DN_TAG_STATUS :                return "DRDNS_01"; break;
                case DN_TAG_OUTPUT_INHIBIT :        return "DRDNS_02"; break;
                case DN_TAG_INPUT_INHIBIT :         return "DRDNS_03"; break;
                case DN_TAG_PTW :                   return "DRDNS_04"; break;
                case DN_PTW_COMMENTS :              return "DRDNS_05"; break;
                case DN_LAST_OPERATOR_SESSION_ID :  return "DRDNS_06"; break;
                case DN_LAST_OPERATOR_TIMESTAMP :   return "DRDNS_07"; break;
                case DN_NOTES :                     return "DRDNS_08"; break;
                case DN_MMS_INHIBIT_STATE :         return "DRDNS_09"; break;

                default :
                
                    TA_ASSERT( false, "Unknown EPersistedItem value" );
                    return "";
            }
                
        }
    }
}

#endif // !defined(EPersistedItem_C2177E5E_3E27_4cdc_9B96_E40F49E1C50A__INCLUDED_)

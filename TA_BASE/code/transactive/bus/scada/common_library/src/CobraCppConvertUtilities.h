#if !defined(CobraCppConvertUtilities__INCLUDED_)
#define CobraCppConvertUtilities__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/CobraCppConvertUtilities.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  *
  * Utility functions for use for convert Corba and cpp type in scada
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"

#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_Bus
{
	////////////////////quality convert////////////////////////

	void converQualityStatus(const TA_Base_Bus::ScadaCorbaTypes::EQuality& quality, TA_Base_Bus::EQualityStatus& ret);
	void converQualityStatus(const TA_Base_Bus::EQualityStatus& quality, TA_Base_Bus::ScadaCorbaTypes::EQuality& ret);

	void converTimeStamp(const TA_Base_Bus::ScadaCorbaTypes::STimeStamp& timestamp, timeb& ret);
	void converTimeStamp(const timeb& timestamp, TA_Base_Bus::ScadaCorbaTypes::STimeStamp& ret);

	////////////////////104 control Operation convert////////////////////////

	void converControlOperation(const TA_Base_Bus::ScadaCorbaTypes::EControlOperation& controlOperation, TA_Base_Bus::EControlOperation& ret);
	void converControlOperation(const TA_Base_Bus::EControlOperation& controlOperation, TA_Base_Bus::ScadaCorbaTypes::EControlOperation& ret);

	////////////////////alarm type and property convert////////////////////////

	void convertAlarmType(const TA_Base_Bus::DataPointCorbaTypes::EDatapointAlarm& alarmType, TA_Base_Bus::EDataPointAlarms& ret);
	void convertAlarmType(const TA_Base_Bus::EDataPointAlarms& alarmType, TA_Base_Bus::DataPointCorbaTypes::EDatapointAlarm& ret);

	void convertAlarmProperty(const TA_Base_Bus::DataPointCorbaTypes::SAlarmProperty& alarmProperty, TA_Base_Bus::AlarmProperty& ret);
	void convertAlarmProperty(const TA_Base_Bus::AlarmProperty& alarmProperty, TA_Base_Bus::DataPointCorbaTypes::SAlarmProperty& ret);

	////////////////////control state convert////////////////////////

	void convertControlStateEnum(const TA_Base_Bus::DataPointCorbaTypes::EControlState& controlState, TA_Base_Bus::EDataPointControlState& ret);
	void convertControlStateEnum(const TA_Base_Bus::EDataPointControlState& controlState, TA_Base_Bus::DataPointCorbaTypes::EControlState& ret);

	void convertControlState(const TA_Base_Bus::DataPointCorbaTypes::SControlState& control, TA_Base_Bus::DataPointControl& ret);
	void convertControlState(const TA_Base_Bus::DataPointControl& control, TA_Base_Bus::DataPointCorbaTypes::SControlState& ret);

	////////////////////inhibit state convert////////////////////////

	void convertInputInhibitState(const TA_Base_Bus::ScadaCorbaTypes::EInputInhibitState& inputInhibitState, TA_Base_Bus::EInputInhibitState& ret);
	void convertInputInhibitState(const TA_Base_Bus::EInputInhibitState& inputInhibitState, TA_Base_Bus::ScadaCorbaTypes::EInputInhibitState& ret);

	void convertOutputInhibitState(const TA_Base_Bus::ScadaCorbaTypes::EOutputInhibitState& outputInhibitState, TA_Base_Bus::EOutputInhibitState& ret);
	void convertOutputInhibitState(const TA_Base_Bus::EOutputInhibitState& outputInhibitState, TA_Base_Bus::ScadaCorbaTypes::EOutputInhibitState& ret);

	void convertInhibitState(const TA_Base_Bus::ScadaCorbaTypes::SInhibitState& inhibitState, TA_Base_Bus::InhibitState& ret);
	void convertInhibitState(const TA_Base_Bus::InhibitState& inhibitState, TA_Base_Bus::ScadaCorbaTypes::SInhibitState& ret);

	////////////////////value convert////////////////////////

	void convertDataPointDataType(const TA_Base_Bus::ScadaCorbaTypes::ETagType& type, TA_Base_Bus::EDataPointDataType& ret);
	void convertDataPointDataType(const TA_Base_Bus::EDataPointDataType& type, TA_Base_Bus::ScadaCorbaTypes::ETagType& ret);

	void convertDataPointValue(const TA_Base_Bus::DataPointValue& dpValue, TA_Base_Bus::ScadaCorbaTypes::UTag& ret);
	void convertDataPointValue(const TA_Base_Bus::ScadaCorbaTypes::UTag& dpValue, TA_Base_Bus::DataPointValue& ret);

	///////////////////last operator//////////////////////////////////

	void convertLastOperatorInfo(const TA_Base_Bus::ScadaCorbaTypes::SLastOperatorInfo& lastOperatorInfo, TA_Base_Bus::LastOperatorInfo& ret);
	void convertLastOperatorInfo(const TA_Base_Bus::LastOperatorInfo& lastOperatorInfo, TA_Base_Bus::ScadaCorbaTypes::SLastOperatorInfo& ret);

	///////////////////UDynamicSyncUpdate//////////////////
	void convertDynamicSyncUpdate(const TA_Base_Bus::DataPointCorbaTypes::UDynamicSyncUpdate& update, TA_Base_Bus::DpDynamicSyncUpdate& ret);
	void convertDynamicSyncUpdate(const TA_Base_Bus::DpDynamicSyncUpdate& update, TA_Base_Bus::DataPointCorbaTypes::UDynamicSyncUpdate& ret);

    void convertAlarmAckState(const TA_Base_Bus::DataPointCorbaTypes::EScadaAlarmAckState& alarmAckState, TA_Base_Bus::EScadaAlarmAckState& ret);
    void convertAlarmAckState(const TA_Base_Bus::EScadaAlarmAckState& alarmAckState, TA_Base_Bus::DataPointCorbaTypes::EScadaAlarmAckState& ret);
        
    void convertDpCompleteUpdate(const TA_Base_Bus::DpCompleteDynamicSyncState& update, TA_Base_Bus::DataPointCorbaTypes::UDynamicSyncUpdate& ret);

	////////////////for convert dp display value////////////////
	void convertDpDisplayUpdate(const TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState& displayInfo, TA_Base_Bus::DataPointDisplayState& ret);
	void convertDpDisplayUpdate(const TA_Base_Bus::DataPointDisplayState& displayInfo, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState& ret);


	///////////////////ETagState////////////////////
	void convertETagState(const TA_Base_Bus::DataNodeCorbaTypes::ETagState& tagState, TA_Base_Bus::ETagState& ret);
	void convertETagState(const TA_Base_Bus::ETagState& tagState, TA_Base_Bus::DataNodeCorbaTypes::ETagState& ret);

	///////////////////TagState////////////////////
	void convertTagState(const TA_Base_Bus::DataNodeCorbaTypes::STagState& inputTagState, TA_Base_Bus::TagState& ret);
	void convertTagState(const TA_Base_Bus::TagState& inputTagState, TA_Base_Bus::DataNodeCorbaTypes::STagState& ret);

	////////////////////tag////////////////////////
	void convertBooleanTag(const TA_Base_Bus::ScadaCorbaTypes::SBooleanTag& booleanValue, TA_Base_Bus::BooleanTag& ret);
	void convertBooleanTag(const TA_Base_Bus::BooleanTag& booleanValue, TA_Base_Bus::ScadaCorbaTypes::SBooleanTag& ret);

	void convertEnumTag(const TA_Base_Bus::ScadaCorbaTypes::SEnumTag& enumValue, TA_Base_Bus::EnumTag& ret);
	void convertEnumTag(const TA_Base_Bus::EnumTag& enumValue, TA_Base_Bus::ScadaCorbaTypes::SEnumTag& ret);

	///////////////////USyncUpdate//////////////////
	void convertDnCompleteUpdate(const TA_Base_Bus::DnCompleteDynamicSyncState & update, TA_Base_Bus::DataNodeCorbaTypes::SCompleteDynamicSyncState& ret);

	void convertDnCompleteUpdate(const TA_Base_Bus::DnCompleteDynamicSyncState & update, TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate& ret);
	void convertDnCompleteUpdate(const TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate & update, TA_Base_Bus::DnCompleteDynamicSyncState& ret);

	void convertDnDynamicSyncUpdate(const TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate& update, TA_Base_Bus::DnSyncUpdate& ret);
	void convertDnDynamicSyncUpdate(const TA_Base_Bus::DnSyncUpdate& update, TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate& ret);
	
	////////////////for convert dp display value////////////////
	void convertDnDisplayUpdate(const TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState& displayInfo, TA_Base_Bus::DataNodeDisplayState& ret);
	void convertDnDisplayUpdate(const TA_Base_Bus::DataNodeDisplayState& displayInfo, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState& ret);

    ////////////////for convert dp EControlCommandState ////////
    void convertEControlCommandState(const TA_Base_Bus::DataPointCorbaTypes::EControlCommandState& input, TA_Base_Bus::EControlCommandState& ret);
    void convertEControlCommandState(const TA_Base_Bus::EControlCommandState& input, TA_Base_Bus::DataPointCorbaTypes::EControlCommandState& ret);

    ////////////////for convert dp DataPointStateUpdate ////////
    void convertDataPointValue(const TA_Base_Bus::DataPointCorbaTypes::UDataPointValue& input, TA_Base_Bus::DataPointValue& ret);
    void convertDataPointValue(const TA_Base_Bus::DataPointValue& input, TA_Base_Bus::DataPointCorbaTypes::UDataPointValue& ret);

    ////////////////for convert dp DataPointStateUpdate ////////
    void convertDataPointStateUpdate(const TA_Base_Bus::DataPointCorbaTypes::DataPointWriteStateUpdate& input, TA_Base_Bus::DataPointStateUpdate& ret);
    void convertDataPointStateUpdate(const TA_Base_Bus::DataPointStateUpdate& input, TA_Base_Bus::DataPointCorbaTypes::DataPointWriteStateUpdate& ret);

	
}; // TA_Base_Bus


#endif // !defined(CobraCppConvertUtilities__INCLUDED_)

    /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/CobraCppConvertUtilities.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
namespace TA_Base_Bus
{
	void converQualityStatus(const TA_Base_Bus::ScadaCorbaTypes::EQuality& quality, TA_Base_Bus::EQualityStatus& ret)
	{
		switch(quality) 
		{
		case TA_Base_Bus::ScadaCorbaTypes::QualityBadNoSpecificReason :
			ret = TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityBadConfigurationError :
			ret = TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityBadNotConnected :
			ret = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityBadDeviceFailure :
			ret = TA_Base_Bus::QUALITY_BAD_DEVICE_FAILURE ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityBadSensorFailure :
			ret = TA_Base_Bus::QUALITY_BAD_SENSOR_FAILURE ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityBadLastKnownValue:
			ret = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityBadCommFailure:
			ret = TA_Base_Bus::QUALITY_BAD_COMM_FAILURE ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityBadOutOfService:
			ret = TA_Base_Bus::QUALITY_BAD_OUT_OF_SERVICE ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityUncertainNoSpecificReason:
			ret = TA_Base_Bus::QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityUncertainLastUsableValue:
			ret = TA_Base_Bus::QUALITY_UNCERTAINTY_LAST_USABLE_VALUE ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityUncertainSensorNotAccurate:
			ret = TA_Base_Bus::QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityUncertainEngUnitExceeded:
			ret = TA_Base_Bus::QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityUncertainSubNormal:
			ret = TA_Base_Bus::QUALITY_UNCERTAINTY_SUB_NORMAL ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityGoodNoSpecificReason:
			ret = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::QualityGoodLocalOverride:
			ret = TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE ; 
			break;
		default:
            // remove assertion
			//TA_ASSERT(false, "unknown TA_Base_Bus::ScadaCorbaTypes::EQuality value");
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "unknown TA_Base_Bus::ScadaCorbaTypes::EQuality value %d",
						 quality);
            ret = TA_Base_Bus::QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
		}
	}
	
	void converQualityStatus(const TA_Base_Bus::EQualityStatus& quality, TA_Base_Bus::ScadaCorbaTypes::EQuality& ret)
	{
		switch(quality) 
		{
		case TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityBadNoSpecificReason ; 
			break;
		case TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityBadConfigurationError ; 
			break;
		case TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityBadNotConnected ; 
			break;
		case TA_Base_Bus::QUALITY_BAD_DEVICE_FAILURE :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityBadDeviceFailure ; 
			break;
		case TA_Base_Bus::QUALITY_BAD_SENSOR_FAILURE :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityBadSensorFailure ; 
			break;
		case TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE:
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityBadLastKnownValue ; 
			break;
		case TA_Base_Bus::QUALITY_BAD_COMM_FAILURE :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityBadCommFailure; 
			break;
		case TA_Base_Bus::QUALITY_BAD_OUT_OF_SERVICE :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityBadOutOfService ; 
			break;
		case TA_Base_Bus::QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityUncertainNoSpecificReason ; 
			break;
		case TA_Base_Bus::QUALITY_UNCERTAINTY_LAST_USABLE_VALUE :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityUncertainLastUsableValue ; 
			break;
		case TA_Base_Bus::QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityUncertainSensorNotAccurate ; 
			break;
		case TA_Base_Bus::QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityUncertainEngUnitExceeded ; 
			break;
		case TA_Base_Bus::QUALITY_UNCERTAINTY_SUB_NORMAL :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityUncertainSubNormal ; 
			break;
		case TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityGoodNoSpecificReason ; 
			break;
		case TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE :
			ret = TA_Base_Bus::ScadaCorbaTypes::QualityGoodLocalOverride ; 
			break;
		default:
            // remove assertion
			//TA_ASSERT(false, "unknown TA_Base_Bus::ScadaCorbaTypes::EQuality value");
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "unknown TA_Base_Bus::EQuality value %d",
						 quality);
            ret = TA_Base_Bus::ScadaCorbaTypes::QualityUncertainNoSpecificReason;
		}
	}

	void converTimeStamp(const TA_Base_Bus::ScadaCorbaTypes::STimeStamp& timestamp, timeb& ret)
	{
		ret.time = timestamp.seconds;
		ret.millitm = timestamp.milliseconds;
	}
	
	void converTimeStamp(const timeb& timestamp, TA_Base_Bus::ScadaCorbaTypes::STimeStamp& ret)
	{
		ret.seconds = timestamp.time;
		ret.milliseconds = timestamp.millitm;
	}

	void converControlOperation(const TA_Base_Bus::ScadaCorbaTypes::EControlOperation& controlOperation, TA_Base_Bus::EControlOperation& ret)
	{
		switch(controlOperation) 
		{
		case TA_Base_Bus::ScadaCorbaTypes::ControlNoOperation :
			ret = TA_Base_Bus::CONTROL_NO_OPERATION ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::ControlSelect :
			ret = TA_Base_Bus::CONTROL_SELECT ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::ControlExecute :
			ret = TA_Base_Bus::CONTROL_EXECUTE ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::ControlCancel :
			ret = TA_Base_Bus::CONTROL_CANCEL ; 
			break;
		default:
			// remove assertion
			//TA_ASSERT(false, "unknown TA_Base_Bus::ScadaCorbaTypes::EQuality value");
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "unknown TA_Base_Bus::ScadaCorbaTypes::EControlOperation value %d",
				controlOperation);
			ret = TA_Base_Bus::CONTROL_NO_OPERATION;
		}
	}

	void converControlOperation(const TA_Base_Bus::EControlOperation& controlOperation, TA_Base_Bus::ScadaCorbaTypes::EControlOperation& ret)
	{
		switch(controlOperation) 
		{
		case TA_Base_Bus::CONTROL_NO_OPERATION :
			ret = TA_Base_Bus::ScadaCorbaTypes::ControlNoOperation ; 
			break;
		case TA_Base_Bus::CONTROL_SELECT :
			ret = TA_Base_Bus::ScadaCorbaTypes::ControlSelect ; 
			break;
		case TA_Base_Bus::CONTROL_EXECUTE :
			ret = TA_Base_Bus::ScadaCorbaTypes::ControlExecute ; 
			break;
		case TA_Base_Bus::CONTROL_CANCEL :
			ret = TA_Base_Bus::ScadaCorbaTypes::ControlCancel ; 
			break;
		default:
			// remove assertion
			//TA_ASSERT(false, "unknown TA_Base_Bus::ScadaCorbaTypes::EQuality value");
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "unknown TA_Base_Bus::EControlOperation value %d",
				controlOperation);
			ret = TA_Base_Bus::ScadaCorbaTypes::ControlNoOperation;
		}
	}

	void convertAlarmType(const TA_Base_Bus::DataPointCorbaTypes::EDatapointAlarm& alarmType, TA_Base_Bus::EDataPointAlarms& ret)
	{
		switch(alarmType) 
		{
		case TA_Base_Bus::DataPointCorbaTypes::NoAlarm :
			ret = TA_Base_Bus::DptNoAlarm ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::OverScaleHighAlarm :
			ret = TA_Base_Bus::DptOverScaleHi ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::OverScaleLowAlarm :
			ret = TA_Base_Bus::DptOverScaleLo ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::VeryHighAlarm :
			ret = TA_Base_Bus::DptHiHi ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::HighAlarm :
			ret = TA_Base_Bus::DptHi ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::LowAlarm :
			ret = TA_Base_Bus::DptLo ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::VeryLowAlarm:
			ret = TA_Base_Bus::DptLoLo ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::TrueAlarm:
			ret = TA_Base_Bus::DptBoolTrue ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::FalseAlarm:
			ret = TA_Base_Bus::DptBoolFalse ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::GeneralAlarm:
			ret = TA_Base_Bus::DptGeneral ; 
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::DataPointCorbaTypes::EDatapointAlarm value");
		}
	}

	void convertAlarmType(const TA_Base_Bus::EDataPointAlarms& alarmType, TA_Base_Bus::DataPointCorbaTypes::EDatapointAlarm& ret)
	{
		switch(alarmType) 
		{
		case TA_Base_Bus::DptNoAlarm :
			ret = TA_Base_Bus::DataPointCorbaTypes::NoAlarm ; 
			break;
		case TA_Base_Bus::DptOverScaleHi :
			ret = TA_Base_Bus::DataPointCorbaTypes::OverScaleHighAlarm ; 
			break;
		case TA_Base_Bus::DptOverScaleLo :
			ret = TA_Base_Bus::DataPointCorbaTypes::OverScaleLowAlarm ; 
			break;
		case TA_Base_Bus::DptHiHi :
			ret = TA_Base_Bus::DataPointCorbaTypes::VeryHighAlarm ; 
			break;
		case TA_Base_Bus::DptHi :
			ret = TA_Base_Bus::DataPointCorbaTypes::HighAlarm ; 
			break;
		case TA_Base_Bus::DptLo :
			ret = TA_Base_Bus::DataPointCorbaTypes::LowAlarm ; 
			break;
		case TA_Base_Bus::DptLoLo :
			ret = TA_Base_Bus::DataPointCorbaTypes::VeryLowAlarm ; 
			break;
		case TA_Base_Bus::DptBoolTrue :
			ret = TA_Base_Bus::DataPointCorbaTypes::TrueAlarm ; 
			break;
		case TA_Base_Bus::DptBoolFalse :
			ret = TA_Base_Bus::DataPointCorbaTypes::FalseAlarm; 
			break;
		case TA_Base_Bus::DptGeneral :
			ret = TA_Base_Bus::DataPointCorbaTypes::GeneralAlarm; 
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::DataPointCorbaTypes::EDataPointAlarms value");
		}
	}

    void convertAlarmAckState(const TA_Base_Bus::DataPointCorbaTypes::EScadaAlarmAckState& alarmAckState, TA_Base_Bus::EScadaAlarmAckState& ret)
    {
        switch(alarmAckState) 
        {
        case TA_Base_Bus::DataPointCorbaTypes::ScadaAlarmAck :
            ret = TA_Base_Bus::ScadaAlarmAck ; 
            break;
        case TA_Base_Bus::DataPointCorbaTypes::ScadaAlarmNack :
            ret = TA_Base_Bus::ScadaAlarmNack ; 
            break;
        case TA_Base_Bus::DataPointCorbaTypes::ScadaAlarmNotPresent :
            ret = TA_Base_Bus::ScadaAlarmNotPresent ; 
            break;
        case TA_Base_Bus::DataPointCorbaTypes::ScadaAlarmUnknown:
            ret = TA_Base_Bus::ScadaAlarmUnknown ; 
            break;
        default:
            TA_ASSERT(false, "unknown TA_Base_Bus::DataPointCorbaTypes::EScadaAlarmAckState value");
        }
    }
    
    void convertAlarmAckState(const TA_Base_Bus::EScadaAlarmAckState& alarmAckState, TA_Base_Bus::DataPointCorbaTypes::EScadaAlarmAckState& ret)
    {
        switch(alarmAckState) 
        {
        case TA_Base_Bus::ScadaAlarmAck :
            ret = TA_Base_Bus::DataPointCorbaTypes::ScadaAlarmAck ; 
            break;
        case TA_Base_Bus::ScadaAlarmNack :
            ret = TA_Base_Bus::DataPointCorbaTypes::ScadaAlarmNack ; 
            break;
        case TA_Base_Bus::ScadaAlarmNotPresent :
            ret = TA_Base_Bus::DataPointCorbaTypes::ScadaAlarmNotPresent ; 
            break;
        case TA_Base_Bus::ScadaAlarmUnknown:
            ret = TA_Base_Bus::DataPointCorbaTypes::ScadaAlarmUnknown ; 
            break;
        default:
            TA_ASSERT(false, "unknown TA_Base_Bus::EScadaAlarmAckState value");
        }
    }
    
    void convertAlarmProperty(const TA_Base_Bus::DataPointCorbaTypes::SAlarmProperty& alarmProperty, TA_Base_Bus::AlarmProperty& ret)
	{
		convertAlarmType(alarmProperty.type, ret.type);
		ret.enabled = alarmProperty.enabled;
		ret.threshold = alarmProperty.threshold;
		ret.enumValue = alarmProperty.enumValue;
	}

	void convertAlarmProperty(const TA_Base_Bus::AlarmProperty& alarmProperty, TA_Base_Bus::DataPointCorbaTypes::SAlarmProperty& ret)
	{
		convertAlarmType(alarmProperty.type, ret.type);
		ret.enabled = ( alarmProperty.enabled ? 1 : 0 );
		ret.threshold = alarmProperty.threshold;
		ret.enumValue = alarmProperty.enumValue;
	}

	void convertControlStateEnum(const TA_Base_Bus::DataPointCorbaTypes::EControlState& controlState, TA_Base_Bus::EDataPointControlState& ret)
	{
		switch(controlState) 
		{
		case TA_Base_Bus::DataPointCorbaTypes::NoControl :
			ret = TA_Base_Bus::noControl; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlWritten :
			ret = TA_Base_Bus::written ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlPending :
			ret = TA_Base_Bus::pending ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlConditionalSuccess :
			ret = TA_Base_Bus::conditionalSuccess ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlSuccess :
			ret = TA_Base_Bus::success ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlFailed :
			ret = TA_Base_Bus::failed ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlLccFailed :
			ret = TA_Base_Bus::LCC_failed ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlRccFailed:
			ret = TA_Base_Bus::RCC_failed ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlCommsFailed:
			ret = TA_Base_Bus::comms_failed ; 
			break;
			//Using for IEC104 Protocol
		case TA_Base_Bus::DataPointCorbaTypes::ControlSelectCmdWritten :
			ret = TA_Base_Bus::selectCmdWritten; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlSelectCmdConfirm :
			ret = TA_Base_Bus::selectCmdConfirm ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlSelectCmdConfirmTimeout :
			ret = TA_Base_Bus::selectCmdConfirmTimeout ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlOperateCmdTimeout:
			ret = TA_Base_Bus::operateCmdTimeout ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdWritten:
			ret = TA_Base_Bus::execCmdWritten ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdConfirm:
			ret = TA_Base_Bus::execCmdConfirm ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdConfirmTimeout:
			ret = TA_Base_Bus::execCmdConfirmTimeout ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdSuccess:
			ret = TA_Base_Bus::execCmdSuccess ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdSuccessTimeout:
			ret = TA_Base_Bus::execCmdSuccessTimeout ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlCancelCmdWritten:
			ret = TA_Base_Bus::cancelCmdWritten ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlCancelCmdConfirm:
			ret = TA_Base_Bus::cancelCmdConfirm ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlCancelCmdConfirmTimeout:
			ret = TA_Base_Bus::cancelCmdConfirmTimeout ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlSelectCmdCfmNegative:
			ret = TA_Base_Bus::selectCmdCfmNegative ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdCfmNegative:
			ret = TA_Base_Bus::execCmdCfmNegative ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlCancelCmdCfmNegative:
			ret = TA_Base_Bus::cancelCmdCfmNegative ; 
			break;
		case TA_Base_Bus::DataPointCorbaTypes::ControlDeviceControlException:
			ret = TA_Base_Bus::deviceControlException ; 
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::DataPointCorbaTypes::EControlState value");
		}
	}

	void convertControlStateEnum(const TA_Base_Bus::EDataPointControlState& controlState, TA_Base_Bus::DataPointCorbaTypes::EControlState& ret)
	{
		switch(controlState) 
		{
		case TA_Base_Bus::noControl :
			ret = TA_Base_Bus::DataPointCorbaTypes::NoControl ; 
			break;
		case TA_Base_Bus::written :
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlWritten ; 
			break;
		case TA_Base_Bus::pending :
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlPending ; 
			break;
		case TA_Base_Bus::conditionalSuccess :
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlConditionalSuccess ; 
			break;
		case TA_Base_Bus::success :
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlSuccess ; 
			break;
		case TA_Base_Bus::failed :
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlFailed ; 
			break;
		case TA_Base_Bus::LCC_failed :
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlLccFailed ; 
			break;
		case TA_Base_Bus::RCC_failed :
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlRccFailed; 
			break;
		case TA_Base_Bus::comms_failed :
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlCommsFailed; 
			break;

			//Using for IEC104 Protocol
		case TA_Base_Bus::selectCmdWritten:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlSelectCmdWritten; 
			break;
		case TA_Base_Bus::selectCmdConfirm:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlSelectCmdConfirm; 
			break;
		case TA_Base_Bus::selectCmdConfirmTimeout :
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlSelectCmdConfirmTimeout ; 
			break;
		case TA_Base_Bus::operateCmdTimeout:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlOperateCmdTimeout ; 
			break;
		case TA_Base_Bus::execCmdWritten:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdWritten ; 
			break;
		case TA_Base_Bus::execCmdConfirm:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdConfirm ; 
			break;
		case TA_Base_Bus::execCmdConfirmTimeout:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdConfirmTimeout ; 
			break;
		case TA_Base_Bus::execCmdSuccess:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdSuccess ; 
			break;
		case TA_Base_Bus::execCmdSuccessTimeout:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdSuccessTimeout ; 
			break;
		case TA_Base_Bus::cancelCmdWritten:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlCancelCmdWritten ; 
			break;
		case TA_Base_Bus::cancelCmdConfirm:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlCancelCmdConfirm ; 
			break;
		case TA_Base_Bus::cancelCmdConfirmTimeout:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlCancelCmdConfirmTimeout ; 
			break;
		case TA_Base_Bus::selectCmdCfmNegative:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlSelectCmdCfmNegative; 
			break;
		case TA_Base_Bus::execCmdCfmNegative:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlExecCmdCfmNegative; 
			break;
		case TA_Base_Bus::cancelCmdCfmNegative:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlCancelCmdCfmNegative; 
			break;
		case TA_Base_Bus::deviceControlException:
			ret = TA_Base_Bus::DataPointCorbaTypes::ControlDeviceControlException;
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::EDataPointControlState value");
		}
	}

	void convertControlState(const TA_Base_Bus::DataPointCorbaTypes::SControlState& control, TA_Base_Bus::DataPointControl& ret)
	{
		convertControlStateEnum(control.controlState, ret.controlState);
		ret.lccResult			= control.lccResult;
		ret.rccResult			= control.rccResult;
		ret.triggerringDpKey	= control.triggerringDpKey;
		ret.triggerringDpValue	= control.triggerringDpValue;
	}

	void convertControlState(const TA_Base_Bus::DataPointControl& control, TA_Base_Bus::DataPointCorbaTypes::SControlState& ret)
	{
		convertControlStateEnum(control.controlState, ret.controlState);
		ret.lccResult			= control.lccResult.c_str();
		ret.rccResult			= (control.rccResult ? 1 :0);
		ret.triggerringDpKey	= control.triggerringDpKey;
		ret.triggerringDpValue	= control.triggerringDpValue.c_str();
	}

	void convertInputInhibitState(const TA_Base_Bus::ScadaCorbaTypes::EInputInhibitState& inputInhibitState, TA_Base_Bus::EInputInhibitState& ret)
	{
		switch(inputInhibitState) 
		{
		case TA_Base_Bus::ScadaCorbaTypes::NoInputInhibit :
			ret = TA_Base_Bus::NoInputInhibit ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::InhibitScan :
			ret = TA_Base_Bus::InhibitScan ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::InhibitAlarm :
			ret = TA_Base_Bus::InhibitAlarm ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::InhibitMmsAlarm :
			ret = TA_Base_Bus::InhibitMmsAlarm ; 
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::ScadaCorbaTypes::EInputInhibitState value");
		}
	}

	void convertInputInhibitState(const TA_Base_Bus::EInputInhibitState& inputInhibitState, TA_Base_Bus::ScadaCorbaTypes::EInputInhibitState& ret)
	{
		switch(inputInhibitState) 
		{
		case TA_Base_Bus::NoInputInhibit :
			ret = TA_Base_Bus::ScadaCorbaTypes::NoInputInhibit ; 
			break;
		case TA_Base_Bus::InhibitScan :
			ret = TA_Base_Bus::ScadaCorbaTypes::InhibitScan ; 
			break;
		case TA_Base_Bus::InhibitAlarm :
			ret = TA_Base_Bus::ScadaCorbaTypes::InhibitAlarm ; 
			break;
		case TA_Base_Bus::InhibitMmsAlarm :
			ret = TA_Base_Bus::ScadaCorbaTypes::InhibitMmsAlarm ; 
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::EInputInhibitState value");
		}
	}

	void convertOutputInhibitState(const TA_Base_Bus::ScadaCorbaTypes::EOutputInhibitState& outputInhibitState, TA_Base_Bus::EOutputInhibitState& ret)
	{
		switch(outputInhibitState) 
		{
		case TA_Base_Bus::ScadaCorbaTypes::NoOutputInhibit :
			ret = TA_Base_Bus::NoOutputInhibit ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::InhibitControl :
			ret = TA_Base_Bus::InhibitControl ; 
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::ScadaCorbaTypes::EOutputInhibitState value");
		}
	}

	void convertOutputInhibitState(const TA_Base_Bus::EOutputInhibitState& outputInhibitState, TA_Base_Bus::ScadaCorbaTypes::EOutputInhibitState& ret)
	{
		switch(outputInhibitState) 
		{
		case TA_Base_Bus::NoOutputInhibit :
			ret = TA_Base_Bus::ScadaCorbaTypes::NoOutputInhibit ; 
			break;
		case TA_Base_Bus::InhibitControl :
			ret = TA_Base_Bus::ScadaCorbaTypes::InhibitControl ; 
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::EOutputInhibitState value");
		}
	}

	void convertInhibitState(const TA_Base_Bus::ScadaCorbaTypes::SInhibitState& inhibitState, TA_Base_Bus::InhibitState& ret)
	{
		convertInputInhibitState(inhibitState.inputInhibit, ret.inputInhibit);
		convertOutputInhibitState(inhibitState.outputInhibit, ret.outputInhibit);
	}

	void convertInhibitState(const TA_Base_Bus::InhibitState& inhibitState, TA_Base_Bus::ScadaCorbaTypes::SInhibitState& ret)
	{
		convertInputInhibitState(inhibitState.inputInhibit, ret.inputInhibit);
		convertOutputInhibitState(inhibitState.outputInhibit, ret.outputInhibit);
	}


	void convertDataPointDataType(const TA_Base_Bus::ScadaCorbaTypes::ETagType& type, TA_Base_Bus::EDataPointDataType& ret)
	{
		switch(type) 
		{
		case TA_Base_Bus::ScadaCorbaTypes::AnalogueTagType :
			//for the analogue type we will convert it into DPT_INT16_TYPE
			ret = TA_Base_Bus::DPT_INT16_TYPE ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::BooleanTagType :
			ret = TA_Base_Bus::DPT_BOOL_TYPE ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::EnumTagType :
			ret = TA_Base_Bus::DPT_ENUM_TYPE ; 
			break;
		case TA_Base_Bus::ScadaCorbaTypes::TextTagType :
			ret = TA_Base_Bus::DPT_TEXT_TYPE ; 
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::ScadaCorbaTypes::ETagType value");
		}
	}

	void convertDataPointDataType(const TA_Base_Bus::EDataPointDataType& type, TA_Base_Bus::ScadaCorbaTypes::ETagType& ret)
	{
		switch(type) 
		{
		case TA_Base_Bus::DPT_INT16_TYPE :
		case TA_Base_Bus::DPT_INT32_TYPE :
		case TA_Base_Bus::DPT_INT16S_TYPE :
		case TA_Base_Bus::DPT_INT32S_TYPE :
		case TA_Base_Bus::DPT_UINT16_TYPE :
		case TA_Base_Bus::DPT_UINT32_TYPE :
		case TA_Base_Bus::DPT_IEEE32_TYPE :
			//for the analogue type we will convert it into DPT_UINT32_TYPE
			ret = TA_Base_Bus::ScadaCorbaTypes::AnalogueTagType ; 
			break;
		case TA_Base_Bus::DPT_BOOL_TYPE :
			ret = TA_Base_Bus::ScadaCorbaTypes::BooleanTagType ; 
			break;
		case TA_Base_Bus::DPT_ENUM_TYPE :
			ret = TA_Base_Bus::ScadaCorbaTypes::EnumTagType ; 
			break;
		case TA_Base_Bus::DPT_TEXT_TYPE :
			ret = TA_Base_Bus::ScadaCorbaTypes::TextTagType ; 
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::EDataPointDataType value");
		}
	}

	void convertDataPointValue(const TA_Base_Bus::DataPointValue& dpValue, TA_Base_Bus::ScadaCorbaTypes::UTag& ret)
	{
		switch(dpValue.dpType) 
		{
		case TA_Base_Bus::DPT_INT16_TYPE :
		case TA_Base_Bus::DPT_INT32_TYPE :
		case TA_Base_Bus::DPT_INT16S_TYPE :
		case TA_Base_Bus::DPT_INT32S_TYPE :
		case TA_Base_Bus::DPT_UINT16_TYPE :
		case TA_Base_Bus::DPT_UINT32_TYPE :
		case TA_Base_Bus::DPT_IEEE32_TYPE :
			{
				//for the analogue type we will convert it into DPT_UINT32_TYPE
				TA_Base_Bus::ScadaCorbaTypes::SAnalogueTag analogueValue;
				analogueValue.value		= dpValue.floatValue;
				converQualityStatus(dpValue.quality, analogueValue.quality);
				converTimeStamp(dpValue.timeStamp, analogueValue.timeStamp);
				converControlOperation(dpValue.controlOperation,analogueValue.controlOperation);
				ret.analogueTag(analogueValue);			
				break;
			}
		case TA_Base_Bus::DPT_BOOL_TYPE :
			{
				TA_Base_Bus::ScadaCorbaTypes::SBooleanTag booleanValue;
				booleanValue.value		= ( dpValue.booleanValue ? 1 :0); 
				converQualityStatus(dpValue.quality, booleanValue.quality);
				converTimeStamp(dpValue.timeStamp, booleanValue.timeStamp);
				converControlOperation(dpValue.controlOperation,booleanValue.controlOperation);
				ret.booleanTag(booleanValue);			
				break;
			}
		case TA_Base_Bus::DPT_ENUM_TYPE :
			{
				TA_Base_Bus::ScadaCorbaTypes::SEnumTag enumValue;
				enumValue.value = dpValue.enumValue; 
				converQualityStatus(dpValue.quality, enumValue.quality);
				converTimeStamp(dpValue.timeStamp, enumValue.timeStamp);
				converControlOperation(dpValue.controlOperation,enumValue.controlOperation);
				ret.enumTag(enumValue);			
				break;
			}
		case TA_Base_Bus::DPT_TEXT_TYPE :
			{
				TA_Base_Bus::ScadaCorbaTypes::STextTag textValue;
				textValue.value.length(dpValue.stringValue.size());
				std::vector< unsigned char >::const_iterator it = dpValue.stringValue.begin();
				for(int i = 0 ; it != dpValue.stringValue.end(); ++it, ++i)
				{
					textValue.value[i] = *it;
				}
				converQualityStatus(dpValue.quality, textValue.quality);
				converTimeStamp(dpValue.timeStamp, textValue.timeStamp);
				converControlOperation(dpValue.controlOperation,textValue.controlOperation);
				ret.textTag(textValue);			
				break;
			}
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::EDataPointDataType value");
		}
	}

	void convertDataPointValue(const TA_Base_Bus::ScadaCorbaTypes::UTag& dpValue, TA_Base_Bus::DataPointValue& ret)
	{
		switch(dpValue._d())
		{
		case TA_Base_Bus::ScadaCorbaTypes::AnalogueTagType :
			{
				//for the analogue type we will convert it into DPT_INT16_TYPE
				ret.dpType		= TA_Base_Bus::DPT_INT16_TYPE ; 
				converQualityStatus(dpValue.analogueTag().quality, ret.quality);
				converTimeStamp(dpValue.analogueTag().timeStamp, ret.timeStamp);
				converControlOperation(dpValue.analogueTag().controlOperation,ret.controlOperation);
				ret.floatValue	= dpValue.analogueTag().value;
				break;
			}
		case TA_Base_Bus::ScadaCorbaTypes::BooleanTagType :
			{
				ret.dpType		= TA_Base_Bus::DPT_BOOL_TYPE ; 
				converQualityStatus(dpValue.booleanTag().quality, ret.quality);
				converTimeStamp(dpValue.booleanTag().timeStamp, ret.timeStamp);
				converControlOperation(dpValue.booleanTag().controlOperation,ret.controlOperation);
				ret.booleanValue = dpValue.booleanTag().value;
				break;
			}
		case TA_Base_Bus::ScadaCorbaTypes::EnumTagType :
			{
				ret.dpType		= TA_Base_Bus::DPT_ENUM_TYPE ; 
				converQualityStatus(dpValue.enumTag().quality, ret.quality);
				converTimeStamp(dpValue.enumTag().timeStamp, ret.timeStamp);
				converControlOperation(dpValue.enumTag().controlOperation,ret.controlOperation);
				ret.enumValue	= dpValue.enumTag().value;
				break;
			}
		case TA_Base_Bus::ScadaCorbaTypes::TextTagType :
			{
				ret.dpType = TA_Base_Bus::DPT_TEXT_TYPE ; 
				converQualityStatus(dpValue.textTag().quality, ret.quality);
				converTimeStamp(dpValue.textTag().timeStamp, ret.timeStamp);
				converControlOperation(dpValue.textTag().controlOperation,ret.controlOperation);
				for(int i=0; i < dpValue.textTag().value.length(); ++i)
				{
					ret.stringValue.push_back(dpValue.textTag().value[i]);
				}
				break;
			}
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::ScadaCorbaTypes::ETagType value");
		}
	}


	void convertLastOperatorInfo(const TA_Base_Bus::ScadaCorbaTypes::SLastOperatorInfo& lastOperatorInfo, TA_Base_Bus::LastOperatorInfo& ret)
	{
		ret.sessionID = lastOperatorInfo.sessionID;
		converTimeStamp(lastOperatorInfo.timeStamp, ret.timeStamp);
	}

	void convertLastOperatorInfo(const TA_Base_Bus::LastOperatorInfo& lastOperatorInfo, TA_Base_Bus::ScadaCorbaTypes::SLastOperatorInfo& ret)
	{
		ret.sessionID = lastOperatorInfo.sessionID.c_str();
		converTimeStamp(lastOperatorInfo.timeStamp, ret.timeStamp);
	}

	void convertDynamicSyncUpdate(const TA_Base_Bus::DataPointCorbaTypes::UDynamicSyncUpdate& update, TA_Base_Bus::DpDynamicSyncUpdate& ret)
	{

		switch(update._d())
		{
		case TA_Base_Bus::DataPointCorbaTypes::CompleteDynamicSyncStateUpdate:
			{
				ret.updateType = CompleteDynamicSyncStateUpdate;
				convertDataPointValue(update.syncCompleteState().engValue, ret.syncCompleteState.value);
				convertAlarmType(update.syncCompleteState().alarmState, ret.syncCompleteState.alarmState);
                convertAlarmAckState(update.syncCompleteState().alarmAckState, ret.syncCompleteState.alarmAckState);
                convertControlState(update.syncCompleteState().controlState, ret.syncCompleteState.controlState);
				convertInhibitState(update.syncCompleteState().inhibitState, ret.syncCompleteState.inhibitState);
				convertLastOperatorInfo(update.syncCompleteState().lastOperatorInfo, ret.syncCompleteState.lastOperatorInfo);

				const TA_Base_Bus::DataPointCorbaTypes::AlarmPropertySequence& alarmPropertySequence =  update.syncCompleteState().alarmProperties;
				for(int i = 0; i < alarmPropertySequence.length() ; ++i)
				{
					TA_Base_Bus::AlarmProperty temp;
					convertAlarmProperty(alarmPropertySequence[i], temp);
					ret.syncCompleteState.alarmProperties.push_back(temp);
				}

				break;
			}			
		case TA_Base_Bus::DataPointCorbaTypes::ValueUpdate:
			{
				ret.updateType = TA_Base_Bus::ValueUpdate;
				convertDataPointValue(update.engValue(), ret.value);
				break;
			}			
		case TA_Base_Bus::DataPointCorbaTypes::AlarmStateUpdate:
			{
				ret.updateType = TA_Base_Bus::AlarmStateUpdate;
				TA_Base_Bus::DataPointCorbaTypes::EDatapointAlarm temp = update.alarmState();
				convertAlarmType(temp, ret.alarmState);
				break;
			}			
        case TA_Base_Bus::DataPointCorbaTypes::AlarmAckStateUpdate:
            {
                ret.updateType = TA_Base_Bus::AlarmAckStateUpdate;
                TA_Base_Bus::DataPointCorbaTypes::EScadaAlarmAckState temp = update.alarmAckState();
                convertAlarmAckState(temp, ret.alarmAckState);
                break;
            }			
        case TA_Base_Bus::DataPointCorbaTypes::AlarmPropertiesUpdate:
			{
				ret.updateType = TA_Base_Bus::AlarmPropertiesUpdate;				
				const TA_Base_Bus::DataPointCorbaTypes::AlarmPropertySequence& alarmPropertySequence =  update.alarmProperties();
				for(int i = 0; i < alarmPropertySequence.length() ; ++i)
				{
					TA_Base_Bus::AlarmProperty temp;
					convertAlarmProperty(alarmPropertySequence[i], temp);
					ret.alarmProperties.push_back(temp);
				}
				break;
			}			
		case TA_Base_Bus::DataPointCorbaTypes::ControlUpdate:
			{
				ret.updateType = TA_Base_Bus::ControlUpdate;
				convertControlState(update.controlState(), ret.controlState);
				break;
			}
		case TA_Base_Bus::DataPointCorbaTypes::InhibitUpdate:
			{
				ret.updateType = TA_Base_Bus::InhibitUpdate;
				convertInhibitState(update.inhibitState(), ret.inhibitState);
				break;
			}
		case TA_Base_Bus::DataPointCorbaTypes::LastOperatorUpdate:
			{
				ret.updateType = TA_Base_Bus::LastOperatorUpdate;
				convertLastOperatorInfo(update.lastOperatorInfo(), ret.lastOperatorInfo);
				break;
			}
		default:
			{
				TA_ASSERT(false, "unknown TA_Base_Bus::DataPointCorbaTypes::EDynamicSyncUpdate type");
			}
		}
	}

	void convertDynamicSyncUpdate(const TA_Base_Bus::DpDynamicSyncUpdate& update, TA_Base_Bus::DataPointCorbaTypes::UDynamicSyncUpdate& ret)
	{
		switch(update.updateType) 
		{
		case TA_Base_Bus::ValueUpdate:
			{
				TA_Base_Bus::ScadaCorbaTypes::UTag value;
				convertDataPointValue(update.value, value);
				ret.engValue( value );
				break;
			}
		case TA_Base_Bus::AlarmStateUpdate:
			{
				TA_Base_Bus::DataPointCorbaTypes::EDatapointAlarm alarmState;
				convertAlarmType(update.alarmState, alarmState);
				ret.alarmState( alarmState );
				break;
			}
        case TA_Base_Bus::AlarmAckStateUpdate:
            {
                TA_Base_Bus::DataPointCorbaTypes::EScadaAlarmAckState alarmAckState;
                convertAlarmAckState(update.alarmAckState, alarmAckState);
                ret.alarmAckState( alarmAckState );
                break;
            }
		case TA_Base_Bus::AlarmPropertiesUpdate:
			{
				TA_Base_Bus::DataPointCorbaTypes::AlarmPropertySequence alarmProperties;
				alarmProperties.length(update.alarmProperties.size());
				
				for (int i=0; i < update.alarmProperties.size(); ++i)
				{
					convertAlarmProperty(update.alarmProperties[i], alarmProperties[i]);
				}
				ret.alarmProperties( alarmProperties );
				break;
			}				
		case TA_Base_Bus::ControlUpdate:
			{
				TA_Base_Bus::DataPointCorbaTypes::SControlState controlState;
				convertControlState(update.controlState, controlState);
				ret.controlState( controlState );
				break;
			}
		case TA_Base_Bus::InhibitUpdate:
			{
				TA_Base_Bus::ScadaCorbaTypes::SInhibitState inhibitState;
				convertInhibitState(update.inhibitState, inhibitState);
				ret.inhibitState( inhibitState );
				break;
			}
		case TA_Base_Bus::LastOperatorUpdate:
			{
				TA_Base_Bus::ScadaCorbaTypes::SLastOperatorInfo lastOperatorInfo;
				convertLastOperatorInfo(update.lastOperatorInfo, lastOperatorInfo);
				ret.lastOperatorInfo( lastOperatorInfo );
				break;
			}
		case TA_Base_Bus::CompleteDynamicSyncStateUpdate:
			{
				TA_Base_Bus::DataPointCorbaTypes::SCompleteDynamicSyncState completeDynamicSyncState;
				convertDataPointValue(update.syncCompleteState.value, completeDynamicSyncState.engValue);
				convertAlarmType(update.syncCompleteState.alarmState, completeDynamicSyncState.alarmState);
                convertAlarmAckState(update.syncCompleteState.alarmAckState, completeDynamicSyncState.alarmAckState);
                convertControlState(update.syncCompleteState.controlState, completeDynamicSyncState.controlState);
				convertInhibitState(update.syncCompleteState.inhibitState, completeDynamicSyncState.inhibitState);
				convertLastOperatorInfo(update.syncCompleteState.lastOperatorInfo, completeDynamicSyncState.lastOperatorInfo);
				
				completeDynamicSyncState.alarmProperties.length(update.syncCompleteState.alarmProperties.size());				
				for (int i=0; i < update.alarmProperties.size(); ++i)
				{
					convertAlarmProperty(update.alarmProperties[i], completeDynamicSyncState.alarmProperties[i]);
				}				
				
				ret.syncCompleteState( completeDynamicSyncState );
				break;
			}
		default:
			{
				TA_ASSERT(false, "unknown TA_Base_Bus::EDynamicSyncUpdate ");
			}
		}
	}

	void convertDpCompleteUpdate(const TA_Base_Bus::DpCompleteDynamicSyncState& update, TA_Base_Bus::DataPointCorbaTypes::UDynamicSyncUpdate& ret)
	{
		TA_Base_Bus::DataPointCorbaTypes::SCompleteDynamicSyncState completeDynamicSyncState;
		convertDataPointValue(update.value, completeDynamicSyncState.engValue);
		convertAlarmType(update.alarmState, completeDynamicSyncState.alarmState);
        convertAlarmAckState(update.alarmAckState, completeDynamicSyncState.alarmAckState);
        convertControlState(update.controlState, completeDynamicSyncState.controlState);
		convertInhibitState(update.inhibitState, completeDynamicSyncState.inhibitState);
		convertLastOperatorInfo(update.lastOperatorInfo, completeDynamicSyncState.lastOperatorInfo);
		
		completeDynamicSyncState.alarmProperties.length(update.alarmProperties.size());				
		for (int i=0; i < update.alarmProperties.size(); ++i)
		{
			convertAlarmProperty(update.alarmProperties[i], completeDynamicSyncState.alarmProperties[i]);
		}				
		
		ret.syncCompleteState( completeDynamicSyncState );
	}

	void convertDpDisplayUpdate(const TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState& displayInfo, TA_Base_Bus::DataPointDisplayState& ret)
	{
		ret.alarmState = displayInfo.alarmState;
        convertAlarmAckState(displayInfo.alarmAckState, ret.alarmAckState);
		ret.dataPointStatus = displayInfo.dataPointStatus;
		ret.entityKey = displayInfo.entityKey;
		switch(displayInfo.value._d())
		{
		case TA_Base_Bus::ScadaCorbaTypes::AnalogueTagType :
			{
				//for the analogue type we will convert it into DPT_INT16_TYPE
				ret.value.dpType = TA_Base_Bus::DPT_INT16_TYPE ; 
				ret.value.floatValue = displayInfo.value.floatValue();
				break;
			}
		case TA_Base_Bus::ScadaCorbaTypes::BooleanTagType :
			{
				ret.value.dpType = TA_Base_Bus::DPT_BOOL_TYPE ; 
				ret.value.booleanValue = displayInfo.value.booleanValue();
				break;
			}
		case TA_Base_Bus::ScadaCorbaTypes::EnumTagType :
			{
				ret.value.dpType = TA_Base_Bus::DPT_ENUM_TYPE ; 
				ret.value.enumValue = displayInfo.value.enumValue();
				break;
			}
		case TA_Base_Bus::ScadaCorbaTypes::TextTagType :
			{
				ret.value.dpType = TA_Base_Bus::DPT_TEXT_TYPE ;
				const TA_Base_Bus::ScadaCorbaTypes::ByteSequence& stringValue = displayInfo.value.stringValue();
				for(int i=0; i < stringValue.length(); ++i)
				{
					ret.value.stringValue.push_back(stringValue[i]);
				}
				break;
			}
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::ScadaCorbaTypes::ETagType value");
		}

	}

	void convertDpDisplayUpdate(const TA_Base_Bus::DataPointDisplayState& displayInfo, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState& ret)
	{
        ret.alarmState = (displayInfo.alarmState ? 1 : 0 );
        convertAlarmAckState(displayInfo.alarmAckState, ret.alarmAckState);
		ret.dataPointStatus = displayInfo.dataPointStatus;
		ret.entityKey = displayInfo.entityKey;

		switch(displayInfo.value.dpType) 
		{
		case TA_Base_Bus::DPT_INT16_TYPE :
		case TA_Base_Bus::DPT_INT32_TYPE :
		case TA_Base_Bus::DPT_INT16S_TYPE :
		case TA_Base_Bus::DPT_INT32S_TYPE :
		case TA_Base_Bus::DPT_UINT16_TYPE :
		case TA_Base_Bus::DPT_UINT32_TYPE :
		case TA_Base_Bus::DPT_IEEE32_TYPE :
			//for the analogue type we will convert it into DPT_UINT32_TYPE
			ret.value.floatValue(displayInfo.value.floatValue); 
			break;
		case TA_Base_Bus::DPT_BOOL_TYPE :
			ret.value.booleanValue( (displayInfo.value.booleanValue ? 1 : 0) ); 
			break;
		case TA_Base_Bus::DPT_ENUM_TYPE :
			ret.value.enumValue(displayInfo.value.enumValue) ;
			break;
		case TA_Base_Bus::DPT_TEXT_TYPE :
			{
				TA_Base_Bus::ScadaCorbaTypes::ByteSequence stringValue;
				stringValue.length(displayInfo.value.stringValue.size());
				std::vector< unsigned char >::const_iterator it = displayInfo.value.stringValue.begin();
				for(int i = 0 ; it != displayInfo.value.stringValue.end(); ++it, ++i)
				{
					stringValue[i] = *it;
				}
				ret.value.stringValue(stringValue);
				break;
			}
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::EDataPointDataType value");
		}

	}

	void convertETagState(const TA_Base_Bus::DataNodeCorbaTypes::ETagState& tagState, TA_Base_Bus::ETagState& ret)
	{
		switch(tagState) 
		{
		case TA_Base_Bus::DataNodeCorbaTypes::NotTagged :
			ret = TA_Base_Bus::NotTagged;
			break;
		case TA_Base_Bus::DataNodeCorbaTypes::Tagged :
			ret = TA_Base_Bus::Tagged;
			break;
		case TA_Base_Bus::DataNodeCorbaTypes::TaggedPtw :
			ret = TA_Base_Bus::TaggedPtw;
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::DataNodeCorbaTypes::ETagState.");
		}
	}

	void convertETagState(const TA_Base_Bus::ETagState& tagState, TA_Base_Bus::DataNodeCorbaTypes::ETagState& ret)
	{
		switch(tagState) 
		{
		case TA_Base_Bus::NotTagged :
			ret = TA_Base_Bus::DataNodeCorbaTypes::NotTagged;
			break;
		case TA_Base_Bus::Tagged :
			ret = TA_Base_Bus::DataNodeCorbaTypes::Tagged;
			break;
		case TA_Base_Bus::TaggedPtw :
			ret = TA_Base_Bus::DataNodeCorbaTypes::TaggedPtw;
			break;
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::ETagState.");
		}
	}

	void convertTagState(const TA_Base_Bus::DataNodeCorbaTypes::STagState& inputTagState, TA_Base_Bus::TagState& ret)
	{
		convertETagState(inputTagState.tagState, ret.tagState);
		convertInhibitState(inputTagState.tagInhibitState, ret.tagInhibitState);
		ret.comment = inputTagState.comment;
		ret.ptwId = inputTagState.ptwId;
	}

	void convertTagState(const TA_Base_Bus::TagState& inputTagState, TA_Base_Bus::DataNodeCorbaTypes::STagState& ret)
	{
		convertETagState(inputTagState.tagState, ret.tagState);
		convertInhibitState(inputTagState.tagInhibitState, ret.tagInhibitState);
		ret.comment = inputTagState.comment.c_str();
		ret.ptwId = inputTagState.ptwId.c_str();
	}

	void convertBooleanTag(const TA_Base_Bus::ScadaCorbaTypes::SBooleanTag& booleanValue, TA_Base_Bus::BooleanTag& ret)
	{
		converQualityStatus(booleanValue.quality, ret.quality);
		ret.value = booleanValue.value;
		//booleanValue.controlOperation =  TA_Base_Bus::ScadaCorbaTypes::ControlNoOperation;
	}

	void convertBooleanTag(const TA_Base_Bus::BooleanTag& booleanValue, TA_Base_Bus::ScadaCorbaTypes::SBooleanTag& ret)
	{
		converQualityStatus(booleanValue.quality, ret.quality);
		ret.value = ( booleanValue.value ? 1 : 0 );

		//Need initialize the controlOperation.
		ret.controlOperation =  TA_Base_Bus::ScadaCorbaTypes::ControlNoOperation;

	}

	void convertEnumTag(const TA_Base_Bus::ScadaCorbaTypes::SEnumTag& enumValue, TA_Base_Bus::EnumTag& ret)
	{
		converQualityStatus(enumValue.quality, ret.quality);
		ret.value = enumValue.value;
		//enumValue.controlOperation =  TA_Base_Bus::ScadaCorbaTypes::ControlNoOperation;

	}

	void convertEnumTag(const TA_Base_Bus::EnumTag& enumValue, TA_Base_Bus::ScadaCorbaTypes::SEnumTag& ret)
	{
		converQualityStatus(enumValue.quality, ret.quality);
		ret.value = enumValue.value;
		//Need initialize the controlOperation.
		ret.controlOperation =  TA_Base_Bus::ScadaCorbaTypes::ControlNoOperation;

	}

	void convertDnCompleteUpdate(const TA_Base_Bus::DnCompleteDynamicSyncState & update, TA_Base_Bus::DataNodeCorbaTypes::SCompleteDynamicSyncState& ret)
	{
		convertTagState(update.tagState, ret.tagState);
		convertBooleanTag(update.alarmSummary, ret.alarmSummary);
		convertEnumTag(update.ackSummary, ret.ackSummary);
		convertBooleanTag(update.scanInhibitSummary, ret.scanInhibitSummary);
		convertBooleanTag(update.alarmInhibitSummary, ret.alarmInhibitSummary);
		convertBooleanTag(update.mmsAlarmInhibitSummary, ret.mmsAlarmInhibitSummary);
		convertBooleanTag(update.controlInhibitSummary, ret.controlInhibitSummary);
		convertBooleanTag(update.overrideSummary, ret.overrideSummary);
		convertEnumTag(update.qualitySummary, ret.qualitySummary);
		convertBooleanTag(update.controlPendingSummary, ret.controlPendingSummary);
		convertBooleanTag(update.combinedInhibitMMSState, ret.combinedInhibitMMSState);
		ret.notes = update.notes.c_str();
		ret.inhibitMMSState = ( update.inhibitMMSState ? 1 : 0 );
		convertLastOperatorInfo(update.lastOperator, ret.lastOperator);
	}

	void convertDnCompleteUpdate(const TA_Base_Bus::DnCompleteDynamicSyncState & update, TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate& ret)
	{
		TA_Base_Bus::DataNodeCorbaTypes::SCompleteDynamicSyncState completeUpdate;
		convertDnCompleteUpdate(update, completeUpdate);
		ret.syncCompleteState(completeUpdate);
	}

	void convertDnCompleteUpdate(const TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate & update, TA_Base_Bus::DnCompleteDynamicSyncState& ret)
	{
		convertTagState(update.syncCompleteState().tagState, ret.tagState);
		convertBooleanTag(update.syncCompleteState().alarmSummary, ret.alarmSummary);
		convertEnumTag(update.syncCompleteState().ackSummary, ret.ackSummary);
		convertBooleanTag(update.syncCompleteState().scanInhibitSummary, ret.scanInhibitSummary);
		convertBooleanTag(update.syncCompleteState().alarmInhibitSummary, ret.alarmInhibitSummary);
		convertBooleanTag(update.syncCompleteState().mmsAlarmInhibitSummary, ret.mmsAlarmInhibitSummary);
		convertBooleanTag(update.syncCompleteState().controlInhibitSummary, ret.controlInhibitSummary);
		convertBooleanTag(update.syncCompleteState().overrideSummary, ret.overrideSummary);
		convertEnumTag(update.syncCompleteState().qualitySummary, ret.qualitySummary);
		convertBooleanTag(update.syncCompleteState().controlPendingSummary, ret.controlPendingSummary);
		convertBooleanTag(update.syncCompleteState().combinedInhibitMMSState, ret.combinedInhibitMMSState);
		ret.notes = update.syncCompleteState().notes;
		ret.inhibitMMSState = update.syncCompleteState().inhibitMMSState;
		convertLastOperatorInfo(update.syncCompleteState().lastOperator, ret.lastOperator);
	}

	void convertDnDynamicSyncUpdate(const TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate& update, TA_Base_Bus::DnSyncUpdate& ret)
	{
		switch(update._d())
		{
		case TA_Base_Bus::DataNodeCorbaTypes::CompleteSyncState:
			{
				ret.updateType = TA_Base_Bus::DnCompleteSyncState;
				convertDnCompleteUpdate(update, ret.completeUpdate);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::TagStateUpdate:
			{
				ret.updateType = TA_Base_Bus::DnTagStateUpdate;
				convertTagState(update.tagState(), ret.tagState);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::AlarmSummaryUpdate:
			{
				ret.updateType = TA_Base_Bus::DnAlarmSummaryUpdate;
				convertBooleanTag(update.alarmSummary(), ret.alarmSummary);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::AlarmAckSummaryUpdate:
			{
				ret.updateType = TA_Base_Bus::DnAlarmAckSummaryUpdate;
				convertEnumTag(update.ackSummary() , ret.ackSummary);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::ScanInhibitSummaryUpdate:
			{
				ret.updateType = TA_Base_Bus::DnScanInhibitSummaryUpdate;
				convertBooleanTag(update.scanInhibitSummary(), ret.scanInhibitSummary);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::AlarmInhibitSummaryUpdate:
			{
				ret.updateType = TA_Base_Bus::DnAlarmInhibitSummaryUpdate;
				convertBooleanTag(update.alarmInhibitSummary(), ret.alarmInhibitSummary);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::MmsAlarmInhibitSummaryUpdate:
			{
				ret.updateType = TA_Base_Bus::DnMmsAlarmInhibitSummaryUpdate;
				convertBooleanTag(update.mmsAlarmInhibitSummary(), ret.mmsAlarmInhibitSummary);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::ControlInhibitSummaryUpdate:
			{
				ret.updateType = TA_Base_Bus::DnControlInhibitSummaryUpdate;
				convertBooleanTag(update.controlInhibitSummary(), ret.controlInhibitSummary);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::OverrideSummaryUpdate:
			{
				ret.updateType = TA_Base_Bus::DnOverrideSummaryUpdate;
				convertBooleanTag(update.overrideSummary(), ret.overrideSummary);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::QualitySummaryUpdate:
			{
				ret.updateType = TA_Base_Bus::DnQualitySummaryUpdate;
				convertEnumTag(update.qualitySummary(), ret.qualitySummary);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::ControlPendingSummaryUpdate:
			{
				ret.updateType = TA_Base_Bus::DnControlPendingSummaryUpdate;
				convertBooleanTag(update.controlPendingSummary(), ret.controlPendingSummary);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::CombinedInhibitMMSUpdate:
			{
				ret.updateType = TA_Base_Bus::DnCombinedInhibitMMSUpdate;
				convertBooleanTag(update.combinedInhibitMMSState(), ret.combinedInhibitMMSState);
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::NotesUpdate:
			{
				ret.updateType = TA_Base_Bus::DnNotesUpdate;
				ret.notes = update.notes();
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::InhibitMMSUpdate:
			{
				ret.updateType = TA_Base_Bus::DnInhibitMMSUpdate;
				ret.inhibitMMSState = update.inhibitMMSState();
				break;
			}
		case TA_Base_Bus::DataNodeCorbaTypes::LastOperatorUpdate:
			{
				ret.updateType = TA_Base_Bus::DnLastOperatorUpdate;
				convertLastOperatorInfo(update.lastOperator(), ret.lastOperator);
				break;
			}
		default:
			TA_ASSERT(false, "unknow item in TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate");
		}
	}

	void convertDnDynamicSyncUpdate(const TA_Base_Bus::DnSyncUpdate& update, TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate& ret)
	{
		switch(update.updateType)
		{
		case TA_Base_Bus::DnCompleteSyncState :
			{
				TA_Base_Bus::DataNodeCorbaTypes::SCompleteDynamicSyncState completeUpdate;
				convertDnCompleteUpdate(update.completeUpdate, completeUpdate);
				ret.syncCompleteState(completeUpdate);
				break;
			}
		case TA_Base_Bus::DnTagStateUpdate :
			{
				TA_Base_Bus::DataNodeCorbaTypes::STagState tagState;
				convertTagState(update.tagState, tagState);
				ret.tagState(tagState);
				break;
			}
		case TA_Base_Bus::DnAlarmSummaryUpdate :
			{
				TA_Base_Bus::ScadaCorbaTypes::SBooleanTag newUpdate;
				convertBooleanTag(update.alarmSummary, newUpdate);
				ret.alarmSummary(newUpdate);
				break;
			}
		case TA_Base_Bus::DnAlarmAckSummaryUpdate :
			{
				TA_Base_Bus::ScadaCorbaTypes::SEnumTag newUpdate;
				convertEnumTag(update.ackSummary, newUpdate);
				ret.ackSummary(newUpdate);
				break;
			}
		case TA_Base_Bus::DnScanInhibitSummaryUpdate :
			{
				TA_Base_Bus::ScadaCorbaTypes::SBooleanTag newUpdate;
				convertBooleanTag(update.scanInhibitSummary, newUpdate);
				ret.scanInhibitSummary(newUpdate);
				break;
			}
		case TA_Base_Bus::DnAlarmInhibitSummaryUpdate :
			{
				TA_Base_Bus::ScadaCorbaTypes::SBooleanTag newUpdate;
				convertBooleanTag(update.alarmInhibitSummary, newUpdate);
				ret.alarmInhibitSummary(newUpdate);
				break;
			}
		case TA_Base_Bus::DnMmsAlarmInhibitSummaryUpdate :
			{
				TA_Base_Bus::ScadaCorbaTypes::SBooleanTag newUpdate;
				convertBooleanTag(update.mmsAlarmInhibitSummary, newUpdate);
				ret.mmsAlarmInhibitSummary(newUpdate);
				break;
			}
		case TA_Base_Bus::DnControlInhibitSummaryUpdate :
			{
				TA_Base_Bus::ScadaCorbaTypes::SBooleanTag newUpdate;
				convertBooleanTag(update.controlInhibitSummary, newUpdate);
				ret.controlInhibitSummary(newUpdate);
				break;
			}
		case TA_Base_Bus::DnOverrideSummaryUpdate :
			{
				TA_Base_Bus::ScadaCorbaTypes::SBooleanTag newUpdate;
				convertBooleanTag(update.overrideSummary, newUpdate);
				ret.overrideSummary(newUpdate);
				break;
			}
		case TA_Base_Bus::DnQualitySummaryUpdate :
			{
				TA_Base_Bus::ScadaCorbaTypes::SEnumTag newUpdate;
				convertEnumTag(update.qualitySummary, newUpdate);
				ret.qualitySummary(newUpdate);
				break;
			}
		case TA_Base_Bus::DnControlPendingSummaryUpdate :
			{
				TA_Base_Bus::ScadaCorbaTypes::SBooleanTag newUpdate;
				convertBooleanTag(update.controlPendingSummary, newUpdate);
				ret.controlPendingSummary(newUpdate);
				break;
			}
		case TA_Base_Bus::DnCombinedInhibitMMSUpdate :
			{
				TA_Base_Bus::ScadaCorbaTypes::SBooleanTag newUpdate;
				convertBooleanTag(update.combinedInhibitMMSState, newUpdate);
				ret.combinedInhibitMMSState(newUpdate);
				break;
			}
		case TA_Base_Bus::DnNotesUpdate :
			{
				ret.notes(update.notes.c_str());
				break;
			}
		case TA_Base_Bus::DnInhibitMMSUpdate :
			{
				ret.inhibitMMSState( ( update.inhibitMMSState ? 1 : 0 ) );
				break;
			}
		case TA_Base_Bus::DnLastOperatorUpdate :
			{
				TA_Base_Bus::ScadaCorbaTypes::SLastOperatorInfo newUpdate;
				convertLastOperatorInfo(update.lastOperator, newUpdate);
				ret.lastOperator(newUpdate);
				break;
			}
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::EDnSyncUpdate.");
		}
	}

	void convertDnDisplayUpdate(const TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState& displayInfo, TA_Base_Bus::DataNodeDisplayState& ret)
	{
		ret.ONITP = displayInfo.ONITP;
		ret.entityKey =displayInfo.entityKey;
		convertEnumTag(displayInfo.alarmAckState, ret.alarmAckState);
		convertBooleanTag(displayInfo.alarmState, ret.alarmState);
		convertEnumTag(displayInfo.quality ,ret.quality);
	}

	void convertDnDisplayUpdate(const TA_Base_Bus::DataNodeDisplayState& displayInfo, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState& ret)
	{
		ret.ONITP = displayInfo.ONITP.c_str();
		ret.entityKey =displayInfo.entityKey;
		convertEnumTag(displayInfo.alarmAckState, ret.alarmAckState);
		convertBooleanTag(displayInfo.alarmState, ret.alarmState);
		convertEnumTag(displayInfo.quality ,ret.quality);
	}

    ////////////////for convert dp EControlCommandState ////////
    void convertEControlCommandState(const TA_Base_Bus::DataPointCorbaTypes::EControlCommandState& input, TA_Base_Bus::EControlCommandState& ret)
    {
        switch(input) 
        {
        case DataPointCorbaTypes::JustReceived:
            ret = JustReceived;
            break;
        case DataPointCorbaTypes::BeingExecuted:
            ret = BeingExecuted;
            break;
        case DataPointCorbaTypes::CompletedWithError:
            ret = CompletedWithError;
            break;
        case DataPointCorbaTypes::CompletedWithSuccess:
            ret = CompletedWithSuccess;
            break;
        default:
            TA_ASSERT(false, "Bad control command state.");
        }
    }

    void convertEControlCommandState(const TA_Base_Bus::EControlCommandState& input, TA_Base_Bus::DataPointCorbaTypes::EControlCommandState& ret)
    {
        switch(input) 
        {
        case JustReceived:
            ret = DataPointCorbaTypes::JustReceived;
            break;
        case BeingExecuted:
            ret = DataPointCorbaTypes::BeingExecuted;
            break;
        case CompletedWithError:
            ret = DataPointCorbaTypes::CompletedWithError;
            break;
        case CompletedWithSuccess:
            ret = DataPointCorbaTypes::CompletedWithSuccess;
            break;
        default:
            TA_ASSERT(false, "Bad control command state.");
        }
    }
    
    ////////////////for convert dp DataPointStateUpdate ////////
    void convertDataPointValue(const TA_Base_Bus::DataPointCorbaTypes::UDataPointValue& input, TA_Base_Bus::DataPointValue& ret)
    {
        switch(input._d())
        {
        case TA_Base_Bus::ScadaCorbaTypes::AnalogueTagType :
            {
                //for the analogue type we will convert it into DPT_INT16_TYPE
                ret.dpType		= TA_Base_Bus::DPT_INT16_TYPE ; 
                ret.floatValue	= input.floatValue();
                break;
            }
        case TA_Base_Bus::ScadaCorbaTypes::BooleanTagType :
            {
                ret.dpType		= TA_Base_Bus::DPT_BOOL_TYPE ; 
                ret.booleanValue = input.booleanValue();
                break;
            }
        case TA_Base_Bus::ScadaCorbaTypes::EnumTagType :
            {
                ret.dpType		= TA_Base_Bus::DPT_ENUM_TYPE ; 
                ret.enumValue	= input.enumValue();
                break;
            }
        case TA_Base_Bus::ScadaCorbaTypes::TextTagType :
            {
                const TA_Base_Bus::ScadaCorbaTypes::ByteSequence& stringValue = input.stringValue();
                for (unsigned long i = 0; i < stringValue.length(); ++ i)
                {
                    ret.stringValue.push_back(stringValue[i]);
                }
                break;
            }
        default:
            TA_ASSERT(false, "unknown TA_Base_Bus::ScadaCorbaTypes::ETagType value");
        }
    }

    void convertDataPointValue(const TA_Base_Bus::DataPointValue& input, TA_Base_Bus::DataPointCorbaTypes::UDataPointValue& ret)
    {
        switch(input.dpType) 
        {
        case TA_Base_Bus::DPT_INT16_TYPE :
        case TA_Base_Bus::DPT_INT32_TYPE :
        case TA_Base_Bus::DPT_INT16S_TYPE :
        case TA_Base_Bus::DPT_INT32S_TYPE :
        case TA_Base_Bus::DPT_UINT16_TYPE :
        case TA_Base_Bus::DPT_UINT32_TYPE :
        case TA_Base_Bus::DPT_IEEE32_TYPE :
                ret.floatValue(input.floatValue);
                break;
        case TA_Base_Bus::DPT_BOOL_TYPE :
                ret.booleanValue(input.booleanValue);
                break;
        case TA_Base_Bus::DPT_ENUM_TYPE :
                ret.enumValue(input.enumValue);
                break;
        case TA_Base_Bus::DPT_TEXT_TYPE :
            {
                TA_Base_Bus::ScadaCorbaTypes::ByteSequence stringValue;
                for (unsigned long i = 0; i < input.stringValue.size(); ++ i)
                {
                    stringValue[i] = input.stringValue[i];
                }
                ret.stringValue(stringValue);			
                break;
            }
        default:
            TA_ASSERT(false, "unknown TA_Base_Bus::EDataPointDataType value");
        }
    }
    
    ////////////////for convert dp DataPointStateUpdate ////////
    void convertDataPointStateUpdate(const TA_Base_Bus::DataPointCorbaTypes::DataPointWriteStateUpdate& input, TA_Base_Bus::DataPointStateUpdate& ret)
    {
        ret.auditEventLoggingEnabled = input.auditEventLoggingEnabled;
        ret.commandId = input.commandId;
        
        convertEControlCommandState(input.controlCommandState, ret.controlCommandState);
        convertControlStateEnum(input.controlState, ret.controlState);
        
        ret.dpKey = input.dpKey;

        convertDataPointValue(input.dpValue, ret.dpValue);
        
        ret.sessionId = input.sessionId;
        ret.triggeringDpKey = input.triggeringDpKey;
        ret.triggeringDpValue = input.triggeringDpValue;
    }

    void convertDataPointStateUpdate(const TA_Base_Bus::DataPointStateUpdate& input, TA_Base_Bus::DataPointCorbaTypes::DataPointWriteStateUpdate& ret)
    {
        ret.auditEventLoggingEnabled = input.auditEventLoggingEnabled;
        ret.commandId = input.commandId;
        
        convertEControlCommandState(input.controlCommandState, ret.controlCommandState);
        convertControlStateEnum(input.controlState, ret.controlState);

        ret.dpKey = input.dpKey;

        convertDataPointValue(input.dpValue, ret.dpValue);
        
        ret.sessionId = input.sessionId.c_str();
        ret.triggeringDpKey = input.triggeringDpKey;
        ret.triggeringDpValue = input.triggeringDpValue.c_str();
    }
    
}; // TA_Base_Bus


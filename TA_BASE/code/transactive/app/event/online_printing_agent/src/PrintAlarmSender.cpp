/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintAlarmSender.cpp $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Sends Online Printing Agent alarm messages.
  * 
  */

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

namespace
{
    std::string PRINTER_NAME_PARAM("PrinterName");
    std::string ADDITIONAL_DETAILS_PARAM("AdditionalDetails");
}

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App;

	
PrintAlarmSender::PrintAlarmSender()
	//TD18093,jianghp
	//m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper())
{
	FUNCTION_ENTRY("PrintAlarmSender");

    

	FUNCTION_EXIT;
}


PrintAlarmSender::~PrintAlarmSender()
{
	FUNCTION_ENTRY("~PrintAlarmSender");
	TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

	FUNCTION_EXIT;
}


void PrintAlarmSender::sendPrintFailureAlarm(const std::string& printer, const std::string& additionalDetails)
{
	FUNCTION_ENTRY("sendPrintFailureAlarm");
    try
    {
        // Set up the description parameters.
        TA_Base_Core::NameValuePair printerNameParam(PRINTER_NAME_PARAM, printer);
        TA_Base_Core::NameValuePair additionalDetailsParam(ADDITIONAL_DETAILS_PARAM, additionalDetails);

        TA_Base_Core::DescriptionParameters alarmParams;
        alarmParams.push_back(&printerNameParam);
        alarmParams.push_back(&additionalDetailsParam);

        // Raise the alarm.
        ConfigData& configData = ConfigData::getInstance();

        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::OnlinePrintingAgentAlarm::PrintFailure,
                                   configData.getAgentEntityKey(),
                                   configData.getAgentEntityTypeKey(),
                                   alarmParams,
                                   configData.getAgentEntityName(),
                                   configData.getAgentEntityLocationKey(),
                                   configData.getAgentEntitySubsystemKey(),
								   configData.getAgentEntityAssetName(),
								   TA_Base_Core::AlarmConstants::defaultTime);

    }
    catch (const std::exception& ex)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to send print failure alarm");
        LOG_EXCEPTION_DETAILS(SourceInfo, typeid(ex).name(), ex.what());
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to send print failure alarm");
        LOG_EXCEPTION_DETAILS(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex));
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to send print failure alarm");
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unknown exception.", "");
    }

	FUNCTION_EXIT;
}


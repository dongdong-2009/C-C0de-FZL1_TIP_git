/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/PrintJobSpooler.cpp $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Submits print jobs to the printing system. Written with LPRng in mind,
  * however, should work with any system that supports the Berkeley LPR
  * print spooler functionality.
  *
  */

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

namespace
{
    const int SYSTEM_CALL_SUCCESS(0);
    const int SYSTEM_CALL_FAILURE(-1);
}

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App;


PrintJobSpooler::PrintJobSpooler()
{
    FUNCTION_ENTRY("PrintJobSpooler()");
    FUNCTION_EXIT;
}


PrintJobSpooler::~PrintJobSpooler()
{
    FUNCTION_ENTRY("~PrintJobSpooler()");
    FUNCTION_EXIT;
}


bool PrintJobSpooler::submit(const PrintableData& printableData)
{
    FUNCTION_ENTRY("submit(const PrintableData& printableData)");

    bool result(false);
    char tempFileName[L_tmpnam];

    ::tmpnam(tempFileName);

    try
    {
        // Dump the output to a temporary file.
        std::ofstream tempFile(tempFileName);
        tempFile << printableData.getOutput();
        tempFile.close();

        // Ask the print system to throw the file onto the print queue.
        std::ostringstream systemCommand;
        systemCommand << ConfigData::getInstance().getPrintCommand() 
                      << " -P " << printableData.getPrinter() 
                      << " " << tempFileName
                      << " > /dev/null";

        int exitCode(::system(systemCommand.str().c_str()));
        result = (exitCode == SYSTEM_CALL_SUCCESS);

        if (result == false)
        {
            std::ostringstream msg, details;
            msg << "Failed to execute command '" << systemCommand.str() << "' (exit code " << exitCode << ").";

            if (exitCode == SYSTEM_CALL_FAILURE)
            {         
                const char* errorDesc = ::strerror(errno);
                if (errorDesc != NULL)
                {
                    details << "System error '" << errorDesc << "'.";
                }
            }

            TA_THROW(OnlinePrintingAgentException(msg.str(), details.str()));
        }
    }
    catch (const OnlinePrintingAgentException& ex)
    {
        std::ostringstream msg;
        msg << ex.what() << " " << ex.getDetails();

        LOG_EXCEPTION_DETAILS(SourceInfo, "OnlinePrintingAgentException", msg.str().c_str());

        // Raise an alarm to alert the operators there's a problem with the printer(s).
        m_alarmSender.sendPrintFailureAlarm(printableData.getPrinter(), ex.getDetails());
    }
    catch (const std::exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, typeid(ex).name(), ex.what());
        result = false;
    }
    catch (...)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unknown exception.", "");
        result = false;
    }   

    // Clean up before returning.
    ::unlink(tempFileName);

    FUNCTION_EXIT;
    return result;
}

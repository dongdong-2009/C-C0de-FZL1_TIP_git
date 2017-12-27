/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/MessageBox.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Utility class used to display exception messages using the 
  * standard Windows message box.
  *
  */

#include "StdAfx.h"
#include "app/security/region_manager/src/RegionManager.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 azenitha

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


void MessageBox::error(const std::string& message, const TA_Base_Core::TransactiveException& exception)
{
    error(formatMessage(message, exception.what(), ""));
}


void MessageBox::error(const std::string& message, const TA_Base_Bus::DutyMonitorFrameworkException& exception)
{
    error(formatMessage(message, exception.what(), exception.getDetails()));
}


void MessageBox::error(const std::string& message)
{
    display(message, MB_ICONERROR);
}


void MessageBox::warning(const std::string& message)
{
    display(message, MB_ICONWARNING);
}


void MessageBox::info(const std::string& message)
{
    display(message, MB_ICONINFORMATION);
}


std::string MessageBox::formatMessage(const std::string& message, const std::string& exceptionMessage, const std::string& exceptionDetail)
{
    std::string formatted = message;

    if (!exceptionMessage.empty()) 
    {
        formatted += "\n\n" + exceptionMessage;

        if (!exceptionDetail.empty())
        {
            formatted += "\n" + exceptionDetail;
        }
    }

    return formatted;  
}


void MessageBox::display(const std::string& message, int iconId)
{
	// TD14164 azenitha++
	TA_Base_Bus::TransActiveMessage userMsg;
	CString description = message.c_str();
	userMsg << description;
	CString errMsg = userMsg.constructMessage(IDS_UI_050063);
    /*AfxMessageBox(_T(message.c_str()), iconId | MB_OK);*/
	AfxMessageBox(errMsg, iconId | MB_OK);
	// TD14164 ++azenitha
}

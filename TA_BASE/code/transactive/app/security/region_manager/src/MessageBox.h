#if !defined(AFX_MESSAGEBOX_H__7D8CB41E_AC7A_4780_ACBF_40FE9F4619B3__INCLUDED_)
#define AFX_MESSAGEBOX_H__7D8CB41E_AC7A_4780_ACBF_40FE9F4619B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/MessageBox.h $
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

namespace TA_Base_App
{
    class MessageBox
    {
    // Operations
    public:
        static void error(const std::string& message, const TA_Base_Core::TransactiveException& exception);
        static void error(const std::string& message, const TA_Base_Bus::DutyMonitorFrameworkException& exception);
        static void error(const std::string& message);
        static void warning(const std::string& message);
        static void info(const std::string& message);

    private:
        static std::string formatMessage(const std::string& message,
                                         const std::string& exceptionMessage,
                                         const std::string& exceptionDetail);

        static void display(const std::string& message, int iconId);
    };

} // namespace TA_Base_App

#endif // !defined(AFX_MESSAGEBOX_H__7D8CB41E_AC7A_4780_ACBF_40FE9F4619B3__INCLUDED_)

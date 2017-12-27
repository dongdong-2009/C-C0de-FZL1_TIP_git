#if !defined(AFX_MESSAGEBOX_H__7DD408A3_F30E_48DF_8B0A_3399C6E64E52__INCLUDED_)
#define AFX_MESSAGEBOX_H__7DD408A3_F30E_48DF_8B0A_3399C6E64E52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/src/MessageBox.h $
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
        static void error(const std::string& message, const TA_Base_Core::TransactiveException& exception, UINT uiMessageID = 0);
        static void error(const std::string& message, const TA_Base_Bus::DutyMonitorFrameworkException& exception, UINT uiMessageID = 0);
        static void error(const std::string& message, UINT uiMessageID = 0);
        static void warning(const std::string& message, UINT uiMessageID = 0);
        static void info(const std::string& message, UINT uiMessageID = 0);

    private:
        static std::string formatMessage(const std::string& message,
                                         const std::string& exceptionMessage,
                                         const std::string& exceptionDetail);
		static TA_Base_Bus::TransActiveMessage* formatUIMessage(const std::string& exceptionMessage,
										const std::string& exceptionDetail);

        static void display(const std::string& message, int iconId);
		static void displayMessageID(TA_Base_Bus::TransActiveMessage* userMsg, UINT uiMessageID);
    };

} // namespace TA_Base_App

#endif // !defined(AFX_MESSAGEBOX_H__7DD408A3_F30E_48DF_8B0A_3399C6E64E52__INCLUDED_)

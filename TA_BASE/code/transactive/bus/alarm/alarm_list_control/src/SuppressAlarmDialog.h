/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/SuppressAlarmDialog.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 17:50:12 $
  * Last modified by:  $Author: Noel $
  *
  */
#if !defined(AFX_SUPPRESSALARMDIALOG_H__20DF57E9_C616_47EC_BA06_5AABE904C03C__INCLUDED_)
#define AFX_SUPPRESSALARMDIALOG_H__20DF57E9_C616_47EC_BA06_5AABE904C03C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SuppressAlarmDialog.h : header file
//

#include "resource.h"
#include "core\message\src\ConfigUpdateMessageSender.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef;
	class IAlarmRuleData;
}


namespace TA_Base_Bus
{

    /////////////////////////////////////////////////////////////////////////////
    // CSuppressAlarmDialog dialog

    class CSuppressAlarmDialog : public CDialog
    {
    // Construction
    public:
		CSuppressAlarmDialog(boost::shared_ptr<TA_Base_Bus::AlarmDataItem> alarm, std::string createdBySessionKey,  unsigned long createdByProfileKey, std::string createdByOperatorName, CWnd* pParent = NULL);   // standard constructor

        bool wasSuccessful();

		void inhibitAlarmByType();

		void inhibitAlarmByDescription();
	    
    // Dialog Data
	    //{{AFX_DATA(CSuppressAlarmDialog)
	    enum { IDD = IDD_SUPPRESS_ALARM };
    		    // NOTE: the ClassWizard will add data members here
	    //}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CSuppressAlarmDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(CSuppressAlarmDialog)
	    virtual void OnCancel();
	    virtual void OnOK();
	    virtual BOOL OnInitDialog();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

    private:

        void inhibitAlarm();
        void sendConfigUpdate(TA_Base_Core::IAlarmRuleData* suppressionRule);
        void setUpDialogTitle();
		void showErrorMsg(std::string reason, std::string type);

    private:

        boost::shared_ptr<TA_Base_Bus::AlarmDataItem> m_alarmDetails;
        bool m_successful;
        std::string m_createdBySessionKey;
		unsigned long m_createdByProfileKey;
		std::string m_createdByOperatorName;
		CFont m_font;
    };
}


#endif // !defined(AFX_SUPPRESSALARMDIALOG_H__20DF57E9_C616_47EC_BA06_5AABE904C03C__INCLUDED_)

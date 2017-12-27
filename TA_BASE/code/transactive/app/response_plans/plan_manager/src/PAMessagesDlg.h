/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
   PaMessages.h : header file
   to select messages for PA Station plan step
*/

#if !defined(AFX_PaMessages_H__19585574_75C8_42FB_9C76_3E170B5F690D__INCLUDED_)
#define AFX_PaMessages_H__19585574_75C8_42FB_9C76_3E170B5F690D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#include <string>

#include "PaMessageStepParameter.h"
#include "PlanStepParameterEditDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CPaMessagesDlg dialog

namespace TA_Base_App
{
    class CPaMessagesDlg : public CPlanStepParameterEditDlg
    {
        // Construction
    public:
        CPaMessagesDlg(std::string messageType, PaMessageStepParameter& parameter, bool canEdit , unsigned long ulLocationKey);
        virtual ~CPaMessagesDlg ();
        CString    GetSelectedMessage () { return m_selectedMessage; }

        // Dialog Data
        // {{AFX_DATA(CPaMessagesDlg)
        enum { IDD = IDD_PA_STATION_MESSAGE_DLG };
        CEdit            m_libraryVerTxt;
        CEdit            m_librarySecTxt;
        CEdit            m_parameterNameTxt;
        CListCtrl    m_messagesList;
        // }}AFX_DATA


        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPaMessagesDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPaMessagesDlg)
        virtual BOOL OnInitDialog();
        virtual void OnCancel();
        virtual void OnOK();
        afx_msg void OnMessageListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        void PopulateMessagesList ();
        std::string formatDvaMessageType( const unsigned short usType );

        CString m_selectedMessage;
        PaMessageStepParameter& m_PAMessage;
        std::string m_messageType;
        unsigned long m_ulLocationKey;
    };
}//namespace

#endif //defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#endif // !defined(AFX_PaMessages_H__19585574_75C8_42FB_9C76_3E170B5F690D__INCLUDED_)

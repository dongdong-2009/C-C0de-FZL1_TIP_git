/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/DirectControlDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_DIRECTCONTROLDIALOG_H__9E492B98_83B7_4205_9655_99F4DC39D41F__INCLUDED_)
#define AFX_DIRECTCONTROLDIALOG_H__9E492B98_83B7_4205_9655_99F4DC39D41F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirectControlDialog.h : header file
//

#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

namespace TA_Base_Core { class IEntityData; }
namespace TA_Base_App
{
	class CEquipment;



    /////////////////////////////////////////////////////////////////////////////
    // CDirectControlDialog dialog

    class CDirectControlDialog : public CDialog, public TA_Base_Bus::IEntityUpdateEventProcessor
    {
    // Construction
    public:
	    CDirectControlDialog(CWnd* pParent = NULL);   // standard constructor
        virtual ~CDirectControlDialog();

    // Dialog Data
	    //{{AFX_DATA(CDirectControlDialog)
	enum { IDD = IDD_CONFIRM_DIRECT_CONTROL_DIALOG };
	CButton	m_confirmButton;
	    CButton	m_test;
	    CStatic	m_newValueLabel;
	    CStatic	m_equipmentLabel;
	    CStatic	m_dataPointLabel;
	//}}AFX_DATA

        void setActionAllowed(bool isEnabled);
        void setEquipmentLabelInMainThread(const std::string& label);
        void setDataPointLabelInMainThread(const std::string& label);
        void setNewValueLabelInMainThread(const std::string& label);

        void resetDialog();

        void setDataPoint(unsigned long entityKey);
        void setEquipment(CEquipment* equipment);
        void setNewValue(const std::string& newValue);
	    void updatePermissions(bool skipAccessCheck = false);

        void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
        {}

		void updateControlState(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

        void destroyDataPoint();

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CDirectControlDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

		std::string addControlStateToStatusLine ( TA_Base_Bus::DataPointProxySmartPtr & datapoint, TA_Base_Bus::EDataPointControlState state );
		

        CEquipment*	    m_equipment;
        unsigned long   m_dataPointKey;
        std::string     m_newValue;
        bool            m_actionAllowed;

	    // Generated message map functions
	    //{{AFX_MSG(CDirectControlDialog)
	    afx_msg void OnConfirm();
        afx_msg LRESULT OnUpdateLabelInMainThread(UINT wParam, LONG lParam);

	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()
    };

}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTCONTROLDIALOG_H__9E492B98_83B7_4205_9655_99F4DC39D41F__INCLUDED_)

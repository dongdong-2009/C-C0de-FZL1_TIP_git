/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/TagDialog.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the display and update of tag settings associated with equipment.
  */

#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/scada/inspector_panel/src/resource.h"
#include "app/scada/inspector_panel/src/ITabDialog.h"

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/common_library/src/AuditMessageUtility.h"


namespace TA_Base_App
{
	class CEquipment;

	/////////////////////////////////////////////////////////////////////////////
	// CTagDialog dialog

	// wenching++ (TD12602, TD13140)
	enum InhibitType
	{
		scanInhibit				= 0,
		alarmInhibit			= 1,
		mmsInhibit				= 2,
		controlInhibit			= 3
	};
	// ++wenching (TD12602, TD13140)

	class CTagDialog : public CDialog, public ITabDialog
	{
	
	public:

	// Construction
		CTagDialog(CWnd* pParent = NULL);   // standard constructor
		
		//
		// ITabDialog Implementation
		//
		virtual void setEquipment(CEquipment* equipment);
		virtual void updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType);
		virtual void updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType);
		virtual bool isUnsavedChangeOnTab();
		virtual bool isOkToChangeTab();
		virtual void getWindowRect(CRect* rect);
		virtual unsigned int getMinimumHeight();
		virtual void resize(unsigned int height);
		virtual void updatePermissions(/*TA_Base_Bus::RightsLibrary* rightsMgr*/);
		virtual bool isAnyDataPointsDisplayedInTab() const;
		virtual void resetList();
		virtual void displayDataPoints();

	// Dialog Data
		//{{AFX_DATA(CTagDialog)
		enum { IDD = IDD_TAG };
		CButton	m_scanInhibitCheck;
		//CButton	m_mmsInhibitCheck;
		CButton	m_controlInhibitCheck;
		CButton	m_alarmInhibitCheck;
		CEdit	m_ptwIdEdit;
		CEdit	m_commentEdit;
	//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CTagDialog)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CTagDialog)
		afx_msg void onApplyTag();
		afx_msg void OnRadioTagPtw();
		afx_msg void OnRadioNotTagged();
		afx_msg void OnRadioTag();
		afx_msg void OnChangeEdit();
		afx_msg void OnRadioInhibitAlarm();
		afx_msg void OnRadioInhibitControl();
		afx_msg void OnRadioInhibitNone();
		afx_msg void OnRadioInhibitScanControl();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnAlarmInhibitCheck();
		afx_msg void OnControlInhibitCheck();
		afx_msg void OnMmsInhibitCheck();
		afx_msg void OnScanInhibitCheck();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		void DisplayEquipmentTagStatus();
		bool applyTag();
		void updateRadioButtonAccess();
		void updatePtwCommentAccess();
		void checkPtwTextCommentField(); //wenching (TD13741)
		
	protected:

		CEquipment*					m_equipment;
		TA_Base_Bus::ETagState		m_originalTagStatus;
		bool						m_originalScanInhibitChecked;
		bool						m_originalAlarmInhibitChecked;
		//bool						m_originalMmsInhibitChecked;
		bool						m_originalControlInhibitChecked;
		std::string					m_originalPtwId;
		std::string					m_originalComment;

		bool						m_scanInhibitChecked;
		bool						m_alarmInhibitChecked;
		//bool						m_mmsInhibitChecked;
		bool						m_controlInhibitChecked;

        // All controls not related to PTW on this page are controlled
        // by the tag access allowed access controlled action
		bool						m_tagAccessAllowed;
        // PTW specific actions must *additionally* have this privilege
        // (to enable / allow the relevant PTW controls)
        bool                        m_ptwTagAccessAllowed;
		unsigned int				m_originalHeight;

    private:

        /**
         * @return true if operator has required priveleges to modify
         *  (and apply changes to) the PTW sections of page
         */
        bool isPtwTagAccessAllowed();

		// wenching++ (TD12602, TD13140)
		/**
         * @return true if operator has required priveleges to inhibit
         *  or uninhibit
         */
		bool isProfileAccessAllowed(bool buttonChecked, InhibitType type);
		// ++wenching (TD12602, TD13140)

	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // ifdef TAGDIALOG_H

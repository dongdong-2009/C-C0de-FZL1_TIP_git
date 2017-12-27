/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/InhibitDialog.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the dialog which displays the Inhibit tab shown
  * by the inspector panel.
  */

#ifndef INHIBITDIALOG_H
#define INHIBITDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/scada/inspector_panel/src/InhibitListCtrl.h"
#include "app/scada/inspector_panel/src/resource.h"
#include "app/scada/inspector_panel/src/ITabDialog.h"
#include "app/scada/inspector_panel/src/Equipment.h"


namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CInhibitDialog dialog

	class CInhibitDialog : public CDialog, public ITabDialog
	{
	
	public:

	// Construction
		CInhibitDialog(CWnd* pParent = NULL);   // standard constructor

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
		//{{AFX_DATA(CInhibitDialog)
		enum { IDD = IDD_INHIBIT };
			// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CInhibitDialog)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CInhibitDialog)
		virtual BOOL OnInitDialog();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		private:

			CInhibitListCtrl	m_list;
			unsigned int			m_heightDifference;
			unsigned int			m_originalHeight;
			CEquipment*			m_equipment;
//			bool					m_accessAllowed;
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // #ifndef INHIBITDIALOG_H

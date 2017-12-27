/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_Review_Branch/TA_BASE/transactive/app/scada/inspector_panel/src/ControlListCtrl.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/10/21 12:46:03 $
  * Last modified by:  $Author: wanzl $
  * 
  * This class manages the list display used to show the basic control information in
  * the CControlDialog class.
  *
  */

#ifndef EO_CONTROLLISTCTRL_H
#define EO_CONTROLLISTCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "bus/mfc_extensions/src/editable_list_control/EditableListCtrl.h"
#include "app/scada/inspector_panel/src/DataPointListCtrl.h"
#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_Bus
{
	class DataPointProxySmartPtr;
}

namespace TA_Base_App
{
	class CEquipment;

	/////////////////////////////////////////////////////////////////////////////
	// CDataPointListCtrl window

	//TD15649, simplified this class 
	//by just responsing to the control state events of the visiable output datapoints
	//instead of using triggering entity of all datapoints to refresh the control state of visiable output datapoints
	
	class CDropDownControlListCtrl : public CAbstractDataPointListCtrl, ICellManager
	{
		// Construction
		public:
			CDropDownControlListCtrl();
			virtual ~CDropDownControlListCtrl();
		// Attributes
		public:

		// Operations
		public:
			

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CDropDownControlListCtrl)
			protected:
			virtual void PreSubclassWindow();
			//}}AFX_VIRTUAL

		// Implementation
		public:
			// ICellManager methods
			virtual CellType GetCellType(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual unsigned long GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem);
			virtual CellData GetCellData(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual CString GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual char GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem);
			virtual bool UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value);
			
			//CAbstractDataPointListCtrl implement
			virtual void setEquipment(CEquipment* equipment);
			virtual void updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType);
			virtual void updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType);
			virtual void updatePermissions(bool skipAccessCheck = false);
			virtual bool isAnyDataPointsDisplayedOnScreen() const;
			virtual void displayDataPoints();
			
			
			
			//TD15438, override the function to get a chance to adjust columns width.
			virtual BOOL SetWindowPos( const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags );
			
			// Generated message map functions
		protected:
			//{{AFX_MSG(CDropDownControlListCtrl)
			afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
			//}}AFX_MSG
        	afx_msg void onCustomDrawList ( NMHDR* pNMHDR, LRESULT* pResult );
			DECLARE_MESSAGE_MAP()

			virtual void setupList();
			virtual void updateDataPointDisplay ( int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType );
			std::string addControlStateToStatusLine ( TA_Base_Bus::DataPointProxySmartPtr & datapoint, TA_Base_Bus::EDataPointControlState state );
			std::string getControlStatusString ( TA_Base_Bus::EDataPointControlState state );
			void UpdateControlState();
			void RefreshPermission(unsigned int iPoint);

		private:

			volatile bool					m_dataPointsDisplayedOnScreen;
			volatile bool					m_initialState; //wenching++ (TD12608)
			
			CEquipment*						m_equipment;
			TA_Base_Core::ReEntrantThreadLockable	m_valueActionLock;
			std::map<unsigned int, bool>	m_setValueActionAllowed;

            static const COLORREF       COLOUR_ALARM;  // colour to be used for showing alarms
            static const COLORREF       COLOUR_DISABLE; // colour to be used for showing disabled data points
            static const COLORREF       COLOUR_NORMAL; // colour to be used for showing normal data points
	};
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // ifndef CONTROLLISTCTRL_H

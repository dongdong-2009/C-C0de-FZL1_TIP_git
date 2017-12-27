/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ControlListCtrl.h $
  * @author:  Andy Parker
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/11/18 14:31:43 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class manages the list display used to show the basic control information in
  * the CControlDialog class.
  *
  */

#ifndef CONTROLLISTCTRL_H
#define CONTROLLISTCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/scada/common_library/src/CommonDefs.h"
#include "app/scada/inspector_panel/src/ControlListCellButton.h"
#include "app/scada/inspector_panel/src/IButtonProcessor.h"
#include "app/scada/inspector_panel/src/DataPointListCtrl.h"
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
	
	class CButtonControlListCtrl : public CAbstractDataPointListCtrl, IButtonProcessor
	{
		
		public:
			// Construction
			CButtonControlListCtrl();
			//DeConstruction
			virtual ~CButtonControlListCtrl();

		// Attributes
		public:
			static const COLORREF       COLOUR_ALARM;  // colour to be used for showing alarms
			static const COLORREF       COLOUR_DISABLE; // colour to be used for showing disabled data points
			static const COLORREF       COLOUR_NORMAL; // colour to be used for showing normal data points
		
		public:
			void DestroyButton(int item);
			void EnableButton(int item,bool bShow);
			void setButtonText(int item,CString strValue,CString strCaption);
			bool getControlPointValueWithRelatedDataPointKey(unsigned long key,CString& strValue);
			bool getControlPointValueWithPosition(UINT iPoint,CString& strValue);
			CControlListCellButton* getButton(int item);

		public:
			//Interface IButtonProcessor
			virtual void processButtonClick(int item, CString stringValue);
		
			//Inferface AbstractDataPointListCtrl
			virtual void setEquipment(CEquipment* equipment);
			virtual void updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType);
			virtual void updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType);
			virtual void updatePermissions(bool skipAccessCheck = false);
			virtual bool isAnyDataPointsDisplayedOnScreen() const;
			virtual void displayDataPoints();
			
		public:
			//TD15438, override the function to get a chance to adjust columns width.
			virtual BOOL SetWindowPos( const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags );
			
			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CButtonControlListCtrl)
		protected:
			virtual void PreSubclassWindow();
			//}}AFX_VIRTUAL
			// Generated message map functions
		protected:
			//{{AFX_MSG(CButtonControlListCtrl)
			afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
			//}}AFX_MSG
        	afx_msg void onCustomDrawList ( NMHDR* pNMHDR, LRESULT* pResult );
			afx_msg BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

			DECLARE_MESSAGE_MAP()

			virtual void setupList();
			virtual void updateDataPointDisplay ( int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType );			
			void OnDrawCellButtons();
			
			std::string addControlStateToStatusLine ( TA_Base_Bus::DataPointProxySmartPtr & datapoint, TA_Base_Bus::EDataPointControlState state );
			std::string getControlStatusString ( TA_Base_Bus::EDataPointControlState state );
			void UpdateControlState();

			void RefreshPermission(unsigned int iPoint);
			void RefreshButtonDisplay(unsigned int iPoint,int nItem);

			void showStatusMessage(const std::string& strMessage);
		private:

			volatile bool					m_dataPointsDisplayedOnScreen;
			volatile bool					m_initialState; //wenching++ (TD12608)
			int								m_CurrentOperateDataPoint; //Using for IEC104 Protocol control
			
			TA_Base_Core::ReEntrantThreadLockable	m_valueActionLock;
			std::map<unsigned int, bool>			m_setValueActionAllowed;
			std::map<unsigned long,CControlListCellButton*> m_buttonMap;
			

	};
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // ifndef CONTROLLISTCTRL_H

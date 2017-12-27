/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/LimitDetailsListCtrl.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the basic control information in
  * the CControlDialog class.
  *
  */


#ifndef LIMITDETAILSLISTCTRL_H
#define LIMITDETAILSLISTCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


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

	class CLimitDetailsListCtrl : public CAbstractDataPointListCtrl, /* CEditableListCtrl, */ ICellManager
	{
		// Construction
		public:
			CLimitDetailsListCtrl();

		// Attributes
		public:

		// Operations
		public:
			void setEquipment(CEquipment* equipment);
			void setDataPoint(int iPoint);

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CLimitDetailsListCtrl)
			protected:
			virtual void PreSubclassWindow();
			//}}AFX_VIRTUAL

		// Implementation
		public:
			virtual ~CLimitDetailsListCtrl();

			virtual bool isAnyDataPointsDisplayedOnScreen() const;

			// ICellManager methods
			virtual CellType GetCellType(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual unsigned long GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem);
			virtual CellData GetCellData(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual CString GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual char GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem);
			virtual bool UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value);
			
			void updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType);
			void updatePermissions(bool allowed);
			void clearList();

			// Generated message map functions
		protected:
			//{{AFX_MSG(CLimitDetailsListCtrl)
			afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()

			bool overrideAlarmConditionConfirm ( const std::string & typeOfChange, TA_Base_Bus::DataPointProxySmartPtr & dataPoint, bool enable );
			void displayLimitDetails();
			void refreshLimitDetails();
			void displayItem(int nItem);
			bool limitDetailChangeConfirm(const int alarmThreshold, TA_Base_Bus::DataPointProxySmartPtr& dataPoint, bool enable);
			bool isCellEnabled (int nItem, int nSubItem);

			virtual void setupList();
			virtual void displayDataPoints();
			virtual void updateDataPointDisplay(int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType);
            
            std::string alarmTypeToString(TA_Base_Bus::EDataPointAlarms alarmType);
                
		private:
			int							m_iPoint;
			bool						m_accessAllowed;
			volatile bool				m_dataPointsDisplayedOnScreen;
	};
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(LIMITDETAILSLISTCTRL_H)

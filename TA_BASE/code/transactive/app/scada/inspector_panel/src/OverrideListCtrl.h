/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/OverrideListCtrl.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the basic information in
  * the CInformationDialog class.
  *
  */

#ifndef OVERRIDELISTCTRL_H
#define OVERRIDELISTCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/scada/inspector_panel/src/DataPointListCtrl.h"
#include "bus/scada/common_library/src/AuditMessageUtility.h"

namespace TA_Base_Bus
{
	class DataPointProxySmartPtr;
}

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// COverrideListCtrl window

	class COverrideListCtrl : public CAbstractDataPointListCtrl, ICellManager
	{
		// Construction
		public:
			COverrideListCtrl();
			void refreshDataPoints();

		// Attributes
		public:

		// Operations
		public:


		// Implementation
		public:
			virtual ~COverrideListCtrl();

			virtual bool isAnyDataPointsDisplayedOnScreen() const;

			// ICellManager methods
			virtual CellType GetCellType(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual unsigned long GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem);
			virtual CellData GetCellData(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual CString GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual char GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem);
			virtual bool UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value);

			//TD15438, override the function to get a chance to adjust columns width.
			virtual BOOL SetWindowPos( const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags );
			void displayDataPoints();
		protected:

            enum eForcedRefreshType
            {
                REFRESH_DEFAULT                         = 0x00,
                REFRESH_DESCRIPTION                     = 0x01,
                REFRESH_TYPE                            = 0x02,
                REFRESH_OVERRIDE                        = 0x04,
                REFRESH_OVERRIDE_VALUE                  = 0x08,
                REFRESH_ALL                             = 0xFFFFFFFF
            };

			bool inhibitOverrideConfirm(const int inhibitOverrideItem, TA_Base_Bus::DataPointProxySmartPtr& dataPoint, bool enable);
			void displayItem(int nItem, DWORD refreshType = REFRESH_DEFAULT);
			bool isCellEnabled (int nItem, int nSubItem);
			//
			// CAbstractDataPointListCtrl methods
			//
			void setupList();
			
			void updateDataPointDisplay(int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType);

		protected:

			volatile bool m_dataPointsDisplayedOnScreen;
	};

	/////////////////////////////////////////////////////////////////////////////

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // #ifndef OVERRIDELISTCTRL_H

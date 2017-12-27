#if !defined(AFX_INFORMATIONLISTCTRL_H__8C394273_7B6C_11D7_B175_0050BAB1D931__INCLUDED_)
#define AFX_INFORMATIONLISTCTRL_H__8C394273_7B6C_11D7_B175_0050BAB1D931__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/InformationListCtrl.h $
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


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/scada/Inspector_Panel/src/DataPointListCtrl.h"

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CInformationListCtrl window

	class CInformationListCtrl : public CAbstractDataPointListCtrl, ICellManager
	{
		// Construction
		public:
			CInformationListCtrl();

		// Attributes
		public:

		// Operations

		// Implementation
		public:
			virtual ~CInformationListCtrl();

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
			void resetList();
			void displayDataPoints();

			// Generated message map functions
		protected:

			DECLARE_MESSAGE_MAP()

			//
			// CAbstractDataPointListCtrl methods
			//

			void setupList();			
			void updateDataPointDisplay(int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType);			

		private:

			volatile bool m_dataPointsDisplayedOnScreen;
			bool m_isReset;
			// wenching++ (TD12997)
			int QualityColumn;
			int TimestampColumn;
			// ++wenching (TD12997)
	};

	/////////////////////////////////////////////////////////////////////////////

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_DATAPOINTLISTCTRL_H__8C394273_7B6C_11D7_B175_0050BAB1D931__INCLUDED_)

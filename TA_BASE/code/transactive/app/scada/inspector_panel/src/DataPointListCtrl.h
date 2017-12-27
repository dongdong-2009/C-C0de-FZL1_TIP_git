#if !defined(AFX_DATAPOINTLISTCTRL_H__8C394273_7B6C_11D7_B175_0050BAB1D931__INCLUDED_)
#define AFX_DATAPOINTLISTCTRL_H__8C394273_7B6C_11D7_B175_0050BAB1D931__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/DataPointListCtrl.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/05 10:04:52 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class manages the list display used to show the basic information in
  * the CInformationDialog class.
  *
  */


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "bus/mfc_extensions/src/editable_list_control/editableListCtrl.h"
#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_Bus
{
    class DataPointProxySmartPtr;
}

namespace TA_Base_App
{
	class CEquipment;


	/////////////////////////////////////////////////////////////////////////////
	// CAbstractDataPointListCtrl window

	class CAbstractDataPointListCtrl : public CEditableListCtrl
	{
		// Construction
		public:
			CAbstractDataPointListCtrl();

		// Attributes
		public:

			// This extended style enables "tool tips" for partially hidden text
			static const int LVS_EX_LABELTIP_XXX;

		// Operations
		public:
			virtual void setEquipment(CEquipment* equipment);
			virtual void updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType);
			virtual void updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType);
			virtual void updatePermissions(bool allowed);
			virtual bool isAnyDataPointsDisplayedOnScreen() const;
			virtual void displayDataPoints() = 0;
		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CAbstractDataPointListCtrl)
			protected:
			virtual void PreSubclassWindow();
			//}}AFX_VIRTUAL

		// Implementation
		public:
			virtual ~CAbstractDataPointListCtrl();

			// Generated message map functions
		protected:
			//{{AFX_MSG(CAbstractDataPointListCtrl)
			afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
			//}}AFX_MSG
            afx_msg void onDividerDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);

			DECLARE_MESSAGE_MAP()

			virtual void setupList() = 0;
			
			virtual void updateDataPointDisplay(int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType) = 0;
			
			int getDataPointStatusImageIndex(UINT iPoint);
			int insertItemTextImage(int iPoint, int insertNItem, std::string& value, bool addImage);
			void setItemTextImage(int nItem, int nSubItem, std::string value, bool addImage);
			void setItemTextYesNo(int nItem, int nSubItem, bool yes);

            /**
             * Helper function to retrieve the string representation of the 'Current Value' of the 
             *  input datapoint 
             * - for use displaying the 'Override Value' and 'Current Value' column text
             */
            static std::string getCurrentValueAsString(TA_Base_Bus::DataPointProxySmartPtr& dataPoint);

		protected:

			CEquipment*					m_equipment;
			bool						m_accessAllowed;

		private:

			HICON						m_greenIcon;
			int							m_greenIconIndex;
			HICON						m_redIcon;
			int							m_redIconIndex;
			HICON						m_orangeIcon;
			int							m_orangeIconIndex;
			HICON						m_blueIcon;
			int							m_blueIconIndex;
			HICON						m_greyIcon;
			int							m_greyIconIndex;

	};

	/////////////////////////////////////////////////////////////////////////////

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_DATAPOINTLISTCTRL_H__8C394273_7B6C_11D7_B175_0050BAB1D931__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/MainTabControl.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/05 10:04:52 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class handles the tab control displayed on the main dialog screen.  It manages the 
  * three associated dialogs CInformationDialog, CControlDialog and CTagDialog.
  */

#ifndef MAINTABCONTROL_H
#define MAINTABCONTROL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

#include "bus/mfc_extensions/src/tab_dialog_control/TabDialogCtrl.h"

#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_Core
{
	class RightsLibrary;
}

namespace TA_Base_App
{
	class CEquipment;
	class ITabDialog;

	/////////////////////////////////////////////////////////////////////////////
	// CMainTabControl window

	class CMainTabControl : public CTabDialogCtrl
	{
	// Construction
	public:
		CMainTabControl();

	// Attributes
	public:

	// Operations
	public:

		// Tab numbers for the required pages
		// Note:  This will need to be updated if the pages are loaded in a different order
		enum ETabNumbers
		{
			InformationTab	= 0,
			ControlTab		= 1,
			NotesTab		= 2,
			TagTab			= 3,
			InhibitTab		= 4,
			OverrideTab		= 5,
			LimitsTab		= 6,
			MeterTab		= 7,
			StatusTab		= 8,
			NoTab			= 9
		};
		
		void setEquipment(CEquipment* equipment);
        void setStatusTabEquipment(CEquipment* equipment);
		void updateDataPoints(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType);
		void updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType);
		bool isUnsavedChangeOnTab(ETabNumbers tab);
		bool isOkToChangeTab(ETabNumbers tab);
		bool isAnyDataPointsDisplayedInTab(ETabNumbers tab);
		int resize();
		unsigned int getMinimumDialogHeight();
		void updatePermissions();
		void updateControlTab();
		void resetList();
		void SetCurSelTab( int nItem );
		void diplayDataPointsOfBgPages();
		
		ETabNumbers getMinTabNumber();
	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMainTabControl)
		//}}AFX_VIRTUAL
	private:
		void clearAllTabDialog();
		void initilizeTabDialog();
	// Implementation
	public:
		virtual ~CMainTabControl();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CMainTabControl)
		virtual void PreSubclassWindow();
		//}}AFX_MSG
		std::map<ETabNumbers, ITabDialog*>	m_tabDialogs;
		
		ETabNumbers							m_minTab;

		CEquipment*							m_equipment;

		unsigned int						m_heightDifference;

		ITabDialog*							m_BackupDialog;
		// Rights Library Manager to get permissions for actions within this application
		//TA_Base_Bus::RightsLibrary*				m_rightsMgr;

		DECLARE_MESSAGE_MAP()
	};

	/////////////////////////////////////////////////////////////////////////////

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}

#endif // #ifndef MAINTABCONTROL_H

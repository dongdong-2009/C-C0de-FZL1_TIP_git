/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/MainTabControl.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/05 10:04:52 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class handles the tab control displayed on the main dialog screen.  It manages the 
  * three associated dialogs CInformationDialog, CControlDialog and CTagDialog.
  */

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InformationDialog.h"
#include "app/scada/inspector_panel/src/ControlDialog.h"
#include "app/scada/inspector_panel/src/LimitDialog.h"
#include "app/scada/inspector_panel/src/NotesDialog.h"
#include "app/scada/inspector_panel/src/TagDialog.h"
#include "app/scada/inspector_panel/src/InhibitDialog.h"
#include "app/scada/inspector_panel/src/OverrideDialog.h"
#include "app/scada/inspector_panel/src/MeterDialog.h"
#include "app/scada/inspector_panel/src/StatusDialog.h"

#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/MainTabControl.h"

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/application_types/src/apptypes.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CMainTabControl

	CMainTabControl::CMainTabControl() : m_equipment (NULL), m_heightDifference (0)//, m_rightsMgr (NULL)
	{
		FUNCTION_ENTRY ("CMainTabControl()");
		this->initilizeTabDialog();
		FUNCTION_EXIT;
	}
	
	CMainTabControl::~CMainTabControl()
	{
		FUNCTION_ENTRY ("~CMainTabControl()");

		/*std::map<ETabNumbers, ITabDialog*>::iterator it;

		for (it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
		{
			if ((*it).second != NULL)
			{
				delete (*it).second;
				(*it).second = NULL;
			}
		}

		if ( NULL != m_BackupDialog )
		{
			delete m_BackupDialog;
			m_BackupDialog = NULL;
		}

		m_tabDialogs.clear();*/

		FUNCTION_EXIT;
	}

	void CMainTabControl::initilizeTabDialog()
	{
		m_tabDialogs.clear();
		CInformationDialog* informationDialog = new CInformationDialog;
		m_tabDialogs[InformationTab] = informationDialog;
		AddPage(*informationDialog,IDD_INFORMATION, CString("信息"));

		CControlDialog* controlDialog = new CControlDialog;
		m_tabDialogs[ControlTab] = controlDialog;
		AddPage(*controlDialog,IDD_CONTROL,CString("控制"));

		CNotesDialog* notesDialog = new CNotesDialog;
		m_tabDialogs[NotesTab] = notesDialog;
		AddPage(*notesDialog,IDD_NOTES,CString("笔记"));

		CTagDialog* tagDialog = new CTagDialog;
		m_tabDialogs[TagTab] = tagDialog;
		AddPage(*tagDialog,IDD_TAG,CString("标签"));

		CInhibitDialog* inhibitDialog = new CInhibitDialog;
		m_tabDialogs[InhibitTab] = inhibitDialog;
		AddPage(*inhibitDialog,IDD_INHIBIT,CString("禁止"));

		COverrideDialog* overrideDialog = new COverrideDialog;
		m_tabDialogs[OverrideTab] = overrideDialog;
		AddPage(*overrideDialog,IDD_OVERRIDE,CString("超驰"));
		
		CLimitDialog* limitDialog = new CLimitDialog;
		m_tabDialogs[LimitsTab] = limitDialog;
		AddPage(*limitDialog,IDD_LIMITS,CString("限值"));

		CMeterDialog* meterDialog = new CMeterDialog;
		m_tabDialogs[MeterTab] = meterDialog;
		AddPage(*meterDialog,IDD_METER,CString("计量"));

		if ( TA_Base_Core::RunParams::getInstance().isSet("StatusTab") )
		{
			CStatusDialog* statusDialog = new CStatusDialog;
			m_tabDialogs[StatusTab] = statusDialog;
			AddPage(*statusDialog,IDD_STATUS,CString("状态"));
		}
		m_minTab = InformationTab;
	}

	void CMainTabControl::clearAllTabDialog()
	{
		std::map<ETabNumbers, ITabDialog*>::iterator it;
		for (it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
		{
			if ((*it).second != NULL)
			{
				delete (*it).second;
				(*it).second = NULL;
			}
		}
		m_tabDialogs.clear();
	}
	void CMainTabControl::setEquipment(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("setEquipment()");
		if(NULL == equipment)
		{
			this->resetAllPages();
			m_equipment =NULL;
			
			std::map<ETabNumbers, ITabDialog*>::iterator it;
			for (it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
			{
				if ((*it).second != NULL)
				{
					(*it).second->setEquipment(equipment);
				}
			}
			return;
		}
		else if(NULL == m_equipment )//&& equipment !=NULL
		{
			if (!equipment->hasInformationPoint())
			{
				DelPage(IDD_INFORMATION);
				DelPage(IDD_OVERRIDE);
			}
			if (!equipment->hasControlPoint())
			{
				DelPage(IDD_CONTROL);
			}
			if (!equipment->hasInformationPoint() && !equipment->hasControlPoint())
			{
				DelPage(IDD_INHIBIT);
			}
			if (!equipment->hasLimitedPoint())
			{
				DelPage(IDD_LIMITS);
			}
			if (!equipment->hasMeteredPoint())
			{
				DelPage(IDD_METER);
			}
			if( m_tabDialogs.size() > 0 )
			{
				m_minTab = m_tabDialogs.begin()->first;
			}

			std::map<ETabNumbers, ITabDialog*>::iterator it;
			for (it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
			{
				if ((*it).second != NULL)
				{
					(*it).second->setEquipment(equipment);
				}
			}

			//displayDataPoints of the selected page
			ITabDialog* activeDlg = dynamic_cast<ITabDialog*>(getActiveDialog());
			for (it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
			{
				if ((*it).second != NULL && (*it).second ==activeDlg )
				{
					(*it).second->displayDataPoints();
					break;
				}
			}
			m_equipment = equipment;
		}
		FUNCTION_EXIT;
	}
	void  CMainTabControl::diplayDataPointsOfBgPages()
	{
		std::map<ETabNumbers, ITabDialog*>::iterator it;

		ITabDialog* activeDlg = dynamic_cast<ITabDialog*>(getActiveDialog());
		for (it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
		{
			if ((*it).second != NULL && (*it).second !=activeDlg )
			{
				(*it).second->displayDataPoints();
				break;
			}
		}
	}
	void CMainTabControl::updateControlTab()
	{
		FUNCTION_ENTRY ("updateControlTab()");
		m_tabDialogs[ControlTab]->setEquipment( m_equipment );
		FUNCTION_EXIT;
	}

    void CMainTabControl::setStatusTabEquipment(CEquipment* equipment)
    {
		FUNCTION_ENTRY ("setStatusTabEquipment()");

		std::map<ETabNumbers, ITabDialog*>::iterator it = m_tabDialogs.find(StatusTab);

        if (it != m_tabDialogs.end())
        {
            it->second->setEquipment(equipment);
		}

		FUNCTION_EXIT;
    }


	void CMainTabControl::updateDataPoints(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPoints()");

		std::map<ETabNumbers, ITabDialog*>::iterator it;

		for (it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
		{
			if ((*it).second != NULL)
			{
				(*it).second->updateDataPoint(key, updateType);
			}
		}

		FUNCTION_EXIT;
	}

	void CMainTabControl::updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataNode()");

		std::map<ETabNumbers, ITabDialog*>::iterator it;

		for (it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
		{
			if ((*it).second != NULL)
			{
				(*it).second->updateDataNode(updateType);
			}
		}

		FUNCTION_EXIT;
	}

	bool CMainTabControl::isUnsavedChangeOnTab(ETabNumbers tab)
	{
		FUNCTION_ENTRY ("isUnsavedChangeOnTab()");

		bool unsavedChange = false;

		std::map<ETabNumbers, ITabDialog*>::iterator it = m_tabDialogs.find(tab);

		if (it != m_tabDialogs.end())
		{
			unsavedChange = (*it).second->isUnsavedChangeOnTab();
		}

		FUNCTION_EXIT;

		return unsavedChange;
	}

	bool CMainTabControl::isOkToChangeTab(ETabNumbers tab)
	{
		FUNCTION_ENTRY ("isOkToChangeTab()");

		bool okToChange = true;

		std::map<ETabNumbers, ITabDialog*>::iterator it = m_tabDialogs.find(tab);

		if (it != m_tabDialogs.end())
		{
			okToChange = (*it).second->isOkToChangeTab();
		}
		
		FUNCTION_EXIT;

		return okToChange;
	}

	bool CMainTabControl::isAnyDataPointsDisplayedInTab(ETabNumbers tab)
	{
		FUNCTION_ENTRY ("isAnyDataPointsDisplayedInTab()");

		bool result = true;	// asssuming that there are some dp displayed on screen

		// find if we have the specified tab
		std::map<ETabNumbers, ITabDialog*>::iterator it = m_tabDialogs.find(tab);

		// if found one
		if (it != m_tabDialogs.end())
		{
			// check if any data point is displayed
			result = (*it).second->isAnyDataPointsDisplayedInTab();
		}
		// else do nothing

		return result;

		FUNCTION_EXIT;
	}

	int CMainTabControl::resize()
	{
		CRect originalTabRect;
		int yOffset = 0;
		
		GetWindowRect(&originalTabRect);
		originalTabRect.NormalizeRect();

		if (m_heightDifference == 0)
		{
			CRect dialogRect;

			std::map<ETabNumbers, ITabDialog*>::iterator it = m_tabDialogs.begin();

			if (it != m_tabDialogs.end() && (*it).second != NULL)
			{
				(*it).second->getWindowRect(&dialogRect);
				dialogRect.NormalizeRect();

				m_heightDifference = originalTabRect.Height() - dialogRect.Height();
			}
		}

		{
			unsigned int dialogHeight = getMinimumDialogHeight();

			std::map<ETabNumbers, ITabDialog*>::iterator it;
			
			for (it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
			{
				if ((*it).second != NULL)
				{
					(*it).second->resize(dialogHeight);
				}
			}

			SetWindowPos(NULL, 0, 0, originalTabRect.Width(), dialogHeight+m_heightDifference, SWP_NOMOVE|SWP_NOZORDER);
		}

		{
			CRect newTabRect;
			GetWindowRect(&newTabRect);
			newTabRect.NormalizeRect();

			yOffset = newTabRect.Height() - originalTabRect.Height();
		}

		return yOffset;
	}

	unsigned int CMainTabControl::getMinimumDialogHeight()
	{
		unsigned int minimumHeight = 0;
		unsigned int height = 0;

		std::map<ETabNumbers, ITabDialog*>::iterator it;

		for (it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
		{
			if ((*it).second != NULL)
			{
				height = (*it).second->getMinimumHeight();
				if (height > minimumHeight)
				{
					minimumHeight = height;
				}
			}
		}

		return minimumHeight;
	}

	void CMainTabControl::updatePermissions()
	{
		std::map<ETabNumbers, ITabDialog*>::iterator it;

		for (it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
		{           
			if ((*it).second != NULL)
			{
				(*it).second->updatePermissions(/*m_rightsMgr*/);
			}
		}
	}


	void CMainTabControl::PreSubclassWindow()
	{
		CTabDialogCtrl::PreSubclassWindow();

		for (std::map<ETabNumbers, ITabDialog*>::iterator it = m_tabDialogs.begin(); it != m_tabDialogs.end(); it++)
		{
            // TD#1601 - include tab pages in tab sequencing
            CDialog* pDialog = dynamic_cast<CDialog*>((*it).second);
            if (0 != pDialog)
            {
                pDialog->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
            }
		}
	}


	void CMainTabControl::resetList()
	{
		m_tabDialogs[InformationTab]->resetList();
	}

	void CMainTabControl::SetCurSelTab( int nItem )
	{
		switch ( nItem )
		{
		case InformationTab :
			SetCurTab( IDD_INFORMATION );
			break;
		case ControlTab :
			SetCurTab( IDD_CONTROL );
			break;
		case NotesTab :
			SetCurTab( IDD_NOTES );
			break;
		case TagTab :
			SetCurTab( IDD_TAG );
			break;
		case InhibitTab :
			SetCurTab( IDD_INHIBIT );
			break;
		case OverrideTab :
			SetCurTab( IDD_OVERRIDE );
			break;
		case LimitsTab :
			SetCurTab( IDD_LIMITS );
			break;
		case MeterTab :
			SetCurTab( IDD_METER );
			break;
		case StatusTab :
			SetCurTab( IDD_STATUS );
			break;
		default:
			break;
		}
	}
	CMainTabControl::ETabNumbers CMainTabControl::getMinTabNumber()
	{
		return m_minTab;
	}

	BEGIN_MESSAGE_MAP(CMainTabControl, CTabDialogCtrl)
		//{{AFX_MSG_MAP(CMainTabControl)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
}

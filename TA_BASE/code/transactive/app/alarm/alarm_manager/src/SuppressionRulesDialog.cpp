/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_manager/src/SuppressionRulesDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/25 17:25:02 $
  * Last modified by:  $Author: lu.xiangmei $
  *
  */
// SuppressionRulesDialog.cpp : implementation file
//

#include <algorithm>

#include "stdafx.h"
#include "alarmsgui.h"
#include "SuppressionRulesDialog.h"
#include "AlarmsGuiDlg.h"
#include "app/alarm/alarm_manager/src/ExportAlarms.h"
#include "app/alarm/alarm_manager/src/Globals.h"

#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/IAlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleData.h"
#include "core/data_access_interface/src/IRegion.h"
#include "core/data_access_interface/src/RegionAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core\data_access_interface\src\ILocation.h"
#include "core\data_access_interface\src\LocationAccessFactory.h"
#include "core\data_access_interface\src\Operator.h"
#include "core\data_access_interface\src\OperatorAccessFactory.h"
#include "core\data_access_interface\entity_access\src\Console.h"
#include "core\data_access_interface\entity_access\src\ConsoleAccessFactory.h"
#include "core\data_access_interface\entity_access\src\AlarmGUI.h"

#include "core/corba/src/CorbaUtil.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/ObjectResolutionException.h"

#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "bus/alarm/alarm_list_control/src/RightsChecker.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#include "bus/resources/resource.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 azenitha
#include "bus/mfc_extensions/src/print_list_ctrl/ListCtrlPrint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace // unnamed namespace
{
	// Sort column 1 REMOVABLE
    bool sortRemovableAscending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {   
        return ( rule1->getRemovable() < rule2->getRemovable() );
    }

    bool sortRemovableDescending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {
        return ( rule1->getRemovable() >= rule2->getRemovable() );
    }

	//Sort column 2 DATE TIME
    bool sortDateTimeAscending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {
		if (rule1->getDateModified() == 0 || rule2->getDateModified() == 0)
		{
			// compare created date
			return ( rule1->getDateCreated() < rule2->getDateCreated() );
		}
		else // compare modified date
		{
			return ( rule1->getDateModified() < rule2->getDateModified() );
		}
    }

    bool sortDateTimeDescending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {
		if (rule1->getDateModified() == 0 || rule2->getDateModified() == 0)
		{
			// compare created date
			return ( rule1->getDateCreated() >= rule2->getDateCreated() );
		}
		else // compare modified date
		{
			return ( rule1->getDateModified() >= rule2->getDateModified() );
		}
    }

	//Sort column 3 ASSET NAME
    bool sortAssetNameAscending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {   
        return ((rule1->getAssetName()).compare(rule2->getAssetName()) < 0);
    }

    bool sortAssetNameDescending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {
        return ((rule1->getAssetName()).compare(rule2->getAssetName()) >= 0);
    }

	//Sort column 4 ALARM TYPE
    bool sortAlarmTypeAscending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {   
        return ((rule1->getAlarmTypeName()).compare(rule2->getAlarmTypeName()) < 0);
    }

    bool sortAlarmTypeDescending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {
        return ((rule1->getAlarmTypeName()).compare(rule2->getAlarmTypeName()) >= 0);
    }

	//Sort column 5 DESCRIPTION
    bool sortDescriptionAscending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {   
        return ((rule1->getAlarmDescription()).compare(rule2->getAlarmDescription()) < 0);
    }

    bool sortDescriptionDescending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {
        return ((rule1->getAlarmDescription()).compare(rule2->getAlarmDescription()) >= 0);
    }

	//Sort column 6 VALUE
    bool sortValueAscending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {   
        return ((rule1->getValue()).compare(rule2->getValue()) < 0);
    }

    bool sortValueDescending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {
        return ((rule1->getValue()).compare(rule2->getValue()) >= 0);
    }

	//Sort column 5 OPERATOR
    bool sortOperatorAscending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {   
        return ((rule1->getOperatorName()).compare(rule2->getOperatorName()) < 0);
    }

    bool sortOperatorDescending(TA_Base_Core::SuppressionRuleData* rule1, TA_Base_Core::SuppressionRuleData* rule2)
    {
        return ((rule1->getOperatorName()).compare(rule2->getOperatorName()) >= 0);
    }
}


namespace TA_Base_App
{
	const int CSuppressionRulesDialog::INHIBITION_DLG_REFRESH_CALLBACK = 5;
	const std::string CSuppressionRulesDialog::REFRESH_TIME_PARAM = "InhibitionDlgRefreshTime";

	const int CSuppressionRulesDialog::DEFAULT_REFRESH_TIME = 30; //secs

	// Column 0
    const int CSuppressionRulesDialog::REMOVABLE_COLUMN = 0;
    //const char* CSuppressionRulesDialog::REMOVABLE_COLUMN_NAME = "Removable";
	const char* CSuppressionRulesDialog::REMOVABLE_COLUMN_NAME = "可移除";
    const int CSuppressionRulesDialog::REMOVABLE_COLUMN_WIDTH = 80;

	// Column 1
    const int CSuppressionRulesDialog::DATE_TIME_COLUMN = 1;
    //const char* CSuppressionRulesDialog::DATE_TIME_COLUMN_NAME = "Date / Time";
	const char* CSuppressionRulesDialog::DATE_TIME_COLUMN_NAME = "日期/时间";
    const int CSuppressionRulesDialog::DATE_TIME_COLUMN_WIDTH = 130;

	// Column 2
    const int CSuppressionRulesDialog::ASSET_NAME_COLUMN = 2;
    //const char* CSuppressionRulesDialog::ASSET_NAME_COLUMN_NAME = "Asset Name";
	const char* CSuppressionRulesDialog::ASSET_NAME_COLUMN_NAME = "设备名";
    const int CSuppressionRulesDialog::ASSET_NAME_COLUMN_WIDTH = 190;

	// Column 3
    const int CSuppressionRulesDialog::ALARM_TYPE_COLUMN = 3;
    //const char* CSuppressionRulesDialog::ALARM_TYPE_COLUMN_NAME = "Alarm Type";
	const char* CSuppressionRulesDialog::ALARM_TYPE_COLUMN_NAME = "报警类型";
    const int CSuppressionRulesDialog::ALARM_TYPE_COLUMN_WIDTH = 140;

	// Column 4
    const int CSuppressionRulesDialog::DESCRIPTION_COLUMN = 4;
    //const char* CSuppressionRulesDialog::DESCRIPTION_COLUMN_NAME = "Description";
	const char* CSuppressionRulesDialog::DESCRIPTION_COLUMN_NAME = "描述";
    const int CSuppressionRulesDialog::DESCRIPTION_COLUMN_WIDTH = 540;

	// Column 5
    const int CSuppressionRulesDialog::VALUE_COLUMN = 5;
    //const char* CSuppressionRulesDialog::VALUE_COLUMN_NAME = "Value";
	const char* CSuppressionRulesDialog::VALUE_COLUMN_NAME = "值";
    const int CSuppressionRulesDialog::VALUE_COLUMN_WIDTH = 70;

	// Column 6
    const int CSuppressionRulesDialog::OPERATOR_COLUMN = 6;
    //const char* CSuppressionRulesDialog::OPERATOR_COLUMN_NAME = "Operator";
	const char* CSuppressionRulesDialog::OPERATOR_COLUMN_NAME = "操作员";
    const int CSuppressionRulesDialog::OPERATOR_COLUMN_WIDTH = 77;

    const int CSuppressionRulesDialog::REMOVABLE_IMAGE = 0;
    const int CSuppressionRulesDialog::BLANK_IMAGE = 1;

    /////////////////////////////////////////////////////////////////////////////
    // CSuppressionRulesDialog dialog


    CSuppressionRulesDialog::CSuppressionRulesDialog(unsigned long profileId, CWnd* pParent /*=NULL*/)
	    : CDialog(CSuppressionRulesDialog::IDD, pParent),
          m_profileId(profileId),
          m_sortAscending(true),
          m_sortColumn(DATE_TIME),
		  m_parent(pParent)
    {
	    //{{AFX_DATA_INIT(CSuppressionRulesDialog)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    void CSuppressionRulesDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CSuppressionRulesDialog)
	DDX_Control(pDX, IDC_COMBO_LOCATIONS, m_locationCombo);
	    DDX_Control(pDX, IDC_RULES_LIST, m_rulesList);
	//}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CSuppressionRulesDialog, CDialog)
	    //{{AFX_MSG_MAP(CSuppressionRulesDialog)
	    ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
	    ON_BN_CLICKED(IDC_RADIO_FILTER_LOCATION, OnRadioFilterLocation)
	    ON_BN_CLICKED(IDC_RADIO_SHOW_ALL, OnRadioShowAll)
	    ON_NOTIFY(NM_CLICK, IDC_RULES_LIST, OnClickRulesList)
        ON_NOTIFY(LVN_COLUMNCLICK, IDC_RULES_LIST, OnColumnclick)
	    ON_BN_CLICKED(IDC_CLOSE, OnClose)
		ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
		ON_BN_CLICKED(IDC_EXPORT_LIST, OnExport)
		ON_BN_CLICKED(IDC_PRINT_LIST, OnPrint)
		ON_CBN_DBLCLK(IDC_COMBO_LOCATIONS, OnSelchangeComboLocations)
		ON_CBN_SELCHANGE(IDC_COMBO_LOCATIONS, OnSelchangeComboLocations)
		ON_WM_DESTROY()
		ON_WM_TIMER()
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CSuppressionRulesDialog message handlers

    BOOL CSuppressionRulesDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();

		// setup font
		m_font.CreatePointFont( 100, "宋体" );
	    
		//Obtain current location key
		std::string locationKey = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
		//Convert to unsigned long and store it
		m_currentLocationKey = atol(locationKey.c_str());
		
		// set up and load the data
	    setUpHeadings();
		// populate the location combo box and location name_key map
		populateLocationCombo();

		// Check for permission to show all rules and disable buttons accordingly
		// Default to show all inhibitions if the operator has the rights
		if ( hasPermissionToViewAllRules() )
		{
			// Check the SHOW_ALL radio button by default
			CheckDlgButton(IDC_RADIO_SHOW_ALL, BST_CHECKED);
			CheckDlgButton(IDC_RADIO_FILTER_LOCATION, BST_UNCHECKED);
			// Populate the list control
			loadAllRules();
			// Disable the location combo box
			m_locationCombo.EnableWindow(FALSE);
		}
		else
		{
			// Check the LOCATION radio button by default
			CheckDlgButton(IDC_RADIO_SHOW_ALL, BST_UNCHECKED);
			CheckDlgButton(IDC_RADIO_FILTER_LOCATION, BST_CHECKED);
			// populate the list control
			loadRulesByLocation(m_currentLocationKey);
			// Disable the show-all button and location combo box for selection
			GetDlgItem(IDC_RADIO_SHOW_ALL)->EnableWindow(FALSE);
			m_locationCombo.EnableWindow(FALSE);
		}

        // Disable the Remove button
        TA_ASSERT(GetDlgItem(IDC_REMOVE_BUTTON) != NULL, "Error creating Remove button.");
        GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);

		//Initialise refresh time
		m_refreshTime = DEFAULT_REFRESH_TIME;
		//Obtain the refresh time from configuration, if any
		std::auto_ptr<TA_Base_Core::IData> entityParam = std::auto_ptr<TA_Base_Core::IData>( TA_Base_Core::EntityAccessFactory::getInstance().getParameter
			( TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME), REFRESH_TIME_PARAM ) );
		if (entityParam.get() != 0 && entityParam->getNumRows() > 0 && entityParam->isNull(0, "VALUE") == false)
		{
			m_refreshTime = entityParam->getIntegerData(0, "VALUE");
		}

		// Set the timer
		//lsx SetTimer(CSuppressionRulesDialog::INHIBITION_DLG_REFRESH_CALLBACK, m_refreshTime*1000, NULL );
	    
		return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void CSuppressionRulesDialog::OnClose() 
    {
        CDialog::OnCancel();
	}

	void CSuppressionRulesDialog::OnDestroy()
	{
	    clearList();
		// clear up the icon vector
		m_imageStringVector.clear();
		// kill the timer
		KillTimer(CSuppressionRulesDialog::INHIBITION_DLG_REFRESH_CALLBACK);

		CDialog::OnDestroy();
	}

	void CSuppressionRulesDialog::OnTimer(UINT nIDEvent) 
	{
		// TODO: Add your message handler code here and/or call default
		if (nIDEvent == CSuppressionRulesDialog::INHIBITION_DLG_REFRESH_CALLBACK)
		{
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "OnTimer is called after elapse time of %d secs.", m_refreshTime);
			OnRefresh();
			return;
		}

		CDialog::OnTimer(nIDEvent);
	}


    void CSuppressionRulesDialog::OnRemoveButton() 
    {
        FUNCTION_ENTRY("OnRemoveButton");

	    int selectedCount = m_rulesList.GetSelectedCount();
	    if (selectedCount > 0)
	    {
            std::vector<int> deleteIndexes;
            POSITION pos = m_rulesList.GetFirstSelectedItemPosition();
            while(pos != NULL)
            {
                int nItem = m_rulesList.GetNextSelectedItem(pos);
                deleteIndexes.push_back(nItem);
            
                try
		        {
                    DWORD data = m_rulesList.GetItemData(nItem);
                    RuleInfo* ruleInfo = reinterpret_cast <RuleInfo*> (data);
                    TA_Base_Core::SuppressionRuleData* ruleDetails = ruleInfo->suppressionRule;
			        
                    ruleDetails->deleteThisObject();
			        sendConfigUpdate(ruleDetails);
            
                }	
		        catch(const TA_Base_Core::DatabaseException& e)
		        {
			        std::ostringstream error;
			        //error << "Remove Suppression Rule failed " << e.what(); 
					error << "移除禁止规则失败 " << e.what(); 
			        
					// TD14164 azenitha++
					TA_Base_Bus::TransActiveMessage userMsg;
					CString reason = e.what();
					userMsg << reason;
					userMsg.showMsgBox(IDS_UE_040041);
					/*AfxMessageBox(error.str().c_str());*/
					// TD14164 ++azenitha
		        }
		    }

            // Delete the items from the list. Delete from the bottom because if we delete from the top, the
            // number of items in the list changes and hence all the items below the deleted item will have
            // different indexes.
            std::vector<int>::iterator it = deleteIndexes.end();
            --it;
            for(; it != deleteIndexes.begin(); --it)
            {
                // Delete item from the list
                m_rulesList.DeleteItem(*it);
            }
            m_rulesList.DeleteItem(*it);

            // Disable the Remove button once all the items have been removed
            TA_ASSERT(GetDlgItem(IDC_REMOVE_BUTTON) != NULL, "Error creating Remove button.");
            GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);
	    }

        FUNCTION_EXIT;
    }


    void CSuppressionRulesDialog::OnRadioFilterLocation() 
    {
		if (hasPermissionToViewAllRules())
		{
			// enable the combo box
			m_locationCombo.EnableWindow(TRUE);
			// load the rules by current location
			loadRulesByLocation(m_currentLocationKey);
		}
    }


    void CSuppressionRulesDialog::OnRadioShowAll() 
    {
		m_locationCombo.EnableWindow(FALSE);
	    loadAllRules();
    }

	void CSuppressionRulesDialog::OnRefresh()
	{
		// reload all the rules
		CButton* showAll = static_cast<CButton*> (GetDlgItem(IDC_RADIO_SHOW_ALL));
		CButton* filterLocation = static_cast<CButton*> (GetDlgItem(IDC_RADIO_FILTER_LOCATION));

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "OnRefresh is called. Auto refresh is reset to %d secs.", m_refreshTime);

		if (showAll->GetCheck() == BST_CHECKED)
		{
			loadAllRules();
		}
		else if (filterLocation->GetCheck() == BST_CHECKED)
		{
			loadRulesByLocation(m_currentLocationKey);
		}

		// reset the refresh timer
	 //lsx	SetTimer(CSuppressionRulesDialog::INHIBITION_DLG_REFRESH_CALLBACK, m_refreshTime*1000, NULL);
	}

	void CSuppressionRulesDialog::OnPrint()
	{
		std::string oper = "";
		std::string console = "";
		TA_Base_Core::IOperator* p_operator = NULL;
		TA_Base_Core::IConsole* p_console = NULL;
		try
		{
			// Get the current operator
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			p_operator = TA_Base_Core::OperatorAccessFactory::getInstance().getOperatorFromSession(sessionId, false);
			oper = p_operator->getDescription();
			p_console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId, false);
			console = p_console->getName();
		}
		catch(...)
		{
			if(p_operator != NULL)
			{
				delete p_operator;
				p_operator = 0;
			}
			if(p_console != NULL)
			{
				delete p_console;
				p_console = 0;
			}
			TA_Base_Bus::TransActiveMessage userMsg;
			UINT selectedButton = userMsg.showMsgBox(IDS_UI_040027);
		}
		
		delete p_operator;
		delete p_console;

		CString header = "Inhibition List : " + CTime::GetCurrentTime().Format("%a %#d/%#m/%Y %#H:%M:%S")
			+ " - " + oper.c_str() + " on console " + console.c_str();


		// Do not delete this structure. It will be deleted inside the thread
		CListCtrlPrint::t_ParamsToReceive* params = new CListCtrlPrint::t_ParamsToReceive;
		params->csPageHead = header;
		params->csPage = "Page %d of %d";
		params->list = &m_rulesList;

		// Now provide the String for printing of image column
		params->imageStringVector = &m_imageStringVector;

		CWinThread* printThread = AfxBeginThread(RUNTIME_CLASS(CListCtrlPrint));
		printThread->PostThreadMessage(CPrinterJob::WM_START_PRINT, 0, (LPARAM)params);
	}

	void CSuppressionRulesDialog::OnExport()
	{
		if (m_parent==NULL) return;

		// Get the GUI entity from which we can get the default export path.
		CAlarmsGuiDlg* alarmGuiDlg = static_cast<CAlarmsGuiDlg*> (m_parent);
		TA_Base_Core::AlarmGUI* alarmGUI = dynamic_cast<TA_Base_Core::AlarmGUI*>((alarmGuiDlg->getControlGUIAccess())->getGUIEntity());
		
		// Do not delete this structure. It will be deleted inside the thread
		CExportAlarms::t_ParamsToReceive* params;
		params = new CExportAlarms::t_ParamsToReceive;

		params->alarmList = &m_rulesList;
		try
		{
			//params->defaultExportPath = alarmGUI->getExportAlarmDirectory().c_str();
			//TD17888 marc_bugfix
			params->defaultExportPath = alarmGUI->getExportDirectory().c_str();
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo,"DatabaseError", "There was an error while attempting to retrieve the default export path. Using default.");
			params->defaultExportPath.Empty();
		}
		// provide the string/text for image column
		params->pImageStringVector = &m_imageStringVector;

		CWinThread* exportThread = AfxBeginThread(RUNTIME_CLASS(CExportAlarms));

		exportThread->PostThreadMessage(START_EXPORT, 0, (LPARAM)params);
	}

    void CSuppressionRulesDialog::OnClickRulesList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    int selectedCount = m_rulesList.GetSelectedCount();
	    if (selectedCount > 0)
        {
            std::vector<unsigned long> regionKeys;
            getCurrentOperatorsRegionAssignment(regionKeys);

            // Check that every rule selected has been created by the same profile. If not, the remove
            // button will be disabled. If so, then the remove button will only be enabled if the current
            // profile has permission to remove rules.
    
            bool cannotRemove = false;
            POSITION pos = m_rulesList.GetFirstSelectedItemPosition();
            while(pos != NULL)
            {
                int nItem = m_rulesList.GetNextSelectedItem(pos);
                DWORD data = m_rulesList.GetItemData(nItem);
                RuleInfo* ruleInfo = reinterpret_cast <RuleInfo*> (data);

                if(!ruleInfo->isCreatedByCurrentProfile)
                {
                    // There is at least one rule that can't be removed. Disable the Remove button.
                    TA_ASSERT(GetDlgItem(IDC_REMOVE_BUTTON) != NULL, "Error creating Remove button.");
                    GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);
                    cannotRemove = true;
                    break;
                }

                TA_Base_Core::SuppressionRuleData* ruleData = ruleInfo->suppressionRule;
        
                unsigned long regionKey = 0;
                if(getRegionKeyByEntity(ruleData->getEntityKey(), regionKey))
                {
                    std::vector<unsigned long>::iterator itRegionKey = std::find(regionKeys.begin(), regionKeys.end(), regionKey);
                    if(itRegionKey == regionKeys.end())
                    {
                        // There is at least one rule that can't be removed. Disable the Remove button.
                        TA_ASSERT(GetDlgItem(IDC_REMOVE_BUTTON) != NULL, "Error creating Remove button.");
                        GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);
                        cannotRemove = true;
                        break;
                    }
                }
            }

            if(!cannotRemove)
            {
                if(hasPermissionToRemoveRules())
                {
                    // Enable the Remove button
                    TA_ASSERT(GetDlgItem(IDC_REMOVE_BUTTON) != NULL, "Error creating Remove button.");
                    GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(true);
                }
            }
        }
        else
        {
            // Disable the Remove button
            TA_ASSERT(GetDlgItem(IDC_REMOVE_BUTTON) != NULL, "Error creating Remove button.");
            GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);
        }
	    
	    *pResult = 0;
    }


    void CSuppressionRulesDialog::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        FUNCTION_ENTRY("onColumnclick");

	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        int column = pNMListView->iSubItem;

        EColumnType oldSortColumn = m_sortColumn;

        switch(column)
        {
            case REMOVABLE_COLUMN:   m_sortColumn = REMOVABLE;
                                break;

            case DATE_TIME_COLUMN:        m_sortColumn = DATE_TIME;
                                break;

            case ASSET_NAME_COLUMN:        m_sortColumn = ASSET_NAME;
                                break;

            case ALARM_TYPE_COLUMN:    m_sortColumn = ALARM_TYPE;
                                break;

            case DESCRIPTION_COLUMN:        m_sortColumn = DESCRIPTION;
                                break;

            case VALUE_COLUMN:        m_sortColumn = VALUE;
                                break;

            case OPERATOR_COLUMN:        m_sortColumn = OPERATOR;
                                break;

            default:            // Do nothing
                                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "No sort on invalid column");
        }

        if(oldSortColumn == m_sortColumn)
        {
            m_sortAscending = !m_sortAscending;
        }
        else
        {
            m_sortAscending = true;
        }
        
        if(IsDlgButtonChecked(IDC_RADIO_SHOW_ALL))
        {
            loadAllRules();
        }
        else
        {
            loadRulesByLocation(m_currentLocationKey);
        }

        //First find if the selected column is currently sorted on. If so, toggle 
		//the sorting arrangment. 
		
        CHeaderCtrl* headerCtrl = m_rulesList.GetHeaderCtrl(); 

		HDITEM headerItem; 
		headerItem.mask = HDI_FORMAT | HDI_BITMAP; 
        headerCtrl->GetItem(column, &headerItem); 
		if (headerItem.hbm != 0) 
		{ 
			DeleteObject(headerItem.hbm); 
			headerItem.hbm = 0; 
		} 
		headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 

        int resourceId = IDB_ASCENDING;
        if(!m_sortAscending)
        {
            resourceId = IDB_DESCENDING;
        }

        /// Load the bitmap based on whether we are sorting ascending or descending
		headerItem.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(resourceId), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS); 

        TA_ASSERT(headerItem.hbm != NULL, "The resource file bus/alarm/alarm_list_control/src/AlarmListCtrl.rc must be included in the application's rc file");

		headerCtrl->SetItem(column, &headerItem); 
        

        // If the column we are sorting on has changed then we must remove the arrow bitmap from the old
        // column header
        if(oldSortColumn != m_sortColumn)
        {
            HDITEM headerItemOld;
		    headerItemOld.mask = HDI_FORMAT | HDI_BITMAP; 
            headerCtrl->GetItem(oldSortColumn, &headerItemOld); 
		    headerItemOld.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT); 
		    if(headerItemOld.hbm != 0) 
		    { 
			    DeleteObject(headerItemOld.hbm); 
			    headerItemOld.hbm = 0; 
		    } 
		    headerCtrl->SetItem(oldSortColumn, &headerItemOld); 
        }

        *pResult = 0;
        FUNCTION_EXIT;
    }


    void CSuppressionRulesDialog::loadAllRules()
    {
        FUNCTION_ENTRY("loadAllRules");

        // Disable the Remove button
        TA_ASSERT(GetDlgItem(IDC_REMOVE_BUTTON) != NULL, "Error creating Remove button.");
        GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);

        // Clear the list first
        clearList();

        // Retrieve the rules
        std::vector<TA_Base_Core::SuppressionRuleData*> suppressionRules;
        
        bool loadError = false;
        try
        {
            suppressionRules = TA_Base_Core::AlarmRuleAccessFactory::getInstance().getAllSuppressionRules();
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            loadError = true;
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            loadError = true;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "An unexpected exception was caught while retrieving suppression rules.");
            loadError = true;
        }

        if(loadError)
        {
            displayErrorMessage();
        }
        else
        {
            // Sort list by the selected column
            sortList(suppressionRules);
    
            populateList(suppressionRules);
        }

        FUNCTION_EXIT;
    }


    void CSuppressionRulesDialog::loadRulesByLocation(unsigned long locationKey)
    {
        FUNCTION_ENTRY("loadRulesByLocation");

        // Disable the Remove button
        TA_ASSERT(GetDlgItem(IDC_REMOVE_BUTTON) != NULL, "Error creating Remove button.");
        GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);

        // Clear the list first
        clearList();

        // Retrieve the rules
        std::vector<TA_Base_Core::SuppressionRuleData*> suppressionRules;

        bool loadError = false;
        try
        {
            suppressionRules = TA_Base_Core::AlarmRuleAccessFactory::getInstance().getSuppressionRulesByLocation(locationKey);
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            loadError = true;
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            loadError = true;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "An unexpected exception was caught while retrieving suppression rules.");
            loadError = true;
        }

        // Remove the rules that don't match the current user's region assignment
        if(!filterByRegionAssignment(suppressionRules))
        {
            loadError = true;
        }

        if(loadError)
        {
            displayErrorMessage();
        }
        else
        {
            // Sort list by the selected column
            sortList(suppressionRules);
    
            populateList(suppressionRules);
        }

        FUNCTION_EXIT;
    }


    void CSuppressionRulesDialog::sendConfigUpdate(TA_Base_Core::IAlarmRuleData* suppressionRule)
    {
        FUNCTION_ENTRY("sendConfigUpdate");

        // First we iterate through and build up a list of changes. We are only interested in
        // which attributes have changed, not the values they have changed from and to.
        std::stringstream entityKey;
        entityKey << suppressionRule->getKey();
	    std::vector<std::string> changes;
        changes.push_back(entityKey.str());

	    // Get the key
	    unsigned long keyOfChangedData = suppressionRule->getKey();

	    // Send the message
	    TA_Base_Core::ConfigUpdateMessageSender* configMessageSender = 0;
        configMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getConfigUpdateMessageSender( TA_Base_Core::ConfigUpdate::Context );
        TA_ASSERT(configMessageSender != NULL, "The config update message sender received is NULL");
	    configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAlarmRule,
                                                       keyOfChangedData,
                                                       TA_Base_Core::Delete,
                                                       changes);
	    delete configMessageSender;
	    configMessageSender = 0;

        FUNCTION_EXIT;
    }


    void CSuppressionRulesDialog::displayErrorMessage()
    {
		// TD14164 azenitha++
		/*AfxMessageBox("An error occurred while retrieving suppression rules.");*/
		TA_Base_Bus::TransActiveMessage userMsg;
		//CString description = "retrieving suppression rules";
		CString description = "正在获取禁止规则";
		userMsg << description;
		userMsg.showMsgBox(IDS_UW_632015);
		// TD14164 ++azenitha
    }


    bool CSuppressionRulesDialog::hasPermissionToRemoveRules()
    {
        FUNCTION_ENTRY("hasPermissionToRemoveRules");

        // Need alarm manager entity key to check the rights of this action
	    TA_Base_Core::IEntityData* myEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(
		    TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));

	    unsigned long entityKey = myEntity->getKey();
	    delete myEntity;
	    TA_Base_Bus::RightsChecker rightsChecker;

        if( !rightsChecker.canPerformAction(entityKey, TA_Base_Bus::aca_SUPER_REMOVE_ALARM_RULE) )
	    {
            if( !rightsChecker.canPerformAction(entityKey, TA_Base_Bus::aca_REMOVE_ALARM_RULE) )
	        {
		        FUNCTION_EXIT;
		        return false;
	        }
	    }
  
        FUNCTION_EXIT;
        return true;
    }


	bool CSuppressionRulesDialog::hasPermissionToViewAllRules()
	{
        FUNCTION_ENTRY("hasPermissionToViewAllRules");

        // Need alarm manager entity key to check the rights of this action
	    TA_Base_Core::IEntityData* myEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(
		    TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));

	    unsigned long entityKey = myEntity->getKey();
	    delete myEntity;
	    TA_Base_Bus::RightsChecker rightsChecker;

        if( !rightsChecker.canPerformAction(entityKey, TA_Base_Bus::aca_SHOW_ALL_SYSTEM_ALARM_INHIBITION) )
	    {
            if( !rightsChecker.canPerformAction(entityKey, TA_Base_Bus::aca_SHOW_ALL_SYSTEM_ALARM_INHIBITION) )
	        {
		        FUNCTION_EXIT;
		        return false;
	        }
	    }

        FUNCTION_EXIT;
        return true;
	}


    void CSuppressionRulesDialog::setUpHeadings()
    {
		// set font
		m_rulesList.SetFont( &m_font, FALSE );

        m_rulesList.SetExtendedStyle( LVS_EX_SUBITEMIMAGES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP);

		// Column 0
        m_rulesList.InsertColumn(REMOVABLE_COLUMN, REMOVABLE_COLUMN_NAME, LVCFMT_IMAGE | LVCFMT_CENTER, REMOVABLE_COLUMN_WIDTH);
        // Column 1
		m_rulesList.InsertColumn(DATE_TIME_COLUMN, DATE_TIME_COLUMN_NAME, LVCFMT_LEFT, DATE_TIME_COLUMN_WIDTH);
        // Column 2
		m_rulesList.InsertColumn(ASSET_NAME_COLUMN, ASSET_NAME_COLUMN_NAME, LVCFMT_LEFT, ASSET_NAME_COLUMN_WIDTH);
        // Column 3
		m_rulesList.InsertColumn(ALARM_TYPE_COLUMN, ALARM_TYPE_COLUMN_NAME, LVCFMT_LEFT, ALARM_TYPE_COLUMN_WIDTH);
		// Column 4
        m_rulesList.InsertColumn(DESCRIPTION_COLUMN, DESCRIPTION_COLUMN_NAME, LVCFMT_LEFT, DESCRIPTION_COLUMN_WIDTH);
		// Column 5
        m_rulesList.InsertColumn(VALUE_COLUMN, VALUE_COLUMN_NAME, LVCFMT_LEFT, VALUE_COLUMN_WIDTH);
		// Column 6
        m_rulesList.InsertColumn(OPERATOR_COLUMN, OPERATOR_COLUMN_NAME, LVCFMT_LEFT, OPERATOR_COLUMN_WIDTH);

        // Set up image list
        m_removableIcon  = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_RULE_REMOVABLE));
        m_blankIcon  = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_UNREMOVABLE_RULE));
	    m_imageList.Create(16, 16, ILC_COLOR8, 4, 4);
        m_imageList.Add(m_removableIcon);
        m_imageList.Add(m_blankIcon);
		// Set up icon vector
		m_imageStringVector.push_back("Removable");
		m_imageStringVector.push_back("Unremovable");

        
        m_rulesList.SetImageList(&m_imageList, LVSIL_SMALL);

        // Set the image in the header that the rules are sorted by default
        CHeaderCtrl* headerCtrl = m_rulesList.GetHeaderCtrl(); 

		HDITEM headerItem; 
		headerItem.mask = HDI_FORMAT | HDI_BITMAP; 
        headerCtrl->GetItem(m_sortColumn, &headerItem); 
		if (headerItem.hbm != 0) 
		{ 
			DeleteObject(headerItem.hbm); 
			headerItem.hbm = 0; 
		} 
		headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 

        int resourceId = IDB_ASCENDING;
        if(!m_sortAscending)
        {
            resourceId = IDB_DESCENDING;
        }

        // Load the bitmap based on whether we are sorting ascending or descending
		headerItem.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(resourceId), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS); 

        TA_ASSERT(headerItem.hbm != NULL, "The resource file bus/alarm/alarm_list_control/src/AlarmListCtrl.rc must be included in the application's rc file");

		headerCtrl->SetItem(m_sortColumn, &headerItem); 
    }


    void CSuppressionRulesDialog::populateList(std::vector<TA_Base_Core::SuppressionRuleData*>& suppressionRules)
    {
        FUNCTION_ENTRY("populateList");

        // Get the current user's region assignment
        std::vector<unsigned long> regionKeys;
        getCurrentOperatorsRegionAssignment(regionKeys);

        // Populate the list and delete the pointers
        int count = 0;
        for(std::vector<TA_Base_Core::SuppressionRuleData*>::iterator it = suppressionRules.begin();
            it != suppressionRules.end();
            ++it)
        {
            if(*it != NULL)
            {
				CTime dateTime = (*it)->getDateModified();
				if (dateTime == 0) dateTime = (*it)->getDateCreated();

                std::string assetName = (*it)->getAssetName();
                if(assetName.empty())
                {
                    assetName = "-";
                }

                std::string alarmType = (*it)->getAlarmTypeName();
                if(alarmType.empty())
                {
                    alarmType = "-";
                }

                std::string description = (*it)->getAlarmDescription();
                if(description.empty())
                {
                    description = "-";
                }

				std::string value = (*it)->getValue();
				if(value.empty())
				{
					value = "-";
				}

				std::string operatorName = (*it)->getOperatorName();
				if(operatorName.empty())
				{
					operatorName = "-";
				}

                // Store the rule and profile information as the item data to be retrieved later
                RuleInfo* ruleInfo = new RuleInfo;
                ruleInfo->isCreatedByCurrentProfile = false;
            
                // Insert an icon to indicate the rule is removable if the rule was created under the 
                // same profile and the entity related to the rule falls under the current user's region
                // assignment
                int image = BLANK_IMAGE;
				unsigned long createdByProfile = (*it)->getCreatedByProfileKey();

                if(createdByProfile == m_profileId)
                {
                    ruleInfo->isCreatedByCurrentProfile = true;
                    
                    unsigned long regionKey = 0;
                    if(getRegionKeyByEntity((*it)->getEntityKey(), regionKey))
                    {
                        std::vector<unsigned long>::iterator itRegionKey = std::find(regionKeys.begin(), regionKeys.end(), regionKey);
                        if(itRegionKey != regionKeys.end())
                        {
                            image = REMOVABLE_IMAGE;
							(*it)->setRemovable(REMOVABLE_IMAGE);
                        }
						else
						{
							image = BLANK_IMAGE;
							(*it)->setRemovable(BLANK_IMAGE);
						}
                    }
                }
                
                ruleInfo->suppressionRule = (*it);

                // Add row to list
				LVITEM item;
				item.mask = LVIF_IMAGE;
				item.iImage = image;
				item.iItem = count;
				item.iSubItem = 0;
				m_rulesList.InsertItem(&item);

				CString displayTime("");
				if (dateTime > 0)
				{
					displayTime = dateTime.Format("%d-%b-%y %H:%M:%S");
				}
				m_rulesList.SetItemText(count, DATE_TIME_COLUMN, displayTime);
                m_rulesList.SetItemText(count, ASSET_NAME_COLUMN, assetName.c_str());
                m_rulesList.SetItemText(count, ALARM_TYPE_COLUMN, alarmType.c_str());
				m_rulesList.SetItemText(count, DESCRIPTION_COLUMN, description.c_str());
				m_rulesList.SetItemText(count, VALUE_COLUMN, value.c_str());
				m_rulesList.SetItemText(count, OPERATOR_COLUMN, operatorName.c_str());

                DWORD itemData = reinterpret_cast <DWORD> (ruleInfo);
                m_rulesList.SetItemData(count, itemData);
                ++count;

                // Don't delete the pointers. They will be deleted when the list is cleared.
            }
        }

        FUNCTION_EXIT;
    }

	void CSuppressionRulesDialog::populateLocationCombo()
	{
		FUNCTION_ENTRY("populateLocationCombo");

		std::vector<TA_Base_Core::ILocation*>allLocationData;
		try
		{
			allLocationData = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DatabaseException caught whilst retrieving locations. There were problems communicating with the database");
			for (unsigned int i = 0;i < allLocationData.size(); ++i)
			{
				delete allLocationData[i];
			}
			allLocationData.clear();
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DataException caught whilst retrieving locations. The data could not be converted to the required format");
			for (unsigned int i = 0;i < allLocationData.size(); ++i)
			{
				delete allLocationData[i];
			}
			allLocationData.clear();
		}
		catch(...)
		{	
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Caught unknown exception whilst retrieving locations.");
			for (unsigned int i = 0;i < allLocationData.size(); ++i)
			{
				delete allLocationData[i];
			}
			allLocationData.clear();
		}

		// store the locations into the map and delete the temporary structure
		m_locationNameKeyMap.clear();
		int count = 0;
		for (unsigned int i = 0;i < allLocationData.size(); ++i)
        {
			if(allLocationData[i]->getKey() != 0)
			{
				m_locationCombo.InsertString( count, allLocationData[i]->getDisplayName().c_str());  //allLocationData[i]->getName().c_str() );
				m_locationNameKeyMap.insert( std::pair<std::string, unsigned long>(allLocationData[i]->getName(), allLocationData[i]->getKey()) );

				if (allLocationData[i]->getKey() == m_currentLocationKey)
				{
					m_locationCombo.SetCurSel(-1); //clear previous selection, if any
					m_locationCombo.SetCurSel(count);
				}
				delete allLocationData[i];
			}
		}
		allLocationData.clear();

		FUNCTION_EXIT;
	}

    bool CSuppressionRulesDialog::filterByRegionAssignment(std::vector<TA_Base_Core::SuppressionRuleData*>& suppressionRules)
    {
        FUNCTION_ENTRY("filterByRegionAssignment");

        std::vector<unsigned long> regionKeys;
        getCurrentOperatorsRegionAssignment(regionKeys);

        // Filter out the rules that don't match the regions
        std::vector<TA_Base_Core::SuppressionRuleData*>::iterator itRule = suppressionRules.begin();
        while(itRule != suppressionRules.end())
        {
            if(*itRule != NULL)
            {
                unsigned long entityKey = (*itRule)->getEntityKey();
                unsigned long regionKey = 0;
            
                if(getRegionKeyByEntity(entityKey, regionKey))
                {
                    std::vector<unsigned long>::iterator itRegionKey = std::find(regionKeys.begin(), regionKeys.end(), regionKey);
                    if(itRegionKey != regionKeys.end())
                    {
                        ++itRule;

                        // Don't delete the pointer, these will be deleted when the list is cleared.
                    }
                    else
                    {
                        delete *itRule;
                        *itRule = NULL;

                        itRule = suppressionRules.erase(itRule);
                    }
                }
            }
        }

        FUNCTION_EXIT;
        return true;
    }

    void CSuppressionRulesDialog::clearList()
    {
        // Clean up the pointers
        for(int i = 0; i < m_rulesList.GetItemCount(); ++i)
        {
            DWORD data = m_rulesList.GetItemData(i);
            RuleInfo* ruleInfo = reinterpret_cast <RuleInfo*> (data);

            if(ruleInfo != NULL)
            {
                TA_Base_Core::SuppressionRuleData* ruleData = ruleInfo->suppressionRule;

                if(ruleData != NULL)
                {
                    delete ruleData;
                    ruleData = NULL;
                }

                delete ruleInfo;
                ruleInfo = NULL;
            }
        }

        m_rulesList.DeleteAllItems();
    }


    bool CSuppressionRulesDialog::getCurrentOperatorsRegionAssignment(std::vector<unsigned long>& regionKeys)
    {
        FUNCTION_ENTRY("getCurrentOperatorsRegionAssignment");

        // Get the current operator
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        unsigned long operatorId = 0;

        try
        {
            TA_Base_Bus::AuthenticationLibrary authLib(true);
            TA_Base_Bus::SessionInfo sessionInfo = authLib.getSessionInfo(sessionId, sessionId);
            operatorId = sessionInfo.UserId;
        }
        catch(const TA_Base_Core::AuthenticationAgentException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException", ex.what());
            FUNCTION_EXIT;
            return false;
        }
        catch(const TA_Base_Core::ObjectResolutionException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
            FUNCTION_EXIT;
            return false;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "An unexpection exception was caught while retrieving the current operator.");
            FUNCTION_EXIT;
            return false;
        }

        // Get the operator's region assignment
        std::vector<TA_Base_Core::IRegion*> regions;

        try
        {
            regions = TA_Base_Core::RegionAccessFactory::getInstance().getRegionsByOperator(operatorId);
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            FUNCTION_EXIT;
            return false;
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            FUNCTION_EXIT;
            return false;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "An unexpection exception was caught while retrieving the current operator's regions.");
            FUNCTION_EXIT;
            return false;
        }

        // Retrieve the region keys
        regionKeys.clear();
        std::vector<TA_Base_Core::IRegion*>::iterator itRegion;
        for(itRegion = regions.begin(); itRegion != regions.end(); ++itRegion)
        {
            if(*itRegion != NULL)
            {
                regionKeys.push_back((*itRegion)->getKey());

                delete *itRegion;
                *itRegion = NULL;
            }
        }

        FUNCTION_EXIT;
        return true;
    }


    bool CSuppressionRulesDialog::getRegionKeyByEntity(unsigned long entityKey, unsigned long& regionKey)
    {
        try
        {
            TA_Base_Core::IEntityData* iEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
            TA_Base_Core::EntityData* entityData = dynamic_cast <TA_Base_Core::EntityData*> (iEntityData);
            regionKey = entityData->getRegion();
            return true;
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "An unexpected exception was caught while retrieving region key.");
        }

        return false;
    }

    
    void CSuppressionRulesDialog::sortList(std::vector<TA_Base_Core::SuppressionRuleData*>& suppressionRules)
    {
        if(m_sortAscending)
        {
            switch(m_sortColumn)
            {
                case REMOVABLE:   std::sort(suppressionRules.begin(), suppressionRules.end(), sortRemovableAscending);
                                    break;
                case DATE_TIME:        std::sort(suppressionRules.begin(), suppressionRules.end(), sortDateTimeAscending);
                                    break;
                case ASSET_NAME:   std::sort(suppressionRules.begin(), suppressionRules.end(), sortAssetNameAscending);
                                    break;
                case ALARM_TYPE:    std::sort(suppressionRules.begin(), suppressionRules.end(), sortAlarmTypeAscending);
                                    break;
                case DESCRIPTION:   std::sort(suppressionRules.begin(), suppressionRules.end(), sortDescriptionAscending);
                                    break;
                case VALUE:   std::sort(suppressionRules.begin(), suppressionRules.end(), sortValueAscending);
                                    break;
                case OPERATOR:   std::sort(suppressionRules.begin(), suppressionRules.end(), sortOperatorAscending);
                                    break;
                default:            std::sort(suppressionRules.begin(), suppressionRules.end(), sortDateTimeAscending);
            }
        }
        else
        {
            switch(m_sortColumn)
            {
                case REMOVABLE:   std::sort(suppressionRules.begin(), suppressionRules.end(), sortRemovableDescending);
                                    break;
                case DATE_TIME:        std::sort(suppressionRules.begin(), suppressionRules.end(), sortDateTimeDescending);
                                    break;
                case ASSET_NAME:   std::sort(suppressionRules.begin(), suppressionRules.end(), sortAssetNameDescending);
                                    break;
                case ALARM_TYPE:    std::sort(suppressionRules.begin(), suppressionRules.end(), sortAlarmTypeDescending);
                                    break;
                case DESCRIPTION:   std::sort(suppressionRules.begin(), suppressionRules.end(), sortDescriptionDescending);
                                    break;
                case VALUE:   std::sort(suppressionRules.begin(), suppressionRules.end(), sortValueDescending);
                                    break;
                case OPERATOR:   std::sort(suppressionRules.begin(), suppressionRules.end(), sortOperatorDescending);
                                    break;
                default:            std::sort(suppressionRules.begin(), suppressionRules.end(), sortDateTimeDescending);
            }
        }
    }

	void CSuppressionRulesDialog::OnSelchangeComboLocations() 
	{
		// Get the select location string
		int index = m_locationCombo.GetCurSel();
		CString selLocationName;
		m_locationCombo.GetLBText(index, selLocationName);
		// Obtain the location key from map
		if (selLocationName.IsEmpty() == false)
		{
			std::map<std::string, unsigned long>::iterator itr = m_locationNameKeyMap.find( (LPCTSTR)selLocationName );
			if (itr != m_locationNameKeyMap.end())
			{
				// Store the key
				m_currentLocationKey = itr->second;
			}
		}

		// reload the rule list
		loadRulesByLocation(m_currentLocationKey);
	}

} // end TA_Base_App


/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/InspectorPanelDlg.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/05 10:04:52 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class provides the main dialog used to display the data point information.  
  * It has a tab control with 4 associated dialogs (1 per tab).   
  */

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include <sstream>
#include "ace/OS.h"

#include "app/scada/Inspector_Panel/src/InspectorPanel.h"
#include "app/scada/Inspector_Panel/src/resource.h"
#include "app/scada/Inspector_Panel/src/InspectorPanelDlg.h"
#include "app/scada/Inspector_Panel/src/AccessControlHelper.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/generic_gui/src/HelpLauncher.h" //TD15124 azenitha

#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include <omp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;
using TA_Base_Bus::TransActiveDialog;
using namespace TA_Base_App;

namespace TA_Base_App
{
	static const std::string  INFORMATION_PAGE         = "INFORMATION";
	static const std::string  CONTROL_PAGE             = "CONTROL";
    static const std::string  LIMITS_PAGE			   = "LIMITS";
	static const std::string  NOTES_PAGE               = "NOTES";
	static const std::string  TAG_PAGE                 = "TAG";
	static const std::string  INHIBIT_PAGE			   = "INHIBIT";
	static const std::string  OVERRIDE_PAGE			   = "OVERRIDE";
    static const std::string  METER_PAGE			   = "METER";
	static const std::string  DIRECT_CONTROL_PAGE      = "DIRECTCONTROL";
	static const std::string  NULL_PAGE					= "NULL";
	/////////////////////////////////////////////////////////////////////////////
	// CInspectorPanelDlg dialog

	//
	// Constructor
	//
	CInspectorPanelDlg::CInspectorPanelDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent /*=NULL*/)
	: TransActiveDialog(genericGUICallback, CInspectorPanelDlg::IDD, pParent), 
      m_resourceId(0), 
	  m_equipment (NULL), 
      m_rightsMgr (NULL), 
      m_heightDifference (0), 
	  m_isInitialTab (false),
      m_isInDirectControl(false),
	  m_state(Start),
	  m_isEquipmentChanged(false)
	{
		FUNCTION_ENTRY ("CInspectorPanelDlg()");

		//{{AFX_DATA_INIT(CInspectorPanelDlg)
		//}}AFX_DATA_INIT
		// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

		m_previousTab = CMainTabControl::NoTab;
		m_requestedTab = CMainTabControl::NoTab;
		m_requestedPage = NULL_PAGE;
	
        TA_Base_Bus::ScadaProxyFactory::getInstance();


		FUNCTION_EXIT;
	}

	//
	// Destructor
	//
	CInspectorPanelDlg::~CInspectorPanelDlg()
	{
		FUNCTION_ENTRY ("~CInspectorPanelDlg()");
		m_state = End;
		AccessControlHelper::getInstance()->terminateAndWait();

		FUNCTION_EXIT;
	}

	//
	// DoDataExchange
	//
	void CInspectorPanelDlg::DoDataExchange(CDataExchange* pDX)
	{
		FUNCTION_ENTRY ("DoDataExchange()");
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "CInspectorPanelDlg::DoDataExchange");
			
		TransActiveDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CInspectorPanelDlg)
		DDX_Control(pDX, IDC_STATUS, m_statusBox);
		DDX_Control(pDX, IDC_MAINTAB, m_MainTab);
		//}}AFX_DATA_MAP

		FUNCTION_EXIT;
	}

	//
	// this function should be called after mainTab initilized.
	//
	void CInspectorPanelDlg::setPage(std::string pageName)
	{
		FUNCTION_ENTRY ("setPage()");

		m_requestedPage = pageName;
		m_isInDirectControl = false;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "*** Requested Page = %s", m_requestedPage.c_str() );

		// For direct control page
        if (pageName==DIRECT_CONTROL_PAGE)
		{
            // Resize direct control dialog to the same as the mainTab
            CRect mainTabDimensions;
	        m_MainTab.GetWindowRect(mainTabDimensions);
	        this->ScreenToClient(mainTabDimensions);
            m_directControlDlg.MoveWindow(mainTabDimensions);

            m_MainTab.ShowWindow(SW_HIDE);			
            m_directControlDlg.ShowWindow(SW_SHOW);			
            m_isInDirectControl = true;

			FUNCTION_EXIT;
            return;
		}

        // Normal tabbed window 
        //m_directControlDlg.ShowWindow(SW_HIDE);
        //m_MainTab.ShowWindow(SW_SHOW);
		
		CMainTabControl::ETabNumbers newTab = m_previousTab;

		// Find to the required page
        if (pageName==INFORMATION_PAGE)
		{
			newTab = CMainTabControl::InformationTab;			
		}
		else if (pageName==CONTROL_PAGE)
		{
			newTab = CMainTabControl::ControlTab;
		}
		else if (pageName==LIMITS_PAGE)
		{
			newTab = CMainTabControl::LimitsTab;
		}
		else if (pageName==TAG_PAGE)
		{
			newTab = CMainTabControl::TagTab;
		}
		else if (pageName==NOTES_PAGE)
		{
			newTab = CMainTabControl::NotesTab;
		}
		else if (pageName==INHIBIT_PAGE)
		{
			newTab = CMainTabControl::InhibitTab;
		}
		else if (pageName==OVERRIDE_PAGE)
		{
			newTab = CMainTabControl::OverrideTab;
		}
		else if (pageName==METER_PAGE)
		{
			newTab = CMainTabControl::MeterTab;
		}
		else
		{
			newTab = m_MainTab.getMinTabNumber();
		}

		if (m_equipment != NULL)
		{
			// if all data points belongs to the equipment have been properly configured
			if ( true == m_equipment->haveAllDataPointsConfigured() )
			{
				// if different tab selected
				if (newTab != m_previousTab)
				{
					// only proceed if previous tab indicates it has no unsaved data
					// and OK to change to different tab
					if ( ( false == m_MainTab.isUnsavedChangeOnTab ( m_previousTab ) ) || 
						 ( true == m_MainTab.isOkToChangeTab ( m_previousTab ) ) )
					{
						// change main tab from previous tab to the selected tab
						//m_MainTab.SetCurSel(newTab);
						m_MainTab.SetCurSelTab(newTab);

						// save selected tab for further reference
						m_previousTab = newTab;					
					}
					m_requestedTab = m_previousTab;
				}
			}
			else
			{
				m_requestedTab = newTab;
			}
		}

		FUNCTION_EXIT;
	}

	//
	//  called after datapoints of equipment are all created, run in GUI thread
	//
	void CInspectorPanelDlg::setEquipmentGUI()
	{
		FUNCTION_ENTRY ("setEquipment()");

		if ( 0 < m_equipment->getChildDataPointCount() && 
			true == m_equipment->haveAllDataPointsConfigured() ) 
		{
			// Update the tab control
			if(!this->m_isInDirectControl)
			{
					m_MainTab.setEquipment(m_equipment);
					PostMessage(LOAD_OTHER_PAGES,NULL,NULL);
			}

			m_directControlDlg.setEquipment(m_equipment);
		}
        else
        {
            // Need to update Status Tab for debugging
            m_MainTab.setStatusTabEquipment(m_equipment);
        }

		FUNCTION_EXIT;
	}

	//setEquipmentNoGUI, should run in MainThread
	bool CInspectorPanelDlg::setEquipmentNoGUI(std::string equipmentName)
		{
			FUNCTION_ENTRY ("setEquipmentNoGUI()");
			bool ret = false;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "CInspectorPanelDlg::setEquipmentNoGUI");
			if (m_equipment != NULL && m_equipment->getDataNode()->getEntityName() != equipmentName 
										  && m_equipment->getEquipmentName() != equipmentName )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"CInspectorPanelDlg::setEquipmentNoGUI set equipment from [%s] to equipment [%s]", m_equipment->getDataNode()->getEntityName().c_str(), equipmentName.c_str());
				
				destroyEquipment();
				AccessControlHelper::getInstance()->terminateAndWait();
				ret = true;//equipment changed
			}
	
			if (m_equipment == NULL)
			{
				try
				{
					m_equipment = new CEquipment(equipmentName);
					m_state		= CreateEquipment;
	
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"CInspectorPanelDlg::setEquipmentNoGUI new equipment [%s] created", equipmentName.c_str());
				}
				catch (TA_Base_Core::ScadaProxyException& e)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"CInspectorPanelDlg::setEquipmentNoGUI() - %s", e.what());
				}
			}
			return ret;
			FUNCTION_EXIT;
		}


	//
	// destroyEquipment
	//
	void CInspectorPanelDlg::destroyEquipment()
	{
        TA_ASSERT(m_equipment != NULL, "Try to destroy NULL equipment");

        delete m_equipment;
		m_equipment = NULL;
		m_MainTab.setEquipment(NULL);
		m_directControlDlg.setEquipment(NULL);
		
	}

	//
	// updateMainDialog
	//
	void CInspectorPanelDlg::updateMainDialog()
	{
		std::string asset = "";
		std::string label = "";
		//std::string title = TA_Base_Core::RunParams::getInstance().get( RPARAM_APPNAME );
		std::string title = _T("监控面板");;

		if (m_equipment != NULL)
		{
			// Update the window title with asset name
			DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();
			try
			{
				asset = dataNode->getAssetName();
				label = dataNode->getLabel();
			}
			catch(ScadaProxyException&)
			{
			}
		}
			
		if (!asset.empty())
		{
			title += " - ";
			title += asset;
		}

		setTitleInMainThread(title);

		// For normal tabbed window
        if (!m_isInDirectControl)
        {
            setEquipmentLabelInMainThread(label.c_str());
        }

		// Check resizing
		//resizeIfRequired();
	}

	//
	// onRunParamChange
	//
	void CInspectorPanelDlg::onRunParamChange(const std::string& name, const std::string& value)
	{
		FUNCTION_ENTRY ("onRunParamChange()");

        std::string* temporalName  = new std::string(name);
        std::string* temporalValue = new std::string(value);
        PostMessage(
            PROCESS_RUNPARAM_IN_MAIN_THREAD_MESSAGE,
            reinterpret_cast<UINT>(temporalName),
            reinterpret_cast<LONG>(temporalValue));

		FUNCTION_EXIT;
	}

	
	/////////////////////////////////////////////////////////////////////////////
	// CInspectorPanelDlg message handlers

	BEGIN_MESSAGE_MAP(CInspectorPanelDlg, TA_Base_Bus::TransActiveDialog)
	//{{AFX_MSG_MAP(CInspectorPanelDlg)
		ON_WM_PAINT()
		ON_WM_QUERYDRAGICON()
		ON_NOTIFY(NM_CLICK, IDC_MAINTAB, OnClickMaintab)
		ON_WM_CLOSE()
		ON_WM_SHOWWINDOW( )
	//}}AFX_MSG_MAP
		ON_MESSAGE (UPDATED_PROXY_MESSAGE, OnProxyEntityUpdate)
		ON_MESSAGE (LOAD_DATAPOINTS_MESSAGE, OnLoadDataPoints)
        ON_MESSAGE (UPDATE_LABEL_IN_MAIN_THREAD_MESSAGE, OnUpdateLabelInMainThread)
        ON_MESSAGE (PROCESS_RUNPARAM_IN_MAIN_THREAD_MESSAGE, OnProcessRunParamInMainThread)
        ON_MESSAGE (UPDATE_STATUS_WINDOW_MESSAGE, OnUpdateStatusWindow)
        ON_MESSAGE (LOAD_OTHER_PAGES,OnLoadOtherPages)
		ON_MESSAGE (UPDATE_PERMISSION, UpdatePermissions)
		ON_MESSAGE (LOAD_DATANODE_COMPLETE, OnLoadDataNodeCompleted)
		ON_MESSAGE (LOAD_DATAPOINT_COMPLETE, OnLoadDataPointCompleted)
		ON_WM_DESTROY()
		ON_BN_CLICKED(ID_HELP_IP, OnHelpButton) //TD15124 azenitha
	END_MESSAGE_MAP()

	//TD15124 azenitha++
	void CInspectorPanelDlg::OnHelpButton() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
	}
	//TD15124 ++azenitha

	
	void CInspectorPanelDlg::init()
	{

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
		"CInspectorPanelDlg::init should run in a different thread from GUI thread!");
		// RR Initialise our Rights Library
        TA_Base_Bus::RightsLibraryFactory rightsFactory;
		
		if ( TA_Base_Core::RunParams::getInstance().isSet("DebugAlwaysTrueRightsLib") )
		{
			m_rightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::ALWAYS_TRUE_RL);
		}
		else
		{
			m_rightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
		}
		
		// Set up the proxies
		TA_Base_Bus::ScadaProxyFactory::getInstance().setProxiesToControlMode();

        // Initialise the DirectControlDialog
		m_directControlDlg.Create(IDD_CONFIRM_DIRECT_CONTROL_DIALOG, this);
        m_directControlDlg.resetDialog();

		// Get the passed equipment and page details
		std::string equipment;
		equipment = TA_Base_Core::RunParams::getInstance().get(EQUIPMENT_RPARAM);	
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "CInspectorPanelDlg::init No equipment name passed");
		//TA_ASSERT (!equipment.empty(),"No equipment name passed");
#pragma omp parallel 
		{
#pragma omp single
			setEquipmentNoGUI(equipment);
#pragma omp master
			{
				// Get the passed value
				std::string value = TA_Base_Core::RunParams::getInstance().get(VALUE_RPARAM);
				if (!value.empty())
				{
					onRunParamChange(VALUE_RPARAM, value);
				}


				// Monitor any changes
				TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, EQUIPMENT_RPARAM);
				TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, SCREEN_RPARAM);
				TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, VALUE_RPARAM);
				TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

			}
		}
		std::string screenName = TA_Base_Core::RunParams::getInstance().get(SCREEN_RPARAM);
		if(screenName.size() > 0)
		{
			//this->setPage(screenName);
			m_requestedPage = screenName;
		}
	}
	//
	// OnInitDialog
	//
	BOOL CInspectorPanelDlg::OnInitDialog()
	{
		FUNCTION_ENTRY ("OnInitDialog()");

		CRect windowSize;
		GetWindowRect(&windowSize);

		TA_Base_Bus::ResizingProperties properties;
		properties.canMaximise = false;
		properties.maxHeight = -1;
		properties.maxWidth = -1;
		properties.minHeight = windowSize.bottom - windowSize.top;
		properties.minWidth = windowSize.right - windowSize.left;
		setResizingProperties(properties);


		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"CInspectorPanelDlg::OnInitDialog before setPage");

		

		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"CInspectorPanelDlg::OnInitDialog before TransActiveDialog::OnInitDialog()");

		TransActiveDialog::OnInitDialog();

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"CInspectorPanelDlg::OnInitDialog aft TransActiveDialog::OnInitDialog()");

		// Set the icon for this dialog.  The framework does this automatically
		//  when the application's main window is not a dialog
		SetIcon(m_hIcon, TRUE);			// Set big icon
		SetIcon(m_hIcon, FALSE);		// Set small icon		

		UnlockWindowUpdate();

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"CInspectorPanelDlg::OnInitDialog");


		FUNCTION_EXIT;
		return TRUE;  // return TRUE  unless you set the focus to a control
	}

	//
	// OnPaint
	//
	void CInspectorPanelDlg::OnPaint() 
	{
		FUNCTION_ENTRY ("OnPaint()");

		if (IsIconic())
		{
			CPaintDC dc(this); // device context for painting

			SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

			// Center icon in client rectangle
			int cxIcon = GetSystemMetrics(SM_CXICON);
			int cyIcon = GetSystemMetrics(SM_CYICON);
			CRect rect;
			GetClientRect(&rect);
			int x = (rect.Width() - cxIcon + 1) / 2;
			int y = (rect.Height() - cyIcon + 1) / 2;

			// Draw the icon
			dc.DrawIcon(x, y, m_hIcon);
		}
		else
		{
			TransActiveDialog::OnPaint();
		}

		FUNCTION_EXIT;
	}

	void CInspectorPanelDlg::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );
		if (m_equipment != NULL)
		{
			delete m_equipment;
			m_equipment = NULL;
		}

        CDialog::OnDestroy();       

        FUNCTION_EXIT;
   }
	//
	// OnQueryDragIcon
	//
	HCURSOR CInspectorPanelDlg::OnQueryDragIcon()
	{
		FUNCTION_ENTRY ("OnQueryDragIcon()");
		FUNCTION_EXIT;

		return (HCURSOR) m_hIcon;
	}

	//
	// OnCancel
	//
	void CInspectorPanelDlg::OnCancel() 
	{
		FUNCTION_ENTRY ("OnCancel()");
		if (m_equipment != NULL)
		{
			delete m_equipment;
			m_equipment = NULL;
		}

		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();

		FUNCTION_EXIT;
	}

	//
	// OnClickMaintab
	//
	void CInspectorPanelDlg::OnClickMaintab(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY ("OnClickMaintab()");

		*pResult = 0;

		// Check to see if we are allowed to change tabs
		CMainTabControl::ETabNumbers iTab = (CMainTabControl::ETabNumbers)m_MainTab.GetCurSel();

		if (m_MainTab.isUnsavedChangeOnTab(m_previousTab))
		{
			if (!m_MainTab.isOkToChangeTab(m_previousTab))
			{
				// Don't change tabs
				//m_MainTab.SetCurSel(m_previousTab);
				m_MainTab.SetCurSelTab(m_previousTab);
				return;
			}
		}

		m_previousTab = iTab;

		FUNCTION_EXIT;
	}

	//
	// OnProxyEntityUpdate, GUI thread
	//
	LRESULT CInspectorPanelDlg::OnProxyEntityUpdate (WPARAM wParam, LPARAM lParam)
	{
		FUNCTION_ENTRY ("OnProxyEntityUpdate()");
		if ((!::IsWindow(m_MainTab.m_hWnd)) || (m_equipment == NULL) )
		{
			return 0;
		}
		//equipment changed, shoud not update tab
		if(m_state != WaitUpdate)
		{
			return 0;
		}
		unsigned long key = (unsigned long)wParam;
		ScadaEntityUpdateType updateType = (ScadaEntityUpdateType)lParam;
		if (m_equipment->getDataNode()->getEntityKey() == key)
		{
			m_MainTab.updateDataNode(updateType);
			return 0;
		}

		if (false == m_isInDirectControl)
		{
			m_MainTab.updateDataPoints(key, updateType);
		}
		else
		{
			m_directControlDlg.updateControlState(key, updateType);
		}
	}

	//
	// SetupPermissions
	//
	void CInspectorPanelDlg::setupPermissions()
	{
		if (m_equipment == NULL)
			return;

		std::vector<AccessEntityInfo> entityKeys;	
        AccessEntityInfo entityInfo;

		// add the equipment data NODE first
		entityInfo.entityKey = m_equipment->getDataNode()->getEntityKey();
        if (entityInfo.entityKey == 0L)			
			return;

        entityInfo.locationKey = m_equipment->getDataNode()->getLocationKey();
        entityInfo.subSystemKey = m_equipment->getDataNode()->getPhysicalSubSystemKey();
        if (entityInfo.subSystemKey == 0)
        {
            entityInfo.subSystemKey = m_equipment->getDataNode()->getSubSystemKey();
        }

		entityKeys.push_back(entityInfo);

		// add all the data POINTS for this equipment
		if (m_equipment->getChildDataPointCount() > 0)
		{
			std::vector<DataPointProxySmartPtr*>& dpVector = m_equipment->getChildDataPointVector();
			for (std::vector<DataPointProxySmartPtr*>::iterator iter = dpVector.begin(); 
																		iter != dpVector.end(); iter++)
			{

				TA_Base_Bus::DataPointProxySmartPtr * pDataPointPtr = *iter;
				entityInfo.entityKey = (*pDataPointPtr)->getEntityKey();
                entityInfo.locationKey = (*pDataPointPtr)->getLocationKey();
                entityInfo.subSystemKey = (*pDataPointPtr)->getPhysicalSubSystemKey();
                if (entityInfo.subSystemKey == 0)
                {
                    entityInfo.subSystemKey = (*pDataPointPtr)->getSubSystemKey();
                }

                entityKeys.push_back(entityInfo);
			}
		}
		
		// initialise the access control permissions map
		std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
		AccessControlHelper::getInstance()->InitAccessControlMap(m_rightsMgr, sessionId, entityKeys);
		PostMessage(UPDATE_PERMISSION, 0, 0);
	}
	LRESULT CInspectorPanelDlg::OnLoadOtherPages (WPARAM wParam, LPARAM lParam)	
	{
		if(!this->m_equipment->isEntityBlank())
		{
			m_MainTab.diplayDataPointsOfBgPages();
		}
		//this->resizeIfRequired();
		return 0;
	}
	//
	// OnLoadDataPoints
	//
	LRESULT CInspectorPanelDlg::OnLoadDataPoints (WPARAM wParam, LPARAM lParam)
	{
		FUNCTION_ENTRY ("OnLoadDataPoints()");

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "CInspectorPanelDlg::OnLoadDataPoints");

		// Get the passed equipment and page details
		try
		{	
			if (m_equipment)
			{
				setupPermissions(); 
			}
       		else
		    {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_081009, "监控面板");
            }
		}
		catch (TA_Base_Core::ScadaProxyException& e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"loadDataPoints() - %s", e.what());

		}
		catch (...)
		{
			std::string message = "Error: Unknown exception when getting equipment details.";
			
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"loadDataPoints() - %s", message.c_str());
        }

		FUNCTION_EXIT;

		return 0;
	}
	LRESULT CInspectorPanelDlg::UpdatePermissions (WPARAM wParam, LPARAM lParam)
	{
		m_MainTab.updatePermissions();
		m_directControlDlg.updatePermissions();

		if (wParam == 1 )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"AccessControlHelper::getInstance()->terminateAndWait()");
			AccessControlHelper::getInstance()->terminateAndWait();
		}

		return 0;
	}

	//
	// resizeIfRequired
	//
	void CInspectorPanelDlg::resizeIfRequired()
	{
		FUNCTION_ENTRY ("resizeIfRequired()");

		if (m_heightDifference == 0)
		{
			CRect windowRect;
			GetWindowRect(&windowRect);

			CRect tabRect;
			m_MainTab.GetWindowRect(&tabRect);

			m_heightDifference = windowRect.Height()-tabRect.Height();
		}
		
		int yOffset = m_MainTab.resize();

		if (yOffset != 0)
		{
			// Move status group box
			CWnd *window = GetDlgItem(IDC_STATUS_GROUP_BOX);
			CRect statusGrpBoxRect;
			window->GetWindowRect(&statusGrpBoxRect);
			ScreenToClient(&statusGrpBoxRect);
			statusGrpBoxRect.OffsetRect(0, yOffset);
			window->SetWindowPos(&wndNoTopMost, statusGrpBoxRect.left, statusGrpBoxRect.top, 
												0, 0, SWP_NOSIZE|SWP_NOZORDER);
		}
	
		if (yOffset != 0)
		{
			// Move status box
			CWnd *window = GetDlgItem(IDC_STATUS);
			CRect statusBoxRect;
			window->GetWindowRect(&statusBoxRect);
			ScreenToClient(&statusBoxRect);
			statusBoxRect.OffsetRect(0, yOffset);
			window->SetWindowPos(&wndNoTopMost, statusBoxRect.left, statusBoxRect.top, 
												0, 0, SWP_NOSIZE|SWP_NOZORDER);
			m_statusBox.Invalidate(); 	
		}

		if (yOffset != 0)
		{
			// Move cancel button
			CWnd *window = GetDlgItem(IDCANCEL);
			CRect cancelRect;
			window->GetWindowRect(&cancelRect);
			ScreenToClient(&cancelRect);
			cancelRect.OffsetRect(0, yOffset);
			window->SetWindowPos(&wndNoTopMost, cancelRect.left, cancelRect.top, 
												0, 0, SWP_NOSIZE|SWP_NOZORDER);
			GetDlgItem(IDCANCEL)->Invalidate(); 
		}

		if (yOffset != 0)
		{
			// Move help button
			CWnd *window = GetDlgItem(ID_HELP_IP); //TD15124 azenitha
			CRect helpRect;
			window->GetWindowRect(&helpRect);
			ScreenToClient(&helpRect);
			helpRect.OffsetRect(0, yOffset);
			window->SetWindowPos(&wndNoTopMost, helpRect.left, helpRect.top, 
												0, 0, SWP_NOSIZE|SWP_NOZORDER);
			GetDlgItem(ID_HELP_IP)->Invalidate(); //TD15124 azenitha
		}

		// Resize window
		CRect windowRect, tabRect;
		GetWindowRect(&windowRect);		
		m_MainTab.GetWindowRect(&tabRect);

		// Stay on top and make sure it ll resize eventually
		SetWindowPos(&wndTopMost, 0, 0, windowRect.Width(), tabRect.Height()+m_heightDifference, SWP_NOMOVE|SWP_SHOWWINDOW);		
		Invalidate();
	
		FUNCTION_EXIT;
	}

    void CInspectorPanelDlg::setEquipmentLabelInMainThread(const std::string& label)
    {
        CWnd* window = GetDlgItem(IDC_EQUIPMENT_DESCRIPTION);
        std::string* temporalString = new std::string(label);
        PostMessage(
            UPDATE_LABEL_IN_MAIN_THREAD_MESSAGE,
            reinterpret_cast<UINT>(window),
            reinterpret_cast<LONG>(temporalString));
    }

    void CInspectorPanelDlg::setTitleInMainThread(const std::string& label)
    {
        std::string* temporalString = new std::string(label);
        PostMessage(
            UPDATE_LABEL_IN_MAIN_THREAD_MESSAGE,
            reinterpret_cast<UINT>(this),
            reinterpret_cast<LONG>(temporalString));
    }

    LRESULT CInspectorPanelDlg::OnUpdateLabelInMainThread(UINT wParam, LONG lParam)
    {
        std::string* temporalString = reinterpret_cast<std::string*>(lParam);
        CWnd* desiredLabel = reinterpret_cast<CWnd*>(wParam);

        if (temporalString)
        {
            if (desiredLabel)
            {
                CString description = temporalString->c_str();

                int pos = description.Find ( "&", 0 );
                if ( -1 != pos )
                {
                    description.Insert ( pos, "&" );
                }

                // Wrap the text around if it will not fit into the label (TD 11255)
                int charsWide = 0;
                SIZE sizeString;
                CRect clientRect;
                desiredLabel->GetClientRect( &clientRect);
                CDC* pdc = desiredLabel->GetDC();

                if (GetTextExtentExPoint( pdc->GetSafeHdc(), 
                    (LPCTSTR) description, description.GetLength(),
                    clientRect.Width(), &charsWide, NULL, &sizeString))
                {
                    if ( description.GetLength() > charsWide)
                    {
                        //  The string is too big to fit into the desiredLabel.
                        //  Default text wrapping will work if there is a space...

                        pos = description.Find ( " ", 0 );
                        if ( ( -1 == pos ) || (pos > charsWide) )
                        {
                            // ... but instead of a space, insert a linebreak to be sure.
                            description.Insert ( charsWide, "\n" );
                        }
                    }
                    desiredLabel->ReleaseDC( pdc);
                }

                desiredLabel->SetWindowText ( description );
            }
            delete temporalString;
        }
        return 0;
    }

    LRESULT CInspectorPanelDlg::OnProcessRunParamInMainThread(UINT wParam, LONG lParam)
    {
        std::string* temporalName  = reinterpret_cast<std::string*>(wParam);
        std::string* temporalValue = reinterpret_cast<std::string*>(lParam);
		
        try
        {
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "CInspectorPanelDlg::OnProcessRunParamInMainThread  tmpName: %s tmpValue: %s", 
				(*temporalName).c_str(),
				(*temporalValue).c_str()
				);
		    if (*temporalName==EQUIPMENT_RPARAM) 
		    {
		    	if(m_state == WaitUpdate)
		    	{
					if(setEquipmentNoGUI(*temporalValue))
	                {
						m_isInitialTab = false;
						std::string *statusLineStr = new std::string("正在改变设备到 ");
						(*statusLineStr) += (*temporalValue);
						PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
	                }
		    	}
				else
				{
					m_isEquipmentChanged = true;
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "ControlEntity changed, process later");
				}
		    }
		    else if (*temporalName==SCREEN_RPARAM)
		    {
			    setPage(*temporalValue);
		    }
		    else if (*temporalName == VALUE_RPARAM)
		    {
                m_directControlDlg.setNewValue(*temporalValue);
		    }		    
            else if (*temporalName == RPARAM_SESSIONID)
		    {
			    setupPermissions();
		    }
        }
        catch (...)
        {
        
        }

        delete temporalName;
        delete temporalValue;
        return 0;
    }

    std::string CInspectorPanelDlg::getEquipmentNameFromDataPointEntity(TA_Base_Core::IEntityData* entity)
    {

        // determine the equipment datanode

        // The approach is not the most efficient - 
        // 1. Obtain the datapoint's parentkey (dataNode's entityKey)
        // 2. Determine the corba name from the key
        // 3. Extract the objectName field (which is equivalent to the dataNode 
        //    entityName)
        // 4. Return the dataNode name
        // 5. CEquipment constructor takes the name and obtains the CorbaName
        //    for the second time.
        //
        // The decision for this approach is to minimise impact.

        std::string equipmentName("");

        unsigned long equipmentKey = entity->getParent();

        if ( equipmentKey!=0 )
        {
            try
            {
                TA_Base_Core::CorbaName equipmentCorbaName(
                    TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity(equipmentKey,false));
                equipmentName = equipmentCorbaName.getObjectName();
            }
            catch (...)
            {

       			// Tell user no data found
			    std::string message = "Warning: Failed to obtain equipment details.";
			    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "%s", message.c_str());

                // Nothing fatal - just return the NULL string
                return "";
            }
            
        }

        return equipmentName;
    }


	void CInspectorPanelDlg::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
    {
        // Find out the coordinates of the current window
        WINDOWPLACEMENT place;
        GetWindowPlacement(&place);

        int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int numScreens = GetSystemMetrics(SM_CMONITORS);

        // Determine the screen we are currently on.
        int currentScreen = place.rcNormalPosition.left / (screenWidth / numScreens);

        // If we have changed screen, then attempt to retrieve the available working area.

        if (currentScreen != m_currentScreen)
        {
            m_currentScreen = currentScreen;

            // Get the available working area from the control station
            try
            {
                m_workingArea = TA_Base_Bus::AppLauncher::getInstance().getRect(
                    TA_Base_Bus::TA_GenericGui::SCREEN_ALL, 
                    TA_Base_Bus::TA_GenericGui::AREA_MAINNAVIGATOR);
            }
            catch(TA_Base_Core::ApplicationException& ex)
            {
                // Log the catch, and set the workingArea rect to 0.
                std::stringstream errorMessage;
                errorMessage << "An exception was caught while attempting to retrieve the schematic area of the current"
                    << "screen. X coord = " << place.rcNormalPosition.left << ". Error Message:";
                LOG_EXCEPTION_CATCH(SourceInfo,"ApplicationException",errorMessage.str().c_str());
                LOGMORE(SourceInfo,ex.what());

                // Set the workingArea rect to 0
                m_workingArea.SetRectEmpty();
            }
        }

        // The resized window, represented by lpwndpos, cannot fall outside the schematic rectangle, so reset 
        // variables as necessary
        if (!m_workingArea.IsRectEmpty())
        {
            // Set the width
            if (lpwndpos->cx > m_workingArea.Width())
            {
                lpwndpos->cx = m_workingArea.Width();
            }

            // Set the height
            if (lpwndpos->cy > m_workingArea.Height())
            {
                lpwndpos->cy = m_workingArea.Height();
            }

            // Set the x-coordinate
            if (lpwndpos->x < m_workingArea.left)
            {
                lpwndpos->x = m_workingArea.left;
            }
            else if (lpwndpos->x + lpwndpos->cx > m_workingArea.right)
            {
                lpwndpos->x = m_workingArea.right - lpwndpos->cx;
            }

            // Set the y-coordinate
            if (lpwndpos->y < m_workingArea.top)
            {
                lpwndpos->y = m_workingArea.top;
            }
            else if (lpwndpos->y + lpwndpos->cy > m_workingArea.bottom)
            {
                lpwndpos->y = m_workingArea.bottom - lpwndpos->cy;
            }
        }

		//TD14564
		//set the window invalid and cause the window to be redrawn.
		Invalidate();   
		ShowWindow(SW_SHOW);
    }


	// TD #11930
	// Handle the PostMessage message to update the status window text.
	// The status text should be in wParam, and the timestamp in lParam
    LRESULT CInspectorPanelDlg::OnUpdateStatusWindow(UINT wParam, LONG lParam)
    {
		if(wParam==0)
		{
			CString str;
			str.LoadString(lParam);
			CInspectorPanelDlg* dlg = dynamic_cast<CInspectorPanelDlg*>(AfxGetApp()->m_pMainWnd);
			dlg->m_statusBox.SetWindowText(str);
		}
        std::string* statusText = reinterpret_cast<std::string*>(wParam);

        if(statusText)
        {
			if(statusText->length() > 0)
			{
				CInspectorPanelDlg* dlg = dynamic_cast<CInspectorPanelDlg*>(AfxGetApp()->m_pMainWnd);

				// use the timestamp if it's set, otherwise use the current time
				time_t timeT = ( lParam > 0 ) ? lParam : time (NULL);
				struct tm localTm;
				ACE_OS::localtime_r( &timeT, &localTm );

				// wenching++ (TD13872)
				// Follow the date\time format of Event Viewer:
				std::string month="";
				switch(localTm.tm_mon)
				{
				//  [3/14/2010 benxun]
				/*
				case 0 : month = "JAN";
						 break;
				case 1 : month = "FEB";
						 break;
				case 2 : month = "MAR";
					     break;
				case 3 : month = "APR";
					     break;
				case 4 : month = "MAY";
					     break;
				case 5 : month = "JUN";
					     break;
				case 6 : month = "JUL";
					     break;
				case 7 : month = "AUG";
					     break;
				case 8 : month = "SEP";
					     break;
				case 9 : month = "OCT";
					     break;
				case 10 : month = "NOV";
					     break;
				case 11 : month = "DEC";
				*/
				case 0 : month = "1";
					break;
				case 1 : month = "2";
					break;
				case 2 : month = "3";
					break;
				case 3 : month = "4";
					break;
				case 4 : month = "5";
					break;
				case 5 : month = "6";
					break;
				case 6 : month = "7";
					break;
				case 7 : month = "8";
					break;
				case 8 : month = "9";
					break;
				case 9 : month = "10";
					break;
				case 10 : month = "11";
					break;
				case 11 : month = "12";
				//  [3/14/2010 benxun]
					     break;
				}
				
				std::stringstream timestampedStatusText;
				//  [3/14/2010 benxun]
				/*
				timestampedStatusText << ( localTm.tm_mday < 10 ? "0" : "" )
									  << localTm.tm_mday << "-"
					                  << month.c_str() << "-"
									  << localTm.tm_year+1900 << " "
									  << ( localTm.tm_hour < 10 ? "0" : "" ) 
									  << localTm.tm_hour << ":" 
									  << ( localTm.tm_min < 10 ? "0" : "" ) 
									  << localTm.tm_min << ":" 
									  << ( localTm.tm_sec < 10 ? "0" : "" ) 
									  << localTm.tm_sec << " - "
									  << *statusText;
			    */
				timestampedStatusText << localTm.tm_year+1900 << "-"
									  << month.c_str() << "-"
									  << ( localTm.tm_mday < 10 ? "0" : "" )
									  << localTm.tm_mday << " "									  
									  << ( localTm.tm_hour < 10 ? "0" : "" ) 
									  << localTm.tm_hour << ":" 
									  << ( localTm.tm_min < 10 ? "0" : "" ) 
									  << localTm.tm_min << ":" 
									  << ( localTm.tm_sec < 10 ? "0" : "" ) 
									  << localTm.tm_sec << " - "
									  << *statusText;
				//++wenching (TD13872)

				// Set the new status text
				dlg->m_statusBox.SetWindowText(timestampedStatusText.str().c_str());
			}

            delete statusText;
        }

		return 0;
	}

	void CInspectorPanelDlg::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"CInspectorPanelDlg::OnShowWindow bShow %d, nStatus: %d",
			  bShow,  nStatus);
	}
	void CInspectorPanelDlg::OnClose() 
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,"IP EXIT"); 
		if (m_equipment != NULL)
		{
			delete m_equipment;
			m_equipment = NULL;
		}
		CInspectorPanelApp::setTeminating();
		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();
	}
	
	void CInspectorPanelDlg::resetList()
	{
		m_MainTab.resetList();
	}

	LRESULT CInspectorPanelDlg::OnLoadDataNodeCompleted(WPARAM wParam, LPARAM lParam)
	{
		this->updateMainDialog();
		m_state = LoadDataNode;
		return 0;
	}
	LRESULT CInspectorPanelDlg::OnLoadDataPointCompleted(WPARAM wParam, LPARAM lParam)
	{
		if (m_equipment)
		{
			setupPermissions(); 
		}
		
		this->setEquipmentGUI();
		if( !m_isInitialTab )
		{
			// if user requests for Control tab
			if ( m_requestedPage == CONTROL_PAGE )
			{
				// if the equipment has no control data points
				if ( false == m_equipment->hasControlPoint() )
				{
					m_requestedPage = NULL_PAGE;
				}
			}
						// if user requests for Meter tab
			else if ( m_requestedPage == METER_PAGE )
			{
				// if the equipment has no metered data points
				if ( false == m_equipment->hasMeteredPoint() )
				{
					m_requestedPage = NULL_PAGE;
				}
			}

			setPage ( m_requestedPage );
			m_isInitialTab = true;
		}
		m_state = WaitUpdate;
		if(m_isEquipmentChanged)
		{
			std::string equipment;
			equipment = TA_Base_Core::RunParams::getInstance().get(EQUIPMENT_RPARAM);
			onRunParamChange(EQUIPMENT_RPARAM, equipment);
			m_isEquipmentChanged = true;
		}
		return 0;
	}
}



/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/QuickToolComboEx.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class extends the CComboBoxEx class. It is used for a combo box which contains
  * quick launch entities. Those are entities that may have a control application,
  * map display, schematic display or camera associated with them.
  * 
  * This class controls loading the entities/devices into the list. It also 
  * decides which buttons should be available once a device is selected. It will
  * perform the actual functions required when the buttons are pressed as well.
  * 
  */

#include "app/system_control/control_station/src/stdafx.h"
#include "app/system_control/control_station/src/QuickToolComboEx.h"
#include "app/system_control/control_station/src/ControlStationBar.h"
#include "app/system_control/control_station/src/IProcessManager.h"
#include "app/system_control/control_station/src/SessionManager.h"
#include "app/system_control/control_station/src/ControlStationConstants.h"
#include "app/system_control/control_station/src/ControlStationRootDialog.h"
#include "app/system_control/control_station/src/Application.h"

#include <sstream>

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/application_types/src/apptypes.h"
#include "bus\security\access_control\actions\src\AccessControlledActions.h"
#include "core/data_access_interface/entity_access/src/QuickFindEntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IQuickFindEntity.h"
#include "core\data_access_interface\src\ResourceAccessFactory.h"
#include "core/data_access_interface/src/IGui.h"
#include "core\data_access_interface\src\IResource.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/ApplicationException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::IQuickFindEntity;
using TA_Base_Core::QuickFindEntityAccessFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::ApplicationException;
using TA_Base_Core::DebugUtil;



// The key representing an invalid application type
const unsigned long CQuickToolComboEx::INVALID_APPTYPE = 0;

// This is the width of this combo box.
const int CQuickToolComboEx::COMBO_BOX_WIDTH = 320;

// This is the width and height of the icons to display on the associated buttons.
const int CQuickToolComboEx::ICON_WIDTH  = 20;
const int CQuickToolComboEx::ICON_HEIGHT = 20;

// This is the initial size the combo box should be (ie number of items)
const int CQuickToolComboEx::NUM_INITIAL_ITEMS = 1024;
// This is the amount the combo box should grow by each time it runs out of
// space (number of items)
const int CQuickToolComboEx::NUM_TO_GROW_BY = 512;

// This is the runtime parameter name for the camera entity
const std::string CQuickToolComboEx::CONTROLENTITY_PARAMNAME = "--control-entity";

// This is the runtime parameter name for the preset
const std::string CQuickToolComboEx::PRESET_PARAMNAME = "--preset";

/////////////////////////////////////////////////////////////////////////////
// CQuickToolComboEx

CQuickToolComboEx::CQuickToolComboEx(ControlStationBar* pParent)
: m_controlStationBar(pParent)
{
}


CQuickToolComboEx::~CQuickToolComboEx()
{
    try
    {
        for (std::vector<IQuickFindEntity*>::iterator iter = m_entities.begin();
             iter != m_entities.end();
             ++iter)
        {
            if (*iter != NULL)
            {
                delete *iter;
                *iter =  NULL;
            }
        }
    }
    catch ( ... )
    {
    }
}


BEGIN_MESSAGE_MAP(CQuickToolComboEx, CComboBoxEx)
	//{{AFX_MSG_MAP(CQuickToolComboEx)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuickToolComboEx message handlers

void CQuickToolComboEx::PreSubclassWindow() 
{
	CComboBoxEx::PreSubclassWindow();
	
	SetDroppedWidth(COMBO_BOX_WIDTH);
	m_imageList = new CImageList();
	m_imageList->Create(ICON_WIDTH,ICON_HEIGHT, TRUE, 4,4);
	SetExtendedStyle(0,CBES_EX_CASESENSITIVE );
	SetImageList(m_imageList);
	SetExtendedUI();

}


void CQuickToolComboEx::loadEntities()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loadEntities");
    Clear();

    // Initialise the storage for maximum efficiency when inserting items
	InitStorage(NUM_INITIAL_ITEMS, NUM_TO_GROW_BY);

	COMBOBOXEXITEM tmpComboItem;

	tmpComboItem.mask = CBEIF_DI_SETITEM | CBEIF_TEXT | CBEIF_IMAGE | CBEIF_INDENT;
	tmpComboItem.mask = tmpComboItem.mask | CBEIF_SELECTEDIMAGE  ; 
	tmpComboItem.mask = tmpComboItem.mask | CBEIF_OVERLAY ; 
	tmpComboItem.mask = tmpComboItem.mask | CBEIF_LPARAM;

    try
    {
        m_entities = QuickFindEntityAccessFactory::getInstance().getAllQuickFindEntities();
    }
    catch ( const DatabaseException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not load a list of quick launch entities");
        LOG( SourceInfo, DebugUtil::FunctionExit, "loadEntities");
        return;
    }
    catch ( const DataException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not load a list of quick launch entities");
        LOG( SourceInfo, DebugUtil::FunctionExit, "loadEntities");
        return;
    }


    for (unsigned int i = 0; i < m_entities.size(); ++i)
    {				
		// Insert the image into the list and store, get the image from the LauchApp
        unsigned long appType(INVALID_APPTYPE);
        try
        {
            appType = m_entities[i]->getGuiApplicationKey();
        }
        catch ( const DatabaseException& )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not retrieve the gui application key for the quick launch entity");
        }
        catch ( const DataException& )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not retrieve the gui application key for the quick launch entity");
        }

        int iconIndex(-1);
        if ( appType != INVALID_APPTYPE )
        {
            // Check to see if the image is already inserted. If it not then we need to add it
		    if (m_apptypeToIndex.end() == m_apptypeToIndex.find(appType))
            {
                // It's a newbie
                IProcessManager* pm = SessionManager::getInstance().getProcessManager();
                if (pm != NULL)
                {
                    IProcessManager::ApplicationMap& pApps = pm->getApplicationObjects( );
                    for (IProcessManager::ApplicationMap::iterator iter = pApps.begin();
                         iter != pApps.end();
                         ++iter )
                    {
                        if( iter->second->getGuiDatabaseEntry().getKey() == appType )
                        {
				            iconIndex = m_imageList->Add(iter->second->getHotIcon());
				            m_apptypeToIndex[appType] = iconIndex;
                            break;
                        }
			        }
                }
            }
		    else
		    {
                // It already exists so we can just use that
		        iconIndex = m_apptypeToIndex[appType];
		    }
        }

        std::string name("");
        try
        {
            name = m_entities[i]->getName();
            name+= " - ";
            name+= m_entities[i]->getDescription();
        }
        catch ( const DatabaseException& )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not retrieve the name for the quick launch entity");
        }
        catch ( const DataException& )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not retrieve the name for the quick launch entity");
        }

		tmpComboItem.iItem = i;
		tmpComboItem.iIndent = 0;
		tmpComboItem.iImage = iconIndex;
		tmpComboItem.iSelectedImage = iconIndex;
		tmpComboItem.iOverlay = iconIndex;
		tmpComboItem.lParam = (DWORD)i;
		tmpComboItem.pszText = _strdup(name.c_str());

        m_namelist.push_back(name);
		
		InsertItem(&tmpComboItem);
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "loadEntities");
}


void CQuickToolComboEx::OnSelchange()//NMHDR* pNMHDR, LRESULT* pResult) 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "OnSelchange");

	// Disable all buttons by default
	m_controlStationBar->m_launchSchematicDisplay.EnableWindow(FALSE);
	m_controlStationBar->m_launchCameraView.EnableWindow(FALSE);

    int pos(GetCurSel());

	if (CB_ERR == pos)
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "OnSelchange");
        return;
    }

    IQuickFindEntity* selected = m_entities[ GetItemData(pos) ];

    if (!selected->getAlarmDisplay().empty())
    {
        m_controlStationBar->m_launchSchematicDisplay.EnableWindow(TRUE);
    }
    // if there is a camera associated with this entity and the operator has
    // the rights to control it, then enable the control
    if (!selected->getCameraAssociation().empty())
    {
        TA_Base_Core::IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(selected->getKey());
        if (resource != NULL)
        {
            unsigned int resourceKey = resource->getKey();
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "IResource key = %d", resourceKey);
            delete resource;
            if (SessionManager::getInstance().isActionPermitted(TA_Base_Bus::aca_CONTROL_CAMERA, resourceKey) == TA_Base_App::TA_ControlStation::PERMITTED)
            {
                m_controlStationBar->m_launchCameraView.EnableWindow(TRUE);
            }
        }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Cannot find the resource key for control station, so not going to enable the Camera View button");
        }
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "OnSelchange");
}


void CQuickToolComboEx::cameraViewPressed()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "cameraViewPressed");
    // Assign a camera to a monitor and move it to the preset if configured

	// Get the current selection
	int pos(GetCurSel());

	// return if there is nothing selected
	if (CB_ERR == pos)
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "cameraViewPressed");
		return;
    }

	// Get the data and find the matching entity in the vector
    IQuickFindEntity* selected = m_entities[ GetItemData(pos) ];
    
	// If NULL return
	if (NULL == selected)
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "cameraViewPressed");
		return;
    }

    std::ostringstream oss;
    // extra command for launching the camera controller and move the camera 
    // to a particular preset
    oss << CONTROLENTITY_PARAMNAME << "=" << selected->getCameraAssociation() << " " 
        << PRESET_PARAMNAME << "=" << selected->getCameraPresetAssociation();

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Extra command is %s", oss.str().c_str());

    // launch the duty manager on the first screen
    RECT boundaryRect = SessionManager::getInstance().getScreenPositioning().getRect(
        TA_Base_Bus::TA_GenericGui::SCREEN_FIRST, 
        TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC);
    SessionManager::getInstance().getProcessManager()->launchApplication(
        CCTV_GUI_APPTYPE, 
        oss.str().c_str(), 
        TA_Base_Bus::TA_GenericGui::POS_BOUNDED, 
        TA_Base_Bus::TA_GenericGui::ALIGN_FIT, 
        TA_Base_Bus::TA_GenericGui::DEFAULT_RECT, 
        boundaryRect);

    LOG( SourceInfo, DebugUtil::FunctionExit, "cameraViewPressed");
}


void CQuickToolComboEx::viewSchematicDisplayPressed()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "viewSchematicDisplayPressed");
    // Load an alarm view into GraphWorX

	// Get the current selection
	int pos(GetCurSel());

	// return if there is nothing selected
	if (CB_ERR == pos)
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "viewSchematicDisplayPressed");
		return;
    }

	// Get the data and find the matching entity in the vector
    IQuickFindEntity* selected = m_entities[ GetItemData(pos) ];
    
	// If NULL return
	if (NULL == selected)
    {
        LOG( SourceInfo, DebugUtil::FunctionExit, "viewSchematicDisplayPressed");
		return;
    }

    try
    {
        std::string display = selected->getAlarmDisplay();
        if (!display.empty())
        {
            // We must pass a x coordinate so the GraphWorX instance running knows it is the one that should load the display
            // If we pass in the x coordinate of where we launch applications then this should be sufficient
            RECT rect = SessionManager::getInstance().getScreenPositioning().getAppRectNextToLaunchBar();

            DisplayInformation* info = new struct DisplayInformation;
            if (info != NULL)
            {
                info->display = CString(display.c_str());
                info->asset = "";
                info->position = (LPARAM)rect.left;
                ControlStationRootDialog::getInstance().PostMessage(WM_LAUNCH_DISPLAY, (WPARAM)info);
                LOG( SourceInfo, DebugUtil::FunctionExit, "viewSchematicDisplayPressed");
                return;
            }
        }
    }
    catch ( const DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the display for entity with pkey = %lu", selected->getKey() );
    }
    catch ( const DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the display for entity with pkey = %lu", selected->getKey() );
    }

    CString itemName;
    TA_VERIFY(itemName.LoadString(IDS_DISPLAY)!=0, "Unable to load IDS_DISPLAY");
    TA_Base_Bus::TransActiveMessage userMsg;
    userMsg << itemName.GetBuffer(itemName.GetLength());
    userMsg.showMsgBox(IDS_UE_020048);
    
    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "display or display info was NULL so we could not load a schematic");
    LOG( SourceInfo, DebugUtil::FunctionExit, "viewSchematicDisplayPressed");
}


void CQuickToolComboEx::OnEditchange() 
{
    // For some reason the item isn't selected when you type in CComboBoxEx, 
    // so, we have to it ourselves

    // If user has not typed in the control's edit box, we don't need to select it
    if (!HasEditChanged())
    {
        return;
    }
    
    CString s;
    // Get the typed text
    GetWindowText(s);
    std::string str(s);

    if (s.IsEmpty())
    {
        SetCurSel(CB_ERR);
        return;
    }

    IF_LOCAL_TEST
    {
        if ("duty" == str || "test duty" == str)
        {
            LocalTest::test_duty();
            return;
        }
        if ("task manager" == str || "test task manager" == str || "test task mgr" == str || "task mgr" == str)
        {
            LocalTest::test_task_manager();
            return;
        }
    }

    if (m_namelist.empty())
    {
        return;
    }

    unsigned int high = m_namelist.size() - 1;
    unsigned int low = 0;
    int hit = CB_ERR;

    // FindString doesn't work for CComboBoxEx, so we have to do the 
    // comparison ourselves
    // The m_entities list is ordered by name, so we can just do a binary
    // search to see which one should be selected from the list
    // If we have entered something that comes alphabetically before or after
    // what we have in the list, then just select the first or the last respectively
    if (str.compare(m_namelist[low]) <= 0) 
    {
        hit = 0;
    }
    else if (str.compare(m_namelist[high]) >= 0)
    {
        hit = high;
    }
    else
    {
        unsigned int probe;
        int res;
        while ((high - low) > 1)
        {
            probe = (high + low) / 2;

            // if str is shorter than or is before what we're comparing or 
            // then the value will be negative; 0 if equal; positive otherwise
            res = str.compare(m_namelist[probe]);
            if (res == 0) 
            {
                high = probe;
                break;
            }
            else if (res < 0) 
            {
                high = probe;
            }
            else
            {
                low = probe;
            }
        }
        hit = high;
    }

    ShowDropDown();	
    if (GetCurSel() != hit)
    {
        SetCurSel(hit);
    }
}

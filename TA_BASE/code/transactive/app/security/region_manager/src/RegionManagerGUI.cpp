/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionManagerGUI.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class provides an interface between GenericGUI and Region Manager. 
  * GenericGUI uses this class the app to various system changes and requests.
  *
  */

#include "StdAfx.h"
#include "app/security/region_manager/src/RegionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using namespace TA_Base_Bus;

RegionManagerGUI::RegionManagerGUI()
:
AbstractDialogGUI(IDD_REGION_MANAGER_DLG)
{
    FUNCTION_ENTRY("RegionManagerGUI()");
    FUNCTION_EXIT;
}


RegionManagerGUI::~RegionManagerGUI()
{
    FUNCTION_ENTRY("~RegionManagerGUI()");
    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// AbstractGUIApplication operations

void RegionManagerGUI::createApplicationObject()
{
    FUNCTION_ENTRY("createApplicationObject()");

    try
    {
		//TD18095, jianghp, to fix the performance of showing manager application
		RegionManagerDlg * pDlg = new RegionManagerDlg(*this);
		//TD18095, jianghp, to fix the performance of showing manager application
        setApplicationWnd(*pDlg);

        FUNCTION_EXIT;
        return;  
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        MessageBox::error("Unable to load duty details.", ex);
    }
    catch (...)
    {
    }

    TA_THROW(TA_Base_Core::GenericGUIException("Region Manager initialisation failed.", 
                                          TA_Base_Core::GenericGUIException::INITIALISATION_ERRORS));
}


void RegionManagerGUI::prepareForClose()
{
    FUNCTION_ENTRY("prepareForClose()");

    try
    {
        saveAllUserSettings();
    }
    catch (const TA_Base_Core::UserSettingsException& ex)
    {
        MessageBox::error("Failed to save user settings.", ex);
    }

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// IGUIApplication operations

unsigned long RegionManagerGUI::getApplicationType()
{
    FUNCTION_ENTRY("getApplicationType()");
    FUNCTION_EXIT;

    return REGION_MANAGER_GUI_APPTYPE;
}


void RegionManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
{
    FUNCTION_ENTRY("checkEntity(TA_Base_Core::IEntityData* guiEntity)");

    TA_ASSERT(guiEntity, "GUI entity pointer is NULL");

    if (guiEntity->getType() != TA_Base_Core::RegionManagerEntityData::getStaticType())
    {
        TA_THROW(TA_Base_Core::GenericGUIException("Entity configuration is not of type Region Manager", 
                                              TA_Base_Core::GenericGUIException::ENTITY_CONFIGURATION_INVALID));
    }

    // Entity config OK
    setGuiEntity(*guiEntity);

    FUNCTION_EXIT;
}


void RegionManagerGUI::checkCommandLine()
{
    FUNCTION_ENTRY("checkCommandLine()");
    FUNCTION_EXIT;
}


void RegionManagerGUI::entityChanged(const std::vector<std::string>& changes)
{
    FUNCTION_ENTRY("entityChanged(const std::vector<std::string>& changes)");
    FUNCTION_EXIT;
}


void RegionManagerGUI::serverIsDown()
{
    FUNCTION_ENTRY("serverIsDown()");
    FUNCTION_EXIT;
}


void RegionManagerGUI::serverIsUp()
{
    FUNCTION_ENTRY("serverIsUp()");
    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// IGUIAccess operations

void RegionManagerGUI::loadAllUserSettings()
{
    FUNCTION_ENTRY("loadAllUserSettings()");

    // First check if we should be loading the user settings.
    if (TA_Base_Core::RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
    {
        setWindowPosition();

        FUNCTION_EXIT;
        return;
    }

    if (getUserSetting() == NULL)
    {
        setUserSetting(*new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType())); // AbstractGUIApplication cleans this up
    }

    // Restore user settings.
    loadPositionSettings();

    FUNCTION_EXIT;
}


void RegionManagerGUI::saveAllUserSettings()
{
    FUNCTION_ENTRY("saveAllUserSettings()");

    // First check if we should be saving the user settings.
    if (TA_Base_Core::RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
    {
        FUNCTION_EXIT;
        return;
    }

    if (getUserSetting() == NULL)
    {
        setUserSetting(*new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType())); // AbstractGUIApplication cleans this up
    }
    
    // Save user settings.
    savePositionSettings();

    FUNCTION_EXIT;
}

//TD18095, jianghp
void RegionManagerGUI::onInitGenericGUICompleted()
{
	CWaitCursor cur;
	// Initialise the contents of the duty data store. Tell it to ignore all 
	// information relating to system operators and non-physical subsystems,
	// as we do not want to display these.
	TA_Base_Bus::DutyDataStore::populate(true, true);	
	
	CWnd *pWnd = getApplicationWnd();
	pWnd->LockWindowUpdate();
	TransActiveDialog * pDlg = dynamic_cast<TransActiveDialog * >(pWnd);
	TA_ASSERT(pDlg != NULL, "the application window is NULL!");
	pDlg->init();
	pDlg->UnlockWindowUpdate();
	pDlg->UpdateWindow();
}

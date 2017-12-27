/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/src/DutyManagerGUI.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/07/16 11:00:18 $
  * Last modified by:  $Author: liwei.gong $
  *
  * This class provides an interface between GenericGUI and Duty Manager. 
  * GenericGUI uses this class the app to various system changes and requests.
  *
  */

#include "StdAfx.h"
#include "app/security/DutyManager/src/DutyManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using namespace TA_Base_Bus;

DutyManagerGUI::DutyManagerGUI()
:
AbstractDialogGUI(IDD_DUTY_MANAGER_DLG)
{
    FUNCTION_ENTRY("DutyManagerGUI()");
    FUNCTION_EXIT;
}


DutyManagerGUI::~DutyManagerGUI()
{
	LOG (SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "~EventViewerGUI");

	try
	{
		CWnd* application = getApplicationWnd();
		if (application != NULL)
		{
			delete application;
		}
	}
	catch( ... )
	{

	}

	LOG (SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "~EventViewerGUI");
}


/////////////////////////////////////////////////////////////////////////////
// AbstractGUIApplication operations

void DutyManagerGUI::createApplicationObject()
{
    FUNCTION_ENTRY("createApplicationObject()");

    try
    {
        // Initialise the contents of the duty data store. Tell it to ignore all 
        // information relating to system operators and non-physical subsystems,
        // as we do not want to display these.
        //TD17082++ - the dutydatastore will now be populated right after the manager has launched
        //TA_Base_Bus::DutyDataStore().populate(true, true); 
		DutyManagerDlg *pDlg = new DutyManagerDlg(*this);
		setApplicationWnd(*pDlg);

        FUNCTION_EXIT;
        return;  
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        //MessageBox::error("Unable to load duty details.", ex);
		MessageBox::error("", ex, IDS_UW_670009);
    }
    catch (...)
    {
    }

    TA_THROW(TA_Base_Core::GenericGUIException("Duty Manager initialisation failed.", 
                                          TA_Base_Core::GenericGUIException::INITIALISATION_ERRORS));
}
void DutyManagerGUI::terminateGUI()
{
	
	FUNCTION_ENTRY("terminateGUI");

	this->prepareForClose();

	// Close all transactive popup messages
	TransActiveMessage::closeAllMessages();

	// Closing the main window may not be enough if there are some other
	// modal (or modeless) dialogs floating around...
	// So we enumerate all top level windows. The enumeration handler
	// will determine if they were created by our process, and if so
	// it will destroy them.
	::EnumWindows(MyWndEnumHandler, ::GetCurrentProcessId());

	FUNCTION_EXIT;
}
BOOL CALLBACK DutyManagerGUI::MyWndEnumHandler(HWND hWnd, LPARAM lParam)
{
	// lparam contains the current process' ID
	// this saves us having to query it for every call...
	DWORD dwOurProcessID = static_cast<DWORD>(lParam);

	// find out which process & thread created this window
	DWORD dwThisWindowProcessID;
	DWORD dwThisWindowThreadID;
	dwThisWindowThreadID = ::GetWindowThreadProcessId(hWnd, &dwThisWindowProcessID);

	// if it's our process
	if (dwOurProcessID == dwThisWindowProcessID)
	{
		// Close the window...
		// The method to use depends on the type of window:
		// modal dialogs should be ended with ::EndDialog(),
		// other windows with ::DestroyWindow().
		// I haven't found a fireproof way of determining
		// if a window is a modal dialog!
		// Calling ::EndDialog() on a modeless window is OK, as
		// long as we also call ::DestroyWindow() after.
		/*
		::EndDialog(hWnd,0);
		::DestroyWindow(hWnd);
		*/
		::PostMessage(hWnd,WM_CLOSE, NULL, NULL);

		// Our GUIs windows should PostQuitMessage() when they
		// get a WM_DESTROY but I don't trust them to do so,
		// so let's do it for them...
		::PostThreadMessage(dwThisWindowThreadID,WM_QUIT,0,0);
	}

	return TRUE;
}
void DutyManagerGUI::prepareForClose()
{
    FUNCTION_ENTRY("prepareForClose()");

    try
    {
        saveAllUserSettings();
    }
    catch (const TA_Base_Core::UserSettingsException& ex)
    {
        //MessageBox::error("Failed to save user settings.", ex);
		MessageBox::error("", ex, IDS_UW_670010);
    }

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// IGUIApplication operations

unsigned long DutyManagerGUI::getApplicationType()
{
    FUNCTION_ENTRY("getApplicationType()");
    FUNCTION_EXIT;

    return REGION_MANAGER_GUI_APPTYPE;
}


void DutyManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
{
    FUNCTION_ENTRY("checkEntity(TA_Base_Core::IEntityData* guiEntity)");

    TA_Base_Core::DutyManagerEntityData* dmEntityData = dynamic_cast<TA_Base_Core::DutyManagerEntityData*>(guiEntity);
	TA_ASSERT(guiEntity  != NULL, "the GUI entity should not be NULL! ");
    // Store away Duty Manager parameters for future reference.
    if (dmEntityData->useLocationLabel())
    {
        TA_Base_Core::RunParams::getInstance().set(RPARAM_USELOCATIONLABEL, "enable"); 
    }
    if (!dmEntityData->getNormalDutyColour().empty())
    {
        TA_Base_Core::RunParams::getInstance().set( RPARAM_NORMALDUTYCOLOUR, dmEntityData->getNormalDutyColour().c_str() );
    }
    if (!dmEntityData->getDelegatedDutyColour().empty())
    {
        TA_Base_Core::RunParams::getInstance().set( RPARAM_DELEGATEDDUTYCOLOUR, dmEntityData->getDelegatedDutyColour().c_str() );
    }
    if (!dmEntityData->getDegradedDutyColour().empty())
    {
        TA_Base_Core::RunParams::getInstance().set( RPARAM_DEGRADEDDUTYCOLOUR, dmEntityData->getDegradedDutyColour().c_str() );
    }
    if (!dmEntityData->getUnallocatedSubsystemColour().empty())
    {
        TA_Base_Core::RunParams::getInstance().set( RPARAM_UNALLOCATEDSUBSYSTEMCOLOUR, dmEntityData->getUnallocatedSubsystemColour().c_str() );
    }

    // Entity config OK
    setGuiEntity(*guiEntity);

    FUNCTION_EXIT;
}


void DutyManagerGUI::checkCommandLine()
{
    FUNCTION_ENTRY("checkCommandLine()");
    FUNCTION_EXIT;
}


void DutyManagerGUI::entityChanged(const std::vector<std::string>& changes)
{
    FUNCTION_ENTRY("entityChanged(const std::vector<std::string>& changes)");
    FUNCTION_EXIT;
}


void DutyManagerGUI::serverIsDown()
{
    FUNCTION_ENTRY("serverIsDown()");
    FUNCTION_EXIT;
}


void DutyManagerGUI::serverIsUp()
{
    FUNCTION_ENTRY("serverIsUp()");
    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// IGUIAccess operations

void DutyManagerGUI::loadAllUserSettings()
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


void DutyManagerGUI::saveAllUserSettings()
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
void DutyManagerGUI::onInitGenericGUICompleted()
{
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] begin DutyManagerGUI::onInitGenericGUICompleted");
	TA_Base_Core::IEntityData* guiEntity = getGUIEntity();
    TA_Base_Core::DutyManagerEntityData* dmEntityData = dynamic_cast<TA_Base_Core::DutyManagerEntityData*>(guiEntity);
	TA_ASSERT(guiEntity  != NULL, "the GUI entity should not be NULL! ");
    // Store away Duty Manager parameters for future reference.
    if (dmEntityData->useLocationLabel())
    {
        TA_Base_Core::RunParams::getInstance().set(RPARAM_USELOCATIONLABEL, "enable"); 
    }
    if (!dmEntityData->getNormalDutyColour().empty())
    {
        TA_Base_Core::RunParams::getInstance().set( RPARAM_NORMALDUTYCOLOUR, dmEntityData->getNormalDutyColour().c_str() );
    }
    if (!dmEntityData->getDelegatedDutyColour().empty())
    {
        TA_Base_Core::RunParams::getInstance().set( RPARAM_DELEGATEDDUTYCOLOUR, dmEntityData->getDelegatedDutyColour().c_str() );
    }
    if (!dmEntityData->getDegradedDutyColour().empty())
    {
        TA_Base_Core::RunParams::getInstance().set( RPARAM_DEGRADEDDUTYCOLOUR, dmEntityData->getDegradedDutyColour().c_str() );
    }
    if (!dmEntityData->getUnallocatedSubsystemColour().empty())
    {
        TA_Base_Core::RunParams::getInstance().set( RPARAM_UNALLOCATEDSUBSYSTEMCOLOUR, dmEntityData->getUnallocatedSubsystemColour().c_str() );
    }

	CWnd *pWnd = getApplicationWnd();
	pWnd->LockWindowUpdate();
	DutyManagerDlg * pDlg = dynamic_cast<DutyManagerDlg * >(pWnd);
	TA_ASSERT(pDlg != NULL, "the application window is NULL!");
		
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyManagerGUI::onInitGenericGUICompleted, begin pDlg->init()");
	pDlg->init();
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] in DutyManagerGUI::onInitGenericGUICompleted, end pDlg->init()");
	pDlg->UnlockWindowUpdate();
	pDlg->UpdateWindow();
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] end DutyManagerGUI::onInitGenericGUICompleted");
};
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/EventViewerGUI.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/07/12 17:46:27 $
 * Last modified by:  $Author: lu.xiangmei $
 * 
 * EventViewerGUI is the AbstractDialogGUI subclass, and is the point of contact for the process manager in the application.
 */

#include <sstream>

#include "app/event/event_viewer/src/stdafx.h"
#include "app/event/event_viewer/src/EventViewer.h"
#include "app/event/event_viewer/src/EventViewerGUI.h"
#include "app/event/event_viewer/src/EventViewerDlg.h"

#include "app/event/event_viewer/EventListComponent/src/Filter.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/data_access_interface/entity_access/src/EventViewer.h"

#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/threads/src/Thread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::RunParams;
using namespace TA_Base_Bus;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EventViewerGUI::EventViewerGUI()
:
AbstractDialogGUI(IDD_EVENTVIEWER_DIALOG),
m_colourCodeAlarmEvents(false),
m_filter(NULL)
{

}

EventViewerGUI::~EventViewerGUI()
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

unsigned long EventViewerGUI::getApplicationType()
{
    return EVENT_GUI_APPTYPE;
}

void EventViewerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
{
    setGuiEntity(*guiEntity);
}

void EventViewerGUI::checkCommandLine()
{
}

void EventViewerGUI::entityChanged(const std::vector<std::string>& changes){}

void EventViewerGUI::serverIsDown(){}

void EventViewerGUI::serverIsUp(){}

void EventViewerGUI::loadAllUserSettings()
{
	FUNCTION_ENTRY("loadAllUserSettings" );

    // Create the user settings object (if necessary)
    if(getUserSetting() == NULL)
	{
        setUserSetting(*(new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType() )) );
	}

   /* try
    {
        // Now load things that are always going to be loaded
        // Fix for TD2557 - invalid user settings causing the Event Viewer to fail initialisation.
        // loadFilterSettings();
    }
    catch(...)
    {
        // An error in the user settings isn't necessarily an error
        LOG_EXCEPTION_CATCH(SourceInfo,"...","An exception was caught while loading filter settings - continuing with defaults");
    }*/

    // Inserted as part fix for TD2038: Initialise the offsets before OnSize() is called.
    dynamic_cast <EventViewerDlg*> (getApplicationWnd())->initialiseWindowOffsets();

    // Finally load things that are option if the run param is set
	if (TA_Base_Core::RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
	{
        // If not set, let the Control Station position the window
		setWindowPosition();
		FUNCTION_EXIT;            
		return;
	}
    else
    {
        // Else load the position it was in the last time.
        loadPositionSettings();
    }

    FUNCTION_EXIT;
}

void EventViewerGUI::saveAllUserSettings()
{
    FUNCTION_ENTRY("saveAllUserSettings");
    // If necessary, create the user settings object
    if(getUserSetting() == NULL)
	{
        setUserSetting(*(new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType() )) );
	}

    // No app-specific settings to be saved at this point (they are all saved in the saveFilterSettings() ) method
    
    // Then check if we should be saving the diableable settings (i.e. window position)
    if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).
              empty() )
    {
        // If not, then just return
        FUNCTION_EXIT;
        return;
    }
    else
    {
        // If so, then save the position settings
        savePositionSettings();
    }

    FUNCTION_EXIT;

}

void EventViewerGUI::createApplicationObject()
{
    // Construct the dialog
	EventViewerDlg* dialog = new EventViewerDlg(*this);

    // And now set the dialog object to the stored applicaiton object
    setApplicationWnd(*dialog);
}

void EventViewerGUI::prepareForClose()
{
    // Close the Event Viewer nicely - call OnClose()
    EventViewerDlg* dialog = dynamic_cast<EventViewerDlg*>(getApplicationWnd());

    dialog->SendMessage(WM_USER_CLOSE_MAIN_WINDOW);

    while (!dialog->isReadyToClose())
    {
        TA_Base_Core::Thread::sleep(100);
    }
}


//TD18095, jianghp
void EventViewerGUI::onInitGenericGUICompleted()
{
	CWaitCursor cur;
	CWnd *pWnd = getApplicationWnd();
	pWnd->LockWindowUpdate();
	TransActiveDialog * pDlg = dynamic_cast<TransActiveDialog * >(pWnd);
	TA_ASSERT(pDlg != NULL, "the application window is NULL!");
	
	pDlg->init();
	pDlg->UnlockWindowUpdate();
	pDlg->UpdateWindow();
}
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/dll_host/src/DllHostGUI.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/07/09 13:53:29 $
  * Last modified by:  $Author: lu.xiangmei $
  *
  */
// DllHostGUI.cpp: implementation of the DllHostGUI class.
//
//////////////////////////////////////////////////////////////////////

#include "app/system_control/dll_host/src/stdafx.h"
#include "app/system_control/dll_host/src/DllHostGUI.h"
#include "app/system_control/dll_host/src/DllHostDlg.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::GenericGUIException;
using TA_Base_Core::DebugUtil;


namespace TA_Base_App
{

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    DllHostGUI::DllHostGUI()
        : AbstractDialogGUI( IDD_TA_DLL_HOST_DIALOG ), 
          m_guiEntity(NULL)
    {
    }


    DllHostGUI::~DllHostGUI()
    {
    }


    void DllHostGUI::createApplicationObject()
    {
		CDllHostDlg * pDlg = new CDllHostDlg(*this); 
        setApplicationWnd( *pDlg );
    }


    unsigned long DllHostGUI::getApplicationType()
    {
	    return DLL_HOST_GUI_APPTYPE;
    }


    void DllHostGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
	    setGuiEntity(*guiEntity);
        m_guiEntity = guiEntity;
    }


    void DllHostGUI::checkCommandLine()
    {
    }


    void DllHostGUI::entityChanged(const std::vector<std::string>& changes)
    {
	    //TODO: This will be called by GenericGUI when it receives a callback
	    //indicating that the GUI entity has been modified. GenericGUI will have
	    //invalidated the entity database object and will tell the GUI which
	    //items were modified. This method just has to retrieve the details out
	    //of it and update accordingly. If the details in it are invalid then this
	    //should print a warning message to the user and continue to use the old settings (ie do not update the GUI).
    }


    void DllHostGUI::serverIsDown()
    {
        // TODO: This method is used to ensure the GUI can handle when the server goes
	    //up or down. The GUI should be updated to indicate the network status.
	    //If the server has gone down it should enter some sort of read-only mode and
	    //if the server comes back up it can restore to the correct operating mode.
    }


    void DllHostGUI::serverIsUp()
    {
        // TODO: This method is used to ensure the GUI can handle when the server goes
	    //up or down. The GUI should be updated to indicate the network status.
	    //If the server has gone down it should enter some sort of read-only mode and
	    //if the server comes back up it can restore to the correct operating mode.
    }


    void DllHostGUI::loadAllUserSettings()
    {
		// replace code to directly access the banner entity from database and position the banner window
		// to its correct position and avoid displaying the banner on the center of the screen.
    }


    void DllHostGUI::saveAllUserSettings()
    {
	    //This method does nothing.
	    //The method definition is needed to instantiate the abstract base class.
	    //There are no user settings for the Banner.
    }


    void DllHostGUI::prepareForClose()
    {
	    //TODO: This provides a standard method for each GUI to use to clean itself 
	    //up ready to exit. This will be called whenever the application exits whether 
	    //it is closed by the Control Station or by the user. This should perform 
	    //tasks such as saving user settings, stopping any threads and asking the user 
	    //if they want to save their settings. When this method is completed the GUI 
	    //should be in a safe state to be closed.

        CDllHostDlg* pDlg = dynamic_cast<CDllHostDlg*>( getApplicationWnd() );

        if ( pDlg != NULL )
        {
            try
            {
                pDlg->uninit();
            }
            catch (...)
            {
            }
        }
    }


	void DllHostGUI::onInitGenericGUICompleted()
	{
		CWaitCursor cur;

		CWnd *pWnd = getApplicationWnd();
		pWnd->LockWindowUpdate();

        TA_Base_Bus::TransActiveDialog * pDlg = dynamic_cast<TA_Base_Bus::TransActiveDialog *>(pWnd);
		TA_ASSERT(pDlg != NULL, "the application window is NULL!");

		pDlg->init();
		pDlg->UnlockWindowUpdate();
		pDlg->UpdateWindow();
	}

} // namespace TA_Base_App

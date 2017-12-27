/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/crutilGui.cpp $
  * @author:  Basil Carbonaro
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Report Manager - GUI derived from AbstractDialogGUI
  *
  */

#include "stdafx.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/generic_gui/src/TransActiveDialog.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/Timer.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "resource.h"
#include "crutilDlg.h"
#include "crutilGui.h"
#include "globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;
using namespace TA_Base_Bus;

namespace TA_Base_App
{
    namespace TA_Reporting
    {
        
        //////////////////////////////////////////////////////////////////////
        // Construction/Destruction
        //////////////////////////////////////////////////////////////////////
        
        crutilGui::crutilGui()
        :
        AbstractDialogGUI(IDD_CRUTIL_DIALOG)
        {   
        }
        
        
        crutilGui::~crutilGui()
        {
            try
            {
                CWnd* app = getApplicationWnd();
                if (app != NULL)
                {
                    delete app;
                }
            }
            catch( ... )
            {
            }
        }
        
        
        void crutilGui::createApplicationObject()
        {
			//TD18095, jianghp, to fix the performance of showing manager application
			CWnd* app = new CCrutilDlg(this);
            if (app == NULL)
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on MyTestDlg returned NULL. Cannot create window");
                return;
            }
            setApplicationWnd(*app);
		}
        
        
        unsigned long crutilGui::getApplicationType()
        {
            return( REPORT_MANAGER_GUI_APPTYPE );
        }
        
        
        void crutilGui::checkEntity(TA_Base_Core::IEntityData* guiEntity)
        {
            setGuiEntity(*guiEntity);

            // TD4213: The entity key needs to be retrieved so it can be passed as a parameter when
            // audit messages are sent.

            std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
            unsigned long entityKey = 0;

            try
            {
                TA_Base_Core::IEntityData* iData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
                entityKey = (dynamic_cast <TA_Base_Core::EntityData*> (iData))->getKey();
                delete iData;
                iData = NULL;
            }
            catch(...)
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Could not retrieve entity key. Audit messages cannot be sent.");
            }

            // Set the entity key as a run param
            std::ostringstream str;
            str << entityKey;
            TA_Base_Core::RunParams::getInstance().set(RPARAM_ENTITYKEY, str.str().c_str());
        }
        
        
        void crutilGui::checkCommandLine()
        {  
        }
        
        
        void crutilGui::entityChanged(const std::vector<std::string>& changes)
        {
            //TODO: This will be called by GenericGUI when it receives a callback
            //indicating that the GUI entity has been modified. GenericGUI will have
            //invalidated the entity database object and will tell the GUI which
            //items were modified. This method just has to retrieve the details out
            //of it and update accordingly. If the details in it are invalid then this
            //should print a warning message to the user and continue to use the old settings (ie do not update the GUI).
        }
        
        
        void crutilGui::serverIsDown()
        {
            // TODO: This method is used to ensure the GUI can handle when the server goes
            //up or down. The GUI should be updated to indicate the network status.
            //If the server has gone down it should enter some sort of read-only mode and
            //if the server comes back up it can restore to the correct operating mode.
        }
        
        
        void crutilGui::serverIsUp()
        {
            // TODO: This method is used to ensure the GUI can handle when the server goes
            //up or down. The GUI should be updated to indicate the network status.
            //If the server has gone down it should enter some sort of read-only mode and
            //if the server comes back up it can restore to the correct operating mode.
        }
        
        
        void crutilGui::loadAllUserSettings()
        {
            // First check if we should be loading the user settings.
            if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
            {
                setWindowPosition();
                LOG( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );            
                return;
            }
            
            if (getUserSetting() == NULL)
            {
                // TD 3734
                // cannot pass a temporary object in setUserSetting as it takes the
                // address of the object and then keeps it.
                std::auto_ptr<SettingsMgr> m(new SettingsMgr(getSessionId(), getApplicationType()));
                setUserSetting(*m);
                m.release();
            }
            
            loadPositionSettings();
        }
        
        
        void crutilGui::saveAllUserSettings()
        {
            // First check if we should be saving the user settings.
            if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
            {
                return;
            }
            
            if (getUserSetting() == NULL)
            {
                // TD 3734
                // cannot pass a temporary object in setUserSetting as it takes the
                // address of the object and then keeps it.
                std::auto_ptr<SettingsMgr> m(new SettingsMgr(getSessionId(), getApplicationType()));
                setUserSetting(*m);
                m.release();
            }
            
            savePositionSettings();
        }
        
        
        void crutilGui::prepareForClose()
        {
            //TODO: This provides a standard method for each GUI to use to clean itself 
            //up ready to exit. This will be called whenever the application exits whether 
            //it is closed by the Control Station or by the user. This should perform 
            //tasks such as saving user settings, stopping any threads and asking the user 
            //if they want to save their settings. When this method is completed the GUI 
            //should be in a safe state to be closed.
            
            try
            {
                saveAllUserSettings();
            }
            catch ( TA_Base_Core::UserSettingsException&)
            {
                // Handle here with appropriate errror message
            }
            
            // TODO: Perform any shutdown tasks here
        }

		//TD18095, jianghp
		void crutilGui::onInitGenericGUICompleted()
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

    } // TA_Reporting
} // TA_Base_App

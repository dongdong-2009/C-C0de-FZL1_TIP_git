/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentStatusViewerGUI.cpp $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * GUI class
 *
 */

#pragma warning ( disable : 4284 )

#include "stdafx.h"
#include "EquipmentStatusViewerGUI.h"
#include "resource.h"
#include "EquipmentStatusViewerDlg.h"
#include "AuditMessageUtility.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/generic_gui/src/TransActiveDialog.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/data_access_interface/entity_access/src/EquipmentStatusViewerEntityData.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;
using TA_Base_Core::EquipmentStatusViewerEntityData;
using namespace TA_Base_Bus;

namespace TA_Base_App
{

    EquipmentStatusViewerGUI::EquipmentStatusViewerGUI()
    :
    AbstractDialogGUI(IDD_ESV_DIALOG),
    m_applicationWnd(NULL),
    m_myUserSettings(NULL)
    {   
    }


    EquipmentStatusViewerGUI::~EquipmentStatusViewerGUI()
    {
    }


    void EquipmentStatusViewerGUI::createApplicationObject()
    {   
        FUNCTION_ENTRY ("EquipmentStatusViewerGUI::createApplicationObject()");
        
        m_applicationWnd = new TA_Base_App::CEquipmentStatusViewerDlg(*this);
        setApplicationWnd(*m_applicationWnd);

        FUNCTION_EXIT;
    }


    unsigned long EquipmentStatusViewerGUI::getApplicationType()
    {       
        return EQUIP_STATUS_VIEWER_GUI_APPTYPE;
    }


    void EquipmentStatusViewerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
        // TD12051: Verify entity data type.
        EquipmentStatusViewerEntityData* esvEntityData = dynamic_cast<EquipmentStatusViewerEntityData*>(guiEntity);

        if (esvEntityData == NULL || esvEntityData->getType() != EquipmentStatusViewerEntityData::getStaticType())
        {
            TA_THROW(GenericGUIException("Entity configuration is not of type Equipment Status Viewer", 
                     GenericGUIException::ENTITY_CONFIGURATION_INVALID));
        }

        setGuiEntity(*guiEntity);

        CAuditMessageUtility::setEntityKey(getGUIEntity()->getKey());

        // TD12051: Make entity parameters available via runtime parameters.
        RunParams::getInstance().set(RPARAM_EXPORTDIR, esvEntityData->getExportDirectory().c_str());    
    }


    void EquipmentStatusViewerGUI::checkCommandLine()
    {       
    }


    void EquipmentStatusViewerGUI::entityChanged(const std::vector<std::string>& changes)
    {
        //TODO: This will be called by GenericGUI when it receives a callback
        //indicating that the GUI entity has been modified. GenericGUI will have
        //invalidated the entity database object and will tell the GUI which
        //items were modified. This method just has to retrieve the details out
        //of it and update accordingly. If the details in it are invalid then this
        //should print a warning message to the user and continue to use the old settings (ie do not update the GUI).

        // TD12051: Update entity parameters in runtime parameters.
        EquipmentStatusViewerEntityData* esvEntityData = dynamic_cast<EquipmentStatusViewerEntityData*>(getGUIEntity());
        TA_ASSERT(esvEntityData != NULL, "Invalid GUI entity");

        RunParams::getInstance().set(RPARAM_EXPORTDIR, esvEntityData->getExportDirectory().c_str());    
    }


    void EquipmentStatusViewerGUI::serverIsDown()
    {
        // TODO: This method is used to ensure the GUI can handle when the server goes
        //up or down. The GUI should be updated to indicate the network status.
        //If the server has gone down it should enter some sort of read-only mode and
        //if the server comes back up it can restore to the correct operating mode.
    }


    void EquipmentStatusViewerGUI::serverIsUp()
    {
        // TODO: This method is used to ensure the GUI can handle when the server goes
        //up or down. The GUI should be updated to indicate the network status.
        //If the server has gone down it should enter some sort of read-only mode and
        //if the server comes back up it can restore to the correct operating mode.
    }


    void EquipmentStatusViewerGUI::loadAllUserSettings()
    {
        if (m_myUserSettings == NULL)
        {
            m_myUserSettings = new SettingsMgr(getSessionId(), getApplicationType());
            setUserSetting(*m_myUserSettings);
        }
        
        // First check if we should be loading the user settings.
        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
        {
            // let control station position the window
            setWindowPosition();           
        } else
        {
            // Else load the position it was in the last time.
            loadPositionSettings();
        }
    }


    void EquipmentStatusViewerGUI::saveAllUserSettings()
    {
        if (m_myUserSettings == NULL)
        {
            m_myUserSettings = new SettingsMgr(getSessionId(), getApplicationType());
            setUserSetting(*m_myUserSettings);
        }
            
        // First check if we should be saving the user settings.
        if (RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())       
            return;
        else
            savePositionSettings();     
    }


    void EquipmentStatusViewerGUI::prepareForClose()
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
    void EquipmentStatusViewerGUI::onInitGenericGUICompleted()
    {
		unsigned long locationKey = getGUIEntity()->getLocation();
		unsigned long physicalSubsystemKey = getGUIEntity()->getPhysicalSubsystem();
		unsigned long subsystemKey = getGUIEntity()->getSubsystem();
		if(physicalSubsystemKey<=0)
		{
			subsystemKey=physicalSubsystemKey;
		}
		
        bool allowed = TA_Base_Bus::ScadaUtilities::getInstance().isActionAllowedWithLocSub ( (unsigned int) TA_Base_Bus::aca_RUN_APPLICATION,
            getGUIEntity()->getKey(), locationKey, subsystemKey, 
            TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) ); 
        if( !allowed )
        {
            TA_THROW(TA_Base_Core::GenericGUIException(TA_Base_Core::GenericGUIException::ACCESS_DENIED));
        }
        
        TA_Base_Bus::ScadaProxyFactory::getInstance().setProxiesToControlMode();
        
        CWaitCursor cur;
        CWnd *pWnd = getApplicationWnd();
        //pWnd->LockWindowUpdate();
        TransActiveDialog * pDlg = dynamic_cast<TransActiveDialog * >(pWnd);
        TA_ASSERT(pDlg != NULL, "the application window is NULL!");
        pDlg->init();
        //pDlg->UnlockWindowUpdate();
        pDlg->Invalidate();
    }
} // TA_Base_App

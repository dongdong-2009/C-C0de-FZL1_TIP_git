/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/SchematicDisplay.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// SchematicDisplay.cpp : Implementation of CSchematicDisplay
#include "stdafx.h"
#include "app\system_control\control_station\src\RippleControlStation_i.h"
#include "app\system_control\control_station\src\SchematicDisplay.h"
#include "app\system_control\control_station\src\SessionManager.h"
#include "app\system_control\control_station\src\ControlStationConstants.h"
#include "app\system_control\control_station\src\ControlStationRootDialog.h"
#include "app\system_control\control_station\src\IProcessManager.h"
#include "core\data_access_interface\entity_access\src\ControlStation.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\RunParams.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::EntityAccessFactory;

const std::string CSchematicDisplay::REPLACEMENT_STR = "###";

/////////////////////////////////////////////////////////////////////////////
// CSchematicDisplay
CSchematicDisplay::CSchematicDisplay() : m_templateName("")
{
    // Set ourselves in the root dialog so it can perform callbacks.
    ControlStationRootDialog::getInstance().setRippleControlStationComObject( this );
}


void CSchematicDisplay::initialise()
{
    static bool initialised = false;
    if (initialised)
    {
        return;
    }

    try
    {
        // Load the Control Station entity and retrieve all the data we need
        std::string entityName = RunParams::getInstance().get(RPARAM_ENTITYNAME);

        if (entityName.empty())
        {
            return;
        }
        // get the name of the schematic that will be used by ATS (specific to 4669)
        using TA_Base_Core::ControlStation;
        ControlStation* controlStationEntity = dynamic_cast<ControlStation*>(EntityAccessFactory::getInstance().getEntity( entityName ));
        m_templateName = controlStationEntity->getExtSchematic();
        initialised = true;
    }
    catch(...)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Unable to get the control station entity from the database");
    }
}


STDMETHODIMP CSchematicDisplay::displayAtsSchematic(BSTR filePath, long leftPosition)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "displayAtsSchematic");

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Trying to launch display %s on screen %li", filePath, leftPosition);

    initialise();

    DisplayInformation* info = new struct DisplayInformation;

    if (info != NULL)
    {
        info->display = CString(filePath);

        // the format is going to be something like ".\SIG\###_ATS_AXC.gdf@..."
        // we need to substitute the ### with the location name that we get from filePath
        CString tmp = m_templateName.c_str();
        if (!tmp.IsEmpty())
        {
            tmp.Replace(REPLACEMENT_STR.c_str(), info->display.Left(REPLACEMENT_STR.size()));
            info->display = tmp + info->display;
        }
        info->asset = "";
        info->position = leftPosition;
        ControlStationRootDialog::getInstance().PostMessage(WM_LAUNCH_DISPLAY, (WPARAM)info);
    }
    else
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new returned a NULL for display info");
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "displayAtsSchematic");
	return S_OK;
}


STDMETHODIMP CSchematicDisplay::getCurrentSessionId(BSTR *sessionId)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getCurrentSessionId");
    CString strResult = SessionManager::getInstance().getSessionDetails().getSessionId().c_str();
	*sessionId = strResult.AllocSysString();

    LOG( SourceInfo, DebugUtil::FunctionExit, "getCurrentSessionId");
	return S_OK;
}


STDMETHODIMP CSchematicDisplay::getCurrentProfile(BSTR *profile)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getCurrentProfile");
    SessionDetails& details = SessionManager::getInstance().getSessionDetails( );

    CString strResult = details.getProfileAndLocationDisplayString().c_str();
    if (details.isOperatorOverridden())
    {
        strResult += TA_Base_App::TA_ControlStation::NAME_SEPARATOR.c_str();
        strResult += details.getOverrideProfileAndLocationDisplayString().c_str();
    }

	*profile = strResult.AllocSysString();

    LOG( SourceInfo, DebugUtil::FunctionExit, "getCurrentProfile");
	return S_OK;
}

STDMETHODIMP CSchematicDisplay::getCurrentLocation(BSTR *location)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getCurrentLocation");
    // location is the physical location of the operator. That is, where is their console
    // sitting.
    SessionDetails& details = SessionManager::getInstance().getSessionDetails();
	CString strResult = details.getConsoleLocationName().c_str();
	*location = strResult.AllocSysString();

    LOG( SourceInfo, DebugUtil::FunctionExit, "getCurrentLocation");
	return S_OK;
}

STDMETHODIMP CSchematicDisplay::getCurrentUserName(BSTR *userName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		
	LOG( SourceInfo, DebugUtil::FunctionEntry, "getCurrentUserName");

	CString strResult = SessionManager::getInstance().getSessionDetails().getOperatorName().c_str();
    if (SessionManager::getInstance().getSessionDetails().isOperatorOverridden())
    {
        strResult += TA_Base_App::TA_ControlStation::NAME_SEPARATOR.c_str();
        strResult += SessionManager::getInstance().getSessionDetails().getOverrideOperatorName().c_str();
    }
    
	*userName = strResult.AllocSysString();
	
	LOG( SourceInfo, DebugUtil::FunctionExit, "getCurrentUserName");	
	return S_OK;
}

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ControlStation2.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class holds the COM server code for the Control Station. It will process all calls
  * from COM clients and has the ability to fire events to them.
  * 
  */

#include "stdafx.h"
#include <comdef.h>
#include "app/system_control/control_station/src/ControlStation.h"
#include "app/system_control/control_station/src/ControlStation2.h"
#include "app/system_control/control_station/src/IProcessManager.h"
#include "app/system_control/control_station/src/IGraphWorxManager.h"
#include "app/system_control/control_station/src/RunningApplication.h"
#include "app/system_control/control_station/src/SessionManager.h"
#include "app/system_control/control_station/src/ControlStationConstants.h"
#include "app/system_control/control_station/src/ControlStationRootDialog.h"
#include "app/system_control/control_station/src/ServerMonitor.h"

#include <memory>

#include "bus/banner_framework/src/constants.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/types/ControlStationComms_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/process_management/IDL/src/IManagedApplicationCorbaDef.h"

//using IProcessManager::ApplicationProperties;
typedef IProcessManager::ApplicationProperties ApplicationProperties;
using TA_Base_Core::DebugUtil;

boost::regex CControlStation2::APP_PATTERN("--entity-name[[:space:]]*=[[:space:]]*"\
										   ".+\\\\([a-z,A-Z]+)\\.exe");

/////////////////////////////////////////////////////////////////////////////
// CControlStation2
std::string CControlStation2::getAppName(std::string ex_str)
{
	boost::match_results<std::string::const_iterator> what;
	if(0 == boost::regex_search(ex_str, what, APP_PATTERN, boost::match_default))
	{
        LOG1( SourceInfo, DebugUtil::DebugError, "the application name format <%s> has parse error", ex_str.c_str());
		return "UnknownAppName";
	}
	if(what[1].matched)
	{
		return std::string(what[1].first, what[1].second);
	}

    LOG1( SourceInfo, DebugUtil::DebugError, "could not find correct application name from string <%s>", ex_str.c_str());
	return "UnknownAppName";
}

CControlStation2::CControlStation2()
{
    FUNCTION_ENTRY("CControlStation2");
    // Set ourselves in the root dialog so it can perform callbacks.
    ControlStationRootDialog::getInstance().setControlStationComObject( this );
    FUNCTION_EXIT;
}


STDMETHODIMP CControlStation2::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IControlStation2
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


STDMETHODIMP CControlStation2::logout()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("logout");

    ControlStationRootDialog::getInstance().PostMessage(WM_LOGOUT);

    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::giveApplicationFocus(long processId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("giveApplicationFocus");

	IProcessManager* mgr = SessionManager::getInstance().getProcessManager();
    if (mgr != NULL)
    {
        mgr->giveApplicationFocus(processId);
    }

    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::getOperator(BSTR* pRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("getOperator");

    CString strResult = SessionManager::getInstance().getSessionDetails().getOperatorName().c_str();
    if (SessionManager::getInstance().getSessionDetails().isOperatorOverridden())
    {
        strResult += TA_Base_App::TA_ControlStation::NAME_SEPARATOR.c_str();
        strResult += SessionManager::getInstance().getSessionDetails().getOverrideOperatorName().c_str();
    }
    
	*pRet = strResult.AllocSysString();
	
    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::getSite(BSTR* pRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("getSite");

	// Site is the physical location of the operator. That is, where is their console
    // sitting.
	CString strResult = SessionManager::getInstance().getSessionDetails().getConsoleLocationName().c_str();

	*pRet = strResult.AllocSysString();
	
    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::getProfile(BSTR* pRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("getProfile");

    CString strResult = SessionManager::getInstance().getSessionDetails().getProfileAndLocationDisplayString().c_str();
    if (SessionManager::getInstance().getSessionDetails().isOperatorOverridden())
    {
        strResult += TA_Base_App::TA_ControlStation::NAME_SEPARATOR.c_str();
        strResult += SessionManager::getInstance().getSessionDetails().getOverrideProfileAndLocationDisplayString().c_str();
    }

	*pRet = strResult.AllocSysString();
	
    FUNCTION_EXIT;
	return S_OK;
}

STDMETHODIMP CControlStation2::launchApplicationEx(long appType, BSTR* extraCommandLine, long posType, long alignType, RECT* objectDim, RECT* boundaryDim)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("launchApplicationEx");

    CString commandCString(*extraCommandLine);
    std::string commandStdString = commandCString;

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Graphworx has requested an application be launched");

    IProcessManager* mgr = SessionManager::getInstance().getProcessManager();
    if (mgr != NULL)
    {
        try
        {
            RECT objectRect = ( (objectDim == NULL) ? (TA_Base_Bus::TA_GenericGui::DEFAULT_RECT) : (*objectDim) );
            RECT boundaryRect = ( (boundaryDim == NULL) ? (TA_Base_Bus::TA_GenericGui::DEFAULT_RECT) : (*boundaryDim) );
            mgr->launchApplication(appType, commandStdString, posType, alignType, objectRect, boundaryRect);
        }
        catch ( TA_Base_Core::ApplicationException& ex)
        { 
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "ApplicationException", "Could not launch the application requested by GraphWorX");
			
			std::string* appName = new std::string(getAppName(ex.what()));
            ControlStationRootDialog::getInstance().PostMessage(WM_DISPLAY_LAUNCH_APPLICATION_ERROR, reinterpret_cast<WPARAM>(appName), 0);
        }
    }
	else
	{
		// TODO handle error
	}

    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::getAllApplications(VARIANT* pRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("getAllApplications");

	VariantInit(pRet);

    IProcessManager* processManager = SessionManager::getInstance().getProcessManager();
    if (processManager == NULL)
    {
        //TODO Handle error?
		return E_OUTOFMEMORY; // ???
    }

    std::vector<ApplicationProperties> allApps = processManager->getApplications();

    createArray(allApps, *pRet);
    
    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::getRunningApplications(VARIANT* pRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("getRunningApplications");

	VariantInit(pRet);

    IProcessManager* processManager = SessionManager::getInstance().getProcessManager();
    if (processManager == NULL)
    {
        return E_OUTOFMEMORY; // TODO: handle error?
    }

    std::map<unsigned long, std::string> allApps = processManager->getRunningApplications();

    createArray(allApps, *pRet);

    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::getLocation(BSTR* pRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("getLocation");

	// Location is the conceptual location of the operator.
	CString strResult = SessionManager::getInstance().getSessionDetails().getLocationName().c_str();

	*pRet = strResult.AllocSysString();
	
	FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::setSelectedAlarms(BSTR FAR* asset, long leftPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("setSelectedAlarms");

	IProcessManager* mgr = SessionManager::getInstance().getProcessManager();
    if (mgr == NULL)
    {
        return S_OK;
    }

    IProcessManager::RunningApplicationMap& runningApps = mgr->getRunningApplicationObjects();
    RunningApplication* bannerToUse = NULL;

    // Iterate through and find the banner on the same screen as the GraphWorx that made the request
    // This is done by seeing if the left position of GraphWorX (passed to the method) is within the same
    // screen as the left position of the banner
    for (IProcessManager::RunningApplicationMap::iterator iter = runningApps.begin();
         iter != runningApps.end();
         ++iter)
    {
        // We are looking for banners.
        if( iter->second->isApplicationTypeEqualTo(BANNER_GUI_APPTYPE) )
        {
            long leftPositionOfBanner;
            long topPositionOfBanner;
            iter->second->getPositionInformation(leftPositionOfBanner, topPositionOfBanner);

            if (leftPositionOfBanner == leftPosition)
            {
                // This is the banner to use
                bannerToUse = iter->second;
                break;
            }
        }
    }

    // Pass the new parameter to the application
    if (bannerToUse != NULL)
    {
        CString assetString(*asset);  // Convert out of a BSTR
        TA_Base_Core::RunParamSeq_var params = new TA_Base_Core::RunParamSeq();
        params->length(1);
        params[0].name = CORBA::string_dup( BANNER_CONST::RPARAM_ASSETALARM.c_str() );
        params[0].value = CORBA::string_dup(assetString);
        bannerToUse->passNewParameters( params );
    }

    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::getProfileDisplay(long leftPosition, BSTR* pRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("getProfileDisplay");

    unsigned long screen = SessionManager::getInstance().getScreenPositioning().getWhichScreenCoordinateIsOn(leftPosition);
    CString strResult = SessionManager::getInstance().getSessionDetails().getProfileDisplay(screen).c_str();
	*pRet = strResult.AllocSysString();
	
    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::getProfileDisplayAtLocation(long leftPosition, BSTR locationName, BSTR* pRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("getProfileDisplayAtLocation");

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Try to get default display for a location");

    unsigned long screen = SessionManager::getInstance().getScreenPositioning().getWhichScreenCoordinateIsOn(leftPosition);
	CString   locationNameStr(locationName);
    CString strResult = SessionManager::getInstance().getSessionDetails().getProfileDisplayAtLocation(screen, std::string(locationNameStr.GetBuffer(locationNameStr.GetLength()))).c_str();

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Get the default display for location %s is %s", locationNameStr, strResult );

	*pRet = strResult.AllocSysString();
	
    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::changeProfile()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("changeProfile");

    ControlStationRootDialog::getInstance().PostMessage(WM_DISPLAY_CHANGE_PROFILE_DIALOG, 0, 0);

    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::changePassword()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("changePassword");

    ControlStationRootDialog::getInstance().PostMessage(WM_DISPLAY_CHANGE_PASSWORD_DIALOG, 0, 0);

    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::isServerContactable(BOOL* isContactable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("isServerContactable");

    *isContactable = ServerMonitor::getInstance().isContactable();

    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::displayTaskManager()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("displayTaskManager");

    ControlStationRootDialog::getInstance().PostMessage(WM_SHOW_TASKMANAGER, 0, 0);

    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::changeOperatorOverride()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("changeOperatorOverride");

    ControlStationRootDialog::getInstance().PostMessage(WM_DISPLAY_OVERRIDE_DIALOG, 0, 0);

    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::isOperatorOverridden(BOOL *isOverridden)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("isOperatorOverridden");

	*isOverridden = SessionManager::getInstance().getSessionDetails().isOperatorOverridden();

    FUNCTION_EXIT;
	return S_OK;
}


void CControlStation2::createArray(std::map<unsigned long, std::string>& mapData, VARIANT FAR& vaResult)
{
    const long ID_COLUMN = 0;
    const long NAME_COLUMN = 1;
    const long NUM_COLUMNS = 2;
    const long NUM_DIMENSIONS = 2;

    // Create an array large enough for all elements in the map. It is a 2D array.
    SAFEARRAYBOUND sabound[NUM_DIMENSIONS];
    sabound[0].cElements = NUM_COLUMNS;        //columns
    sabound[0].lLbound = 0;    
    sabound[1].cElements = mapData.size();     //rows
    sabound[1].lLbound = 0;    

    // Two dimensional array of strings
    SAFEARRAY FAR* psa = SafeArrayCreate(VT_BSTR, NUM_DIMENSIONS, sabound); 
    if (psa == NULL)
    {
        AfxThrowOleDispatchException(1004, "Out of Memory");
    }

    int insertPosition = 0;
    for ( std::map<unsigned long, std::string>::iterator iter = mapData.begin();
          iter != mapData.end();
          ++iter
        )
    {
        long indexes[NUM_DIMENSIONS];
        indexes[1] = insertPosition;  //row

        // First add the application type id as the first entry
        indexes[0] = ID_COLUMN;       //column
        CString id;
        id.Format("%lu",iter->first);
        HRESULT hr = SafeArrayPutElement(psa, indexes, id.AllocSysString());
        if (FAILED(hr))
        {
            if (psa)
            {
                SafeArrayDestroy(psa);
            }
            logError(hr);
            AfxThrowOleDispatchException(1003, "Unexpected Failure");
            return;
        }

        // Now insert the name of the application
        indexes[0] = NAME_COLUMN;     //column
        CString application = iter->second.c_str();
        hr = SafeArrayPutElement(psa, indexes, application.AllocSysString());
        if (FAILED(hr))
        {
            if (psa)
            {
                SafeArrayDestroy(psa);
            }
            logError(hr);
            AfxThrowOleDispatchException(1003, "Unexpected Failure");
            return;
        }

        // Increase the insert position so we are ready to go around again and insert the next
        // application.
        ++insertPosition;
    }

    V_VT(&vaResult) = VT_ARRAY | VT_BSTR;
    V_ARRAY(&vaResult) = psa;    
}


void CControlStation2::createArray(std::vector<ApplicationProperties>& applist, VARIANT FAR& vaResult)
{
    const long ID_COLUMN = 0;
    const long NAME_COLUMN = 1;
    const long ISACCESSIBLE_COLUMN = 2;
    const long NUM_COLUMNS = 3;
    const long NUM_DIMENSIONS = 3;

    // Create an array large enough for all elements in the vector. It is a 3D array.
    SAFEARRAYBOUND sabound[NUM_DIMENSIONS];
    sabound[0].cElements = NUM_DIMENSIONS;        //columns
    sabound[0].lLbound = 0;    
    sabound[1].cElements = applist.size();     //rows
    sabound[1].lLbound = 0;    
    sabound[2].cElements = 1;     //rows
    sabound[2].lLbound = 0;    

    // Two dimensional array of strings
    SAFEARRAY FAR* psa = SafeArrayCreate(VT_BSTR, NUM_DIMENSIONS, sabound); 
    if (psa == NULL)
    {
        AfxThrowOleDispatchException(1004, "Out of Memory");
    }

    int insertPosition = 0;
    for (unsigned int i = 0; i < applist.size(); i++)
    {
        long indexes[NUM_DIMENSIONS];
        indexes[2] = 0;
        indexes[1] = i;               //row

        // First add the application type id as the first entry
        indexes[0] = ID_COLUMN;       //column
        CString id;
        id.Format("%lu",applist[i].key);
        HRESULT hr = SafeArrayPutElement(psa, indexes, id.AllocSysString());
        if (FAILED(hr))
        {
            if (psa)
            {
                SafeArrayDestroy(psa);
            }
            logError(hr);
            AfxThrowOleDispatchException(1003, "Unexpected Failure");
            return;
        }

        // Now insert the name of the application
        indexes[0] = NAME_COLUMN;     //column
        CString application = applist[i].name.c_str();
        hr = SafeArrayPutElement(psa, indexes, application.AllocSysString());
        if (FAILED(hr))
        {
            if (psa)
            {
                SafeArrayDestroy(psa);
            }
            logError(hr);
            AfxThrowOleDispatchException(1003, "Unexpected Failure");
            return;
        }
        // Now insert the enable condition of the application
        indexes[0] = ISACCESSIBLE_COLUMN;     //column
        CString isAccessible;
        // display the boolean as unsigned int string
        isAccessible.Format("%lu",applist[i].isAccessible); 
        hr = SafeArrayPutElement(psa, indexes, isAccessible.AllocSysString());
        if (FAILED(hr))
        {
            if (psa)
            {
                SafeArrayDestroy(psa);
            }
            logError(hr);
            AfxThrowOleDispatchException(1003, "Unexpected Failure");
            return;
        }
        // Increase the insert position so we are ready to go around again and insert the next
        // application.
        ++insertPosition;
    }

    V_VT(&vaResult) = VT_ARRAY | VT_BSTR;
    V_ARRAY(&vaResult) = psa;    
}


void CControlStation2::logError(const HRESULT& hr)
{
    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,"Array of applications could not be returned because: ");
    switch (hr)
    {
    case(DISP_E_BADINDEX):
        LOGMORE( SourceInfo, "The specified index was invalid." );
        break;
    case(E_INVALIDARG):
        LOGMORE( SourceInfo, "One of the arguments is invalid." );
        break;
    case(E_OUTOFMEMORY):
        LOGMORE( SourceInfo, "Memory could not be allocated for the element.");
        break;
    }
}


STDMETHODIMP CControlStation2::launchDisplayEx(BSTR display, BSTR asset, long leftPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("launchDisplayEx");

    LOG( SourceInfo, DebugUtil::FunctionEntry, "launchDisplay");
    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Trying to launch display %s with asset %s on screen %li", display, asset, leftPosition);

    DisplayInformation* info = new struct DisplayInformation;

    if (info != NULL)
    {
        info->display = CString(display);
        info->asset = CString(asset);
        info->position = leftPosition;
        ControlStationRootDialog::getInstance().PostMessage(WM_LAUNCH_DISPLAY, (WPARAM)info);
    }
    else
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new returned a NULL for display info");
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "launchDisplay");
    
	FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::setDisplayDimension(RECT* dim)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("setDisplayDimension");

    SessionManager::getInstance().getScreenPositioning().setDisplayDimension(*dim);

    FUNCTION_EXIT;
    return S_OK;
}


STDMETHODIMP CControlStation2::getRect(EScreen targetScreen, EArea targetArea, long val, RECT* dim)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    FUNCTION_ENTRY("getRect");
    *dim = SessionManager::getInstance().getScreenPositioning().getRect(
                                static_cast<TA_Base_Bus::TA_GenericGui::EScreen>(targetScreen), 
                                static_cast<TA_Base_Bus::TA_GenericGui::EArea>(targetArea), 
                                val);
    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::getCurrentDisplayName(BSTR *display)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("getCurrentDisplayName");

    // This method will only ever be used by the ODS which has 1 display, so we want to get the display on screen 1
    CString name = SessionManager::getInstance().getGraphWorxManager()->getCurrentDisplay(1).c_str();
	*display = name.AllocSysString();

    FUNCTION_EXIT;
	return S_OK;
}



// these are old methods that should probably be deprecated some time in future when none
// of the GraphWorx displays uses these methods.

STDMETHODIMP CControlStation2::launchApplication(long appType, long leftPosition, long topPosition, long width, long height, BSTR FAR* addtionalCommands)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("launchApplication");

    CString commandCString(*addtionalCommands);
    std::string commandStdString = commandCString;

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Graphworx has requested an application be launched");
    LOGMORE( SourceInfo, "Application Type: %li    Position: (%li,%li)   size: (%li,%li)   Additional commandline: %s",appType,leftPosition,topPosition,width,height,commandStdString.c_str());

    IProcessManager* mgr = SessionManager::getInstance().getProcessManager();
    if (mgr != NULL)
    {
        try
        {
            RECT objRect = {leftPosition, 
                            topPosition, 
                            ( width == -1 ) ? leftPosition : leftPosition + width, 
                            ( height == -1 ) ? topPosition : topPosition+ height };
            RECT boundaryRect = SessionManager::getInstance().getScreenPositioning().getRect(
                TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
                TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
                leftPosition);
            mgr->launchApplication( appType,
                                    commandStdString,
                                    TA_Base_Bus::TA_GenericGui::POS_BOUNDED | TA_Base_Bus::TA_GenericGui::POS_RELATIVE,
                                    TA_Base_Bus::TA_GenericGui::ALIGN_TOP | TA_Base_Bus::TA_GenericGui::ALIGN_LEFT,
                                    objRect,
                                    boundaryRect);
        }
        catch ( TA_Base_Core::ApplicationException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "ApplicationException", "Could not launch the application requested by GraphWorX");
            ControlStationRootDialog::getInstance().PostMessage(WM_DISPLAY_LAUNCH_APPLICATION_ERROR, 0, 0);
        }
    }
	else
	{
		// TODO handle error
	}

    FUNCTION_EXIT;
	return S_OK;
}


STDMETHODIMP CControlStation2::launchDisplay(BSTR display, long leftPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("launchDisplay");

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Trying to launch display %s on screen %li", display, leftPosition);

    DisplayInformation* info = new struct DisplayInformation;

    if (info != NULL)
    {
        info->display = CString(display);
        info->asset = "";
        info->position = leftPosition;
        ControlStationRootDialog::getInstance().PostMessage(WM_LAUNCH_DISPLAY, (WPARAM)info);
    }
    else
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new returned a NULL for display info");
    }

   
    FUNCTION_EXIT;
	return S_OK;
}



STDMETHODIMP CControlStation2::getSessionId(BSTR *sessionId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("getSessionId");

    CString strResult = SessionManager::getInstance().getSessionDetails().getSessionId().c_str();
	*sessionId = strResult.AllocSysString();

    FUNCTION_EXIT;
	return S_OK;
}

STDMETHODIMP CControlStation2::loadComplete(long leftPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("loadComplete");

    SessionManager::getInstance().getGraphWorxManager()->loadComplete(leftPosition);

    FUNCTION_EXIT;
	return S_OK;
}

STDMETHODIMP CControlStation2::launchHelp()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    FUNCTION_ENTRY("launchHelp");

	TA_Base_Bus::HelpLauncher::getInstance().displayHelp(SessionManager::getInstance().getGraphWorxHelpfile(), true);

    FUNCTION_EXIT;
	return S_OK;
}

STDMETHODIMP CControlStation2::launchPlan(long planId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    FUNCTION_ENTRY("launchPlan");

    // Get the location ID with which to create the plan agent library
    unsigned long locationId = SessionManager::getInstance().getSessionDetails().getLocationKey();
    TA_Base_Bus::PlanAgentLibrary planLibrary(locationId);

    // Get the session ID.
    std::string sessionId = SessionManager::getInstance().getSessionDetails().getSessionId();

    // The ActivePlanId is a struct, but only the "plan" field is used.
    TA_Base_Core::ActivePlanId planDetails;
    planDetails.plan = planId;

    // Tell the plan library to run the plan
    planLibrary.runPlan(sessionId.c_str(),planDetails);

    FUNCTION_EXIT;
	return S_OK;
}

STDMETHODIMP CControlStation2::launchPlanByName(BSTR planPath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    FUNCTION_ENTRY("launchPlanByName");

    // Get the location ID with which to create the plan agent library
    unsigned long locationId = SessionManager::getInstance().getSessionDetails().getLocationKey();
    TA_Base_Bus::PlanAgentLibrary planLibrary(locationId);

    // Get the session ID.
    std::string sessionId = SessionManager::getInstance().getSessionDetails().getSessionId();

    // Tell the plan library to run the plan
    _bstr_t planPathStr(planPath);
    planLibrary.runPlanByName(sessionId.c_str(), planPathStr);

    FUNCTION_EXIT;
	return S_OK;
}

STDMETHODIMP CControlStation2::isActionPermittedOnEntity(BSTR entityName, long actionKey, /*[out, retval]*/ ResponseEnum* response)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    FUNCTION_ENTRY("isActionPermittedOnEntity");
    TA_Base_App::TA_ControlStation::EPermission permission;
    CString entityNameStr(entityName);

    // Convert entity entity into resource key.
    try
    {
        std::auto_ptr<TA_Base_Core::IEntityData> entity(TA_Base_Core::EntityAccessFactory::getInstance().getEntity( (LPCTSTR)entityNameStr));
        std::auto_ptr<TA_Base_Core::IResource> resource(TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(entity->getKey()));

        permission = SessionManager::getInstance().isActionPermitted(actionKey, resource->getKey());
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Error occurred while attempting to fetch resource from database");
        *response = TAError;
        FUNCTION_EXIT;
        return S_OK;
    }

    switch(permission)
    {
        case TA_Base_App::TA_ControlStation::DENIED:
            *response = Deny;
            break;
        case TA_Base_App::TA_ControlStation::PERMITTED:
            *response = Permit;
            break;
        case TA_Base_App::TA_ControlStation::UNKNOWN:
            *response = Indeterminate;
            break;
        case TA_Base_App::TA_ControlStation::GENERAL_ERROR:
            *response = TAError;
            break;
    }

    FUNCTION_EXIT;
    return S_OK;
}

STDMETHODIMP CControlStation2::entityCommand(EntityMessageType entityType, BSTR location, BSTR sourceSchematicID, BSTR message)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    FUNCTION_ENTRY("entityCommand");
	// TODO: Add your implementation code here
    CString locationStr(location);
    CString sourceSchematicIDStr(sourceSchematicID);
    CString messageStr(message);

	// Check if the command is targeted at a location. If it is do a send comms message, else broadcast
	// Location is the conceptual location of the operator.
	CString strResult = SessionManager::getInstance().getSessionDetails().getLocationName().c_str();
	if(strResult == locationStr)
	{
		TA_Base_Core::IManagedApplicationCorbaDef::GXToTAMessage   event;
		CORBA::Any                                data;
		switch(entityType)
		{
		case All:
			event.entityType = TA_Base_Core::IManagedApplicationCorbaDef::EntityMessageType::All;
			break;
		case RadioAgent:
			event.entityType = TA_Base_Core::IManagedApplicationCorbaDef::EntityMessageType::RadioAgent;
			break;
		//lizettejl++ (DP-changes)
		case ControlStation:
			event.entityType = TA_Base_Core::IManagedApplicationCorbaDef::EntityMessageType::ControlStation;
			break;
		//++lizettejl
		default:
			break;
		}
		event.sourceSchematicID = CORBA::string_dup((LPCTSTR)sourceSchematicIDStr);
		event.message = CORBA::string_dup((LPCTSTR)messageStr);
		event.sessionId = CORBA::string_dup(SessionManager::getInstance().getSessionDetails().getSessionId().c_str());

		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::ControlStationComms::GXToTAMessageNotification );

		commsMessageSender->sendCommsMessage(TA_Base_Core::ControlStationComms::GXToTAMessageNotification,
											 SessionManager::getInstance().getControlStationEntity().getKey(),
											 data,
											 0,
											 SessionManager::getInstance().getSessionDetails().getLocationKey(),
											 NULL);
	}
	// The current messaging subsystem does not support the same message type to be used for both broadcast and
	// non-broadcast comms messages. The below implementation is expected to be supported by the new
	// messaging system and can be uncommented when that is in place
/*	else
	{
		TA_Base_Core::IManagedApplicationCorbaDef::GXToTAMessage   event;
		CORBA::Any                                data;
		switch(entityType)
		{
		case All:
			event.entityType = TA_Base_Core::IManagedApplicationCorbaDef::EntityMessageType::All;
			break;
		case RadioAgent:
			event.entityType = TA_Base_Core::IManagedApplicationCorbaDef::EntityMessageType::RadioAgent;
			break;
		default:
			break;
		}
		event.sourceSchematicID = CORBA::string_dup((LPCTSTR)sourceSchematicIDStr);
		event.message = CORBA::string_dup((LPCTSTR)messageStr);
		event.sessionId = CORBA::string_dup(SessionManager::getInstance().getSessionDetails().getSessionId().c_str());

		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::ControlStationComms::GXToTAMessageNotification );

		commsMessageSender->sendBroadcastCommsMessage(TA_Base_Core::ControlStationComms::GXToTAMessageNotification, data);
	}*/

    FUNCTION_EXIT;
	return S_OK;
}



//TD12495 - added login verification 
STDMETHODIMP CControlStation2::isLoginAllowed(BSTR bstrOperator, BSTR bstrPassword, BOOL *bLogin)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*bLogin = FALSE;

	// TODO: Add your implementation code here
	try
	{			
		SessionDetails& details = SessionManager::getInstance().getSessionDetails();
		CString strOperator = details.getOperatorName().c_str();
		if (strOperator.IsEmpty()) 
		{
			return S_OK;
		}
		
		char* pszOperator = _com_util::ConvertBSTRToString(bstrOperator);
		if (strOperator.Compare(pszOperator) != 0)
		{
			delete []pszOperator;
			return S_OK;
		}
		
		SessionDetails::SessionInfo info = details.getSessionInfo();	
		char* pszInput = _com_util::ConvertBSTRToString(bstrPassword);
		
 		CString strPwd(info.password.c_str());
		if (strPwd.Compare(pszInput) == 0)	
		{
			*bLogin = TRUE;
		}
		
		delete []pszInput;
		delete []pszOperator;
		return S_OK;
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Error occurred while attempting verify password from given operator");		
	}

	return S_OK;
}

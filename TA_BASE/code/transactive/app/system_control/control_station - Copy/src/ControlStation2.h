/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ControlStation2.h $
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

#ifndef __CONTROLSTATION2_H_
#define __CONTROLSTATION2_H_

#include "app\system_control\control_station\src\resource.h"       // main symbols
#include "app\system_control\control_station\src\ControlStationCP.h"
#include "app\system_control\control_station\src\IProcessManager.h"

#include <string>
#include <boost/regex.hpp>
#include <map>
#include <vector>
#include "ControlStationCP.h"

/////////////////////////////////////////////////////////////////////////////
// CControlStation3
class ATL_NO_VTABLE CControlStation2 : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CControlStation2, &CLSID_ControlStation2>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CControlStation2>,
	public IDispatchImpl<IControlStation2, &IID_IControlStation2, &LIBID_ControlStationLib>,
	public CProxy_IControlStation2Events< CControlStation2 >
{
public:
	CControlStation2();

DECLARE_REGISTRY_RESOURCEID(IDR_CONTROLSTATION2)

DECLARE_PROTECT_FINAL_CONSTRUCT()

DECLARE_CLASSFACTORY_SINGLETON(CControlStation2)

BEGIN_COM_MAP(CControlStation2)
	COM_INTERFACE_ENTRY(IControlStation2)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CControlStation2)
CONNECTION_POINT_ENTRY(DIID__IControlStation2Events)
END_CONNECTION_POINT_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IControlStation2
public:
	STDMETHOD(entityCommand)(EntityMessageType entityType, BSTR location, BSTR sourceSchematicID, BSTR message);
	STDMETHOD(isLoginAllowed)(BSTR bstrOperator, BSTR bstrPassword, /*[out, retval]*/BOOL* bLogin);
    STDMETHOD(isActionPermittedOnEntity)(BSTR entityName, long actionKey, /*[out, retval]*/ ResponseEnum* response);	
	STDMETHOD(launchPlan)(long planId);
	STDMETHOD(launchPlanByName)(BSTR planPath);
	STDMETHOD(launchHelp)();
	STDMETHOD(loadComplete)(long leftPosition);
	STDMETHOD(getSessionId)(/*[out, retval]*/ BSTR* sessionId);
	STDMETHOD(getCurrentDisplayName)(/*[out, retval]*/ BSTR* display);
	STDMETHOD(getRect)(EScreen targetScreen, EArea targetArea, long val, /*[out, retval]*/ RECT* dim);
    STDMETHOD(setDisplayDimension)(RECT* dim);
	STDMETHOD(launchDisplayEx)(BSTR display, BSTR asset, long leftPosition);
	STDMETHOD(changePassword)();

	STDMETHOD(isOperatorOverridden)(BOOL*);
	STDMETHOD(changeOperatorOverride)();
	STDMETHOD(displayTaskManager)();
    STDMETHOD(isServerContactable)(BOOL*);
	STDMETHOD(logout)();
    STDMETHOD(giveApplicationFocus)(long processId);
	STDMETHOD(getOperator)(BSTR*);
    STDMETHOD(getSite)(BSTR*);
    STDMETHOD(getProfile)(BSTR*);
    STDMETHOD(launchApplicationEx)(long appType, BSTR* extraCommandLine, long posType, long alignType, RECT* objectDim, RECT* boundaryDim);
	STDMETHOD(getAllApplications)(VARIANT*);
	STDMETHOD(getRunningApplications)(VARIANT*);
    STDMETHOD(getLocation)(BSTR*);
    STDMETHOD(setSelectedAlarms)(BSTR FAR* asset, long leftPosition);
	STDMETHOD(getProfileDisplay)(long leftPosition, BSTR*);
	STDMETHOD(getProfileDisplayAtLocation)(long leftPosition, BSTR locationName, BSTR*);
	STDMETHOD(changeProfile)();

    // these are old methods that should probably be deprecated some time in future when none
    // of the GraphWorx displays uses these methods.
	STDMETHOD(launchDisplay)(BSTR display, long leftPosition);
	STDMETHOD(launchApplication)(long appType, long leftPosition, long topPosition, long width, long height, BSTR FAR* addtionalCommands);

private:
	static boost::regex APP_PATTERN;

    /**
    * createArray
    *
    * This creates a 2D array using the map passed. The first column will contain the indexes of the map
    * and the second column will contain the data.
    *
    * @param map<unsigned long, string> mapData - The map of data to convert into the 2D array.
    *
    * @return VARIANT FAR& - The two dimesional array.
    */
    void createArray(std::map<unsigned long, std::string>& mapData, VARIANT FAR& vaResult);


    /**
    * createArray
    *
    * This creates a 3D array using the vector passed. The first column will contain the key, second column
    * will contain the name and third column a boolean (0/1) indicating whether the current operator has 
    * the rights to access the application
    *
    * @param std::vector<TA_Base_App::TA_ControlStation::ApplicationProperties>& - The vector containing 
    *        the application data for conversion
    *
    * @return VARIANT FAR& - The two dimesional array.
    */
    void CControlStation2::createArray(std::vector<IProcessManager::ApplicationProperties>& applist, VARIANT FAR& vaResult);


    /**
    * logError
    *
    * If an error is called this can be called to use DebugUtil to log the error.
    *
    * @param HRESULT - This contains the error that occurred. A switch statement is used on this to
    *                  determine what type of error occurred and which log to use.
    */
	void logError(const HRESULT& hr);
	 /**
    * getAppName
    *
    * use the exception msg extract the application name
    *
    * @param ex_str - the Application Exception string 
	*
	* @return std::string Application name
    *                 
    */
	std::string getAppName(std::string ex_str);

};

#endif //__CONTROLSTATION2_H_

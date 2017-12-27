/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionManagerApp.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class provides the entry point into the application.
  * It instantiates RegionManagerGUI, but leaves it to the
  * GenericGUI to display.
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


/////////////////////////////////////////////////////////////////////////////
// The one and only RegionManagerApp object

RegionManagerApp regionManagerApp;


/////////////////////////////////////////////////////////////////////////////
// RegionManagerApp

BEGIN_MESSAGE_MAP(RegionManagerApp, TA_Base_Bus::TransActiveWinApp)
	//{{AFX_MSG_MAP(RegionManagerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RegionManagerApp construction/destruction

RegionManagerApp::RegionManagerApp() : 
    TA_Base_Bus::TransActiveWinApp(new RegionManagerGUI, "Region Manager")
{
    FUNCTION_ENTRY("RegionManagerApp()");

    // Note: Place all significant initialization in InitInstance

    FUNCTION_EXIT;
}


RegionManagerApp::~RegionManagerApp()
{
    FUNCTION_ENTRY("~RegionManagerApp()");
    FUNCTION_EXIT;
}

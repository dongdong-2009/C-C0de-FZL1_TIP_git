/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/transactiveHelp/transactiveHelp.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// prnScreen.cpp : Defines the entry point for the DLL application.
//


#include "bus/mfc_extensions/src/transactiveHelp/transactiveHelp.h"

#include <fstream>

namespace TA_Base_Bus
{
	TRANSACTIVEHELP_API int positionTransactiveHelp(const char* szHelp, 
		const char* szWindow,
		const char* szCaption,
		const char* szHome,
		long left, 
		long top, 
		long right, 
		long bottom)
	{
		HH_WINTYPE WinType;
	
		//ofstream input;
		//input.open("loghelp.txt");
		
		//input << "szHelp = " << szHelp << endl;
		//input << "szWindow = " << szWindow << endl;
		//input << "szCaption = " << szCaption << endl;
		//input << "szHome = " << szHome << endl;
		
		//input << "top = " << top << endl;
		//input << "left = " << left << endl;
		//input << "right  = " << right << endl;
		//input << "bottom = " << bottom << endl;
		
		//input.close();

		memset(&WinType,0,sizeof(WinType));
		WinType.cbStruct = sizeof(WinType);
		
		// WinType.fUniCodeStrings;
		
		// Specify the new window type
		WinType.pszType = (LPCTSTR) szWindow;
		
		// Put together the desired window parameters
		WinType.fsValidMembers = 
			HHWIN_PARAM_PROPERTIES | 
			HHWIN_PARAM_RECT | 
			HHWIN_PARAM_TB_FLAGS | 
			HHWIN_PARAM_STYLES | 
			HHWIN_PARAM_EXSTYLES | 
			HHWIN_PARAM_SHOWSTATE | 
			HHWIN_PARAM_EXPANSION | 
			HHWIN_PARAM_TABPOS;  
		
		// Specify the new window's properties
		WinType.fsWinProperties = 
			HHWIN_PROP_TRI_PANE | /* Create a three-pane window - help/nav and toolbar*/ 
			HHWIN_PROP_TAB_SEARCH | /* Include the "Search" tab in the navigation window*/ 
			HHWIN_PROP_TAB_ADVSEARCH | 
			HHWIN_PROP_NODEF_EXSTYLES | /* No extended styles should be used*/ 
			HHWIN_PROP_TRACKING | /* Send notification messages to the owner window*/ 
			HHWIN_PROP_AUTO_SYNC | /* Auto sync the content tab and the help window automatically*/ 
			HHWIN_PROP_POST_QUIT;  
		
		// List which toolbar buttons to display
		WinType.fsToolBarFlags = 
			HHWIN_BUTTON_EXPAND | 
			HHWIN_BUTTON_BACK | 
			HHWIN_BUTTON_FORWARD | 
			HHWIN_BUTTON_PRINT | 
			HHWIN_BUTTON_OPTIONS; 
		
		// Give the window a caption
		WinType.pszCaption = (LPCTSTR) szCaption;     
		
		// Use any of the styles listed for use with 
		// the CreateWindow API call
		WinType.dwStyles = WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX ;    
		
		// Use any of the extended styles listed for 
		// use with the CreateWindowEx API call
		WinType.dwExStyles = 0;
		
		// Use any of the states listed for use
		// with the ShowWindow API call.
		// You should only use one here
		WinType.nShowState = SW_SHOW;
		
		// WinType.hwndHelp;     
		// WinType.hwndCaller;
		// WinType.hwndToolBar;      
		// WinType.hwndNavigation;   
		// WinType.hwndHTML; 
		
		// Set the width of the navigation pane
		WinType.iNavWidth = 250;
		
		//WinType.rcHTML;
		
		// Specify the TOC, Index, default, and Home button
		// files as found within the source files for the CHM
		WinType.pszFile = (LPCTSTR) szHome;    
		WinType.pszHome =  (LPCTSTR) szHome;    
		
		// TRUE = don't show the nav pane, and FALSE = show it
		WinType.fNotExpanded = FALSE; 
		
		RECT wndPos;
		wndPos.top = top;
		wndPos.left = left;
		wndPos.right = right;
		wndPos.bottom = bottom;
		
		WinType.rcWindowPos = wndPos;
		
		HtmlHelp(
			0,
			(LPCTSTR) szHelp,
			HH_SET_WIN_TYPE,
			(DWORD) &WinType);
		
		HtmlHelp(
			0,
			(LPCTSTR) szHelp,
			HH_DISPLAY_TOPIC,
			NULL) ;
		
		return TRUE;
		
	}
	
}
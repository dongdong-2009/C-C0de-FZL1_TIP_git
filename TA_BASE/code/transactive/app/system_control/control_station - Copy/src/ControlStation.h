/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ControlStation.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the application starting point. This is the first object that will be created
  * (from our perspective) and InitInstance will be called.
  */


#if !defined(AFX_CONTROLSTATION_H__4D34729A_3EE4_4CC5_BC82_2A82EABBBFAE__INCLUDED_)
#define AFX_CONTROLSTATION_H__4D34729A_3EE4_4CC5_BC82_2A82EABBBFAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "app\system_control\control_station\src\resource.h"		// main symbols
#include "app\system_control\control_station\src\ControlStation_i.h"
#include "app\system_control\control_station\src\LogsThread.h"
#include <string>
#include <afxmt.h>		// for CMutex
/////////////////////////////////////////////////////////////////////////////
// ControlStationApp:
// See ControlStation.cpp for the implementation of this class
//

class ControlStationApp : public CWinApp
{
public:
	ControlStationApp();
	virtual ~ControlStationApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ControlStationApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(ControlStationApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    /** 
      * initATL
      *
      * Perform ATL specific initialisation
      *
      * @return BOOL 
      *
      */
	BOOL initATL();


    /**
     * initialiseUtilities
     *
     * This parses the command line and sets up CorbaUtil and DebugUtil.
     *
     * @param string - The command line passed to the application
     *
     * @exception InvalidCommandLineException thrown if the command line 
     *            arguments are not of the form: foo --parameter-name=BAR
     * @exception UtilitiesException raised if CorbaUtil or DebugUtil fail
     *            to initialise
     */
    void initialiseUtilities(const std::string& commandLine);


    /** 
      * restartWorkstation
      *
      * description
      *
      */
    void restartWorkstation();


    /** 
      * exitFromWindows
      *
      * This method determine if it has to exit to the desktop or to logout the 
      * windows operator to get to the windows logon screen
      *
      */
    void exitFromWindows();



    //TD12436 ++wenguang
	/**
	  * archiveLogFiles
	  * 
	  * This zips all the old log files in the startup of control station.
	  * 
	  * @param : const std::string& debugFileName
	  * 
	  * @exception <exceptions> Optional
	  */
    void archiveLogFiles(const std::string& debugFileName);
    //TD12436 wenguang++


	BOOL m_bATLInited; // was ATL initialised?

    DWORD m_focusTimeout;

	CMutex * m_mutex;

	CLogsThread *logsthread;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLSTATION_H__4D34729A_3EE4_4CC5_BC82_2A82EABBBFAE__INCLUDED_)

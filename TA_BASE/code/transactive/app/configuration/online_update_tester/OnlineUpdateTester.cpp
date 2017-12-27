/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/online_update_tester/OnlineUpdateTester.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#include "stdafx.h"
#include "Afxwin.h"

#include "OnlineUpdateMessageReceiver.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include <stdio.h>

using TA_Base_Core::RunParams;

int main(int argc, char* argv[])
{
//	printf("nooni\n");
	RunParams::getInstance().parseCmdLine(argc,argv);
  
    //printf("%s, %s\n", RunParams::getInstance().get(RPARAM_NOTIFYHOSTS), RunParams::getInstance().get(RPARAM_DBCONNECTION));
    if (RunParams::getInstance().get(RPARAM_NOTIFYHOSTS).empty() ||
		RunParams::getInstance().get(RPARAM_DBCONNECTION).empty() ||
		RunParams::getInstance().get(RPARAM_DEBUGLEVEL).empty() ||
		RunParams::getInstance().get(RPARAM_DEBUGFILE).empty() ||
		RunParams::getInstance().get(RPARAM_LOCATIONKEY).empty() )
	{
		AfxMessageBox("usage: \n OnlineUpdateTester.exe --notify-hosts=occa:6667,occa:6668 \n	--db-connection=tra_int:it261:it261 \n	--location-key=1 \n	--debug-level=TRACE \n	--debug-file=c:\\transactive\\logs\\onlinedoovyhicky.log");
    }
    
	TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::getDebugLevelFromString(RunParams::getInstance().get(RPARAM_DEBUGLEVEL).c_str()));
    TA_Base_Core::DebugUtil::getInstance().setFile(RunParams::getInstance().get(RPARAM_DEBUGFILE).c_str());
    TA_Base_Core::CorbaUtil::getInstance().initialise();
    TA_Base_Core::CorbaUtil::getInstance().activate();

    OnlineUpdateMessageReceiver receiver;

    do
    {
        Sleep(1000);
    }while (true);
  	return 0;
}

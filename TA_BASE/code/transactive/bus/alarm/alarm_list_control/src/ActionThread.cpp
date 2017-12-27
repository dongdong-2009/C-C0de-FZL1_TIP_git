/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/ActionThread.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class performs an action in a separate thread to the main thread. This is to ensure the action
 * does not freeze the main application's update.
 */

#include "bus/alarm/alarm_list_control/src/stdafx.h"
#include "bus/alarm/alarm_list_control/src/ActionThread.h"
#include "bus/alarm/alarm_list_control/src/IAlarmAction.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/AlarmListCtrl.h"
#include "bus/alarm/alarm_list_control/src/resource.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionSuppressAlarm.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/exceptions/src/AlarmActionException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::AlarmActionException;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    const UINT ActionThread::PERFORM_ACTION = WM_USER + 200;
	const UINT ActionThread::PERFORM_PREACTION = WM_USER + 201;

    IMPLEMENT_DYNCREATE(ActionThread, CWinThread)

    ActionThread::ActionThread()
    {
    }

    ActionThread::~ActionThread()
    {
    }


    BOOL ActionThread::InitInstance()
    {
	    // No per-thread initialization required here
		CoInitialize(0);
	    return TRUE;
    }

    int ActionThread::ExitInstance()
    {
	    // No per-thread cleanup required here
		CoUninitialize();
	    return CWinThread::ExitInstance();
    }

BEGIN_MESSAGE_MAP(ActionThread, CWinThread)
	//{{AFX_MSG_MAP(ActionThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
    ON_THREAD_MESSAGE(PERFORM_ACTION, performThreadedAction)
	ON_THREAD_MESSAGE(PERFORM_PREACTION, performPreAction)
END_MESSAGE_MAP()


    /////////////////////////////////////////////////////////////////////////////
    // ActionThread message handlers
	void ActionThread::performPreAction( WPARAM wParam, LPARAM lParam )
	{
		reinterpret_cast<AlarmListCtrl*>(wParam)->doPreAction();
		AfxEndThread(0,true);
	}
    void ActionThread::performThreadedAction( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY("performThreadedAction");

        IAlarmAction* action = reinterpret_cast<IAlarmAction*>(wParam);
        AlarmRetriever* alarmRetriever = reinterpret_cast<AlarmRetriever*>(lParam);

        if (action == NULL || alarmRetriever == NULL)
        {
            AfxEndThread(0,true);
			FUNCTION_EXIT;
			return;
        }

		try
		{
			if ( action->getActionControlId() == INHIBIT_ALARM )
			{
				ActionSuppressAlarm* suppressAction = dynamic_cast<ActionSuppressAlarm*> (action);
				if (suppressAction == NULL)
				{
					AfxEndThread(0,true);
					FUNCTION_EXIT;
					return;
				}
				if (suppressAction->getSubActionControlId() == INHIBIT_ALARM_BY_TYPE)
				{
					suppressAction->performActionByType(*alarmRetriever);
				}
				else if ( suppressAction->getSubActionControlId()  == INHIBIT_ALARM_BY_DESCRIPTION )			
				{
					suppressAction->performActionByDescription(*alarmRetriever);
				}
				else
				{
					action->performAction(*alarmRetriever);
				}
			}
			else 
			{
				action->performAction(*alarmRetriever);
			}
		}
		catch( const TA_Base_Core::AlarmActionException& ex)
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName;
			unsigned int errorId;

			switch (ex.getFailType())
			{
				case AlarmActionException::CANNOT_FIND_ALARM:
					errorId = IDS_UE_040027;
					break;
				case AlarmActionException::CANNOT_UPDATE_ALARM_COMMENTS:
					errorId = IDS_UE_040023;
					actionName.LoadString(IDS_COMMENT);
					break;
				case AlarmActionException::CANNOT_ACKNOWLEDGE_ALARMS:
					errorId = IDS_UE_040023;
					actionName.LoadString(IDS_ACKNOWLEDGE);
					break;
				case AlarmActionException::CANNOT_ACKNOWLEDGE_ALARM:
					errorId = IDS_UE_040025;
					break;
				case AlarmActionException::CANNOT_ACKNOWLEDGE_AND_CLOSE_ALARMS:
					errorId = IDS_UE_040023;
					actionName.LoadString(IDS_ACKNOWLEDGE_AND_CLOSE);
					break;
				case AlarmActionException::CANNOT_FIND_ASSOCIATED_PLAN:
					errorId = IDS_UE_040008;
					break;
				case AlarmActionException::CANNOT_RUN_ASSOCIATED_PLAN:
					errorId = IDS_UE_040024;
					actionName.LoadString(IDS_RUN);
					break;
				case AlarmActionException::CANNOT_POSTPONE_PLAN:
					errorId = IDS_UE_040024;
					actionName.LoadString(IDS_POSTPONE);
					break;
				case AlarmActionException::CANNOT_LAUNCH_SCHEMATIC:
					errorId = IDS_UE_010002;
					actionName.LoadString(IDS_SCHEMATIC);
					break;
				case AlarmActionException::CANNOT_LAUNCH_REQUEST_MANAGER:
					errorId = IDS_UE_010002;
					actionName.LoadString(IDS_REQUEST_MANAGER);
					break;
				case AlarmActionException::CANNOT_LAUNCH_EVENT_VIEWER:
					errorId = IDS_UE_010002;
					actionName.LoadString(IDS_EVENT_VIEWER);
					break;
				case AlarmActionException::CANNOT_LAUNCH_INSPECTOR_PANEL:
					errorId = IDS_UE_010002;
					actionName.LoadString(IDS_INSPECTOR_PANEL);
					break;
				case AlarmActionException::CANNOT_LAUNCH_TREND_VIEWER:
					errorId = IDS_UE_010002;
					actionName.LoadString(IDS_TREND_VIEWER);
					break;
				case AlarmActionException::CANNOT_CREATE_SUPPRESSION_RULE:
					errorId = IDS_UE_010002;
					actionName.LoadString(IDS_INHIBIT_ALARM);
					break;
				case AlarmActionException::CANNOT_LAUNCH_INCIDENT_MANAGER:
					errorId = IDS_UE_010002;
					actionName.LoadString(IDS_INCIDENT_MANAGER);
					break;
				default:
					break;
			}

			if (!actionName.IsEmpty())
			{
				userMsg << actionName;
			}

			userMsg.showMsgBox(errorId);
		}
        catch( const TA_Base_Core::TransactiveException& te )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransActiveException", te.what() );
        }
        catch( ... )
        {
            std::ostringstream desc;
            desc << "[ActionThread::performThreadedAction] Caught unknown exception WPARAM " << wParam 
                << " LPARAM " << lParam;
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", desc.str().c_str() );
        }
        alarmRetriever->setIsActionAlreadyRunning(false);
        AfxEndThread(0,true);
        FUNCTION_EXIT;
    }

} // end of ActionThread

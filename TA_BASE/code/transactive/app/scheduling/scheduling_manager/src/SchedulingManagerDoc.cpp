/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_manager/src/SchedulingManagerDoc.cpp $
  * @author:  
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  * 
  * This document class is responsible for passing requests associated with jobs from 
  * the View to JobManager and passing update requests from the JobManager to the 
  * View.  
  * It communicates with the view through the MFC framework.
  * The JobManager communicates with it through the IJobUpdatable interface which the
  * document implements.  
  */

#include "app/scheduling/scheduling_manager/src/stdafx.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManager.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerConstants.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerDoc.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerView.h"
#include "app/scheduling/scheduling_manager/src/JobManager.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/exceptions/src/SchedulingManagerException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "bus\security\authentication_library\src\AuthenticationLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App::SchedulingManager;
using TA_Base_Bus::TransActiveMessage;
using TA_Base_Core::SchedulingManagerException;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CSchedulingManagerDoc

IMPLEMENT_DYNCREATE(CSchedulingManagerDoc, CDocument)

BEGIN_MESSAGE_MAP(CSchedulingManagerDoc, CDocument)
	//{{AFX_MSG_MAP(CSchedulingManagerDoc)
	ON_COMMAND(ID_VIEW_ALLJOBS, onViewAlljobs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALLJOBS, onUpdateViewAlljobs)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP_SCHEDULINGMANAGER, OnHelpSchedulingmanager)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CSchedulingManagerDoc construction/destruction

    CSchedulingManagerDoc::CSchedulingManagerDoc() :
    m_jobManager(NULL),
    m_isProfileFilterOn(true)
    {
	    FUNCTION_ENTRY( "Constructor" );
	    // TODO: add one-time construction code here
	    FUNCTION_EXIT;

    }


    CSchedulingManagerDoc::~CSchedulingManagerDoc()
    {
        cleanUp();
    }


    void CSchedulingManagerDoc::cleanUp()
    {
        if (m_jobManager != NULL)
        {
            delete m_jobManager;
            m_jobManager = NULL;
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // CSchedulingManagerDoc serialization

    void CSchedulingManagerDoc::Serialize(CArchive& ar)
    {
	    if (ar.IsStoring())
	    {
		    // TODO: add storing code here
	    }
	    else
	    {
		    // TODO: add loading code here
	    }
    }

    /////////////////////////////////////////////////////////////////////////////
    // CSchedulingManagerDoc diagnostics

    #ifdef _DEBUG
    void CSchedulingManagerDoc::AssertValid() const
    {
	    CDocument::AssertValid();
    }

    void CSchedulingManagerDoc::Dump(CDumpContext& dc) const
    {
	    CDocument::Dump(dc);
    }
    #endif //_DEBUG

    /////////////////////////////////////////////////////////////////////////////
    // CSchedulingManagerDoc commands

    void CSchedulingManagerDoc::initialise()
    {
	    FUNCTION_ENTRY( "initialise" );

        std::vector<std::string> jobTypes;
        try
        {
            m_jobManager = new JobManager(this);

            jobTypes = m_jobManager->getJobTypes();
        }
        catch (SchedulingManagerException& ex)
        {
            bool ok = false;
            TransActiveMessage userMsg;
            unsigned long errorId;
            CString action, agent;
            TA_VERIFY(action.LoadString(IDS_INITIALISE_APP) != 0, "String does not exist in string table");
				if (ex.getFailType() != SchedulingManagerException::ACCESS_DENIED)
				{
					userMsg << action;
				}

            switch(ex.getFailType())
            {
            case SchedulingManagerException::SCHEDULING_AGENT_FAILURE: // Couldn't contact the scheduling agent
                TA_VERIFY(agent.LoadString(IDS_SCHEDULING_AGENT) != 0, "String does not exist in string table");
                userMsg << agent;
                errorId = IDS_UE_030080;
                break;
            case SchedulingManagerException::RIGHTS_AGENT_FAILURE:   // Couldn't contact the rights agent
                TA_VERIFY(agent.LoadString(IDS_RIGHTS_AGENT) != 0, "String does not exist in string table");
                userMsg << agent;
                errorId = IDS_UE_030080;
                break;
            case SchedulingManagerException::UNKNOWN_EXCEPTION:      // Unknown exception is thrown
					 userMsg << action;
                break;
            case SchedulingManagerException::ACCESS_DENIED:          // Session has insufficient rights to perform action
					{
						errorId = IDS_UE_250017;
						//std::string userName;
						//std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
						//TA_Base_Bus::AuthenticationLibrary authLibrary(true);
						//TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
						//std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
						//userName = iOperator->getName();
						//userMsg << userName;
						//userMsg << action;
						break;
					}
            case SchedulingManagerException::DATABASE_ERROR:         // Unable to use the database 
                errorId = IDS_UE_250016;
                break;
            case SchedulingManagerException::DATA_ERROR:             // Error in database configuration
                errorId = IDS_UE_010011;
                break;
            case SchedulingManagerException::BAD_SCHEDULE:           // Some jobs contained bad schedules, but it's ok to continue
            case SchedulingManagerException::NO_DAY_OF_WEEK:         // The day of week is not specified in weekly tasks
                ok = true;
                break;
            default:
                TA_ASSERT(false, "Unknown exception type");
            }
            if (!ok)
            {
                userMsg.showMsgBox(errorId);

                delete m_jobManager;
                m_jobManager = NULL;

                // close the application
                AfxGetMainWnd()->PostMessage(WM_CLOSE);
                FUNCTION_EXIT;
                return;
            }
        }

        POSITION pos = GetFirstViewPosition();
        while (pos != NULL)
        {
            CSchedulingManagerView* pView = dynamic_cast<CSchedulingManagerView*>(GetNextView(pos));
            TA_ASSERT(pView != NULL, "Unable to get SchedulingManagerView");
            pView->initialise(jobTypes);
        }   

        UpdateAllViews(NULL);

        // If the job info is set, then we need to inform the job manager so that it could tell this 
        // doc to update.
        std::string jobInfo = RunParams::getInstance().get(TA_Base_App::SchedulingManager::RPARAM_JOBINFO.c_str());
        if (!jobInfo.empty())
        {
            RunParams::getInstance().set(TA_Base_App::SchedulingManager::RPARAM_JOBINFO.c_str(), jobInfo.c_str());
        }

        FUNCTION_EXIT;
    }


    bool CSchedulingManagerDoc::saveJob(const Job& job)
    {
	    FUNCTION_ENTRY( "saveJob" );
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        try
        {
            m_jobManager->saveJob(job);
            return true;
        }
        catch (const SchedulingManagerException& ex)
        {
            TransActiveMessage userMsg;
            unsigned long errorId;
            CString action, agent, itemName;
            TA_VERIFY(action.LoadString(IDS_SAVE_JOB) != 0, "String does not exist in string table");
				if (ex.getFailType() != SchedulingManagerException::ACCESS_DENIED)
				{
					userMsg << action;
				}

            switch(ex.getFailType())
            {
            case SchedulingManagerException::SCHEDULING_AGENT_FAILURE: // Couldn't contact the scheduling agent
                TA_VERIFY(agent.LoadString(IDS_SCHEDULING_AGENT) != 0, "String does not exist in string table");
                userMsg << agent;
                errorId = IDS_UE_030080;
                break;
            case SchedulingManagerException::RIGHTS_AGENT_FAILURE:   // Couldn't contact the rights agent
                TA_VERIFY(agent.LoadString(IDS_RIGHTS_AGENT) != 0, "String does not exist in string table");
                userMsg << agent;
                errorId = IDS_UE_030080;
                break;
            case SchedulingManagerException::UNKNOWN_EXCEPTION:      // Unknown exception is thrown
                errorId = IDS_UE_250015;
                break;
            case SchedulingManagerException::ACCESS_DENIED:          // Session has insufficient rights to perform action
					{
						errorId = IDS_UE_250017;
						//std::string userName;
						//std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
						//TA_Base_Bus::AuthenticationLibrary authLibrary(true);
						//TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
						//std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
						//userName = iOperator->getName();
						//userMsg << userName;
						//userMsg << action;
						break;
					}
            case SchedulingManagerException::DATABASE_ERROR:         // Unable to use the database or error in database
                errorId = IDS_UE_250016;
                break;
            case SchedulingManagerException::JOB_IN_PAST:            // Schedule is set to run in the past
                errorId = IDS_UE_250018;
                break;
            case SchedulingManagerException::NO_JOB_PARAMETER:       // No job specified details are specified
                TA_VERIFY(itemName.LoadString(IDS_JOB_SETTING) != 0, "String does not exist in string table");
                userMsg << itemName;
                errorId = IDS_UE_250014;
                break;
            case SchedulingManagerException::NO_DAY_OF_WEEK:         // Job contain bad dates etc, output the error returned by agent
					 errorId = IDS_UE_250019;
                break;
            case SchedulingManagerException::BAD_SCHEDULE:           // Schedule is invalid, just assert as the job dialog supposed to stop it
                TA_ASSERT(false, "Job dialog supposed to prevent bad schedules");
            case SchedulingManagerException::INVALID_JOB_PARAMETER:  // Job parameters are not configured properly
					 errorId = IDS_UE_250020;
                break;
            case SchedulingManagerException::MISSING_JOB_PARAMETER:  // Job parameters are missing
					 errorId = IDS_UE_250021;
                break;
            case SchedulingManagerException::JOB_NOT_EXIST:          // Job does not exist
					 errorId = IDS_UE_250022;
                break;
            case SchedulingManagerException::TOO_MANY_JOBS:          // Too many jobs are scheduled to start on the same time
					 errorId = IDS_UE_250023;
                break;
            default:
                TA_ASSERT(false, "Unknown exception type");
            }

            userMsg.showMsgBox(errorId);
            return false;
        }
	    FUNCTION_EXIT;
    }


    bool CSchedulingManagerDoc::saveNewJob(Job& job)
    {
	    FUNCTION_ENTRY( "saveNewJob" );
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        try
        {
            m_jobManager->saveNewJob(job);
            return true;
        }
        catch (const SchedulingManagerException& ex)
        {
            TransActiveMessage userMsg;
            unsigned long errorId;
            CString action, agent, itemName;
            TA_VERIFY(action.LoadString(IDS_SAVE_NEW_JOB) != 0, "String does not exist in string table");
            if (ex.getFailType() != SchedulingManagerException::ACCESS_DENIED)
				{
					userMsg << action;
				}

            switch(ex.getFailType())
            {
            case SchedulingManagerException::SCHEDULING_AGENT_FAILURE: // Couldn't contact the scheduling agent
                TA_VERIFY(agent.LoadString(IDS_SCHEDULING_AGENT) != 0, "String does not exist in string table");
                userMsg << agent;
                errorId = IDS_UE_030080;
                break;
            case SchedulingManagerException::RIGHTS_AGENT_FAILURE:   // Couldn't contact the rights agent
                TA_VERIFY(agent.LoadString(IDS_RIGHTS_AGENT) != 0, "String does not exist in string table");
                userMsg << agent;
                errorId = IDS_UE_030080;
                break;
            case SchedulingManagerException::UNKNOWN_EXCEPTION:      // Unknown exception is thrown
                errorId = IDS_UE_250015;
                break;
            case SchedulingManagerException::ACCESS_DENIED:          // Session has insufficient rights to perform action
					{
						errorId = IDS_UE_250017;
						//std::string userName;
						//std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
						//TA_Base_Bus::AuthenticationLibrary authLibrary(true);
						//TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
						//std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
						//userName = iOperator->getName();
						//userMsg << userName;
						//userMsg << action;
						break;
					}
            case SchedulingManagerException::DATABASE_ERROR:         // Unable to use the database or error in database
                errorId = IDS_UE_250016;
                break;
            case SchedulingManagerException::JOB_IN_PAST:            // Schedule is set to run in the past
                errorId = IDS_UE_250018;
                break;
            case SchedulingManagerException::NO_JOB_PARAMETER:       // No job specified details are specified
                TA_VERIFY(itemName.LoadString(IDS_JOB_SETTING) != 0, "String does not exist in string table");
                userMsg << itemName;
                errorId = IDS_UE_250014;
                break;
            case SchedulingManagerException::NO_DAY_OF_WEEK:         // Job contain bad dates etc, output the error returned by agent
                errorId = IDS_UE_250019;
                break;
            case SchedulingManagerException::BAD_SCHEDULE:           // Schedule is invalid, just assert as the job dialog supposed to stop it
                TA_ASSERT(false, "Job dialog supposed to prevent bad schedules");
            case SchedulingManagerException::INVALID_JOB_PARAMETER:  // Job parameters are not configured properly
                errorId = IDS_UE_250020;
                break;
            case SchedulingManagerException::MISSING_JOB_PARAMETER:  // Job parameters are missing
                errorId = IDS_UE_250021;
                break;
            case SchedulingManagerException::JOB_NOT_EXIST:          // Job does not exist
                errorId = IDS_UE_250022;
                break;
            case SchedulingManagerException::TOO_MANY_JOBS:          // Too many jobs are scheduled to start on the same time
                errorId = IDS_UE_250023;
                break;
            case SchedulingManagerException::JOB_EXIST:              // Job is identical to original
                errorId = IDS_UE_250011;
                break;
            default:
                TA_ASSERT(false, "Unknown exception type");
            }
            userMsg.showMsgBox(errorId);
            return false;
        }
	    FUNCTION_EXIT;
    }


    void CSchedulingManagerDoc::deleteJob(const std::string& jobId)
    {
	    FUNCTION_ENTRY( "deleteJob" );
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        m_jobManager->deleteJob(jobId);
	    FUNCTION_EXIT;
    }


    void CSchedulingManagerDoc::enableJob(const std::string& jobId, const bool enable)
    {
	    FUNCTION_ENTRY( "enableJob" );
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        m_jobManager->enableJob(jobId, enable);
	    FUNCTION_EXIT;
    }

	//TD16352++
	void CSchedulingManagerDoc::updateEditLock(const std::string& jobId, const bool bLock)
	{
		FUNCTION_ENTRY( "updateEditLock" );
		TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
		m_jobManager->updateEditLock(jobId, bLock);
		FUNCTION_EXIT;
	}

	bool CSchedulingManagerDoc::isJobLockedForEdit(const std::string& jobId)
	{
		FUNCTION_ENTRY( "isJobLockedForEdit" );
		TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
		FUNCTION_EXIT;
		return m_jobManager->isJobLockedForEdit(jobId);
	}
	//++TD16352


    JobPtrMap CSchedulingManagerDoc::getAllJobs()
    {
	    FUNCTION_ENTRY( "getAllJobs" );
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        if (m_isProfileFilterOn)
        {
            return m_jobManager->getAllJobsByProfile();
        }
        return m_jobManager->getAllJobs();
	    FUNCTION_EXIT;
    }


    unsigned long CSchedulingManagerDoc::getJobCount()
    {
	    FUNCTION_ENTRY( "getJobCount" );
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        if (m_isProfileFilterOn)
        {
            return m_jobManager->getTotalJobCountByProfile();
        }
        return m_jobManager->getTotalJobCount();
	    FUNCTION_EXIT;
    }


    bool CSchedulingManagerDoc::isActionOnJobPermitted(SchedulingManager::EAction action, JobPtrVector* job)
    {
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        return m_jobManager->isActionOnJobPermitted(action, job);
    }


    std::vector<unsigned long> CSchedulingManagerDoc::getSessionProfiles()
    {
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        return m_jobManager->getSessionProfiles();
    }


    unsigned long CSchedulingManagerDoc::getSessionLocation() const
    {
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        return m_jobManager->getSessionLocation();
    }


    ProfileVector CSchedulingManagerDoc::getAllOperatorProfiles() const
    {
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        return m_jobManager->getAllOperatorProfiles();
    }


    ProfileMap CSchedulingManagerDoc::getAllSchedulableProfiles() const
    {
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        return m_jobManager->getAllSchedulableProfiles();
    }


    LocationMap CSchedulingManagerDoc::getAllLocations() const
    {
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        return m_jobManager->getAllLocations();
    }


    void CSchedulingManagerDoc::onViewAlljobs() 
    {
        m_isProfileFilterOn = !m_isProfileFilterOn;
        UpdateAllViews(NULL);
    }


    void CSchedulingManagerDoc::onUpdateViewAlljobs(CCmdUI* pCmdUI) 
    {
        pCmdUI->SetCheck(!m_isProfileFilterOn);
    }


    void CSchedulingManagerDoc::OnAppAbout() 
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
    }


    void CSchedulingManagerDoc::OnHelpSchedulingmanager() 
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }


    time_t CSchedulingManagerDoc::getLastUpdateTime() const
    {
        TA_ASSERT(m_jobManager != NULL, "JobManager is NULL, cannot continue");
        return m_jobManager->getLastUpdateTime();
    }


    //////////////////////////////////////////////////////////////////////////////////
    //
    //
    // Methods inherited from IJobUpdatable
    //
    //
    //////////////////////////////////////////////////////////////////////////////////

    void CSchedulingManagerDoc::updateJobs()
    {
	    FUNCTION_ENTRY( "updateJobs" );
        POSITION pos = GetFirstViewPosition();
        while (pos != NULL)
        {
            CView* pView = GetNextView(pos);
            TA_ASSERT(pView != NULL, "Unable to get the view");
            pView->PostMessage(WM_JOBLIST_UPDATE);
        }   
	    FUNCTION_EXIT;
    }


    void CSchedulingManagerDoc::updateRights()
    {
	    FUNCTION_ENTRY( "updateRights" );
        updateJobs();
	    FUNCTION_EXIT;
    }


    void CSchedulingManagerDoc::createJob(const Job& job)
    {
	    FUNCTION_ENTRY( "createJob" );
        POSITION pos = GetFirstViewPosition();
        while (pos != NULL)
        {
            CView* pView = GetNextView(pos);
            TA_ASSERT(pView != NULL, "Failed to get a view");
            Job* newJob = new Job(job);
            pView->PostMessage(WM_JOB_CREATE, reinterpret_cast<WPARAM>(newJob), 0);
        }   
	    FUNCTION_EXIT;
    }


    void CSchedulingManagerDoc::jobUpdated(const Job& job, const bool isAssociatedWithSessionProfiles)
    {
	    FUNCTION_ENTRY( "updateJobs" );

		// let the job go through here - do the check later on in SchedulingManagerView.
		// if the job's profile does not match the current session profile, but DID before this update
		// (eg. session profile is "Super". this job update involved changing job profile from "Super" to "SO"),
		// then it needs to be removed from the list control. if we chop off the update here, this won't happen.
        /*if (m_isProfileFilterOn && !isAssociatedWithSessionProfiles)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Profile filter is on but the job is not associated with the session profiles.  Not going to update view");
            return;
        }*/

        POSITION pos = GetFirstViewPosition();
        while (pos != NULL)
        {
            CView* pView = GetNextView(pos);
            TA_ASSERT(pView != NULL, "Unable to get the view");
            pView->PostMessage(WM_JOB_UPDATED, reinterpret_cast<WPARAM>(&job), 0);
        }   
	    FUNCTION_EXIT;
    }


    void CSchedulingManagerDoc::jobDeleted(const Job& job, const bool isAssociatedWithSessionProfiles)
    {
	    FUNCTION_ENTRY( "jobDeleted" );

        if (m_isProfileFilterOn && !isAssociatedWithSessionProfiles)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Profile filter is on but the job is not associated with the session profiles.  Not going to update view");
            return;
        }

        POSITION pos = GetFirstViewPosition();
        while (pos != NULL)
        {
            CView* pView = GetNextView(pos);
            TA_ASSERT(pView != NULL, "Unable to get the view");
            pView->PostMessage(WM_JOB_DELETED, reinterpret_cast<WPARAM>(&job), 0);
        }   
	    FUNCTION_EXIT;
    }


};


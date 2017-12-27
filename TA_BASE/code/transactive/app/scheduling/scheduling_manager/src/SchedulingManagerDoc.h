/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_manager/src/SchedulingManagerDoc.h $
  * @author:  
  * @version: $Revision: #3 $
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

#if !defined(AFX_SCHEDULINGMANAGERDOC_H__D0E5B0FF_ECF2_4F0F_AC0E_F01265E93D30__INCLUDED_)
#define AFX_SCHEDULINGMANAGERDOC_H__D0E5B0FF_ECF2_4F0F_AC0E_F01265E93D30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/scheduling/scheduling_manager/src/IJobUpdatable.h"
#include "app/scheduling/scheduling_manager/src/Job.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerStructs.h"
#include <vector>
#include <map>
#include "bus/scheduling/schedulingagentcorbadef/IDL/src/SchedulingAgentCorbaDef.h"


namespace TA_Base_App
{
    class JobManager;

    class CSchedulingManagerDoc : public CDocument, 
                                  public IJobUpdatable
    {
    protected: // create from serialization only
	    CSchedulingManagerDoc();
	    DECLARE_DYNCREATE(CSchedulingManagerDoc)

    // Attributes
    public:

    // Operations
    public:

        /**
         * Initialises the document to use the JobManager.
         */
        void initialise();


        bool isInitialised() const
        {
            return (m_jobManager != NULL);
        }


        /** 
          * saveJob
          *
          * Adds the job to the agent's schedule if it's not already there, update it otherwise.
          *
          * @param job The job that is going to be added or updated
          * 
          * @return True if successfully saved the job
          */
        bool saveJob(const Job& job);


        /** 
          * saveNewJob
          *
          * Adds the job to the agent's schedule if it is not identical to the existing job that 
          * has the specified job's Id.
          *
          * @param job The job that is going to be added
          *
          * @return True if successfully saved the save.
          */
        bool saveNewJob(Job& job);


        /** 
          * deleteJob
          *
          * Removes the specified job from the agent's scheduling queue.
          *
          * @param id The id of the job to delete
          */
        void deleteJob(const std::string& jobId);


        /** 
          * enableJob
          *
          * Enables/disables the specified job.
          *
          * @param id The id of the job to enable/disable
          * @param enable The enabling condition
          *
          */
        void enableJob(const std::string& jobId, const bool enable);

		//TD16352++
		/** 
          * enableJob
          *
          * Updates edit lock
          *
          * @param id The id of the job to enable/disable
          * @param enable The enable edit lock
          *
          */
        void updateEditLock(const std::string& jobId, const bool bLock);

		/** 
          * isJobLockedForEdit
          *
          * check if job is locked for edit
          *
          * @param id The id of the job
          *
          */
		bool isJobLockedForEdit(const std::string& jobId);
		//++TD16352


        /** 
          * getAllJobs
          *
          * Returns all jobs that the operator has the privilege to handle 
          *
          *
          * @return All jobs if the profile filter is off.  Otherwise return only 
          *         jobs that the operator has the privilege to handle 
          */
        JobPtrMap getAllJobs();


        /** 
          * getJobCount
          *
          * Returns the number of jobs that the operator has the privilege to handle 
          *
          *
          * @return The number of all jobs if the profile filter is off.  Otherwise return only 
          *         jobs that the operator has the privilege to handle 
          */
        unsigned long getJobCount();


        /** 
          * isActionOnJobPermitted
          *
          * Returns true if the operator has the rights to perform the action *AND*
          * one of operator's profile is the profile associated with the job.
          *
          * @param action The action to be performed on the job
          * @param job The job
          *
          */
        bool isActionOnJobPermitted(SchedulingManager::EAction action, JobPtrVector* job);


        /** 
          * getSessionProfiles
          *
          * All profile keys associated with this profile
          *
          *
          * @return All profile keys associated with this profile
          */
        std::vector<unsigned long> getSessionProfiles();


        /** 
          * getSessionLocation
          *
          * Returns the session location
          *
          *
          * @return the session location
          */
        unsigned long getSessionLocation() const;

        
        /** 
          * getAllOperatorProfiles
          *
          * Returns all profiles that are associated with the login operator
          *
          *
          * @return all profiles that are associated with the login operator
          */
        ProfileVector getAllOperatorProfiles() const;


        /** 
          * getAllSchedulableProfiles
          *
          * Returns a map of profiles that could be used to schedule a job
          *
          *
          * @return a map of profiles that could be used to schedule a job
          */
        ProfileMap getAllSchedulableProfiles() const;


        /** 
          * getAllLocations
          *
          * Returns a map of all locations
          *
          *
          * @return a map of all locations
          */
        LocationMap getAllLocations() const;


        /** 
          * isProfileFilterOn
          *
          * Returns true if the profile filter is on
          *
          *
          * @return true if the profile filter is on
          */
        bool isProfileFilterOn() const
        {
            return m_isProfileFilterOn;
        }


        /** 
          * cleanUp
          *
          * Perform any clean up if necessary
          *
          */
        void cleanUp();


        /** 
          * getLastUpdateTime
          *
          * The last time that the job list has been updated
          *
          *
          * @return The last time that the job list has been updated
          */
        time_t getLastUpdateTime() const;

        //////////////////////////////////////////////////////////////////////////////////
        //
        //
        // Methods inherited from IJobUpdatable
        //
        //
        //////////////////////////////////////////////////////////////////////////////////

        /** 
          * updateJobs
          *
          * Calls by the JobManager to update the job list whenever there is a job update.
          *
          */
	    virtual void updateJobs();


        /** 
          * createJob
          *
          * Displays the job dialog for creating a job.
          *
          * @param job The job object that contains the preliminary data to be displayed on the dialog.
          */
	    virtual void createJob(const Job& job);


        /** 
          * updateRights
          *
          * Update the operator privileges on the selected job.
          *
          */
	    virtual void updateRights();


        /** 
          * jobDeleted
          *
          * This is called when a job has been removed from the job list.  
          *
          * @param job The job that has been removed.  
          * @param isAssociatedWithSessionProfiles True if the job is associated with profiles available to this session
          */
        virtual void jobDeleted(const Job& job, const bool isAssociatedWithSessionProfiles);


        /** 
          * jobUpdated
          *
          * This is called when a job has been updated in the job list.  
          *
          * @param job The job that has updated
          * @param isAssociatedWithSessionProfiles True if the job is associated with profiles available to this session
          */
        virtual void jobUpdated(const Job& job, const bool isAssociatedWithSessionProfiles);
        
        

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CSchedulingManagerDoc)
	    public:
	    virtual void Serialize(CArchive& ar);
	    //}}AFX_VIRTUAL

    // Implementation
    public:
	    virtual ~CSchedulingManagerDoc();
    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

    protected:

    // Generated message map functions
    protected:
	    //{{AFX_MSG(CSchedulingManagerDoc)
	    afx_msg void onViewAlljobs();
	    afx_msg void onUpdateViewAlljobs(CCmdUI* pCmdUI);
	    afx_msg void OnAppAbout();
	    afx_msg void OnHelpSchedulingmanager();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        //
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        //
        CSchedulingManagerDoc( const CSchedulingManagerDoc& theSchedulingManagerDoc);
        CSchedulingManagerDoc& operator=(const CSchedulingManagerDoc &);

        // This is the object that does all the job processing
        JobManager* m_jobManager;

        // True if the view is shows jobs that are created by operator's profiles
        bool m_isProfileFilterOn;
    };

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULINGMANAGERDOC_H__D0E5B0FF_ECF2_4F0F_AC0E_F01265E93D30__INCLUDED_)

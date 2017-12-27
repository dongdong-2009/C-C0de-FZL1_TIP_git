/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/AdminManager.h $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * AdminManager is a singleton class that is the central class of the 
  * System Manager. It retreives data from a System Controller and
  * sends the GUI's orders to that System Controller.
  */

#if !defined(AFX_ADMINMANAGER_H__679184C0_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_)
#define AFX_ADMINMANAGER_H__679184C0_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>
#include <vector>

#include "AdminObserver.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/process_management/IDL/src/ISystemControllerAdminCorbaDef.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"

using TA_Base_Core::ProcessData;
using TA_Base_Core::ProcessDataSeq_var;

const int ERR_LOGIN_NO_PROCESS           = -1;
const int ERR_AUTH_NO_ERROR              = 0;
const int ERR_AUTH_SOCKET_ERROR          = 1;
const int ERR_AUTH_SERVICE_ERROR         = 2;
const int ERR_AUTH_ACCESS_DENIED         = 3;
const int ERR_GET_ADMIN_ERROR            = 4;
const int ERR_SESSION_RUNNING            = 5;
const int ERR_AUTH_NO_PERMISSION         = 6;
const int ERR_AUTH_INVALID_KEYS          = 7;
const int ERR_NO_SYSTEM_CONTROLLER_ERROR = 8;
const int ERR_RETRIEVING_PROCESS_DATA    = 9;

#define WM_SHUTDOWN_SYSTEM_CONTROLLER (WM_USER + 1)
#define WM_NEW_PROCESS_DATA			  (WM_USER + 2)
#define WM_TAB_SEL_CHANGED            (WM_USER + 3)
#define WM_PROCESS_LIST_SEL_CHANGED   (WM_USER + 4)
#define WM_TRACK_CONTEX_MENU          (WM_USER + 5)
#define WM_LOGIN_DONE                 (WM_USER + 6)

namespace TA_Base_Bus
{
    class RightsLibrary;
}

namespace TA_Base_App
{
    struct ProcessExactData
    {
        TA_Base_Core::ProcessData* pOrgData;
        std::string strProcessName;
        bool bCanBeManagered;

        ProcessExactData::ProcessExactData() :
        pOrgData( NULL ),
        strProcessName( "" ),
        bCanBeManagered( false )
        {
        };
    };

    class AdminManager : public TA_Base_Core::Thread
    {
    public:

        // Destructor
        virtual ~AdminManager();

        AdminManager( const std::string& strHostName, unsigned long ulEntityKey );

        /**
          * login
          *
          * Starts a System Manager session.
          *
          * @return error code:
          *         ERR_AUTH_SOCKET_ERROR           Cannot identify local console
          *         ERR_AUTH_SERVICE_ERROR          Authentication service error
          *         ERR_NO_SYSTEM_CONTROLLER_ERROR  No System Controller
          *         ERR_GET_ADMIN_ERROR             Cannot get System Controller
          *         ERR_AUTH_NO_ERROR               No error
          *
          * @param portNumber    Port number the System Controller is running on.
          * @param sessionId     The session ID for this login.
          * @param operatorName  The name of the operator.
          */
        static int loginSetup( const std::string& portNumber, const std::string& sessionId, const std::string& operatorName );
        /**
          * logout
          *
          * Terminates a session.
          */
        void logout();

        int login();

        /**
          * refreshData
          *
          * Ask the System Controller for new process data and update the dialog.
          *
          * @exception SystemControllerException  An error occurred communicating
          *                                       with the System Controller.
          */
        void refreshData();

        /**
          * registerObserver
          *
          * Registers a AdminObserver object as an observer.
          * Registered observers will be notified if a data chage occurs
          * by a call to their OnNewData function.
          *
          * @param A pointer to the observer to register.
          */
        static void registerObserver(const AdminObserver* observer);

        /**
          * unregisterObserver
          *
          * Unregisters an observer.
          *
          * @param A pointer to the observer to unregister
          */
        static void unregisterObserver(const AdminObserver* observer);

        /**
          * Retrieves the hostname of the SystemController the current session
          * is logged into
          *
          * @return The hostname as a string.
          */
        inline std::string getHostName()
        {
            return m_hostName;
        }

        /**
          * getRefreshIntervals
          *
          * Retrieves the available refresh intervals (in seconds).
          *
          * @return A vector of ints containing the refresh intervals.
          */
        static std::vector<int> getRefreshIntervals();

        /**
          * getCurrentRefreshRate
          *
          * Returns the current refresh rate.
          *
          * @return The refresh rate.
          */
        static unsigned int getCurrentRefreshRate();

        /**
          * setRefreshRate
          *
          * Sets the refresh rate
          *
          * @param newRate  The new rate
          */
        static void setRefreshRate(unsigned int newRate);

        /**
          * startProcess
          *
          * Ask the System Controller to start a process.
          *
          * @param processId  The entity name of the process.
          *
          * @exception SystemControllerException  An error occurred communicating
          *                                       with the System Controller.
          */
        void startProcess(const std::string& processId);

        /**
          * changeProcessOperationMode
          *
          * Ask the System Controller to change the operation mode of a process.
          *
          * @param processId  The entity name of the process.
          *
          * @exception SystemControllerException  An error occurred communicating
          *                                       with the System Controller.
          */
        void changeProcessOperationMode(const std::string& processId,
                                        TA_Base_Core::EOperationMode opMode);

        /**
          * stopProcess
          *
          * Ask the System Controller to stop a running process.
          *
          * @param  processId  The entity name of the process.
          *
          * @exception SystemControllerException  An error occurred communicating
          *                                       with the System Controller.
          */
        void stopProcess(const std::string& processId);

        /**
          * shutdownSystemController
          *
          * Ask the System Controller to shut down.
          *
          * @exception SystemControllerException  An error occurred communicating
          *                                       with the System Controller.
          */
        void shutdownSystemController();

        /**
          * setProcessRunParam
          *
          * Sets a runtime parameter for the specified process.
          *
          * @param processId  The entity name of the process.
          *        param      The runparam to set.
          *
          * @exception SystemControllerException  An error occurred communicating
          *                                       with the System Controller.
          */
        void setProcessRunParam(const std::string& processId, 
                                const TA_Base_Core::RunParam& param);

        /**
          * getProcessDebugLevel
          *
          * Returns the debug level for the specified process.
          *
          * @return The debug level as a string.
          *
          * @param processId  The entity name of the process.
          */
        std::string getProcessDebugLevel(const std::string& processId);

        /**
          * getProcesses
          *
          * Returns the process information last retrieved from the System 
          * Controller.
          *
          * @return a vector of ProcessData structs.
          */
        std::vector<ProcessExactData> getProcesses();

        /** 
          * getEntityDescription
          *
          * Returns the description for this entity.
          *
          * @param entityName  The name of the entity.
          *
          * @return The entity's description.
          */
        std::string getEntityDescription(const std::string& entityName) const;

        /**
          * isManagedProcess
          *
          * Returns whether this process is managed or not. This lets the GUI
          * decide whether the process modes can be changed
          *
          * @param entityName  The name of the entity.
          *
          * @return Whether the entity is for a managed process or not
          */
        bool isManagedProcessFromName(const std::string& entityName) const;
        bool isManagedProcessFromDescription(const std::string& entityDescription) const;

        /**
          * isServiceProcess
          *
          * Returns whether this process is a service or not. This lets the GUI
          * decide whether the process modes can be changed
          *
          * @param entityName  The name of the entity.
          *
          * @return Whether the entity is for a service process or not
          */
        bool isServiceProcessFromName(const std::string& entityName) const;
        bool isServiceProcessFromDescription(const std::string& entityDescription) const;
		bool isAutoFailBackFromDescription(const std::string& entityDescription) const;
        
        void retryLogin();

        /**
          * getSessionId
          *
          * Returns the session ID.
          *
          */
        inline static std::string getSessionId()
	    {
		    return m_sessionId;
	    }

        /**
          * getOperatorName
          *
          * Returns the currently logged in operator's name.
          */
        inline static std::string getOperatorName()
        {
            return m_operatorName;
        }

        /**
          * Checks if a valid session is currently running
          * ie (with a valid session ID).
          *
          * @return bool  True if a valid session is active, and
          *               false otherwise.
          */
        inline static bool validSession()
        {
            return (m_sessionId.empty());
        }

        inline static void setRightLibrary( TA_Base_Bus::RightsLibrary* pLibrary )
        {
            m_rightsLibrary = pLibrary;
        }

        inline static CWnd* getMainWnd()
        {
            return m_mainWnd;
        }

        inline bool isStartProcessAllowed()
        {
            return m_isStartProcessAllowed;
        }

        inline bool isStopProcessAllowed()
        {
            return m_isStopProcessAllowed;
        }

        inline bool isSetProcessParamAllowed()
        {
            return m_isSetProcessParamAllowed;
        }

        inline bool isShutdownAllowed()
        {
            return m_isShutdownAllowed;
        }

        inline bool isSetOperatingModeAllowed()
        {
            return m_isSetOperatingModeAllowed;
        }

        inline bool isSetOptionsAllowed()
        {
            return m_isSetOptionsAllowed;
        }

        inline int getLoginResult()
        {
            return m_nLoginResult;
        }
        
        inline unsigned long getHostEntityKey()
        {
            return m_hostEntityKey;
        }

        static void InitializeStaticMember( CWnd* pMainWnd );
        
		/**
		  * run
		  *
		  * Implementation of the Thread::run(). Periodically fetches
		  * process data from the System Controller.
		  */
		void run();

		/**
		  * terminate
		  *
		  * Terminates the thread.
		  */
		void terminate();

        /**
          * checkPermissions
          *
          * Sets the m_is...Allowed boolean member variables by checking the available
          * rights from the rights library.
          */
        void checkPermissions();

    private:

        //TD8617 Mintao++
        static const unsigned int DEFAULT_REFRESH_RATE; 

        // Made private as this is a singleton
        AdminManager();
	    AdminManager( const AdminManager& AdminManager);
	    AdminManager& operator=(const AdminManager&);

        struct ProcessInformation
        {
            std::string entityName;
            std::string entityDescription;
            bool isManagedProcess;
            bool isServiceProcess;
			bool isAutoFailBack;
        };

        /**
          * TimerProc
          *
          * Function notified when the time-out value elapses
          * See TimerProc in the MSDN library.
          * Calls the RefreshData function.
          */
        static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

        /**
          * buildData
          *
          * Builds the process vector from a sequence.
          *
          * @param Process data sequence
          */
        void buildData(ProcessDataSeq_var processes);

        /**
          * notifyObservers
          *
          * Notifies the registered observers that new data is available
          * by calling their OnNewData() function
          */
        void notifyObservers();

        /**
          * compareProcesses
          *
          * Compares two ProcessData structures.
          *
          * @return true if structures are identical structure
          *         false if different
          *
          * @param References to ProcessData structures.
          */
        bool compareProcesses(const ProcessData& process1, const ProcessData& process2) const;

		/**
		  * True if the thread is running, false otherwise.
		  */
		volatile bool m_isRunning;

		/**
		  * True if an error occurred the last time we attempted
		  * to retrieve process data.
		  */
		bool m_errorOccurred;

        static bool m_bStaticMemberInitialized;

        /**
          * The list of observers to notify.
          */
        static std::list<AdminObserver*> m_observers;

        /**
          * A vector of the possible refresh intervals, in seconds.
          */
        static std::vector<int> m_refreshIntervals;

        /**
          * The index of the currently selected refresh rate.
          */
        static unsigned int m_currentRefreshRate;

        /**
          * The process data.
          */
        std::vector<ProcessData> m_processes;

        /**
          * The System Controller CORBA interface.
          */
        TA_Base_Core::ISystemControllerAdminCorbaDef_var m_systemController;

        /**
          * The sesssion Id.
          */
        static std::string m_sessionId;

        /**
          * The operator name.
          */
        static std::string m_operatorName;

        static std::string m_strPortNumber;

        /**
          * The hostname of the System Controller we're logged into.
          */
        std::string m_hostName;

        typedef std::map<std::string, ProcessInformation> ProcessInformationMap;

        /**
          * A map of entity names to entity details.
          */
        ProcessInformationMap m_entityNameToDetails;

        /**
          * A map of entity descriptions to entity details.
          */
        ProcessInformationMap m_entityDescriptionToDetails;

        /**
          * Used to synchronise access to the process data.
          */
        TA_Base_Core::NonReEntrantThreadLockable m_processLock;

        /**
          * Used to synchronise access to the observers.
          */
        static TA_Base_Core::NonReEntrantThreadLockable m_observerLock;

		int m_nThreshhold;

        int m_nLoginResult;

        /**
          * Whether or not all of the access controlled functionality is
          * allowed for the currently logged in user.
          */
        bool m_isStartProcessAllowed;
        bool m_isStopProcessAllowed;
        bool m_isSetProcessParamAllowed;
        bool m_isShutdownAllowed;
        bool m_isSetOperatingModeAllowed;
        bool m_isSetOptionsAllowed;

        unsigned long m_resourceKey;
        unsigned long m_hostEntityKey;

        TA_Base_Core::Semaphore m_loginSemaphore;

        static TA_Base_Bus::RightsLibrary* m_rightsLibrary;

        static CWnd* m_mainWnd;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_ADMINMANAGER_H__679184C0_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/RunningApplication.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2013/06/03 17:28:56 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * This holds all information about a running application. The Control Station
  * will maintain a collection of these so it knows exactly what is running on
  * a workstation.
  */


#if !defined(RunningApplication_97C44E6B_4212_47c3_BD5E_552057896E53__INCLUDED_)
#define RunningApplication_97C44E6B_4212_47c3_BD5E_552057896E53__INCLUDED_

#include "app\system_control\control_station\src\ControlStationConstants.h"
#include "app\system_control\control_station\src\stdafx.h"
#include "app/system_control/control_station/src/ApplicationMover.h"

#include <string>

#include "core/process_management/IDL/src/IManagedProcessCorbaDef.h"
#include "core/process_management/IDL/src/IManagedGuiCorbaDef.h"

// 
// Forward declaration of classes
//
namespace TA_Base_Core
{
    class IGui;
}

class IProcessManager;


class RunningApplication
{
public:
    /**
      * Constructor
      * 
      * This constructs a RunningApplication object with enough information about the
      * application so it can be launched. This constructor assumes that the application
      * is not yet running and will need to be launched.
      *
      * @param IGui - A database object holding all the configuration information for
      *               this GUI application.
      * @param bool  - This indicates if the application is visible to the users as a
      *                running application that they can switch to. Obviously we don't
      *                want things like a banner to be available in a list of running
      *                applications. This defaults to true and only needs to be set if
      *                the application is 'special'.
      *
      * @exception DatabaseException A DatabaseException will be thrown if there is a
      *            problem establishing a connection with the database, or if an
      *            error is encountered while retrieving data.
      * @exception DataException A DataException will be thrown if the data cannot be 
      *            converted to the required format (e.g. ACCESSLEVEL should be an 
      *            unsigned long), or if the wrong ammount of data is retrieved.
      * @exception ApplicationException This indicates the application could not be launched for
      *            some reason.
      */
    RunningApplication(TA_Base_Core::IGui& entity,
                       bool isVisibleToUserAsRunning =true);


    /**
      * Constructor
      * 
      * This constructs a RunningApplication object with enough information about the
      * application so it can be launched. This constructor assumes that the application
      * is already running and is a managed process.
      *
      * @param IGui& - A database object holding all the configuration information for
      *                this GUI application.
      * @param unsigned long - This is the processId of the application.
      * @param IManagedProcessCorbaDef_var - If the application is already running then it must
      *                                      have registered as a managed process.
      * @param bool  - This indicates if the application is visible to the users as a
      *                running application that they can switch to. Obviously we don't
      *                want things like a banner to be available in a list of running
      *                applications. This defaults to true and only needs to be set if
      *                the application is 'special'.
      *
      * @exception DatabaseException A DatabaseException will be thrown if there is a
      *            problem establishing a connection with the database, or if an
      *            error is encountered while retrieving data.
      * @exception DataException A DataException will be thrown if the data cannot be 
      *            converted to the required format (e.g. ACCESSLEVEL should be an 
      *            unsigned long), or if the wrong ammount of data is retrieved.
      * @exception ApplicationException This indicates the application could not be launched for
      *            some reason.
      */
    RunningApplication(TA_Base_Core::IGui& entity,
                       unsigned long processId,
                       TA_Base_Core::IManagedProcessCorbaDef_var managedProcess,
                       bool isVisibleToUserAsRunning =true);


    /**
      * Destructor
      */
	virtual ~RunningApplication();


    /** 
      * launch
      *
      * This launches the application. It will run the application and store the pid. This won't
      * know whether the application is a managed process or managed GUI until the application 
      * lets the ProcessManager know.
      * PreCondition: The application is not yet running.
      *
      * @param string additionalCommands - This is a string containing any additional commands to 
      *                                    be passed to the application. This should take the form
      *                                    of a command line you would pass to the application if
      *                                    you ran it from the command line. The default is no
      *                                    parameters.
      * @param posFlag - How the application is going to be positioned relative to boundary paramter
      * @param alignFlag - How the application is going to be positioned relative to point parameter
      * @param objectDim - The dimension of the object that this application will be launched relative
      *                    to (if POS_RELATIVE or POS_NOOVERLAP is specified).
      * @param boundaryDim - The dimension of the object that this application will be bounded in
      *                      (if POS_BOUNDED is specified)
      *
      * @exception DatabaseException A DatabaseException will be thrown if there is a
      *            problem establishing a connection with the database, or if an
      *            error is encountered while retrieving data.
      * @exception DataException A DataException will be thrown if the data cannot be 
      *            converted to the required format (e.g. ACCESSLEVEL should be an 
      *            unsigned long), or if the wrong ammount of data is retrieved.
      * @exception ApplicationException This indicates the application could not be launched for
      *            some reason.
      */
    void launch(const std::string& extraCommandLine, 
               const unsigned long posFlag,
               const unsigned long alignFlag,
               const RECT& objectDim,
               const RECT& boundaryDim);

    void relaunch( IProcessManager& processManager );
    std::string m_additionalCommandLine;
    unsigned long m_posFlag;
    unsigned long m_alignFlag;
    RECT m_objectDim;
    RECT m_boundaryDim;
    bool isRelaunchAvailable;


    /**
      * terminate
      * 
      * This terminates the application.  If it is a managed process it will tell it to terminate
      * nicely. If it is not a managed process then it will attempt to 'kill' the application.
      *
      */
	void terminate();


    /**
      * kill
      * 
      * This kills the application. This should only be used after a terminate has been tried.
      *
      */
	void kill();


    /**
      * shouldTerminateOnLogout
      * 
      * This returns true if the application is to terminate on logout
      *
      * @return bool - true if the application is to terminate on logout
      *
      */
    bool shouldTerminateOnLogout();


    /**
      * changeApplicationFocus
      * 
      * This will force an application to change it's focus.
      *
      * @param EFocusType - This indicates if the application should gain the focus, maximise
      *                     or minimise.
      *
      * @exception ApplicationException - Thrown if the application could not be given focus.
      *            This probably means the application is no longer running.
      */
    void changeApplicationFocus(const TA_Base_Core::EFocusType focus);


    /**
      * changeServerState
      * 
      * This will alert an application that the server state has changed. This will only apply
      * to managed gui's.
      *
      * @param bool - true if the server is down and false if the server is functioning again.
      *
      */
	void changeServerState(const bool isServerDown);


    /**
      * passNewParameteres
      * 
      * This will pass the RunParams to the application as well as the parameters specified on
      * the passed in command line.
      *
      * @param RunParamSeq_var - The list of parameters to be passed to the application.
      *
      */
    void passNewParameters(TA_Base_Core::RunParamSeq_var additionalParameters);


    /**
      * passNewParameteres
      * 
      * This will pass the RunParams to the application as well as the parameters specified on
      * the passed in command line.
      *
      * @param string - The command line of parameters to be passed to the application.
      *
      */
    void passNewParameters(const std::string& additionalCommands);


    /**
      * getApplicationName
      * 
      * This retrieves the display name of the application from the IGui object.
      *
      * @return string representing the name of the appliation
      *
      */
    std::string getApplicationName();


    /**
      * getApplicationFullName
      * 
      * This retrieves the full display name of the application from the IGui object.
      *
      * @return string representing the name of the appliation
      *
      */
    std::string getApplicationFullName();


    /**
      * getHelpFile
      * 
      * This retrieves the name of application's help file from the IGui object.
      *
      * @return string representing the name of the help file
      *
      */
    std::string getHelpFile();


    /**
      * isVisibleToUserAsRunning
      * 
      * This determines if the running application should be displayed to the user in a list
      * of running applications.
      *
      * @return bool true if the application is visible
      *
      */
    bool isVisibleToUserAsRunning() const
    {
        return m_isVisibleToUserAsRunning;
    };


    /**
      * isRunning
      * 
      * This determines if the application is running or if it is in an intermediate state.
      *
      * @return bool true if the application is running
      *
      */
    bool isRunning() const
    {
        return m_isRunning;
    }


    /**
      * getProcessId
      * 
      * This returns the process Id of this application
      *
      * @return unsigned long The process id of this application.
      *
      */
    unsigned long getProcessId() const
    {
        return m_processInformation.dwProcessId;
    };


    /** 
      * getProcessHandle
      *
      * This returns the process handle of this application
      *
      * @return the process handle of this application
      */
    HANDLE getProcessHandle() const
    {
        return m_processInformation.hProcess;
    }


    /**
      * getGUIEntity
      * 
      * This returns the IGui for this application
      *
      * @return IGui* The database details for the GUI application
      *
      */
    TA_Base_Core::IGui& getGUIEntity() const
    {
        return m_guiEntity;
    };


    /**
      * getEntityName
      * 
      * This returns the entity name for the running application.
      *
      * @return string - The name of the entity. It will be an empty string if no entity
      *                  name has been specified.
      *
      */
    std::string getEntityName() const
    {
        return m_entityName;
    }


    /**
     * getPositionInformation
     *
     * This will return the position of the top left corner of the GUI application when it was
     * launched. This can be used to determine which instance of an application is running.
     *
     * @param long leftPosition - This is an out parameter and will be filled with the left position
     *                            of the GUI.
     * @param long topPosition - This is an out parameter and will be filled with the top position
     *                            of the GUI.
     */
    void getPositionInformation(long& leftPosition, long& topPosition);


    /**
     * getParameters
     *
     * If this is a managed process this will return the current list of applications that the
     * application is using.
     *
     * @return RunParamSeq_ptr - The list of parameters the application is currently using.
     */
    TA_Base_Core::RunParamSeq* getParameters();


    /**
      * isApplicationTypeEqualTo
      * 
      * This compares the passed in application type to the stored application type to see
      * if it is a match.
      *
      * @return bool true if the application's application type matches
      *
      */
    bool isApplicationTypeEqualTo(const unsigned long appType);


    /**
      * setManagedProcess
      * 
      * This is called if the application registers as a Managed Process. This will set
      * the member variable m_managedProcess.
      *
      * @param IManagedProcessCorbaDef_var - This is the actual CORBA object that the managed process
      *                                      implements so we can perform calls on it.
      *
      */
    void setManagedProcess(const TA_Base_Core::IManagedProcessCorbaDef_var managedProcess);


    /**
      * setManagedGui
      * 
      * This is called if the application registers as a Managed Gui. This will set
      * the member variable m_managedGUI.
      *
      * @param IManagedGuiCorbaDef_var - This is the actual CORBA object that the managed Gui
      *                                  implements so we can perform calls on it.
      *
      */
    void setManagedGui(const TA_Base_Core::IManagedGUICorbaDef_var managedGUI);


    /**
      * setEntityName
      * 
      * This is called if the application registers as a Managed Application and passes in an 
      * entity name. It will be used to determine a log file for the application.
      *
      * @param string - The entity name to store
      *
      */
     void setEntityName(const std::string& entity)
     {
        m_entityName = entity;
     }


    /**
     * updateSessionId
     *
     * Update the session Id of the application
     */
     void updateSessionId(std::string& sessionId);


    /**
     * dutyChanged
     *
     * Notify the application that the duties for the current session have changed.
     */
     void dutyChanged();


    /** 
      * move
      *
      * This moves and resizes the application to the specified parameters.  Does not do anything
      * if the application is not already started.
      *
      * @param posFlag - How the application is going to be positioned relative to boundary paramter
      * @param alignFlag - How the application is going to be positioned relative to point parameter
      * @param objectDim - The dimension of the object that this application will be launched relative
      *                    to (if POS_RELATIVE or POS_NOOVERLAP is specified).
      * @param boundaryDim - The dimension of the object that this application will be bounded in
      *                      (if POS_BOUNDED is specified)
      */
    void move( const unsigned long posFlag,
               const unsigned long alignFlag,
               const RECT& objectDim,
               const RECT& boundaryDim);


    /** 
      * areInstancesLimited
      *
      * Returns false if launching multiple instances of this application is permissible
      *
      *
      * @return false if launching multiple instances of this application is permissible
      */
    bool areInstancesLimited();


private:
    // These are private as they are not used.
	RunningApplication( const RunningApplication& theRunningApplication);
	RunningApplication& operator=(const RunningApplication& ra);

    
    /**
     * parseProcessAddress()
     *
     * This method is used to return the startPath and command line arguments for
     * a process.
     * 
     * @param std::string& startPath - the absolute location of the process. This gets populated with
     *                                 the path and executable name.
     * @param std::string& argList - this string will get populated with the string of command line
     *                               arguments..
     * @param const std::string& processAddress - the string containing the executable path, name
     *                                            and command line arguments.
     *                                            eg c:\transactive\bin\AlarmViewer.exe --entity-name=AlarmViewer
     */
    void parseProcessAddress(std::string& startPath, std::string& argList, const std::string& processAddress);


    /**
      * focusNonManagedGui
      *
      * This handles callbacks from ::EnumWindows()
      * This is a handler for top level window enumeration, which lets us
      * determine which window should get the focus
      *
      * @return TRUE to continue enumerating windows, FALSE to stop.
      *
      */
    static BOOL CALLBACK focusNonManagedGui(HWND hWnd, LPARAM lParam);

    
    // This is the Windows process information of the running application. This will always be
    // populated after launch() is called successfully.
    PROCESS_INFORMATION m_processInformation;

    // This holds all the details from the database about the GUI application.
    TA_Base_Core::IGui& m_guiEntity;

    // If the application is a managed process then this will hold a valid value and
    // it can be told to terminate nicely.
	TA_Base_Core::IManagedProcessCorbaDef_var m_managedProcess;

    // If this application is also a managed GUI then this will hold a valid value
	TA_Base_Core::IManagedGUICorbaDef_var m_managedGUI;

    // This is used to determine if this application should be displayed to the user in a list
    // of running applications.
	bool m_isVisibleToUserAsRunning;

    // This indicates if the application is running in case the object is in an intermediate
    // state. ie Constructed but not launched. Terminated but not deleted
    bool m_isRunning;

    // The entity name this application is running with.
    std::string m_entityName;

    // This indicates if the window should be positioned when it registers as a ManagedGUI.
    // If it is then the coordinates of the left, top corner are stored.
    bool m_isPositioned;
    long m_leftPosition, m_topPosition, m_width, m_height;

    ApplicationMover m_applicationMover;  // This is used to position the application.

    struct WindowFocusInfo
    {
        unsigned long processId;    // The processId of the application
        TA_Base_Core::EFocusType focus;  // The type of focus 
    };

    WindowFocusInfo m_focusInfo;
};

#endif // !defined(RunningApplication_97C44E6B_4212_47c3_BD5E_552057896E53__INCLUDED_)

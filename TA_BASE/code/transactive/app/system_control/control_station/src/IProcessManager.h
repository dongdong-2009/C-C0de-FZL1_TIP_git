/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/IProcessManager.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *  
  * This interface advertises the calls the AutoProxy will need to callback.
  */

#if !defined(IProcessManager_36889BB4_BAF3_4bc1_B189_C5757C976786__INCLUDED_)
#define IProcessManager_36889BB4_BAF3_4bc1_B189_C5757C976786__INCLUDED_

#include "app\system_control\control_station\src\ControlStationConstants.h"
#include <map>
#include <vector>
#include <string>
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

// Forward declarations
class RunningApplication;
class Application;

class IProcessManager
{

public:
    struct ApplicationProperties
    {
        unsigned long key;
        std::string name;
        bool isAccessible;
    };

    typedef std::map<unsigned long,Application*> ApplicationMap;
    typedef std::map<TA_Base_Core::ProcessId,RunningApplication*> RunningApplicationMap;


    /**
     * Destructor
     */
    virtual ~IProcessManager() { };


    /** 
      * launchApplication
      *
      * This will launch an application in a specified position. Any additional arguments
      * to provide to the application can be specified.
      *
      * @param appType - The application to launch
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
      * @param bool isVisible - This indicates if the running application should be displayed
      *                         to the user as running. For example a banner should not be
      *                         displayed as a running application that the user can switch to.
      *                         Therefore for this 'false' should be passed in. The default is 
      *                         'true' as most applications will want to be seen.
      *
      * @exception ApplicationException - This indicates that the application could not be launched.
      */
    virtual void launchApplication(const unsigned long appType, 
                                   const std::string& additionalCommands, 
                                   const unsigned long posFlag,
                                   const unsigned long alignFlag,
                                   const RECT& objectDim,
                                   const RECT& boundaryDim,
                                   const bool isVisible = true) =0;
	

    /**
      * giveApplicationFocus
      * 
      * This will tell the specified application to change it's focus so it is the current
      * active window.
      *
      * @param unsigned long pid - This is the process id of the application to gain the focus.
      *
      */
    virtual void giveApplicationFocus(unsigned long pid) =0;


    /**
     * getRunningApplications
     *
     * This will retrieve a list of all running applications and return it. It will return
     * a display name and the process id so that the caller can display the list to the user
     * and then has the processId to identify the application.
     *
     * @param bool - Indicates if we are retrieving only the visible applications (true) or all of
     *               the applications( false). The default is to retrieve only the visible applications.
     *
     * @return map<unsigned long, string> - This contains the name of each running application
     *                                      keyed by process id.
     *
	 */
    virtual std::map<unsigned long,std::string> getRunningApplications(bool areGettingJustVisible =true) =0;


    /**
     * getAllApplications
     *
     * This will retrieve a list of all applications available to the user to be launched
     * and returns it. It will return a display name and the application type so that the
     * caller can display the list to the user and then has the application type to identify
     * the application.
     *
     * @return std::vector<ApplicationProperties> 
     *         - This contains the key, name and a boolean indicating whether the current
     *           operator has the rights to run the application.
	 */
    virtual std::vector<ApplicationProperties> getApplications() = 0;


    /**
     * getRunningApplicationObjects
     *
     * This will return a reference to the list of running applications.
     *
     * @return RunningApplicationMap& - This returns a reference to the map of running applications
     */
    virtual RunningApplicationMap& getRunningApplicationObjects() =0;


    /**
     * getApplicationObjects
     *
     * This will return a reference to the list of running applications.
     *
     * @return ApplicationMap& - This returns a reference to the map of all applications
     */
    virtual ApplicationMap& getApplicationObjects() =0;
};

#endif // !defined(IProcessManager_36889BB4_BAF3_4bc1_B189_C5757C976786__INCLUDED_)

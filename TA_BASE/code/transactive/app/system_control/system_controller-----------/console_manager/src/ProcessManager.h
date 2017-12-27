/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_controller/console_manager/src/ProcessManager.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// ProcessManager.h
//
// Demonstration implementation of the ProcessManagerDmIf interface
//
// Author:      Bruce Fountain 08-Feb-2001
// Modified by: $Author: haijun.li $
// Modified on: $DateTime: 2012/02/06 16:15:14 $
// Version:     $Revision: #2 $
//

#ifndef PROCESSMANAGER_H_INCLUDED
#define PROCESSMANAGER_H_INCLUDED

#include "IProcessManagerCorbaDef.h"
#include "IManagedApplicationCorbaDef.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <vector>
#include <string>
#include <map>


class ProcessManager :
               public virtual POA_TA_Base_Core::IProcessManagerCorbaDef,
               public virtual TA_Base_Core::ServantBase,
               public virtual TA_Base_Core::RunParamUser
{
public:
	ProcessManager();
    virtual ~ProcessManager() {};

    //
    // Gets a list of current running applications
    //
    std::string getRunningApps();
    
    //
    // Terminates the specified application
    //
    void terminateApp(unsigned long id);

    //
    // Changes an agent mode from control to monitor and vice versa
    //
    void changeMode(bool isControl, unsigned long id);

    //
    // Changes the display of a GUI
    //
    void changeGUI(TA_Base_Core::EFocusType focus, unsigned long id);

    //
    // Alerts the GUI that a server has gone up or down
    //
    void serverChange(bool isUp, unsigned long id);

	//
	// Sets the runparam for the managed process with the specified id
	//
	bool setClientRunParam(const int id, const std::string& paramName, const std::string& paramValue);

    //
    // Methods to meet the ProcessManagerCorbaDef interface requirements.
    //

    void terminating( TA_Base_Core::ProcessId p_processID, 
                      TA_Base_Core::EManagedProcessTerminateCode p_code);

    void registerManagedProcess( TA_Base_Core::IManagedProcessCorbaDef_ptr p_managedProcess,
                                 TA_Base_Core::ProcessId p_processID, 
                                 CORBA::ULong applicationType,
                                 const char* entity);

    void registerManagedApplication(TA_Base_Core::IManagedApplicationCorbaDef_ptr p_managedApp,
                                    TA_Base_Core::ProcessId p_processId);

    TA_Base_Core::RunParamSeq* getParams(TA_Base_Core::ProcessId id, const char* hostname);


    void poll();


    void statusChanged(TA_Base_Core::EProcessStatus status,TA_Base_Core::ProcessId p_processId);

    //
    // RunParamUser implementation
    //
    void onRunParamChange(const std::string& paramName, const std::string& paramValue);


private:

	// Structures to store the process/client information
	struct Parameter
	{
        std::string name;       // Parameter Name
        std::string value;      // Parameter Value
	};

    TA_Base_Core::RunParams::ParamVector m_defaultParams;

	struct Client
	{
        TA_Base_Core::IManagedProcessCorbaDef_var managedProcess;
        std::string entity;
        unsigned long applicationType;
        std::string type;
	};

    typedef std::map<TA_Base_Core::ProcessId, Client> ClientMap;
    typedef std::map<TA_Base_Core::ProcessId, TA_Base_Core::IManagedApplicationCorbaDef_var> AppMap;
    ClientMap       m_clients;
    AppMap          m_apps;

    TA_Base_Core::ReEntrantThreadLockable m_mapLock;

    // For debug...
    static const char* StatusCodeToStr(TA_Base_Core::EManagedProcessTerminateCode p_code);
};

#endif // PROCESSMANAGER_H_INCLUDED

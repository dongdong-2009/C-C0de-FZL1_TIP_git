/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/main.cpp $
  * @author:  Kent Yip
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Main method for the Module Test Controller.  This will control the
  * distributed messaging module test through the Software Testing Automated
  * Framework.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "../src/Common.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TA_String.h"

#include "cots/staf/include/STAF.h"
#include "core/database/src/SimpleDB.h"
#include "core/message/src/TA_CosUtility.h"

#include <set>

// STARTING THE TEST ENVIRONMENT
// ------------------------------

// DATABASE CONFIGURATION:
// Note: Only fields that are required by the test framework are indicated.  It may be necessary
// to populate other fields if they are required by constraints.

// 1. ENTITY table must have one record for each parent MonitoredProcess of every notification 
// service.  A given MonitoredProcess entity (and the NotificationServiceAgent) can run redundantly
// on multiple hosts (see SN_MANAGED_PROCESS below).  For example:
// PKEY = 1001
// NAME = NS_MonitoredProcess
// TYPEKEY = 2                (MUST correspond to MonitoredProcess)

// 2. ENTITY table must have at least one NotificationServiceAgent entity for every location
// that is referenced by NST_SIMULATORCONFIG. For example:
// PKEY = 101
// NAME = NS_9991     (MUST be of the form NS_<NOTIFICATIONPORT>)
// ADDRESS = 9991             (MUST be NOTIFICATIONPORT)
// LOCATIONKEY = 1            (MUST be LOCATIONKEY)
// TYPEKEY = 37               (MUST correspond to NotificationServiceAgent)
// PARENTKEY = 1001           (PKEY of the parent entity, such as NS_MonitoredProcess)

// NOTE that the notification service entity NAME must be of the form "NShost_port"
// so that this test framework can automatically generate the name given only 
// the host and port.

// 3. ENTITY table must have one record for every event channel that the notification service
// hosts.  The test framework currently only uses "GeneralComms" channels. For example:
// PKEY = 102
// NAME = NScallisto_9991_GeneralComms
// ADDRESS = GeneralComms
// TYPEKEY = 38               (MUST correspond to NotifyEventChannel)
// PARENTKEY = 101            (PKEY of the notification service entity, such as NScallisto_9991)

// 4. ENTITYPARAMETERVALUE table must have a record for each notification service
// for the parameter type Broadcast, with the value set to false (otherwise 
// NotificationAgent::findRemoteNSAs() fails).  For example:
// PKEY = 100000
// ENTITYKEY = 101            (PKEY of the notification service entity, such as NScallisto_9991)
// PARAMETERKEY = 85          (MUST correspond to Broadcast)
// VALUE = 0

// 5. SN_SYSTEM_CONTROLLER needs one record representing each host.  For example:
// SNSCON_ID = 50
// NAME = callisto            (hostname)

// 6. SN_MANAGED_PROCESS needs a record for each parent MonitoredProcess entity for each
// host that it (and the NotificationServiceAgent) runs on.
// For example:
// MANAGED_PROCESS_PKEY = 1001 (PKEY of the parent entity, such as NS_MonitoredProcess)
// SNSCON_ID = 50              (SNSCON_ID of the host in SN_SYSTEM_CONTROLLER table)

// 7. NST_SIMULATORCONFIG needs one record for every simulator (consumer or supplier).


// TEST ENVIRONMENT:
// 1. Install STAF on each host
// 2. Install NST_Controller.exe, NST_Simulator.exe (and any other simulator executables
// referenced in NST_SIMULATORCONFIG.EXEFILENAME), NotificationServiceAgent.exe, 
// ConsoleManager.exe in a single directory.
// 3. Copy the omni notification configuration file (omni_notification.cfg) to the directory
// that STAF was started from (processes started by STAF seem to use STAF's current 
// directory), or use --notify-config-file to specify an alternate location.


// RUNNING THE TEST:
// 1. Start STAF on each host  "C:\Program Files\STAF\bin\STAFProc.exe"

// 2. Set STAF trust level of requestor to 5 (minimum required for starting a process).
// staf LOCAL TRUST SET LEVEL 5 DEFAULT
// or   staf LOCAL TRUST SET MACHINE callisto LEVEL 5
// TODO LPP: Need to investigate STAF's concept of trust level further.  Does the
// trust level need to be set locally on each machine, or can we set it remotely?

// 3. Run NST_Controller.exe
// The command line arguments passed to main() will be passed through to each 
// supplier and consumer that is started (except for the first argument, the 
// name of the executable).
// The following arguments are mandatory, but are only used by the controller:
// --simulator-exe-path=xxxx        // identifies the path to the various simulator executables. 
//                                  // No trailing backslash.  
//                                  // The SAME path is used on ALL hosts.

// The following mandatory arguments are used by both the controller and the simulators:
// --Db-Connection-File=xxxx        // identifies the path to the ConnectionStrings file

// The following optional arguments are also useful:
// --debug-level=xxxx
// --debug-file=xxxx                // a suffix (eg. the simulator id) and ".log" will be appended to the filename
// --debug-file-max-size=xxxx       // size of each debug log file in bytes
// --notify-config-file=xxxx        // the path and filename of the omni_notification.cfg file. 

// The following optional arguments enable omniOrb debug logging.  See CorbaUtil::initialise() for more information.
// --WithOmniLog                    // writes omniOrb logging to a file named with the prefix <entityName>_OMNILog_
// --OmniToDbgLog                   // if specified in addition to --WithOmniLog, writes omniOrb logging to the debug log
// --OmniLogLevel=xx                

// A typical command line is:
// --DebugFileMaxSize=5000000 --WithOmniLog --OmniToDbgLog --OmniLogLevel=10 --NotifyConfigFile=C:\Sandbox\9999_Code_PRIV6R4\bin\win32_nd\omni_notification.cfg --simulator-exe-path=C:\Sandbox\9999_Code_PRIV6R4\bin\win32_nd --db-connection-file=C:\Sandbox\9999_code_PRIV6R4\bin\win32_nd\DBConnectionStrings --debug-level=PERIODIC --debug-file=C:\transActive\log\NST_ 

// 4. After the simulators have terminated, the ConsoleManager and NotificationServiceAgents 
// that were started by the controller need to be MANUALLY terminated before another test
// is run.


// TODO LPP NOW:  Manual termination isn't really practical for tests on multiple hosts.

// TODO LPP NOW:  We only support one --notify-host for each simulator.  We need to be able to
// test redundant notification services properly, so we should get the --notify-hosts from
// SN_MANAGED_PROCESS etc rather than NST_SIMULATORCONFIG.

// TODO LPP NOW:  --simulator-exe-path probably needs to be different for UNIX and Win32 hosts.


// TODO LPP: It might be a good idea to allow a process to be started and stopped 
// during the test (to simulate failure of processes).  Although a random element 
// would be useful for verification, we really need deterministic tests that can 
// be reproduced.  How can we do this?  Note that this is point 6 below.


struct RemoteProcess
{
    STAFString handle;
    std::string host;
    int resultCode;
};

typedef std::map<unsigned long, RemoteProcess> ProcessHandleMap;


void killAll(STAFHandlePtr handle, ProcessHandleMap& processHandles )
{
    for( ProcessHandleMap::iterator it = processHandles.begin(); it != processHandles.end(); ++it )
    {
        STAFResultPtr result = handle->submit( 
            it->second.host.c_str(), "PROCESS", STAFString("STOP HANDLE ") + it->second.handle );
     
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "STOP HANDLE PROCESS RC: %d, Result: %d", result->rc, result->result );
    }
}


std::string getRunParamsAsCmdLine(const std::string& suffix)
{
    // TODO LPP: This is a bit of a hack to get at the command line.  There must
    // be a better way.

    std::string cmdLine;

    TA_Base_Core::RunParams::ParamVector params;
    TA_Base_Core::RunParams::getInstance().getAll(params);
    for (unsigned int i = 0; i < params.size(); ++i)
    {
        std::string value = params[i].value;

        // We need to modify the --debug-file run param so it suits the particular process,
        // so we append the suffix to it.
        // TODO LPP: We should append the suffix after the filename but before the prefix.
        // Then we wouldn't need to add '.log'
        if (params[i].name == "DebugFile")
        {
            value = params[i].value + suffix + ".log";
        }

        // Note that the command line won't be exactly the same as the original.
        // For example, --test-param will become --TestParam.  But the RunParam
        // library will not notice the difference.
        cmdLine += "--" + params[i].name + "=" + value + " ";    
    }

    return cmdLine;
}

                             
RemoteProcess spawnSimulator(
    STAFHandlePtr handle, 
    const std::string& executableName,
    const std::string& simulatorID, 
    const std::string& host)
{
    // SimulatorExePath identifies the path to the simulator executables (they are
    // all expected to be in the same directory).
    std::string exePath;
    exePath = TA_Base_Core::RunParams::getInstance().get("SimulatorExePath");
    TA_Assert(!exePath.empty());

    std::string startCmd(
        "START COMMAND " + 
        exePath + "/" + executableName + std::string(" ") + 
		simulatorID + std::string(" ") +
        getRunParamsAsCmdLine(simulatorID) +
        "SAMECONSOLE" );

    STAFResultPtr result = handle->submit( host.c_str(), "PROCESS", startCmd.c_str() );
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "%s: %s\n"
        "START COMMAND PROCESS RC: %d, Result: %d", 
        host.c_str(), startCmd.c_str(), result->rc, result->result );

    RemoteProcess process;
    process.handle = result->result;
    process.host = host.c_str();
    process.resultCode = result->rc;

    return process;
}


bool startAllSimulators( STAFHandlePtr handle, ProcessHandleMap& processHandles )
{
    // Relevant columns from NST_SIMULATORCONFIG:
    // simulatorID      - a unique index for the simulator.  NOTE that the simulators are ordered 
    //                    by simulatorID.  Consumers need to be started before suppliers, so these 
    //                    should have lower simulatorIDs.
    // enabled          - the simulator will be started if enabled == 1, otherwise it will be ignored.
    // simulatorHost    - the host that the simulator is launched on
    // exeFileName      - the executable file that is launched
    // postSpawnDelay   - the time period (ms) that must elapse before the next simulator is started.
    //                    Allows consumers time to start before suppliers are launched.

    std::string host, user, pass;
    getDbConnectParams(host, user, pass);

    SimpleDb db( host.c_str(), user.c_str(), pass.c_str() );
    db.Open();
    SimpleDb::Buffer data;
    std::string sql("select simulatorID, simulatorHost, exeFileName, postSpawnDelay from NST_SIMULATORCONFIG where simulatorID >= 0 AND enabled = 1 order by simulatorID");
    db.Exec( sql.c_str(), 4, data );
    db.Close();

    SimpleDb::Buffer::iterator it;
    for (it = data.begin(); it != data.end(); ++it)
    {
        std::string simulatorID = it[0][0];
        std::string simulatorHost = it[0][1];
        std::string exeFileName = it[0][2];
        std::string postSpawnDelay = it[0][3]; 

		RemoteProcess rp = spawnSimulator(handle, exeFileName, simulatorID, simulatorHost );
        processHandles[ atol(simulatorID.c_str()) ] = rp;
        if (rp.resultCode != 0) 
        {
            return false;
        }

        TA_Base_Core::Thread::sleep( atol(postSpawnDelay.c_str()) );
    }

    return true;
}

bool spawnNotificationService(
    STAFHandlePtr& handle, 
    const std::string& host, 
    unsigned short port, 
    const std::string& mgrPort, 
    unsigned int locationKey)
{
    TA_Base_Core::RunParams::getInstance().set("MgrPort", mgrPort.c_str());

	std::string notificationServiceExe("NotificationServiceAgent.exe");
    std::string exeFolder = TA_Base_Core::RunParams::getInstance().get("SimulatorExePath");
    TA_Assert(!exeFolder.empty());

    // The name of the entity representing the notification service in the ENTITY table 
    // must be of the form "NS_port" (note that we cant use : as the separator, as 
    // this causes truncation of the WithOmniLog filename).

    std::string portStr = TA_Base_Core::gStringFromUnsignedLong(port);

	std::string startCmd = "START COMMAND " + //cmd PARAMS /c " + 
        exeFolder + "/" + notificationServiceExe + std::string(" ") + 
        getRunParamsAsCmdLine("NS" + portStr) +
		std::string(" --entity-name=NS") + std::string("_") + portStr +
        std::string(" --notify-port=") + portStr +
        std::string(" --location-key=") + TA_Base_Core::gStringFromUnsignedLong(locationKey) +
        std::string(" --notify-hosts=") + getNotifyHostsForLocation(locationKey) +
		std::string(" --operation-mode=Control TITLE Notification Service ");

    STAFResultPtr result = handle->submit( host.c_str(), "PROCESS", startCmd.c_str() );

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "%s: %s\n"
        "START COMMAND PROCESS RC: %d, Result: %d", 
        host.c_str(), startCmd.c_str(), result->rc, result->result );

	if (result->rc != 0)
	{
		return false;
	}

	return true;
}

bool spawnConsoleMgr(STAFHandlePtr& handle, const std::string& host, const std::string& mgrPort)
{
    TA_Base_Core::RunParams::getInstance().set("MgrPort", mgrPort.c_str());

    // We don't explicitly set NotifyHosts or LocationKey parameters here, as the simulator
    // sets appropriate values.  These would be used by applications that connect to the 
    // ConsoleManager if not specified explicitly.
	std::string consoleMgrExe("ConsoleManager.exe");
    std::string exeFolder = TA_Base_Core::RunParams::getInstance().get("SimulatorExePath");

    // TODO LPP: The ConsoleManager doesn't seem to initialise DebugUtil by calling 
    // gSetDebugUtilFromRunParams(), so the --debug-file RunParam is ignored.
    std::string startCmd(
        "START COMMAND cmd PARAMS /c" + 
        exeFolder + "/" + consoleMgrExe + std::string(" ") + 
        getRunParamsAsCmdLine(host) +
		" TITLE ConsoleManager" );

    STAFResultPtr result = handle->submit( host.c_str(), "PROCESS", startCmd.c_str() );

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "%s: %s\n"
        "START COMMAND PROCESS RC: %d, Result: %d", 
        host.c_str(), startCmd.c_str(), result->rc, result->result );

	if (result->rc != 0)
	{
		return false;
	}

    return true;
}

bool startAllServices(STAFHandlePtr& handle)
{
    std::string host, user, pass;
    getDbConnectParams(host, user, pass);

    SimpleDb db( host.c_str(), user.c_str(), pass.c_str() );
    db.Open();
    SimpleDb::Buffer data;

    // The set of services that need to be started depends on the locations of the 
    // enabled simulators (ie. simulatorlocationkey).
    // We need to start one console manager on each distinct host, and we need to
    // start a notification service for every distinct serviceAddr.

    // Obtain the set of locationKeys that simulators are running at.
    std::vector<unsigned int> locationKeys;
    {
        std::string sql("select unique simulatorlocationkey from NST_SIMULATORCONFIG where enabled = 1");
        db.Exec( sql.c_str(), 1, data );

        SimpleDb::Buffer::iterator it;
        for (it = data.begin(); it != data.end(); ++it)
        {
            unsigned int locationKey = atoi(it[0][0].c_str());
            TA_Assert(locationKey != 0);
            locationKeys.push_back(locationKey);
        }
    }

    // distinct hosts that console managers need to be started on
    std::set<std::string> hosts;

    // distinct serviceAddrs that notification services need to be started for
    // serviceAddr -> NSADetail
    typedef std::map<std::string, TA_Base_App::NSALocator::NSADetail> ServiceAddrToNSADetailMap;
    ServiceAddrToNSADetailMap serviceAddrs;

    {
        TA_Base_App::NSALocator locator;

        // Find all of the NSAs that are servicing the locations and
        // build the set of distinct hosts and serviceAddrs.
        TA_Base_App::NSALocator::NSADetailVec nsaDetailVec;
        for (unsigned int i1 = 0; i1 < locationKeys.size(); ++i1)
        {
            locator.findNSAsAtLocation(locationKeys[i1], nsaDetailVec);
        }

        for (unsigned int i2 = 0; i2 < nsaDetailVec.size(); ++i2)
        {
            hosts.insert(nsaDetailVec[i2].m_hostname);

            // NOTE that multiple locations cannot share a notification service.  Each
            // notification service requires a single --location-key RunParam to be specified.
            // TODO LPP: We should check that we haven't duplicated a serviceAddr for multiple
            // locations.
            serviceAddrs.insert( ServiceAddrToNSADetailMap::value_type(
                TA_Base_Core::gMakeServiceAddr(nsaDetailVec[i2].m_hostname, nsaDetailVec[i2].m_portnum), nsaDetailVec[i2] ));    
        }
    }

    // START THE CONSOLE MANAGERS
    // TODO LPP: Do we also need a console manager on every simulatorhost?
    TA_Assert(!hosts.empty());
    for (std::set<std::string>::iterator cmIt = hosts.begin(); cmIt != hosts.end(); ++cmIt)
    {
        if ( !spawnConsoleMgr(handle, *cmIt, TA_Base_Core::gStringFromUnsignedLong(DEFAULT_MGRPORT)) ) 
        {
            return false;
        }
    }

    // START THE NOTIFICATION SERVICES
    TA_Assert(!serviceAddrs.empty());
    for (ServiceAddrToNSADetailMap::iterator nsIt = serviceAddrs.begin(); nsIt != serviceAddrs.end(); ++nsIt)
    {
        if (!spawnNotificationService(
            handle, 
            nsIt->second.m_hostname, 
            nsIt->second.m_portnum, 
            TA_Base_Core::gStringFromUnsignedLong(DEFAULT_MGRPORT), 
            nsIt->second.m_locationKey) ) 
        {
            return false;
        }
    }
    
    return true;
}


int main(int argc, char* argv[])
{
    // NST_Controller.exe performs the following tasks:
    // 1. Register with STAF
    // 2. Start console manager and notification services on each host.
    // 3. Start all simulators on relevant hosts

    if( false == TA_Base_Core::RunParams::getInstance().parseCmdLine( argc - 1, &argv[1] ) )
    {
	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Unable to parse command line. Aborting." );
        return 1;
    }

    // Map of all the simulatorID's and their handles - used when killing processes.
    ProcessHandleMap processHandles;

    // 0.  Register with STAF
    STAFHandlePtr handle;
    unsigned int rc  = STAFHandle::create( "ModuleTest", handle );
    if ( rc != 0 )
    {
	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Error registering with STAF, RC: %d. Aborting.", rc );
        return 1;
    }

	if ( !startAllServices(handle) )
	{
	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Could not start services on all hosts. Aborting." );
		return 1;
	}

	Sleep(5000);

	if ( !startAllSimulators( handle, processHandles ) )
	{
	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Could not start all simulators. Aborting." );
		#ifdef WIN32
			Sleep( 3000 );
		#endif
		killAll(handle, processHandles);

        return 1;
	}

    // TODO LPP: Bring up and down message consumers, message suppliers and notification services
    // to simulate failure.

    // Kill all simulators.
    // TODO LPP: It would be preferable to WAIT until the simulators have finished.  We need
    // to be able to detect when all of the simulator processes have terminated.
    // In fact, it isn't really necessary to wait at all, since this process currently does
    // nothing after this point - so we won't wait.
	// killAll(handle, processHandles);

    // TODO LPP: Kill the notification services and console managers.

    return 0;
}



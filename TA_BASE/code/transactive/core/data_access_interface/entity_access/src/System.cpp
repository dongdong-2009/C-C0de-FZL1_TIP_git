/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/System.cpp $
  * @author Rob Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * System is a concrete implementation of ISystem, which is in turn an implementation of
  * IEntityData. It proves read-only access to System entity data, and a specific framework
  * for System entity parameters.
  *
  * 
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/System.h"
#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string System::ENTITY_TYPE           = "System";
    const std::string System::SERVICES = "Services";
    const std::string System::DEBUGLOGDIR = "DebugLogDir";
    const std::string System::MONITORSERVERLIST = "MonitorServerList";
    const std::string System::MONITORSLAVELIST = "MonitorSlaveList";
    const std::string System::FAILOVER = "Failover";

    const std::string System::PRESTARTACTIVEPROG = "PreStartActiveProg";
    const std::string System::PRESTOPACTIVEPROG = "PreStopActiveProg";
    const std::string System::POSTSTOPACTIVEPROG = "PostStopActiveProg";
    const std::string System::PRESHUTDOWNPROG = "PreShutdownProg";
    const std::string System::POSTSHUTDOWNPROG = "PostShutdownProg";
    const std::string System::PREFAILOVERPROG = "PreFailoverProg";
    const std::string System::FAILOVERPROG = "FailoverProg";
    const std::string System::NONFAILOVERPROG = "NonFailoverProg";
    const std::string System::POSTFAILOVERPROG = "PostFailoverProg";
    const std::string System::DBFAILOVERPROG = "DBFailoverProg";
    const std::string System::POSTDBFAILOVERPROG = "PostDBFailoverProg";
    const std::string System::DBCLEANUPPROG = "DBCleanup";
    const std::string System::POSTDBCLEANUPPROG = "PostDBCleanupProg";
    const std::string System::CLEANUPPROG = "CleanupProg";
    const std::string System::POSTCLEANUPPROG = "PostCleanupProg";
    const std::string System::SECONDARYFAILPROG = "SecondaryFailProg";
    const std::string System::POSTSECONDARYFAILPROG = "PostSecondaryFailProg";
    const std::string System::DBSTATUSPROG = "DBStatusProg";
    const std::string System::DBSTATUSFAILPROG = "DBStatusFailProg";

    

	System::System()
	{
	}

	
    System::System(unsigned long key) : EntityData( key, getStaticType() )
    {
    }


    System::~System()
    {
    }


    std::string System::getType()
    {
        return getStaticType();
    }


    std::string System::getStaticType()
    {
        return ENTITY_TYPE;
    }


	IEntityData* System::clone(unsigned long key)
	{
		return new System( key );
	}


    std::string System::getCleanupProg()
    {
		return getHelper()->getParameter(CLEANUPPROG);
    }


    std::string System::getDBCleanupProg()
    {
        return getHelper()->getParameter(DBCLEANUPPROG);
    }


    std::string System::getDBFailoverProg()
    {
		return getHelper()->getParameter(DBFAILOVERPROG);
    }


    std::string System::getDBStatusFailProg()
    {
		return getHelper()->getParameter(DBSTATUSFAILPROG);
    }


    std::string System::getDBStatusProg()
    {
		return getHelper()->getParameter(DBSTATUSPROG);
    }


    std::string System::getDebugLogDir()
    {
		return getHelper()->getParameter(DEBUGLOGDIR);
    }


    std::string System::getFailover()
    {
		return getHelper()->getParameter(FAILOVER);
    }


    std::string System::getFailoverProg()
    {
		return getHelper()->getParameter(FAILOVERPROG);
    }


    std::string System::getMonitorServerList()
    {
		return getHelper()->getParameter(MONITORSERVERLIST);
    }


    std::string System::getMonitorSlaveList()
    {
		return getHelper()->getParameter(MONITORSLAVELIST);
    }


    std::string System::getNonFailoverProg()
    {
		return getHelper()->getParameter(NONFAILOVERPROG);
    }


    std::string System::getPostCleanupProg()
    {
		return getHelper()->getParameter(POSTCLEANUPPROG);
    }


    std::string System::getPostDBCleanupProg()
    {
		return getHelper()->getParameter(POSTDBCLEANUPPROG);
    }


    std::string System::getPostDBFailoverProg()
    {
        return getHelper()->getParameter(POSTDBFAILOVERPROG);
    }


    std::string System::getPostFailoverProg()
    {
		return getHelper()->getParameter(POSTFAILOVERPROG);
    }


    std::string System::getPostSecondaryFailProg()
    {
		return getHelper()->getParameter(POSTSECONDARYFAILPROG);
    }


    std::string System::getPostShutdownProg()
    {
		return getHelper()->getParameter(POSTSHUTDOWNPROG);
    }


    std::string System::getPostStopActiveProg()
    {
		return getHelper()->getParameter(POSTSTOPACTIVEPROG);
    }


    std::string System::getPreFailoverProg()
    {
        return getHelper()->getParameter(PREFAILOVERPROG);
    }


    std::string System::getPreShutdownProg()
    {
		return getHelper()->getParameter(PRESHUTDOWNPROG);
    }


    std::string System::getPreStartActiveProg()
    {
		return getHelper()->getParameter(PRESTARTACTIVEPROG);
    }


    std::string System::getPreStopActiveProg()
    {
        return getHelper()->getParameter(PRESTOPACTIVEPROG);
    }


    std::string System::getSecondaryFailProg()
    {
		return getHelper()->getParameter(SECONDARYFAILPROG);
    }


    std::string System::getServices()
    {
		return getHelper()->getParameter(SERVICES);
    }
    

    void System::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core

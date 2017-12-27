/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/Process.cpp $
  * @author Rob Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Process is a concrete implementation of IProcess, which is in turn an implementation of
  * IEntityData. It proves read-only access to Process entity data, and a specific framework
  * for Process entity parameters.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER


#include "core/data_access_interface/entity_access/src/Process.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    
    const std::string Process::ENTITY_TYPE      = "MonitoredProcess";
    // The entity parameters as they appear in the Database
    const std::string Process::USERNAME = "UserName";
	const std::string Process::PASSWORD = "Passwd";
    const std::string Process::MANAGEDPROCESS = "ManagedProcess";
    const std::string Process::SERVICEPROCESS = "ServiceProcess";
    const std::string Process::STARTLAG = "StartLag";
    const std::string Process::TERMINATETIMEOUT = "TerminateTimeout";
    const std::string Process::KILLTIMEOUT = "KillTimeout";
    const std::string Process::STARTUPTIMEOUT = "StartTimeout";
    const std::string Process::CONTROLTIMEOUT = "ControlTimeout";
    const std::string Process::MONITORTIMEOUT = "MonitorTimeout";
    const std::string Process::AUTOFAILBACK = "AutoFailback";
	const std::string Process::PRIORITY = "Priority";
	const std::string Process::PROCESSOR = "Processor";

	// A constant for the PROCESSOR entity parameter.
	const int Process::NOT_BOUND = -1;


	Process::Process()
	{
	}


    Process::Process(unsigned long key) :
		EntityData( key, getStaticType() )
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "Process (constructor)");
        LOG( SourceInfo, DebugUtil::FunctionExit, "Process (constructor)");
    }


    Process::~Process()
    {
    }

    
    std::string Process::getType()
    {
        return getStaticType();
    }


    std::string Process::getStaticType()
    {
        return ENTITY_TYPE;
    }


    bool Process::isManagedProcess()
    {
        bool isManagedProcess = false;
		std::string value = getHelper()->getParameter(MANAGEDPROCESS);
		if(!value.empty())
		{
			isManagedProcess = EntityHelper::getBooleanData(value);
		}

        return isManagedProcess;
    }


    std::string Process::getUserName()
    {
		return getHelper()->getParameter(USERNAME);
    }

	std::string Process::getPassWord()
    {
        return getHelper()->getParameter(PASSWORD);
    }
    unsigned int Process::getKillTimeout()
    {
        return EntityHelper::getIntegerData(getHelper()->getParameter(KILLTIMEOUT), 0);
    }

    unsigned int Process::getTerminateTimeout()
    {
        return EntityHelper::getIntegerData(getHelper()->getParameter(TERMINATETIMEOUT), 0);
    }

    unsigned int Process::getStartLag()
    {
        return EntityHelper::getIntegerData(getHelper()->getParameter(STARTLAG), 0);
    }

    unsigned int Process::getStartupTimeout()
    {
        return EntityHelper::getIntegerData(getHelper()->getParameter(STARTUPTIMEOUT), 0);
    }

    unsigned int Process::getChangeControlTimeout()
    {
        return EntityHelper::getIntegerData(getHelper()->getParameter(CONTROLTIMEOUT), 0);
    }

    unsigned int Process::getChangeMonitorTimeout()
    {
        return EntityHelper::getIntegerData(getHelper()->getParameter(MONITORTIMEOUT), 0);
    }


    bool Process::isServiceProcess()
    {
        bool isServiceProcess = false;

		std::string value = getHelper()->getParameter(SERVICEPROCESS);
		try
		{
			isServiceProcess = EntityHelper::getBooleanData(value);
		}
		catch (DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

        return isServiceProcess;
    }
    
    bool Process::shouldAutoFailback()
    {
        bool shouldAutoFailback = false;
		std::string value = getHelper()->getParameter(AUTOFAILBACK);
		try
		{
			shouldAutoFailback = EntityHelper::getBooleanData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}
        return shouldAutoFailback;
    }

	int Process::getPriority()
	{
        return EntityHelper::getIntegerData(getHelper()->getParameter(PRIORITY), -1);
	}
 
	int Process::getBoundProcessor()
	{
        return EntityHelper::getIntegerData(getHelper()->getParameter(PROCESSOR), NOT_BOUND);
	}

    IEntityData* Process::clone(unsigned long key)
    {
        return new Process(key);        
    }

    void Process::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core

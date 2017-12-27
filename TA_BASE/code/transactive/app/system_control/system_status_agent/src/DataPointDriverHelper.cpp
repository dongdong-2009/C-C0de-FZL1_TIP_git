/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/DataPointDriverHelper.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * DataPointDriverHelper class implimentation. See header file for class descrption
  */

#include "app/system_control/system_status_agent/src/DataPointDriverHelper.h"
#include "app/system_control/system_status_agent/src/SystemStatusAgentConfigException.h"
#include "app/system_control/system_status_agent/src/pingDriver/PingableDataPointDriver.h"
#include "app/system_control/system_status_agent/src/SystemStatusAgent.h"
#include "app/system_control/system_status_agent/src/taComponentsDriver/TaComponentsDriver.h"
#include "app/system_control/system_status_agent/src/pingDriver/PingableDataPointDriver.h"
#include "app/system_control/system_status_agent/src/taComponentsDriver/AbstractPingDependentPoint.h"

namespace TA_Base_App
{


	DataPointDriverHelper::DataPointDriverHelper(SystemStatusAgent& agentUser)
		: m_agentUser(agentUser)
	{
		FUNCTION_ENTRY("DataPointDriverHelper");

		//bolt the avaiable drivers in
		m_drivers.clear();
		addDrivers();

		FUNCTION_EXIT;
	}

	void DataPointDriverHelper::setControl()
	{
		FUNCTION_ENTRY("setControl");

		driverIter iter = m_drivers.begin();

		for(iter; iter != m_drivers.end(); iter++)
		{
			IDataPointDriver* pDriver = *iter;
			TA_ASSERT(0 != pDriver, " pointer to driver is null");

			if(0 != pDriver)
			{
				pDriver->setControl();
			}
		}

		FUNCTION_EXIT;
	}

	void DataPointDriverHelper::setMonitor()
	{

	
		FUNCTION_ENTRY("setMonitor");

		//We will iterate through the drivers in reverse order so that those with
		//dependencies on the Ping Driver are set to monitor mode first
		std::vector<IDataPointDriver*>::reverse_iterator rIter;
		rIter = m_drivers.rbegin();

		for(rIter; rIter != m_drivers.rend(); rIter++)
		{
			IDataPointDriver* pDriver = *rIter;
			TA_ASSERT(0 != pDriver, " pointer to driver is null");

			if(0 != pDriver)
			{
				pDriver->setMonitor();
			}
		}

		FUNCTION_EXIT;
	}


	void DataPointDriverHelper::addDrivers()
	{
		FUNCTION_ENTRY("addDrivers");

		//DO NOT PUT ANY CALLS in front of adding the PingableDataPointDriver and
		//calling AbstractPingDependentPoint::setPingDriver(). Setting Control and monitor are dependent
		//on the PingableDataPointDriver being the first in the vector
		PingableDataPointDriver* pingableDataPointDriver = new PingableDataPointDriver(m_agentUser);
		m_drivers.push_back(pingableDataPointDriver);

		//Note that the AbstractPingDependentPoint has a dependency on the PingableDataPointDriver
		//In general, dependencies accross drivers or Datapoint types is to be avoided, but...
		AbstractPingDependentPoint::setPingDriver(pingableDataPointDriver);

		m_drivers.push_back(new TaComponentsDriver(m_agentUser));

		//add one line for each driver type;
		//as example:
		//m_drivers.push_back(new YourDriverType());
		
		FUNCTION_EXIT;
	}

	void DataPointDriverHelper::processOnlineAgentUpdate(std::vector<std::string> changedParams)
	{
		FUNCTION_ENTRY("processOnlineAgentUpdate");

		driverIter iter = m_drivers.begin();

		for(iter; iter != m_drivers.end(); iter++)
		{			

			IDataPointDriver* pDriver = *iter;

			TA_ASSERT(0 != pDriver, " pointer to driver is null");

			if(0 != pDriver)
			{
				pDriver->processOnlineAgentUpdate(changedParams);
			}
		}
		
		FUNCTION_EXIT;
	}

	void DataPointDriverHelper::onTerminate()
	{
		FUNCTION_ENTRY("terminate");

		driverIter iter = m_drivers.begin();

		for(iter; iter != m_drivers.end(); iter++)
		{
			IDataPointDriver* pDriver = *iter;
			if(0 != pDriver)
			{
				pDriver->onTerminate();
			}
		}
		
		FUNCTION_EXIT;
	}

	DataPointDriverHelper::~DataPointDriverHelper()
	{
		FUNCTION_ENTRY("~DataPointDriverHelper");

		driverIter iter = m_drivers.begin();

		for(iter; iter != m_drivers.end(); iter++)
		{
			IDataPointDriver* pDriver = *iter;
			if(0 != pDriver)
			{
				//this should have already been called but...
				pDriver->onTerminate();
				delete pDriver;
				pDriver = 0;
			}
		}
		
		FUNCTION_EXIT;
	}

	void DataPointDriverHelper::giveDataPointToADriver(TA_Base_Bus::DataPoint* const dataPoint)
	{
		FUNCTION_ENTRY("giveDataPointToADriver");

		TA_ASSERT(0 != dataPoint, "dataPoint pointer is null");

		bool someDriverSaidOK = false;

		driverIter iter = m_drivers.begin();

		for(iter; iter != m_drivers.end(); iter++)
		{
			IDataPointDriver* pDriver = *iter;
			TA_ASSERT(0 != pDriver, " pointer to driver is null");
			TA_ASSERT(0 != dataPoint, "dataPoint pointer is null");
			TA_ASSERT(0 == (pDriver->getMode().compare(IDataPointDriver::INIT) ), 
				"giveDataPointToADriver() called when driver is not in INIT mode");

			//if this driver agrees to keep the point updated...
			if( pDriver->keepThisPointUpdated(dataPoint) )
			{
				someDriverSaidOK = true;
				break;
			}

		}

		//if no driver agrees to keep the point updated...
		if(!someDriverSaidOK)
		{
			std::ostringstream msg;
			msg << "There is no IDataPointDriver implimentation avaiable that knows " <<
				"how to update the datapoint with pkey " << dataPoint->getEntityKey();  
			TA_THROW( SystemStatusAgentConfigException( msg.str() ) ); 
		}
		
		FUNCTION_EXIT;
	}
} // end namespace TA_Base_App

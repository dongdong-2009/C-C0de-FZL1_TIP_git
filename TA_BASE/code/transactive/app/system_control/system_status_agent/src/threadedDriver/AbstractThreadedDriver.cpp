/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/threadedDriver/AbstractThreadedDriver.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * AbstractThreadedDriver class implimentation. See header file for class descrption
  */

//we need to pick up the sockets stuff

#ifdef LINUX
#include <sys/types.h>
#endif
#ifdef WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#include "app/system_control/system_status_agent/src/threadedDriver/AbstractThreadedDriver.h"


#include "app/system_control/system_status_agent/src/SystemStatusAgent.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "app/system_control/system_status_agent/src/IPointContainer.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


#include <algorithm>
#include <time.h>
#include <cctype>
#include <sstream>
#include <math.h>

static const long FORCED_POLLING_REST_MSEC = 500;

namespace TA_Base_App
{

	AbstractThreadedDriver::AbstractThreadedDriver(SystemStatusAgent& agentUser)
		: m_mode(IDataPointDriver::INIT),
		m_agentUser(agentUser),
		m_pollingRateSeconds(1),
		m_tellThreadToStop(true),
		m_isRunning(false)
	{

		FUNCTION_ENTRY("AbstractThreadedDriver");


		FUNCTION_EXIT;
	}

	AbstractThreadedDriver::~AbstractThreadedDriver()
	{
		FUNCTION_ENTRY("~AbstractThreadedDriver");

		//Calling on terminate here will have no effect if
		//this termiantation took place via the framework.
		//But if the app just dies. This will deactivate serants
		//and free memory
		onTerminate();

		FUNCTION_EXIT;
	}


	TA_Base_Bus::DataPoint* AbstractThreadedDriver::getDataPointByAddressIdentifier(const std::string& address)
	{
    	FUNCTION_ENTRY("getDataPointByAddressIdentifier");
		IterToDp iter = m_dataPoints.begin();
		for(iter; iter != m_dataPoints.end(); iter++)
		{
			std::string pointAddress = (*iter)->getAddressIdentifier();
			
			//If the address matches return the pointer
			if(0 == address.compare(pointAddress))
			{
	            FUNCTION_EXIT;
				return (*iter)->getDataPoint();	
			}

		}

		//If you made it here a point with the specified address was never found
    	FUNCTION_EXIT;
		return 0;
	}

	bool AbstractThreadedDriver::keepThisPointUpdated(TA_Base_Bus::DataPoint* const dataPoint)
	{
		FUNCTION_ENTRY("keepThisPointUpdated");

		IPointContainer* newPoint = convertToIPointContainer(dataPoint);

		if(0 == newPoint)
		{
			FUNCTION_EXIT;
			return false;
		}

		m_dataPoints.push_back(newPoint);

		FUNCTION_EXIT;

		return true;

	}

	bool AbstractThreadedDriver::convertAddressFieldToIpAddress(std::string& addressMixedCase, 
		const std::string& prefixMixedCase, bool disableAddressValidation /*= false*/)
	{
		FUNCTION_ENTRY("convertAddressFieldToIpAddress");

		if(addressMixedCase.size() < prefixMixedCase.size())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"keepThisPointUpdated() refused point because address (%s) is shorter than specified prefix (%s)",
				addressMixedCase.c_str(), prefixMixedCase.c_str());

			FUNCTION_EXIT;

			return false;
		}

		//convert args to lower case
		std::string address(addressMixedCase);
		std::transform(addressMixedCase.begin(), addressMixedCase.end(), address.begin(), ::tolower);
		std::string prefix(prefixMixedCase);
		std::transform(prefixMixedCase.begin(), prefixMixedCase.end(), prefix.begin(), ::tolower);
		

		//if the string does not start with "<prefix>"...
		//std::string start(prefix);

		if(0 != address.find(prefix))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"keepThisPointUpdated() refused point because address (%s) does not conform to required format",
				addressMixedCase.c_str());

			FUNCTION_EXIT;

			return false;
		}

		//if validation is turned off just set the address param to what is left and return true
		if(disableAddressValidation)
		{
			if(address.size() == prefix.size())
			{
				addressMixedCase = "";

			}
			else
			{
				//copy the bit after the prefix into a new string
				int colonIndex = prefix.size();

				std::string temp = addressMixedCase.substr(colonIndex);

				addressMixedCase = temp;
			}

			FUNCTION_EXIT;
			return true;
		}

		//see if the remaining bit looks like a valid IP address,
		// i.e. xxx.xxx.xxx.xxx where xxx are 0 to 255

		//copy the bit after the prefix into a new string
		int colonIndex = prefix.size();
		
		std::string ipAddress = addressMixedCase.substr(colonIndex);

		//for the three decimal points...
		std::istringstream ipStream(ipAddress);
		
		int validOctets = 0;
		int validDecimals = 0;

		int octet = -1;
		//read and verify first octet
		ipStream >> octet;
		if(0 <= octet && octet <= 255)
		{
			validOctets++;
		}		

		//read the first decimal
		char decimal;
		ipStream >> decimal;
		if('.' == decimal)
		{
			validDecimals++;
		}

		//read and verify second octet
		octet = -1;
		ipStream >> octet;		
		if(0 <= octet && octet <= 255)
		{
			validOctets++;
		}

		//read the second decimal
		decimal = '?';
		ipStream >> decimal;
		if('.' == decimal)
		{
			validDecimals++;
		}

		//read and verify third octet
		octet = -1;
		ipStream >> octet;
		if(0 <= octet && octet <= 255)
		{
			validOctets++;
		}

		//read the third decimal
		decimal = '?';
		ipStream >> decimal;
		if('.' == decimal)
		{
			validDecimals++;
		}

		//read and verify fourth octet
		octet = -1;
		ipStream >> octet;
		if(0 <= octet && octet <= 255)
		{
			validOctets++;
		}

		//There should no longer be anything left in the ipStream
		char c;
		//if we managed to read a char that should not be there...
		if(ipStream.get(c))
		{
			return false;
		}

		if(validOctets != 4 || validDecimals != 3 )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"keepThisPointUpdated() refused point because IP address (%s) does not conform to required format",
			ipAddress.c_str());

			FUNCTION_EXIT;

			return false;
		}

		//copy the valid address into the param
		addressMixedCase = ipAddress;

		FUNCTION_EXIT;

		return true;
	}

	std::string AbstractThreadedDriver::getMode()
	{
		FUNCTION_ENTRY("getMode");

		return m_mode;

		FUNCTION_EXIT;
	}

	void AbstractThreadedDriver::setControl()
	{

		FUNCTION_ENTRY("setControl");

		if(0 == m_mode.compare(IDataPointDriver::INIT))
		{

			init();

			//set each point to control mode
			IterToDp iter = m_dataPoints.begin();
			for(iter; iter != m_dataPoints.end(); iter++)
			{
				(*iter)->getDataPoint()->setToControlMode();
			}

			//start the thread that does all the pinging
			//this is a non-blocking call
			start();


			m_mode = IDataPointDriver::CONTROL;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"moved from INIT to CONTROL mode");
			

			FUNCTION_EXIT;

			return;

		}

		//Note that for pinging the transition from monitor to control
		//is the same as from init to control, less the init.
		//Framework will set children to control mode so you do not need to
		//do it here
		else if(0 == m_mode.compare(IDataPointDriver::MONITOR))
		{

			//start the thread
			start();

			m_mode = IDataPointDriver::CONTROL;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"moved from MONITOR to CONTROL mode");
			

			FUNCTION_EXIT;

			return;

		}
		
		else if(0 == m_mode.compare(IDataPointDriver::CONTROL))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"setControl() called when driver was already in control mode");

			FUNCTION_EXIT;

			return;

		}

		TA_ASSERT(0, "You should not reach this code!");
		return;
	}

	void AbstractThreadedDriver::setMonitor()
	{

		FUNCTION_ENTRY("setMonitor");

		if(0 == m_mode.compare(IDataPointDriver::INIT))
		{
			init();
			
			//don't need to do anything

			//just for safty's sake make sure the thread is not
			//running
			m_tellThreadToStop = true;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"moved from INIT to MONITOR mode");
			m_mode = IDataPointDriver::MONITOR;

			FUNCTION_EXIT;

			return;

		}

		else if(0 == m_mode.compare(IDataPointDriver::MONITOR))
		{
			//You are already in this more - you could probably assert here
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"setMonitor() called when driver was already in monitor mode");

			FUNCTION_EXIT;

			return;

		}

		else if(0 == m_mode.compare(IDataPointDriver::CONTROL))
		{
			//Stop the thread
			this->terminateAndWait();

			/*//set each point from control to monitor mode
			IterToDp iter = m_dataPoints.begin();
			for(iter; iter != m_dataPoints.end(); iter++)
			{
				setPointFromControlToMonitorState((*iter)->getDataPoint());
			}*/

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"moved from CONTROL to MONITOR mode");

			m_mode = IDataPointDriver::MONITOR;

			FUNCTION_EXIT;

			return;

		}
		TA_ASSERT(0, "You should not reach this code!");		

		return;
	}

	void AbstractThreadedDriver::init()
	{
		FUNCTION_ENTRY("init");

		m_agentEntityData = m_agentUser.getAgentData();
		
		//make sure the data is valid
		//m_agentEntityData->invalidate();

		// get the relavent params:
		m_pollingRateSeconds = m_agentEntityData->getIntParamValue(getPollingRateParamName());

		//tell derived driver to init()
		onInit(m_agentEntityData);

		FUNCTION_EXIT;
	}

	/*void AbstractThreadedDriver::setPointFromControlToMonitorState(TA_Base_Bus::DataPoint* const dataPoint)
	{
		FUNCTION_ENTRY("setPointToMonitorState");

		if(dataPoint->getStatus() == TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE)
		{
			//datapoint already has the appropriate status for montor mode
			return;
		}
		
		//get the current state of the datapoint so that you can
		//extract the current value and pass it back to the updateFieldState()
		//method
		TA_Base_Bus::DpValue oldValue = dataPoint->getCurrentValue();
		
		//TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE indicates we had a value but now
		//we can't communcate
		TA_Base_Bus::DataPointState newState ( oldValue, time( NULL ), TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE );
		dataPoint->updateFieldState( newState );

		FUNCTION_EXIT;
	}*/

	void AbstractThreadedDriver::processOnlineAgentUpdate(std::vector<std::string> changedParams)
	{ 
		FUNCTION_ENTRY("processOnlineAgentUpdate");

		std::vector<std::string>::iterator pString;

		//make sure the data is valid
		//m_agentEntityData->invalidate();

		//See if the polling rate param has been changed...
		pString = std::find(changedParams.begin(), changedParams.end(),
			getPollingRateParamName());
		
		if(pString != changedParams.end())
		{
			setPingRate(m_agentEntityData->getIntParamValue(getPollingRateParamName()));
		}

		onlineConfigUpdate(changedParams);

		FUNCTION_EXIT;
	}

	void AbstractThreadedDriver::onTerminate()
	{
		FUNCTION_ENTRY("onTerminate");
		
		//If in control mode set the points to a state appropriate for
		//no comms
		if(0 == m_mode.compare(IDataPointDriver::CONTROL))
		{
			//Stop the thread
			this->terminateAndWait();

			/*//set each point from control to monitor mode
			IterToDp iter = m_dataPoints.begin();
			for(iter; iter != m_dataPoints.end(); iter++)
			{
				if(0 != (*iter))
				{
					setPointFromControlToMonitorState((*iter)->getDataPoint());
				}
			}*/

			//delete the points
			IterToDp iter = m_dataPoints.begin();
			for(iter; iter != m_dataPoints.end(); iter++)
			{
				if(0 != (*iter))
				{
					delete (*iter);
					(*iter) = 0;
				}
			}

			m_mode = IDataPointDriver::MONITOR;

			FUNCTION_EXIT;

			return;

		}

		FUNCTION_EXIT;
	}

	void AbstractThreadedDriver::terminate()
	{
		FUNCTION_ENTRY("setPingRate");

		m_tellThreadToStop = true;

		FUNCTION_EXIT;
	}

	void AbstractThreadedDriver::setPingRate(unsigned int newRate)
	{
		FUNCTION_ENTRY("setPingRate");

		if(newRate != m_pollingRateSeconds)
		{
			m_pollingRateSeconds = newRate;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"setPingRate() has applied a new ping rate of %u", m_pollingRateSeconds);

		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"setPingRate() was called but the specified ping rate was already applied");

		}

		FUNCTION_EXIT;
	}





	void AbstractThreadedDriver::run()
	{
		FUNCTION_ENTRY("run");

		try
		{
			// this thread does all the hard work...
			m_isRunning = true;

			//don't want it just stopping again now do we?
			m_tellThreadToStop = false;

			// start by adding some observers to the entitys
			// This is done here because we only want observers to be observing
			// whilst the ping monitoring is happening.  This gets around having
			// to worry about activating/deactivating observers in control/monitor
			// modes.
			unsigned int numEntities = m_dataPoints.size();

			while(!m_tellThreadToStop)
			{

				
				time_t startTime;
				time(&startTime);
				
				/**
				  * pollPoints
				  * 
				  * for all the points you own check the related state and value
				  * of field equipment. Note that doPostPollUpdate() 
				  * is called following each call to this method. Hence, you can either update your
				  * datapoints in this call or retoactivly update them in the doPostPollUpdate()
				  * call. This mechanism is provided because some driver implimentations may
				  * not wish to serialy scan and update datapoints.
				  *
				  * This method is called in the thread when the driver is in control mode
				  */
				pollPoints();

				//if we've been told to stop - then break;
				if (m_tellThreadToStop)
				{
					break;	
				}

				//after polling the network set the value and/or states of the points
				//based on what the alarm policy state is for each point. Raise
				//alarms as required
				doPostPollUpdate();	

				//just hang out for any remaining time

				time_t now;
				time(&now);
				time_t elapsed = now - startTime;
				unsigned int remaining = 0;
				if(m_pollingRateSeconds > static_cast<unsigned int>(elapsed))
				{
					remaining = m_pollingRateSeconds - elapsed;
				}

				if(remaining > 1)
				{

					TA_ASSERT(remaining < 120, "calculated > 120 seconds remaining in loop.");

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Telling thread to sleep for %u seconds", remaining);

					sleep(remaining * 1000);

				}
                else
                {
                    // Force a sleep just to avoid thrashing the CPU
                	sleep(FORCED_POLLING_REST_MSEC);
                }
				
			} //end of thread loop

			//if here we must be terminating...

			m_isRunning = false;
		}
		catch(std::exception e)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
			"An exception was caught while trying to start a driver's thread:\n%s\nAgent will now terminate.", e.what());
			m_agentUser.agentTerminate();
			exit(1);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
			"An exception was caught while trying to start a driver's thread.\nAgent will now terminate.");
			m_agentUser.agentTerminate();
			exit(1);
		}

		FUNCTION_EXIT;
	}

} // end namespace TA_Base_App

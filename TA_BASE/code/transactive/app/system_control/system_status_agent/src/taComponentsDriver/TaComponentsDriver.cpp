/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/TaComponentsDriver.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * TaComponentsDriver class implimentation. See header file for class descrption
  */

#include "app/system_control/system_status_agent/src/taComponentsDriver/TaComponentsDriver.h"
#include "app/system_control/system_status_agent/src/taComponentsDriver/ISelfResolvingPointContainer.h"
#include "app/system_control/system_status_agent/src/taComponentsDriver/SystemControllerPoint.h"
#include "app/system_control/system_status_agent/src/taComponentsDriver/ControlStationPoint.h"
#include "app/system_control/system_status_agent/src/taComponentsDriver/DbServerPoint.h"
#include "app/system_control/system_status_agent/src/SystemStatusAgent.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/sockets/src/InetAddress.h"
#include "core/sockets/src/ICMPMessageCodec.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/SystemStatusAgentAlarms_MessageTypes.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <time.h>
static const long FORCED_POLLING_REST_MSEC = 500;

namespace TA_Base_App
{
	std::vector<unsigned long> TaComponentsDriver::m_pkeysOfPointsMissingPingPoints;

	TaComponentsDriver::TaComponentsDriver(SystemStatusAgent& agentUser)
		: m_mode(IDataPointDriver::INIT),
		  m_agentUser(agentUser),
		  NUM_THREADS(10),
		  m_terminateAllThreads(false),
		  m_pollingRateSec(5)

	{
		FUNCTION_ENTRY("TaComponentsDriver");


		FUNCTION_EXIT;
	}

	TaComponentsDriver::~TaComponentsDriver()
	{
		FUNCTION_ENTRY("~TaComponentsDriver");

		//Calling on terminate here will have no effect if
		//this termiantation took place via the framework.
		//But if the app just dies. This will deactivate serants
		//and free memory
		onTerminate();

		//delete the threads
		std::vector<DpUpdateThread*>::iterator iter;
		iter = m_threads.begin();

		while(iter != m_threads.end())
		{
			delete *iter;
			iter++;
		}

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

		FUNCTION_EXIT;
	}

	std::string TaComponentsDriver::getMode()
	{
		FUNCTION_ENTRY("getMode");

		FUNCTION_EXIT;
		return m_mode;

	}

	void TaComponentsDriver::reportPointMissingPingPoint(unsigned long pkey)
	{
		FUNCTION_ENTRY("reportPointMissingPingPoint");
		m_pkeysOfPointsMissingPingPoints.push_back(pkey);
		FUNCTION_EXIT;
	}

	void TaComponentsDriver::raiseAlarmOnMissingPingPoints()
	{
		FUNCTION_ENTRY("raiseAlarmOnMissingPingPoints");

		//If any missing ping points have been reported...
		if(m_pkeysOfPointsMissingPingPoints.size() > 0)
		{

			try
			{
				//the string should be the first key in the list, with an additional ...
				//if there is more than one key
				std::ostringstream keyString;
				std::vector<unsigned long>::iterator iter = m_pkeysOfPointsMissingPingPoints.begin();
				keyString << *iter;
				if(1 < m_pkeysOfPointsMissingPingPoints.size())
				{
					keyString << "...";
				}
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"%d datapoints have been reported as not having paired ping points. Will raise NoLanStatusPoint alarm."
					,m_pkeysOfPointsMissingPingPoints.size());				

				// Set up the description parameters
				TA_Base_Core::DescriptionParameters dp;
				dp.push_back(new TA_Base_Core::NameValuePair("DataPointPkey", keyString.str().c_str() ) );

				// Submit the alarm
				TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::SystemStatusAgentAlarms::NoLanStatusPoint, 
					m_agentEntityData->getKey(),
					m_agentEntityData->getTypeKey(),
					dp,
					m_agentEntityData->getName(),
					m_agentEntityData->getLocation(),
					m_agentEntityData->getSubsystem(),
					m_agentEntityData->getStringParamValue(TA_Base_Core::SystemStatusAgentEntityData::ASSET_NAME_PARAM),
					TA_Base_Core::AlarmConstants::defaultTime );
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Caught exception while trying to raise NoLanStatusPoint alarm");

			}
		}
		else
		{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"No points reported. No NoLanStatusPoint alarms need to be raised.");
		}

		
		FUNCTION_EXIT;
	}


	bool TaComponentsDriver::keepThisPointUpdated(TA_Base_Bus::DataPoint* const dataPoint)
	{
		FUNCTION_ENTRY("keepThisPointUpdated");

		ISelfResolvingPointContainer* newPoint = convertToIPointContainer(dataPoint);

		if(0 == newPoint)
		{
			FUNCTION_EXIT;
			return false;
		}

		m_dataPoints.push_back(newPoint);

		FUNCTION_EXIT;

		return true;

	}

	ISelfResolvingPointContainer* TaComponentsDriver::convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint)
	{
		FUNCTION_ENTRY("convertToIPointContainer");

		ISelfResolvingPointContainer* pPoint = 0;

		//Support for SystemControllerPoint
		pPoint = SystemControllerPoint::convertToIPointContainer(dataPoint);
		if(pPoint != 0)
		{
    		FUNCTION_EXIT;
			return pPoint;
		}

		//Support for ControlStationPoint  
		pPoint = ControlStationPoint::convertToIPointContainer(dataPoint);
		if(pPoint != 0)
		{
    		FUNCTION_EXIT;
			return pPoint;
		}

		//Support for DbServerPoint  
		pPoint = DbServerPoint::convertToIPointContainer(dataPoint);
		if(pPoint != 0)
		{
    		FUNCTION_EXIT;
			return pPoint;
		}

		//Add a line here if you want to support a new ISelfResolvingPointContainer type

		FUNCTION_EXIT;

		return 0;

	}

	void TaComponentsDriver::init()
	{
		FUNCTION_ENTRY("init");

		m_agentEntityData = m_agentUser.getAgentData();
		
		//make sure the data is valid
		//m_agentEntityData->invalidate();

		m_pollingRateSec = m_agentEntityData->getIntParamValue( TA_Base_Core::SystemStatusAgentEntityData::TA_POLL_RATE_PARAM);


		//init the data points
		IterToDp iter = m_dataPoints.begin();
		for(iter; iter != m_dataPoints.end(); iter++)
		{
			(*iter)->onInit(m_agentEntityData);
		}

		
		//This will cause the monitor mode agent to try and raise a duplicate
		//alarm but I don't see this as a problem
		raiseAlarmOnMissingPingPoints();
		
		//init the iterator to the datapoints
		m_iterator = m_dataPoints.begin();

		//create the threads in the thread pool
		for(int i = 0; i < NUM_THREADS; i++)
		{
			m_threads.push_back(new DpUpdateThread(this));
		}




		FUNCTION_EXIT;
	}

	void TaComponentsDriver::setControl()
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

			//start the thread that do all the updating
			//this is a non-blocking call
			startAllThreads();


			m_mode = IDataPointDriver::CONTROL;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
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

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"begin moving from MONITOR to CONTROL mode.");

			//start the thread that do all the updating
			//this is a non-blocking call
			startAllThreads();

			m_mode = IDataPointDriver::CONTROL;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
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
		FUNCTION_EXIT;
		return;
	}

	void TaComponentsDriver::setMonitor()
	{

		FUNCTION_ENTRY("setMonitor");

		if(0 == m_mode.compare(IDataPointDriver::INIT))
		{
			init();
			
			//don't need to do anything to the data points. They have the appropriate
			//quality when created. 

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"moved from INIT to MONITOR mode");

			m_mode = IDataPointDriver::MONITOR;

			FUNCTION_EXIT;

			return;

		}

		else if(0 == m_mode.compare(IDataPointDriver::MONITOR))
		{
			//You are already in this mode - you could probably assert here
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"setMonitor() called when driver was already in monitor mode");
			//start the thread that do all the updating
			//this is a non-blocking call
			startAllThreads();

			FUNCTION_EXIT;

			return;

		}

		else if(0 == m_mode.compare(IDataPointDriver::CONTROL))
		{
			//Stop the threads - blocking call
			//terminateAllThreads();

			/*//set each point from control to monitor mode
			IterToDp iter = m_dataPoints.begin();
			for(iter; iter != m_dataPoints.end(); iter++)
			{
				if(0 != (*iter))
				{
					setPointFromControlToMonitorState((*iter)->getDataPoint());
				}
			}*/

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"moved from CONTROL to MONITOR mode");

			m_mode = IDataPointDriver::MONITOR;

			FUNCTION_EXIT;

			return;

		}

		TA_ASSERT(0, "You should not reach this code!");		

		FUNCTION_EXIT;
		return;
	}

	/*void TaComponentsDriver::setPointFromControlToMonitorState(TA_Base_Bus::DataPoint* const dataPoint)
	{
		FUNCTION_ENTRY("setPointToMonitorState");

		if(dataPoint->getStatus() == TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE)
		{
			//datapoint already has the appropriate status for montor mode
		    FUNCTION_EXIT;
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

	void TaComponentsDriver::processOnlineAgentUpdate(std::vector<std::string> changedParams)
	{
		FUNCTION_ENTRY("processOnlineAgentUpdate");

		//make sure the data is valid
		//m_agentEntityData->invalidate();

		std::vector<std::string>::iterator pString;

		//See if the polling rate param has been changed...
		pString = std::find(changedParams.begin(), changedParams.end(),
			TA_Base_Core::SystemStatusAgentEntityData::TA_POLL_RATE_PARAM);
		
		if(pString != changedParams.end())
		{
			m_pollingRateSec = m_agentEntityData->getIntParamValue( TA_Base_Core::SystemStatusAgentEntityData::TA_POLL_RATE_PARAM);
		}


		IterToDp iter = m_dataPoints.begin();
		for(iter; iter != m_dataPoints.end(); iter++)
		{
			(*iter)->onlineConfigUpdate(changedParams);
		}

		FUNCTION_EXIT;
	}

	void TaComponentsDriver::onTerminate()
	{
		FUNCTION_ENTRY("onTerminate");
		
		//If in control mode set to monitor mode
		if(0 == m_mode.compare(IDataPointDriver::CONTROL))
		{
			//This will stop all the threads and set quality to bad on all points
			setMonitor();
		}

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

		FUNCTION_EXIT;

		return;

	}

	void TaComponentsDriver::terminateAllThreads()
	{
		FUNCTION_ENTRY("terminateAllThreads");

		m_terminateAllThreads = true;

		//stop the threads
		std::vector<DpUpdateThread*>::iterator iterThread = m_threads.begin();
		for(iterThread; iterThread != m_threads.end(); iterThread++)
		{
			(*iterThread)->terminateAndWait();
		}

		FUNCTION_EXIT;
	}

	void TaComponentsDriver::startAllThreads()
	{
		FUNCTION_ENTRY("startAllThreads");

		//TODO rem:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
		"begin startAllThreads().");

		m_terminateAllThreads = false;

		//If there are no points don't start the threads
		if(m_dataPoints.size() == 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"The TaComponentsDriver owns no points");

			FUNCTION_EXIT;
			return;
		}

		//this initializes the time that the main loop is started - see getNextDataPoint 
		//for usage
		time(&m_lastTime);

		//start the threads

		//TODO rem:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
		"will now call start on each thread.");

		std::vector<DpUpdateThread*>::iterator iterThread = m_threads.begin();
		for(iterThread; iterThread != m_threads.end(); iterThread++)
		{
			(*iterThread)->start();
		}

		//TODO rem:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
		"finished calling start on each thread. Returning from startAllThreads().");


		FUNCTION_EXIT;
	}


	ISelfResolvingPointContainer* TaComponentsDriver::getNextDataPoint()
	{
		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_lock );

		FUNCTION_ENTRY("getNextDataPoint");

		if(m_terminateAllThreads)
		{
			FUNCTION_EXIT;
			return 0;
		}	

		//If there are no points return 0
		if(m_dataPoints.size() == 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"The TaComponentsDriver owns no points");

            // Force a sleep just to avoid thrashing the CPU
            TA_Base_Core::Thread::sleep(FORCED_POLLING_REST_MSEC);

			FUNCTION_EXIT;
			return 0;
		}		
		
		
		//note that given that this is initialized to m_dataPoints.begin() in init()

		//This loop keeps incrementing the iterator until it finds a point
		//that is not already updating. Note that this is possible if the entire datapoint
		//loop is processed before one datapoint returns from updateDataPoint().

        // Loop for one iteration only before giving up and return NULL.  

		do
		{
			if(m_iterator == m_dataPoints.end())
			{
                //If we have reached the end, start at the beginning again
                m_iterator = m_dataPoints.begin();
            }
            else
            {
                m_iterator++;
            }

			if ( (m_iterator!= m_dataPoints.end()) && (*m_iterator)->isUpdating() )
			{
				 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				   "point with pkey %u. is udpdating", (*m_iterator)->getDataPoint()->getEntityKey());
	
			}


        } while( (m_iterator!= m_dataPoints.end()) && (*m_iterator)->isUpdating());

        if(m_iterator!= m_dataPoints.end()) 
        {
            // i.e. found one to process

		    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		    "getNextDataPoint() returning pointer to point with pkey %u.", (*m_iterator)->getDataPoint()->getEntityKey());

		    FUNCTION_EXIT;
		    return *m_iterator;
        }

      	//If we are at the end of the list of datapoints tell this thread to chill for any 
		//remaining time.  Note that all other threads will block at the threadguard.

		time_t now;
		time(&now);
		time_t elapsed = now - m_lastTime;
		unsigned int remaining = 0;
		if(m_pollingRateSec > static_cast<unsigned int>(elapsed))
		{
			remaining = m_pollingRateSec - elapsed;
		}
		
		if(remaining > 1)
		{
			TA_ASSERT(remaining < 120, "calculated > 120 seconds remaining in loop.");

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"Telling threads to sleep for %u seconds", remaining);


			//Only the threads call this method and given that this method
			//is thread-locked at the top then by putting the calling thread
			//to sleep here 
			unsigned int slept = 0;
			//The whole point of this loop is to allow us to check for 
			//termiante requests while sleeping so that we don't just nap
			//when we should be terminating
			while(slept < remaining )
			{
				if(m_terminateAllThreads)
				{
					break; //break out of the sleep loop if we are to die
				}
				//sleep for one second
				TA_Base_Core::Thread::sleep(1000);
				slept++;
			}
			
		}
        else // no time remaining
        {
            // Force a sleep just to avoid thrashing the CPU
            TA_Base_Core::Thread::sleep(FORCED_POLLING_REST_MSEC);
        }

		//store the beginning time of the next loop
		m_lastTime = now;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		    "getNextDataPoint() returning NULL as there is nothing to process this iteration");

	    FUNCTION_EXIT;
        return 0;
	}
		


} // end namespace TA_Base_App





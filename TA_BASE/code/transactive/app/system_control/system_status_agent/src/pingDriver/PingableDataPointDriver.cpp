/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/pingDriver/PingableDataPointDriver.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * PingableDataPointDriver class implimentation. See header file for class descrption
  */

#ifdef _MSC_VER
  #pragma warning(disable: 4018) // signed/unsigned mismatch
#endif // _MSC_VER

//we need to pick up the sockets stuff
#ifdef WIN32
#pragma warning( disable : 4503 ) 
#endif

#ifdef LINUX
#include <sys/types.h>
#endif
#ifdef WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#include "app/system_control/system_status_agent/src/pingDriver/PingableDataPointDriver.h"
#include "app/system_control/system_status_agent/src/pingDriver/PingablePoint.h"
#include "app/system_control/system_status_agent/src/pingDriver/SocketFailedException.h"
#include "app/system_control/system_status_agent/src/pingDriver/PercentageNetworkFailureAlarmPolicy.h"
#include "app/system_control/system_status_agent/src/pingDriver/ConsecutiveNetworkFailureAlarmPolicy.h"

#include "app/system_control/system_status_agent/src/SystemStatusAgent.h"
#include "core/data_access_interface/entity_access/src/SystemStatusAgentEntityData.h"
#include "core/threads/src/Thread.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/BooleanDataPoint.h"

#include "core/sockets/src/InetAddress.h"
#include "core/sockets/src/ICMPMessageCodec.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include <algorithm>
#include <time.h>
#include <cctype>
#include <sstream>
#include <math.h>

static const long FORCED_POST_POLL_UPDATE_REST_MSEC = 5;



namespace TA_Base_App
{
	//see comments in header
	const unsigned int PingableDataPointDriver::PING_RETRIES_PER_CYCLE = 2;

	PingableDataPointDriver::PingableDataPointDriver(SystemStatusAgent& agentUser)
		: AbstractThreadedDriver(agentUser),
		m_socket("icmp", true),
		m_pingTimeoutSeconds(10),
		m_pingRetries(2)

	{


		FUNCTION_ENTRY("PingableDataPointDriver");

		#ifdef WIN32
			m_pid = _getpid();
		#else
			m_pid = getpid();
		#endif

		FUNCTION_EXIT;
	}

	PingableDataPointDriver::~PingableDataPointDriver()
	{
		FUNCTION_ENTRY("~PingableDataPointDriver");

		FUNCTION_EXIT;
	}

	IPointContainer* PingableDataPointDriver::convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint)
	{
		FUNCTION_ENTRY("convertToIPointContainer");

		//see if this is a DI point:
		TA_Base_Bus::BooleanDataPoint* pBDP = dynamic_cast<TA_Base_Bus::BooleanDataPoint*>(dataPoint);

		if(0 == pBDP)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"convertToIPointContainer() refused non-bool point with pkey %u", dataPoint->getEntityKey());

			return 0; //I only handle BooleanDataPoint damn it!
		}

		//see if the address has the form "ping:<valid IP address>
		//note that some other driver in this agent might handle points with the IP address
		//set in the address field hence we prepend the "ping:" text to say how they want the
		//datapoint to be updated

		std::string addressMixedCase = dataPoint->getDataPointAddress();

		if(!convertAddressFieldToIpAddress(addressMixedCase, "ping:", true) )
		{

			return 0; //note that call already did logging so don't do it here
		}


		PingablePoint* newPoint = new PingablePoint(pBDP, addressMixedCase);


		FUNCTION_EXIT;

		return newPoint;

	}

	void PingableDataPointDriver::onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData)
	{
		FUNCTION_ENTRY("onInit");

		if(!m_socket.open())
		{
			TA_THROW(SocketFailedException("Could not open the socket for pinging"));
		}

		m_pingTimeoutSeconds = pEntityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::PING_TIMEOUT_PARAM);
		m_pingRetries = pEntityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::PING_RETRIES_PARAM);

		//Each datapoint is designed so that it can use a specific alarm policy, each
		//with its own pingRetries setting, hence we set each one here.
		// But the current implimentation uses the same policy for all points
		//So for future compatability we set all points here:
		IterToDp iter = m_dataPoints.begin();
		for(iter; iter != m_dataPoints.end(); iter++)
		{
			PingablePoint* pPoint = dynamic_cast<PingablePoint*>(*iter);
			pPoint->setMaxConsecutiveFails(m_pingRetries);
		}

		FUNCTION_EXIT;
	}

	void PingableDataPointDriver::doPostPollUpdate()
	{

		FUNCTION_ENTRY("doPostPollUpdate");

		IterToDp iter = m_dataPoints.begin();
		for(iter; iter != m_dataPoints.end(); iter++)
		{
			PingablePoint* pPoint = dynamic_cast<PingablePoint*>((*iter));

			INetworkFailureAlarmPolicy* const policy = pPoint->getAlarmPolicy();

			TA_Base_Bus::DpValue dpValue(pPoint->getDataPoint()->getDataPointDataType(),
										 pPoint->getDataPoint()->getEnumLabelsMap(),
										 pPoint->getDataPoint()->getBooleanLabels());

			//if the point is broken...
			if( ! ( policy->pointIsPingable() ) )
			{
				//set its value low		
				dpValue.setBoolean( false );

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Setting ping point with pkey %u to false", pPoint->getDataPoint()->getEntityKey());

			}
			else //if the we can ping the point...
			{
				//set its value to high
				dpValue.setBoolean( true );

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Setting ping point with pkey %u to true", pPoint->getDataPoint()->getEntityKey());			}

			TA_Base_Bus::DataPointState newState( dpValue, 
				time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );


			pPoint->getDataPoint()->updateFieldState(newState);

            // Force a sleep just to avoid thrashing the CPU
            sleep(FORCED_POST_POLL_UPDATE_REST_MSEC);
		}

		FUNCTION_EXIT;
	}

	
	void PingableDataPointDriver::pollPoints()
	{
		FUNCTION_ENTRY("pollPoints");

		const unsigned int NUM_ENTITIES = m_dataPoints.size();

		//how many times have we retried to ping
		unsigned int numRetries = 0;

		for (unsigned int i = 0; i < NUM_ENTITIES; i++)
		{
			//This member (when true) is used to mark this point as needing 
			//no further processing. We do this if we get a ping back for it
			//or we get sick of trying
			PingablePoint* pPoint = dynamic_cast<PingablePoint*>(m_dataPoints[i]);
			pPoint->setProcessed(false);
		}

		time_t timeAllSent = 0;
		time(&timeAllSent);

		//we expect to get a responce back from all points until we find out different
		unsigned int expectedResponses = NUM_ENTITIES;

		//the index of the datapoint you are currently dealing with
		unsigned int dataPointIndex = 0;

		//number of responces to ping we have received
		unsigned int responsesReceived = 0;

		//while any points have not either responded or failed to send...
		while (responsesReceived < expectedResponses)
		{
			//check and see if we've been told to terminate
			if (m_tellThreadToStop)
			{
				break;
			}

			//API REF:
				// wait for io on only one socket.  One of read or write must be
				// true otherwise function will return immediately with false.
				// static bool waitForOneIO(SocketType *socket, bool read, bool write,
				//					 bool &canRead, bool &canWrite,
				//					 unsigned int timeMillis);
				//
			// wait in 200ms blocks so we can bail out on
			// m_tellThreadToStop in a reasonably responsive manner.
			//this call sets the canRead and canWrite
			bool canRead = false;
			bool canWrite = false;

			if (TA_Base_Core::SocketSet<TA_Base_Core::IDatagramSocket>::waitForOneIO(&m_socket,
					true, numRetries < PING_RETRIES_PER_CYCLE, canRead, canWrite, 200))
			{
				if (canWrite)
				{
					// try to send at least one...
					bool triedToSendOne = false;

					while (!triedToSendOne)
					{
						PingablePoint* pPoint = dynamic_cast<PingablePoint*>(m_dataPoints[dataPointIndex]);

						if (!pPoint->hasBeenProcessed())
						{
							//note that the address obj is modified by the send method
							TA_Base_Core::InetAddress& ina = pPoint->getInternetAddressRef();

							//construct a datagram
							TA_Base_Core::Datagram datagram;

							//fill out packet structure so that it is ECHO, i.e. a "ping"
							TA_Base_Core::ICMPMessageCodec::getInstance()->constructECHO(datagram, m_pid, dataPointIndex);

							//if the sending of this datagram fails...
							//note that the address obj is modified by the send method
							if (!m_socket.send(ina, datagram))
							{
								//The failure is reported here so it does not need
								//to be set in the final loop below
								pPoint->getAlarmPolicy()->addResult(false);

								//mark it as sent
								pPoint->setProcessed(true);

								const std::string& name = ina.getHostName();
								LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "failed to send to ping for %s", name.c_str());

								// well, we're not trying this one again so don't expect a response.
								expectedResponses--;
							}

							triedToSendOne = true;
						} //end: if (!pPoint->hasBeenProcessed())

						dataPointIndex++;

						//If we have tried to send them all..
						if (dataPointIndex >= NUM_ENTITIES)
						{
							dataPointIndex = 0;
							numRetries++;
						}
						//If we have tried to send them all the max number of times...
						if (numRetries >= PING_RETRIES_PER_CYCLE)
						{
							// take note of the time when the last ping is sent
							// we now wait m_pingTimeoutSeconds seconds for the remainder
							// to be read
							time(&timeAllSent);
							//We add a bit of slack because time() truncates... how true
							timeAllSent++;
							break;  // out of while (!triedToSendOne)
						}

					} //end while (!triedToSendOne)

				} //end if (canWrite)

				//if a ping responce has arrived...
				if (canRead)
				{
					//These variables are populated by the receive method
					TA_Base_Core::InetAddress outa;
					TA_Base_Core::Datagram datagram;

					//if we can read the responce from the socket...
					if (m_socket.receive(outa, datagram))
					{
						unsigned short id = 0, index = 0;

						//if we can get the data out of the package...
						if (TA_Base_Core::ICMPMessageCodec::getInstance()->decodeECHO(datagram, id, index))
						{

							//if this is a datagram we sent...
							if (id == m_pid && index < NUM_ENTITIES)
							{
								PingablePoint* pPoint = dynamic_cast<PingablePoint*>(m_dataPoints[index]);

								// if this point has not already been processed...
								if (!pPoint->hasBeenProcessed())
								{
									//The success is reported here so it does not need
									//to be set in the final loop below
									pPoint->getAlarmPolicy()->addResult(true);
									pPoint->setProcessed(true);

									
									LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
									"point has been succesfully pinged");
						
									responsesReceived++;
								}
							}
							else
							{
								// an old or someone else's ping
								LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
									"decodeECHO() got bad id=%u or bad seq=%u", id, index);
							}
						}
						else
						{
							// this isn't necessarily bad. It could be someone else's packet.
							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "response failed checksum.");
						}
					}
					else
					{
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "receive() failed");
					}
				} //end : if(canRead)
			} //end: if socket is ready to read or write
			else //if socket is not ready to either read or write...
			{
				time_t now;
				time(&now);

				double current = now;
				double sentTime = timeAllSent;
				double maxTime = m_pingTimeoutSeconds;

				//if m_pingTimeoutSeconds has elapsed sence all were sent...
				if (current - sentTime >= maxTime)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "timed out.");
					break; //break out of the main while loop
				}
			}
		} //end while (responsesReceived < expectedResponses)

		// now set any not received as a failure...
		if (responsesReceived < NUM_ENTITIES)
		{
			for (unsigned int i = 0; i < NUM_ENTITIES; i++)
			{
				PingablePoint* pPoint = dynamic_cast<PingablePoint*>(m_dataPoints[i]);

				if (!pPoint->hasBeenProcessed())
				{
					pPoint->getAlarmPolicy()->addResult(false);
				}
			}
		}

		FUNCTION_EXIT;
	}

	void PingableDataPointDriver::onlineConfigUpdate(std::vector<std::string> changedParams)
	{
		FUNCTION_ENTRY("onlineConfigUpdate");

		//See if the PING_TIMEOUT_PARAM has been changed...
		std::vector<std::string>::iterator pString;
		pString = std::find(changedParams.begin(), changedParams.end(),
			TA_Base_Core::SystemStatusAgentEntityData::PING_TIMEOUT_PARAM);

		if(pString != changedParams.end())
		{
			setTimeOut(m_agentEntityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::PING_TIMEOUT_PARAM));
		}


		pString = std::find(changedParams.begin(), changedParams.end(),
			TA_Base_Core::SystemStatusAgentEntityData::PING_RETRIES_PARAM);

		//If the PING_RETRIES_PARAM has been changed...
		if(pString != changedParams.end())
		{
			setRetries(m_agentEntityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::PING_RETRIES_PARAM));
		}

		FUNCTION_EXIT;
	}
		
	std::string PingableDataPointDriver::getPollingRateParamName()
	{
		FUNCTION_ENTRY("getPollingRateParamName");
			return TA_Base_Core::SystemStatusAgentEntityData::PING_RATE_PARAM;
		FUNCTION_EXIT;
	}

	void PingableDataPointDriver::setTimeOut(unsigned int newTimeOut)
	{
		FUNCTION_ENTRY("setTimeOut");

		if(newTimeOut != m_pingTimeoutSeconds)
		{
			m_pingTimeoutSeconds = newTimeOut;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"setTimeOut() has applied a new ping timeout period of %u", m_pingTimeoutSeconds);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"setTimeOut() was called but the specified timeout period was already applied.");

		}

		FUNCTION_EXIT;
	}

	void PingableDataPointDriver::setRetries(unsigned int newRetrySetting)
	{
		FUNCTION_ENTRY("setRetries");


		if(newRetrySetting != m_pingRetries)
		{

			//Each datapoint is designed so that it can use a sopecific alarm policy
			//though the current implimentation uses the same policy for all points
			//for future compatability we set all points here:
			IterToDp iter = m_dataPoints.begin();
			for(iter; iter != m_dataPoints.end(); iter++)
			{
				PingablePoint* pPoint = dynamic_cast<PingablePoint*>(*iter);
				pPoint->setMaxConsecutiveFails(newRetrySetting);
			}

			m_pingRetries = newRetrySetting;


			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"setRetries() has applied a new retry setting of %u", m_pingRetries);

		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"setRetries() was called but the specified retry setting was already applied.");

		}

		FUNCTION_EXIT;
	}


} // end namespace TA_Base_App

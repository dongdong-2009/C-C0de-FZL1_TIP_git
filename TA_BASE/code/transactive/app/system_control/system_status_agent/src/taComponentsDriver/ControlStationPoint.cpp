/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/ControlStationPoint.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * ControlStationPoint class implimentation. See header file for class descrption
  */

#include "app/system_control/system_status_agent/src/taComponentsDriver/ControlStationPoint.h"
#include "app/system_control/system_status_agent/src/threadedDriver/AbstractThreadedDriver.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/RunParams.h"
#include <sstream>
#include <time.h>

namespace TA_Base_App
{

	ControlStationPoint::ControlStationPoint
	( 
		TA_Base_Bus::BooleanDataPoint* const datapoint,
		const std::string& hostName
	)
		: m_datapoint(datapoint),
		  m_hostName(hostName),
		  m_controlStation(TA_Base_Core::IControlStationCorbaDef::_nil()),
		  m_portNumber(TA_Base_Core::DEFAULT_MGR_PORT),
		  m_corbaTimeOutSec(20)
	{
		FUNCTION_ENTRY("ControlStationPoint");

		FUNCTION_EXIT;
	}



	ControlStationPoint::~ControlStationPoint()
	{
		FUNCTION_ENTRY("~ControlStationPoint");


		FUNCTION_EXIT;
	}

	ISelfResolvingPointContainer* ControlStationPoint::convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint)
	{

		FUNCTION_ENTRY("convertToIPointContainer");

		//see if this is a BooleanDataPoint:
		TA_Base_Bus::BooleanDataPoint* pBDP = dynamic_cast<TA_Base_Bus::BooleanDataPoint*>(dataPoint);

		if(0 == pBDP)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"convertToIPointContainer() refused non-BooleanDataPoint with pkey %u", dataPoint->getEntityKey());

			FUNCTION_EXIT;

			return 0;
		}

		//see if the address has the form "mftStatus:<host name>"

		std::string hostName = dataPoint->getDataPointAddress();

		//The true parameter in this call disables validation of the IP address portion of the string.
		//This is a good thing given that we're after the form mftStatus:<host name>
		if(!AbstractThreadedDriver::convertAddressFieldToIpAddress(hostName, "mftStatus:", true) )
		{
			FUNCTION_EXIT;

			return 0; //note that call already did logging so don't do it here
		}


		ControlStationPoint* newPoint = new ControlStationPoint(pBDP, hostName);

		FUNCTION_EXIT;

		return newPoint;
	}

	TA_Base_Bus::DataPoint* const ControlStationPoint::getDataPoint()
	{
		FUNCTION_ENTRY("getDataPoint");

		FUNCTION_EXIT;
		return m_datapoint;
	}

	std::string ControlStationPoint::getAddressIdentifier()
	{
		FUNCTION_ENTRY("getAddressIdentifier");
		FUNCTION_EXIT;
		return m_hostName;
	}


	void ControlStationPoint::onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData)
	{
		FUNCTION_ENTRY("onInit");

		//call this base class method so that the related ping point is resolved
		AbstractPingDependentPoint::onInit(pEntityData);

		
		m_entityData = pEntityData;
		m_portNumber = pEntityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::CONTROL_STATION_PORT_PARAM);
		m_corbaTimeOutSec = pEntityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::CONTROL_STATION_TIMEOUT_PARAM);

		//removed this so that time is not taken to resolve the obj during init
		//locateControlStation();

		FUNCTION_EXIT;
	}

	bool ControlStationPoint::updatePointBasedOnProcessStatus()
	{
		FUNCTION_ENTRY("updatePointBasedOnProcessStatus");

		//If the object is not resolved someone screwed up
		TA_ASSERT(isResolved(), "isResolved() returned false");

		//We will set the point false if we can not resolve the control station
		bool couldPoll = false;		
		
		try 			
		{
			//This call throws if the control station does not communicate
			m_controlStation->poll();

			//if it polled OK set the value to true
			couldPoll = true;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"Control Station poll() was OK on %s", m_hostName.c_str());
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"threw when trying to retrieve status on host %s. Datapoint pkey = %u. Will set state to 0 (false).", m_hostName.c_str(), m_datapoint->getEntityKey());
			couldPoll = false;
		}


        if (!couldPoll)
        {
            // this could be caused by paired pingpiont not pingable.
/*          if(!updateDataPointBasedOnPing())
            {
                //paired pingpoint not reachable already set to QUALITY_BAD_LAST_KNOWN_VALUE, no need proceed.
                return couldPoll;
            }*/
			TA_Base_Bus::DpValue oldValue = getDataPoint()->getCurrentValue();
			//TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED indicates we've never had a good value
			TA_Base_Bus::DataPointState newState ( oldValue, time( NULL ), TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED );
			//if the ping point is not configured correctly just set the 
			//quality to TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED and return
			if(0 == m_pingPoint)
			{			
				//TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED indicates we've never had a good value
				getDataPoint()->updateFieldState( newState );

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"No ping point for point with pkey %u. Setting quality bad", getDataPoint()->getEntityKey());

				FUNCTION_EXIT;
				return false;

			}

			//See if you can ping the coorosponding ping point
			TA_Base_Bus::DpValue value = m_pingPoint->getCurrentValue();
			bool pingable = value.getBoolean();
			if(!pingable)
			{
				//Set the quality of the point based on wether or not the
				//paired ping point has ever been pingable:
				if(!m_pingPointHasEverBeenHigh)
				{
					//the quality is already set correctly to TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED
				}
				else //the point was once pingable so we should have a last known value
				{
					newState.setStatus (TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE);
				}
				getDataPoint()->updateFieldState( newState );

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Paired ping point (pkey = %u) for point with pkey %u not pingable. Setting quality bad", m_pingPoint->getEntityKey(), getDataPoint()->getEntityKey());

				FUNCTION_EXIT;
				return couldPoll;
			}
			else
			{
				if(!m_pingPointHasEverBeenHigh)
				{
					//the quality is already set correctly to TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED
				}
				else //the point was once pingable so we should have a last known value
				{
					newState.setStatus (TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
				}
				getDataPoint()->updateFieldState( newState );

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Paired ping point (pkey = %u) for point with pkey %u is pingable. Setting quality good", m_pingPoint->getEntityKey(), getDataPoint()->getEntityKey());

				FUNCTION_EXIT;
				return couldPoll;
			}
        }
		TA_Base_Bus::DpValue dpValue(getDataPoint()->getDataPointDataType(),
									 getDataPoint()->getEnumLabelsMap(),
									 getDataPoint()->getBooleanLabels());

		dpValue.setBoolean( couldPoll );

		TA_Base_Bus::DataPointState newState( dpValue, 
			time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

		getDataPoint()->updateFieldState( newState );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
		"Set datapoint (pkey = %u) state (%i, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON).",getDataPoint()->getEntityKey(), (int)couldPoll);


		FUNCTION_EXIT;

		return couldPoll;

	}

	bool ControlStationPoint::isResolved()
	{
		return !(CORBA::is_nil( m_controlStation ) );
	}

	/**
	* setStateForNoProcessResolution
	*
	* update the state of the data point to an appropriate state given that you can not read the status of the
	* related process.
	*
	* @throw if you can not write to the datapoint
	* 
	*/
	void ControlStationPoint::setStateForNoProcessResolution()
	{
		FUNCTION_ENTRY("setStateForNoProcessResolution");

		//We will set the point false
		TA_Base_Bus::DpValue dpValue(getDataPoint()->getDataPointDataType(),
									 getDataPoint()->getEnumLabelsMap(),
									 getDataPoint()->getBooleanLabels());

		dpValue.setBoolean( false );

		TA_Base_Bus::DataPointState newState( dpValue, 
			time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

		getDataPoint()->updateFieldState( newState );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"Set datapoint (pkey = %u) state (false, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON).",getDataPoint()->getEntityKey());

		FUNCTION_EXIT;

		return;
	}


	bool ControlStationPoint::resolve()
    {

        FUNCTION_ENTRY("resolve");

		std::string connectionString = "";
	    try
	    {
			//construct the connection string
			std::stringstream connectionStringStrm;
			connectionStringStrm << "corbaloc::" << m_hostName << ":" << m_portNumber << "/ControlStation";	    
			connectionString = connectionStringStrm.str();
			

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Attempting to resolve Control Station with IOR = %s", 
				connectionString.c_str());

			CORBA::Object_var tmpVar;

			//ensure that the member reports nil if the resolution fails
			m_controlStation = TA_Base_Core::IControlStationCorbaDef::_nil();


			//resolve the object
			tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(connectionString.c_str());

			//narrow it
			m_controlStation = TA_Base_Core::IControlStationCorbaDef::_narrow(tmpVar);

			//If the object is nill return false
			if (CORBA::is_nil(m_controlStation))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "_narrow(tmpVar) returned null with IOR = %s", 
				connectionString.c_str());

				FUNCTION_EXIT;				
				return false;
			}

        }
        catch ( ... ) //either the resolution or the narrow threw
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "exception caught when trying to get a Control Station out of IOR = %s", 
			connectionString.c_str());

			//ensure that the member reports nil if the resolution fails
			m_controlStation = TA_Base_Core::IControlStationCorbaDef::_nil();

			FUNCTION_EXIT;
			return false;
        }

		try //set the timeout
		{

			omniORB::setClientCallTimeout( m_controlStation, ( m_corbaTimeOutSec * 1000 ) );

		}
		catch(...)
		{
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Attempt to set Control Station timeout failed");

			//ensure that the member reports nil if the setting the time out fails
			m_controlStation = TA_Base_Core::IControlStationCorbaDef::_nil();

			FUNCTION_EXIT;
			return false;
		}

		
        FUNCTION_EXIT;		
		return true;
    }

	void ControlStationPoint::onlineConfigUpdate(std::vector<std::string> changedParams)
	{
        FUNCTION_ENTRY("onlineConfigUpdate");

			//See if the CONTROL_STATION_PORT_PARAM has been changed...
		std::vector<std::string>::iterator pString;
		pString = std::find(changedParams.begin(), changedParams.end(),
			TA_Base_Core::SystemStatusAgentEntityData::CONTROL_STATION_PORT_PARAM);

		

		if(pString != changedParams.end())
		{
			m_portNumber = m_entityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::CONTROL_STATION_PORT_PARAM);
			
		}


		pString = std::find(changedParams.begin(), changedParams.end(),
			TA_Base_Core::SystemStatusAgentEntityData::CONTROL_STATION_TIMEOUT_PARAM);

		//If the CONTROL_STATION_TIMEOUT_PARAM has been changed...
		if(pString != changedParams.end())
		{
			m_corbaTimeOutSec = m_entityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::CONTROL_STATION_TIMEOUT_PARAM);
			
		}


		FUNCTION_EXIT;
	}

} //end namespace TA_Base_App

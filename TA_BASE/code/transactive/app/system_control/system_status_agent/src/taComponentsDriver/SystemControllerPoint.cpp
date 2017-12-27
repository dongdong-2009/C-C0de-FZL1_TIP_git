/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/SystemControllerPoint.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * SystemControllerPoint class implimentation. See header file for class descrption
  */

#include "app/system_control/system_status_agent/src/taComponentsDriver/SystemControllerPoint.h"
#include "bus/scada/datapoint_library/src/EnumDataPoint.h"
#include "app/system_control/system_status_agent/src/threadedDriver/AbstractThreadedDriver.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/RunParams.h"
#include <sstream>
#include <time.h>

namespace TA_Base_App
{
	const int SystemControllerPoint::STOPPED = 0;
	const int SystemControllerPoint::NORMAL = 1;
	const int SystemControllerPoint::ABNORMAL = 2;
	const int SystemControllerPoint::AGENT_FAIL = 3;


	SystemControllerPoint::SystemControllerPoint
	( 
		TA_Base_Bus::EnumDataPoint* const datapoint,
		const std::string& hostName
	)
		: m_datapoint(datapoint),
		  m_hostName(hostName),
		  m_systemController(TA_Base_Core::ISystemControllerAdminCorbaDef::_nil()),
		  m_portNumber(TA_Base_Core::DEFAULT_MGR_PORT),
		  m_corbaTimeOutSec(20)
	{
		FUNCTION_ENTRY("SystemControllerPoint");

		FUNCTION_EXIT;
	}



	SystemControllerPoint::~SystemControllerPoint()
	{
		FUNCTION_ENTRY("~SystemControllerPoint");


		FUNCTION_EXIT;
	}

	ISelfResolvingPointContainer* SystemControllerPoint::convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint)
	{

		FUNCTION_ENTRY("convertToIPointContainer");

		//see if this is a AnalogueDataPoint:
		TA_Base_Bus::EnumDataPoint* pTDP = dynamic_cast<TA_Base_Bus::EnumDataPoint*>(dataPoint);

		if(0 == pTDP)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"convertToIPointContainer() refused non-EnumDataPoint with pkey %u", dataPoint->getEntityKey());

			FUNCTION_EXIT;

			return 0;
		}

		//see if the address has the form "sysConStatus:<host name>"

		std::string hostName = dataPoint->getDataPointAddress();

		//The true parameter in this call disables validation of the IP address portion of the string.
		//This is a good thing given that we're after the form sysConStatus:<host name>
		if(!AbstractThreadedDriver::convertAddressFieldToIpAddress(hostName, "sysconstatus:", true) )
		{
			FUNCTION_EXIT;

			return 0; //note that call already did logging so don't do it here
		}


		SystemControllerPoint* newPoint = new SystemControllerPoint(pTDP, hostName);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"SystemControllerPoint::convertToIPointContainer() returning true on hostName = %s",hostName.c_str());

		FUNCTION_EXIT;

		return newPoint;
	}

	TA_Base_Bus::DataPoint* const SystemControllerPoint::getDataPoint()
	{
		FUNCTION_ENTRY("updateDataPoint");

		FUNCTION_EXIT;
		return m_datapoint;
	}

	std::string SystemControllerPoint::getAddressIdentifier()
	{
		FUNCTION_ENTRY("getAddressIdentifier");
		FUNCTION_EXIT;
		return m_hostName;
	}


	void SystemControllerPoint::onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData)
	{
		FUNCTION_ENTRY("onInit");

		//call this base class method so that the related ping point is resolved
		AbstractPingDependentPoint::onInit(pEntityData);

		m_entityData = pEntityData;

		m_portNumber = pEntityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::SYS_CON_PORT_PARAM);
		m_corbaTimeOutSec = pEntityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::SYS_CON_TIMEOUT_PARAM);
		
		//removed this so that time is not taken to resolve the obj during init
		//locateSystemController();

		FUNCTION_EXIT;
	}

	bool SystemControllerPoint::isResolved()
	{
		return !(CORBA::is_nil( m_systemController ) );
	}


	bool SystemControllerPoint::updatePointBasedOnProcessStatus()
	{
		FUNCTION_ENTRY("updatePointBasedOnProcessStatus");

		//If the object is not resolved someone screwed up
		TA_ASSERT(isResolved(), "isResolved() returned false");

		//Will set to stopped if the proces can not be resolved
		int newValue = SystemControllerPoint::STOPPED;

		try 			
		{
			//Get the status of the Sys Con
			TA_Base_Core::AgentStatusEnum status = m_systemController->getAgentsStatus();			

			//Convert status to integer value
			switch(status)
			{
				case TA_Base_Core::Normal :
				{
					newValue = SystemControllerPoint::NORMAL;
					break;
				}
				case TA_Base_Core::Abnormal :
				{
					newValue = SystemControllerPoint::ABNORMAL;
					break;
				}
				case TA_Base_Core::AgentFault :
				{
					newValue = SystemControllerPoint::AGENT_FAIL;
					break;
				}
				default :
				{
					TA_ASSERT(false, "Did not get a known value from m_systemController->getAgentsStatus()");
				}
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"threw when trying to retrieve status on host %s. Datapoint pkey = %u. Will set state to STOPPED.", m_hostName.c_str(), m_datapoint->getEntityKey());
		}

		//You now have the correct value to set even if you could not reach the process

		//Create DpValue from int status value
		TA_Base_Bus::DpValue dpValue(getDataPoint()->getDataPointDataType(),
									 getDataPoint()->getEnumLabelsMap(),
									 getDataPoint()->getBooleanLabels());

		dpValue.setEnum( newValue );


		//Create DataPointState with value above and GOOD quality
		TA_Base_Bus::DataPointState state( dpValue, 
			time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

		//Update the datapoint
		getDataPoint()->updateFieldState( state );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"Set datapoint state (pkey = %u) to %i,  TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON. Returning true.", m_datapoint->getEntityKey(), newValue);

		FUNCTION_EXIT;
		return ( !(newValue == SystemControllerPoint::STOPPED) );

	}

	void SystemControllerPoint::setStateForNoProcessResolution()
	{
		FUNCTION_ENTRY("setStateForNoProcessResolution");
		TA_Base_Bus::DpValue dpValue(getDataPoint()->getDataPointDataType(),
									 getDataPoint()->getEnumLabelsMap(),
									 getDataPoint()->getBooleanLabels());

		dpValue.setEnum( SystemControllerPoint::STOPPED );


		TA_Base_Bus::DataPointState state( dpValue, 
			time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

		getDataPoint()->updateFieldState( state );


		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
		"datapoint (pkey = %u) state set to %i,  TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON.", m_datapoint->getEntityKey(), SystemControllerPoint::STOPPED);
		FUNCTION_EXIT;
	}

   /*
	* resolve
	*
	* resolve or re-resove (and store) the process or object that is used to 
	* determine the state that the datapoint should be set to
	*
	*@return true if the resolution is OK - else false
	* 
	*/
    bool SystemControllerPoint::resolve()
    {

        FUNCTION_ENTRY("resolve");
		std::string connectionString = "";
	    try
	    {
			//create the connection string
			std::stringstream connectionStringStrm;
			connectionStringStrm << "corbaloc::" << m_hostName << ":" << m_portNumber << "/SystemControllerAdmin";	    
			connectionString = connectionStringStrm.str();
			

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Attempting to resolve System Controller with IOR = %s", 
				connectionString.c_str());

			CORBA::Object_var tmpVar;

			//ensure that the member reports nil if the resolution fails
			m_systemController = TA_Base_Core::ISystemControllerAdminCorbaDef::_nil();


			tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(connectionString.c_str());
			m_systemController = TA_Base_Core::ISystemControllerAdminCorbaDef::_narrow(tmpVar);
			if (CORBA::is_nil(m_systemController))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "_narrow(tmpVar) returned null with IOR = %s. (Pkey for dp = %u)", 
				connectionString.c_str(), m_datapoint->getEntityKey());

				FUNCTION_EXIT;				
				return false;
			}

        }
        catch ( ... )
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "exception caught when trying to resolve System Controller with IOR = %s. (Pkey for dp = %u)", 
			connectionString.c_str(), m_datapoint->getEntityKey());

			//ensure that the member reports nil if the resolution fails
			m_systemController = TA_Base_Core::ISystemControllerAdminCorbaDef::_nil();

			FUNCTION_EXIT;			
			return false;
        }

		try //try to set the timeout
		{
			omniORB::setClientCallTimeout( m_systemController, ( m_corbaTimeOutSec * 1000 ) );

		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Attempt to set timeout failed");
			//ensure that the member reports nil if the setting the timeout fails
			m_systemController = TA_Base_Core::ISystemControllerAdminCorbaDef::_nil();

			FUNCTION_EXIT;			
			return false;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "resolve() success for IOR = %s. (Pkey for dp = %u)", 
		connectionString.c_str(), m_datapoint->getEntityKey());


        FUNCTION_EXIT;		
		return true;
    }

	void SystemControllerPoint::onlineConfigUpdate(std::vector<std::string> changedParams)
	{
        FUNCTION_ENTRY("onlineConfigUpdate");

			//See if the SYS_CON_PORT_PARAM has been changed...
		std::vector<std::string>::iterator pString;
		pString = std::find(changedParams.begin(), changedParams.end(),
			TA_Base_Core::SystemStatusAgentEntityData::SYS_CON_PORT_PARAM);

		

		if(pString != changedParams.end())
		{
			m_portNumber = m_entityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::SYS_CON_PORT_PARAM);
			
		}


		pString = std::find(changedParams.begin(), changedParams.end(),
			TA_Base_Core::SystemStatusAgentEntityData::SYS_CON_TIMEOUT_PARAM);

		//If the SYS_CON_TIMEOUT_PARAM has been changed...
		if(pString != changedParams.end())
		{
			m_corbaTimeOutSec = m_entityData->getIntParamValue(TA_Base_Core::SystemStatusAgentEntityData::SYS_CON_TIMEOUT_PARAM);
			
		}

		FUNCTION_EXIT;
	}

} //end namespace TA_Base_App

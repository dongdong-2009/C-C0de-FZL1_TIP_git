/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/AbstractPingDependentPoint.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * AbstractPingDependentPoint class implimentation. See header file for class descrption
  */

#include "app/system_control/system_status_agent/src/taComponentsDriver/AbstractPingDependentPoint.h"
#include "app/system_control/system_status_agent/src/DataPointDriverHelper.h"
#include "app/system_control/system_status_agent/src/pingDriver/PingableDataPointDriver.h"

#include "app/system_control/system_status_agent/src/taComponentsDriver/TaComponentsDriver.h"
#include <time.h>

namespace TA_Base_App
{
	//static member init:
	PingableDataPointDriver* AbstractPingDependentPoint::m_pingableDataPointDriver = 0;



	AbstractPingDependentPoint::AbstractPingDependentPoint()
		: m_pingPoint(0), m_pingPointHasEverBeenHigh(false), m_isUpdating(false)
	{
		FUNCTION_ENTRY("AbstractPingDependentPoint");
		FUNCTION_EXIT;
	}

	void AbstractPingDependentPoint::setPingDriver(PingableDataPointDriver* driver)
	{
		m_pingableDataPointDriver = driver;
		TA_ASSERT(0 != m_pingableDataPointDriver, "The method setPingDriver() was passed a null pointer");
	}

	AbstractPingDependentPoint::~AbstractPingDependentPoint()
	{
		FUNCTION_ENTRY("~AbstractPingDependentPoint");
		FUNCTION_EXIT;
	}


	bool AbstractPingDependentPoint::updateDataPointBasedOnPing()
	{

		FUNCTION_ENTRY("updateDataPointBasedOnPing");

		//Get the current value of the data point
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
			return false;

		}
		//if here it is pingable
		m_pingPointHasEverBeenHigh = true;


		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"Paired ping point (pkey = %u) for point with pkey %u is pingable. Point not updated.", m_pingPoint->getEntityKey(), getDataPoint()->getEntityKey());

		FUNCTION_EXIT;

		return true;

	}

	bool AbstractPingDependentPoint::isUpdating()
	{
		FUNCTION_ENTRY("isUpdating");
		FUNCTION_EXIT;
		return m_isUpdating;
	}

	void AbstractPingDependentPoint::updateDataPoint()
	{
    	FUNCTION_ENTRY("updateDataPoint");
		m_isUpdating = true;
		try
		{
			updateDataPointWithoutFlag();
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Threw when trying to update datapoint (pkey = %u).", getDataPoint()->getEntityKey());
			//If here datapoint write threw (not the process resolution) , so forget about it

		}
		m_isUpdating = false;
		FUNCTION_EXIT;
	}

	void AbstractPingDependentPoint::updateDataPointWithoutFlag()
	{

		FUNCTION_ENTRY("updateDataPoint"); //note that this is a lie because updateDataPoint()
		//is just a wrapper for this method

		//If you can't ping the paired point the following returns false and sets the quality accordingly
		if(!updateDataPointBasedOnPing())
		{

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"Will not update datapoint (pkey = %u) further because updateDataPointBasedOnPing() returned false.",getDataPoint()->getEntityKey());

			FUNCTION_EXIT;
			return;
		}		
		
		//if the CORBA object is not resolved try and resolve it
		if( !isResolved() )
		{
			if( !( resolve() ) ) //if you can not resolve the process set the datapoint to the appropriate state and return
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Could not resolve process associated with datapoint (pkey = %u)", getDataPoint()->getEntityKey());
				
				try //set datapoint state to indicate no resolution
				{
					setStateForNoProcessResolution(); //note that this method logs success

				}			
				catch(...) //The write to the datapoint (not just the process resolution) failed
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Threw when trying to update datapoint (pkey = %u). Aborting update.", getDataPoint()->getEntityKey());
					//If here datapoint write threw (not the process resolution) , so forget about it

				}
				
				FUNCTION_EXIT;
				return;
			}
		}

		try //if here you have resolved the point or it was already resolved so try and update the status
		{
			//Try and update the datapoint based on the status of the process
			//If the update is OK you are done
			if(updatePointBasedOnProcessStatus()) //note that this method logs success
			{
				FUNCTION_EXIT;
				return;
			}
		}			
		catch(...) //The write to the datapoint (not just the process resolution) failed
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Threw when trying to update datapoint (pkey = %u). Aborting update.", getDataPoint()->getEntityKey());
			//If here datapoint write threw (not the process resolution) , so forget about it
			FUNCTION_EXIT;
			return;
		}


		//If here updatePointBasedOnProcessStatus() could not read the status and has updated point accordingly.
		//So try to re-resolve the object and try to get state again if resolution is OK

		//If you still can't resolve just return
		//note that the state of the point is already set to indicate no process contact
		if( !resolve() )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Could not resolve process associated with datapoint (pkey = %u)", getDataPoint()->getEntityKey());	

			FUNCTION_EXIT;
			return;
		}

		//if here we have re-resolved the object after failing the first attempt to talk to the process.
		//So try and get the status and update the datapoin again.
		try 
		{
			//Try and update the datapoint based on the status of the process
			//If the update is OK you are done
			if(updatePointBasedOnProcessStatus()) //note that this method logs success
			{
				FUNCTION_EXIT;
				return;
			}
		}			
		catch(...) //The write to the datapoint (not just the process resolution) failed. The status is already apprortiatly set
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Threw when trying to update datapoint (pkey = %u). Aborting update.", getDataPoint()->getEntityKey());

			FUNCTION_EXIT;
			return;
		}
	}

	void AbstractPingDependentPoint::onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData)
	{

		FUNCTION_ENTRY("onInit");
		TA_ASSERT(0 != m_pingableDataPointDriver, "m_pingableDataPointDriver == 0.\nThe method setPingDriver() must be called prior to onInit()");
		TA_Base_Bus::DataPoint* p = m_pingableDataPointDriver->getDataPointByAddressIdentifier(getAddressIdentifier());
		
		//If you can't find a paired ping point tell the driver and set the ping point pointer
		//to 0

		if(0 == p)
		{
			m_pingPoint = 0;
			TaComponentsDriver::reportPointMissingPingPoint(getDataPoint()->getEntityKey());

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"could not find a paired ping point for point with pkey %u, address = %s.", getDataPoint()->getEntityKey(), getAddressIdentifier().c_str());

			FUNCTION_EXIT;
			return;

		}

		m_pingPoint = dynamic_cast<TA_Base_Bus::BooleanDataPoint*>(p);
		TA_ASSERT(0 != m_pingPoint, "dynamic_cast<TA_Base_Bus::BooleanDataPoint*>(p) failed");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"found the paired ping point for point with pkey %u.", getDataPoint()->getEntityKey());

		FUNCTION_EXIT;

	}

} // end namespace TA_Base_App

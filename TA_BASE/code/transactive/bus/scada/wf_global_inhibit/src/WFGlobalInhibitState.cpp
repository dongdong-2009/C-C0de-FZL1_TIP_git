/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/TA_BASE/transactive/bus/scada/wf_global_inhibit/src/WFGlobalInhibitState.cpp $
  * @author: Liu Yong
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  * 
  * Implementation of the WFGlobalInhibitState class
  */

#include "bus/scada/wf_global_inhibit/src/WFGlobalInhibitState.h"
#include "bus/scada/wf_global_inhibit/src/WFDefs.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/BooleanDataPoint.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/utilities/src/TAAssert.h"
#include <time.h>
#include <algorithm>

namespace TA_Base_Bus
{
	WFGlobalInhibitState* WFGlobalInhibitState::m_instance = 0;
	TA_Base_Core::ReEntrantThreadLockable WFGlobalInhibitState::m_lock;

	WFGlobalInhibitState* WFGlobalInhibitState::getInstance()
	{
		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_lock );

		FUNCTION_ENTRY("getInstance");
		
		if(0 == m_instance)
		{
			m_instance = new WFGlobalInhibitState();
		}

		FUNCTION_EXIT;
		return m_instance;
	}

	WFGlobalInhibitState::WFGlobalInhibitState()
	: 
    m_globalInhibitPoint(NULL)
	{
		FUNCTION_ENTRY("WFGlobalInhibitState");
		FUNCTION_EXIT;
	}

	WFGlobalInhibitState::~WFGlobalInhibitState()
	{
		FUNCTION_ENTRY("~WFGlobalInhibitState");

		if(0 != m_instance)
		{
			delete m_instance;
		}

		FUNCTION_EXIT;
	}

	void WFGlobalInhibitState::initWFGlobalInhibitState()
	{
		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_lock );

		FUNCTION_ENTRY("initWFGlobalInhibitState");

		if(0 != m_globalInhibitPoint)
        {
            try
            {
				TA_Base_Bus::EQualityStatus quality = m_globalInhibitPoint->getStatus();

				if (TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON != quality &&
					TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE != quality)
				{
					//m_globalInhibitPoint->setToControlMode();
					setGlobalInhibitState( false );
				}
            }
            catch(...)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "setGlobalInhibitState() threw");
                //TA_ASSERT(false, "setGlobalInhibitDataPoint() throw");
            }
		}
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "0 == m_globalInhibitPoint");
        }

		FUNCTION_EXIT;
	}

	bool WFGlobalInhibitState::isGlobalInhibitDataPoint(TA_Base_Bus::DataPoint* dataPoint)
	{
		FUNCTION_ENTRY("isGlobalInhibitDataPoint");
		//see if this is a DI point:
		TA_Base_Bus::BooleanDataPoint* pBDP = dynamic_cast<TA_Base_Bus::BooleanDataPoint*>(dataPoint);

		if(0 == pBDP)
		{
			return false;
		}

		//see if the address is set to indicate that is either of the points we care about

        std::string addressMixedCase = dataPoint->getDataPointAddress();
		std::string address(addressMixedCase);

		//convert the string to lower case:
		std::transform(addressMixedCase.begin(), addressMixedCase.end(), address.begin(), ::tolower);

        std::string WFGlobalInhibitStr = TA_Base_Bus::WF_GLOBAL_INHIBIT_ADDRESS;
		std::string lowerGlobalInhibitAddress(WFGlobalInhibitStr);

		//convert the string to lower case:
		std::transform(WFGlobalInhibitStr.begin(), WFGlobalInhibitStr.end(), lowerGlobalInhibitAddress.begin(), ::tolower);
		
		if(std::string::npos != address.find(lowerGlobalInhibitAddress) )
		{
			m_globalInhibitPoint = pBDP;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"find WF Global Inhibit datapoint [%s] with Key %lu and address %s", 
				dataPoint->getDataPointName().c_str(),
				dataPoint->getEntityKey(),
				addressMixedCase.c_str());
			FUNCTION_EXIT;
			return true;

		}

		FUNCTION_EXIT;
		return false;
	}
	
	
	unsigned long WFGlobalInhibitState::getGlobalInhibitKey()
	{
		FUNCTION_ENTRY("setGlobalInhibitDataPoint");


		if(0 == m_globalInhibitPoint)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"The Global Inhibit datapoint is null");

			FUNCTION_EXIT;
			return 0;

		}

		unsigned int key = m_globalInhibitPoint->getEntityKey();

		FUNCTION_EXIT;
		return key;
	}

	void WFGlobalInhibitState::setGlobalInhibitState(bool applied)
	{

		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_lock );

		FUNCTION_ENTRY("setGlobalInhibitState");
		
		if(0 == m_globalInhibitPoint)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"The Global Inhibit datapoint is null and could not be updated");
            return;
		}

		//update the datapoint
		TA_Base_Bus::DpValue dpValue( m_globalInhibitPoint->getDataPointDataType(), m_globalInhibitPoint->getEnumLabelsMap(), m_globalInhibitPoint->getBooleanLabels() );
		dpValue.setBoolean( applied );

		TA_Base_Bus::DataPointState newState( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

		try
		{
			m_globalInhibitPoint->updateFieldState( newState );

			std::string trueOrFalse = "false";
			if(applied) trueOrFalse = "true";

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"setGlobalInhibitDataPoint( %s ) call was OK.", trueOrFalse.c_str());

		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"The Global Inhibit datapoint could not be updated");
			TA_ASSERT(false, "The Global Inhibit datapoint is null and could not be updated");

		}

		FUNCTION_EXIT;		
	}

	bool WFGlobalInhibitState::getGlobalInhibitState(bool& wfInhibitState)
	{
		FUNCTION_ENTRY("getGlobalInhibitState");
		TA_Base_Core::ThreadGuard guard( m_lock );

		if (NULL != m_globalInhibitPoint)
		{
			TA_Base_Bus::EQualityStatus quality = m_globalInhibitPoint->getStatus();
			TA_Base_Bus::DpValue dpValue(m_globalInhibitPoint->getCurrentValue());

			if (TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON == quality ||
				TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE == quality)
			{
				TA_Base_Bus::DpValue dpValue(m_globalInhibitPoint->getCurrentValue());
				
				wfInhibitState = dpValue.getBoolean();
				return true;
			}
			else
			{
				FUNCTION_EXIT;		
				return false;
			}
		}

		FUNCTION_EXIT;		
		return false;
	}
} //end namespace TA_IRS_App

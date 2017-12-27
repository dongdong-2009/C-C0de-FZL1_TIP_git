/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/ProtocolIOBase.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "ProtocolIOBase.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    ProtocolIOBase::ProtocolIOBase(std::string name)
		:
		m_name(name),
		m_dpProcessor(0),
		m_protocolManager(0)
    {
        
    }

	ProtocolIOBase::~ProtocolIOBase()
	{
	}

	bool ProtocolIOBase::addDataPoint(DataPoint* dp)
    {
        bool matched = false;
		matched = isDataPointMatched(dp);
		if (matched)
		{
			TA_Base_Core::ThreadGuard guard(m_lock);
			m_dataPoints.push_back(dp);
		}
		return matched;
    }

	void ProtocolIOBase::registerDPProcessor(IDataPointProcessor* psr)
	{
		m_dpProcessor = psr;
	}

	void ProtocolIOBase::registerProtocolManager(IProtocolManager* mgr)
	{
		m_protocolManager = mgr;
	}
}

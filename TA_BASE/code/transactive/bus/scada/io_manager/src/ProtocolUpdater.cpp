/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/ProtocolUpdater.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "ProtocolUpdater.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    ProtocolUpdater::ProtocolUpdater(std::string name)
		:
		ProtocolIOBase(name)
    {
        
    }

	ProtocolUpdater::~ProtocolUpdater()
	{
	}

	void ProtocolUpdater::updateDPFieldValue(DataPoint* dp, DataPointState& dpState)
	{
		if(dp)
		{
			this->m_dpProcessor->submitData(dp, dpState);
		}
	}
}

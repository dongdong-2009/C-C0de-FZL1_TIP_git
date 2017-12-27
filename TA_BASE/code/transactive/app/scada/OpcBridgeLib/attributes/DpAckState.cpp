/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File$
  * @author:	Darren Sampson
  * @version:	$Revision$
  *
  * Last modification:	$DateTime$
  * Last modified by:	$Author$
  *
  * Description:
  * Represents the DataPoint's DpAckState attribute.  The DpAckState attribute
  * receives and processes the alarm information from the Alarm and Messaging
  * sub systems.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <sstream>

#include "app/scada/OpcBridgeLib/attributes/DpAckState.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    DpAckState::DpAckState(DataPointAttributes* dpAttribute, const std::string& tagName)
	:
	Attribute ( tagName, (long) TA_Base_Bus::ScadaAlarmNotPresent, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED ),
    m_dpAttribute( dpAttribute )
	{
		//update((long) TA_Base_Bus::ScadaAlarmNack, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);
	}


	DpAckState::~DpAckState()
	{
	}


	void DpAckState::processProxyUpdate()
	{
       update((double)m_dpAttribute->getAlarmAckState(), m_dpAttribute->getStatus());
    }
}

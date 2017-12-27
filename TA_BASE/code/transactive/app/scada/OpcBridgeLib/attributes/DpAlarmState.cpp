/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  HoaVu
  * @version $Revision$
  *
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  * Represents the DataPoint's DpAlarmState attribute.  The DpAlarmState attribute
  * receives and processes the alarm information from the Alarm and Messaging
  * sub systems.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <sstream>

#include "app/scada/OpcBridgeLib/attributes/DpAlarmState.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "app/scada/opcbridgelib/src/DataPointAttributes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    DpAlarmState::DpAlarmState(DataPointAttributes* dpAttribute, const std::string& tagName)
	:
	Attribute( tagName, (bool)false, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED ),
	m_dpAttribute( dpAttribute )
	{
		//update( true, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED );
	}

	DpAlarmState::~DpAlarmState()
	{
	}

	void DpAlarmState::processProxyUpdate()
	{
		update(m_dpAttribute->isInAlarmState(), m_dpAttribute->getStatus());
	}
}

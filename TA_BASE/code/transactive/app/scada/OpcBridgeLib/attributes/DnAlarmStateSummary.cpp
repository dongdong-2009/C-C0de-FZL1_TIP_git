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
  * Class representing the Alarm Status attribute of a DataNode
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER


#include <sstream>

#include "app/scada/OpcBridgeLib/attributes/DnAlarmStateSummary.h"
#include "app/scada/OpcBridgeLib/src/DataNodeAttributes.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	DnAlarmStateSummary::DnAlarmStateSummary(DataNodeAttributes* dnAttribute, const std::string& tagName)
	:
	Attribute( tagName, (bool)false, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED ),
	m_dnAttribute( dnAttribute )
	{
		// intialise the _variant_t
		//update((bool) false, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);
	}

	DnAlarmStateSummary::~DnAlarmStateSummary()
	{
	}

	void DnAlarmStateSummary::processProxyUpdate()
	{
       TA_Base_Bus::BooleanTag dnTag = m_dnAttribute->getAlarmSummary();
	   update(dnTag.value, dnTag.quality);
	}
}

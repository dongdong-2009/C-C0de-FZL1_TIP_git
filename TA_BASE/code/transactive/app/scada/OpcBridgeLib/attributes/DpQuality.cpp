/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  
  * @version $Revision$
  *
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  * Class representing the status of the value of a DataPoint
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <sstream>

#include "app/scada/OpcBridgeLib/attributes/DpQuality.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "app/scada/opcbridgelib/src/DataPointAttributes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    DpQuality::DpQuality(DataPointAttributes* dpAttribute, const std::string& tagName)
	:
	Attribute( tagName, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED ),
	m_dpAttribute( dpAttribute )
	{
		// intialise the _variant_t			
		//update((double) TA_Base_Bus::QUALITY_BAD, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);
	}

	DpQuality::~DpQuality()
	{
	}

	void DpQuality::processProxyUpdate()
	{
		update((double)m_dpAttribute->getStatus(), m_dpAttribute->getStatus());
	}
}

/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File$
  * @author:	Chin Wenching
  * @version:	$Revision$
  *
  * Last modification:	$DateTime$
  * Last modified by:	$Author$
  *
  * Description:
  * Class representing the ONITP attribute of a DataNode (TD13873)
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif // defined _MSC_VER

#include <sstream>

#include "app/scada/OpcBridgeLib/attributes/DnOnitp.h"
#include "app/scada/OpcBridgeLib/src/DataNodeAttributes.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	DnOnitp::DnOnitp(DataNodeAttributes* dnAttribute, const std::string& tagName)
	:
	Attribute( tagName, "", TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED ),
	m_dnAttribute( dnAttribute )
	{
		// intialise the _variant_t
		//update("", TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);
	}

	DnOnitp::~DnOnitp()
	{
	}

	void DnOnitp::processProxyUpdate()
	{
        update(m_dnAttribute->getVariantONITP(), m_dnAttribute->getQualityValue());
	}

}

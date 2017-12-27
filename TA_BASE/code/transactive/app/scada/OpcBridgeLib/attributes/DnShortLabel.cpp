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
  * Class representing the Short Label attribute of a DataNode
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <sstream>

#include "app/scada/OpcBridgeLib/attributes/DnShortLabel.h"
#include "app/scada/OpcBridgeLib/src/DataNodeAttributes.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	DnShortLabel::DnShortLabel(DataNodeAttributes* dnAttribute, const std::string& tagName, const std::string& defaultValue)
	:
	Attribute( tagName, defaultValue.c_str(), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON ),
	m_dnAttribute( dnAttribute )
	{
		// intialise the _variant_t
		//update(defaultValue.c_str(), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
	}

	DnShortLabel::~DnShortLabel()
	{
	}
}

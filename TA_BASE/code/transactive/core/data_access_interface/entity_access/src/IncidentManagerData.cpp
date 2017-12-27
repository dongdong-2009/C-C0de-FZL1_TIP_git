/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IncidentManagerData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/IncidentManagerData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    const std::string IncidentManagerData::ENTITY_TYPE  = "IncidentManager";

	void IncidentManagerData::invalidate()
    {
        getHelper()->invalidate();
    }

    
    std::string IncidentManagerData::getType()
    {
        return getStaticType();
    }

    std::string IncidentManagerData::getStaticType()
    {
        return ENTITY_TYPE;
    }

} // end TA_Base_Core

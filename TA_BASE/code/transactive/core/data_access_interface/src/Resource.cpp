/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Resource.cpp $
  * @author:  Julian Chevalley
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Implementation of the Resource class
  *
  */

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER

#include "core/data_access_interface/src/Resource.h"

namespace TA_Base_Core
{
	Resource::Resource( const unsigned long key )
		: m_helper( new ResourceHelper(key) )
	{
	}

	Resource::Resource(const unsigned long row, TA_Base_Core::IData& data)
		: m_helper( new ResourceHelper( row, data ) )
	{
	}

    Resource::~Resource()
    {
		if( NULL !=  m_helper )
		{
			delete m_helper;
		}
    }

    unsigned long Resource::getKey()
    {
        return m_helper->getKey();
    }

	unsigned long Resource::getSubsystem()
	{
		return m_helper->getSubsystem();
	}

} // TA_Base_Core

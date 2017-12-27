/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ResourceAccessFactory.cpp $
  * @author:  Julian Chevalley
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Implements the ResourceAccessFactory class
  *
  */

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER

#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/Resource.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
	// initialise statics
	ResourceAccessFactory* ResourceAccessFactory::m_instance = 0;
	ReEntrantThreadLockable ResourceAccessFactory::m_singletonLock;

    ResourceAccessFactory::ResourceAccessFactory()
    {
    }


    ResourceAccessFactory& ResourceAccessFactory::getInstance()
    {
        // protect the instance check / creation
		TA_Base_Core::ThreadGuard guard(m_singletonLock);

		// if instance not already created
		if (m_instance == 0)
		{
			// create it
			m_instance = new ResourceAccessFactory();

			TA_ASSERT(m_instance != 0, "Memory allocation failure");
		}

		return *m_instance;

    }
	
	IResource* ResourceAccessFactory::getResourceFromEntity( unsigned long key )
	{
		IResource* resource = new Resource( key );
		return resource;
	}
} // closes TA_Base_Core



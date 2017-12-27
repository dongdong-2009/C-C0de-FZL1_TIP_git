/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/corba/src/ServantActivator.cpp $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class is used to nicely activate & de-activate
  * CORBA servants (interface implementations)
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/corba/src/ServantActivator.h"
#include "core/corba/src/ServantBase.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

	ServantActivator::ServantActivator()
	{
	}


	ServantActivator::~ServantActivator()
	{
	}


	PortableServer::Servant ServantActivator::incarnate(const PortableServer::ObjectId& oid,
								PortableServer::POA_ptr adapter)
	{
		return PortableServer::Servant();
	}

  
	void ServantActivator::etherealize(const PortableServer::ObjectId& oid,
								PortableServer::POA_ptr adapter,
								PortableServer::Servant serv,
								CORBA::Boolean cleanup_in_progress, 
								CORBA::Boolean remaining_activations)
	{
		// Do not clean up if there are still activations remaining
		if (remaining_activations)
		{
			return;
		}

		// This should be a servant base
			TA_Base_Core::ServantBase* servantBase = NULL;
			servantBase = dynamic_cast<TA_Base_Core::ServantBase*>(serv);

		// If the servantBase is not null, notify it that it is now
		// deactivated
		if (NULL != servantBase)
		{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
					TA_Base_Core::DebugUtil::DebugDebug,
				"ServantActivator::Etherealize() - Servant Deactivated");
			servantBase->setDeactivated();
		}
		else
		{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
					TA_Base_Core::DebugUtil::DebugDebug,
				"ServantActivator::Etherealize() - Servant Deleted");
			delete serv;
		}
		
	}


}; // namespace TA_Base_Core

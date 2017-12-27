/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataPointSyncManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  DataPointSyncManager.h
//  Implementation of the Class DataPointSyncManager
//  Created on:      28-Jun-2004 09:40:02 AM
///////////////////////////////////////////////////////////

#if !defined(DataPointSyncManager_1B993F4D_6709_466b_B984_C5195B7BF1C4__INCLUDED_)
#define DataPointSyncManager_1B993F4D_6709_466b_B984_C5195B7BF1C4__INCLUDED_

#include <map>
#include <string>

#include "boost/shared_ptr.hpp"

#include "bus/scada/proxy_library/src/SynchronisationManager.h"
#include "bus/scada/common_library/src/ThreadsafeMap.h"

#include "core/naming/src/NamedObject.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

/**
 * This class manages the list of data points (local and remote) that are inuse by
 * the factory.  A dedicated class is used to manage the sharing of this resource
 * between the synchronisation classes and the DataPointProxy (for write
 * functions)  Subscription is also handled in this class to assist with unit testing
 * @version 1.0
 * @created 28-Jun-2004 09:40:02 AM
 */
namespace TA_Base_Bus
{
	class DataPointAgentSyncIf;

	class DataPointSyncManager : public SynchronisationManager
	{
    public:

        DataPointSyncManager();
        virtual ~DataPointSyncManager();
		
		void setCallback( DataPointProxyManager& updateCallback );
	protected:

		 /**
          * This function will create a new agent interface.  It will use a direct
          * interface if the data point is local, else it will attempt to use the bulk
          * polling approach.  If this fails then it will default to the original event
          * driven interface.
          */
		virtual boost::shared_ptr<AgentSynchronisationInterface> createAgentInterface(TA_Base_Core::IEntityDataPtr entityData);
 
	private:

		DataPointProxyManager*			m_callback;
	};
};
#endif // !defined(DataPointSyncManager_1B993F4D_6709_466b_B984_C5195B7BF1C4__INCLUDED_)

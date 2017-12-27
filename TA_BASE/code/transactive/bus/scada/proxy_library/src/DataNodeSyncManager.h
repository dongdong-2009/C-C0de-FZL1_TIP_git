/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeSyncManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  DataNodeSyncManager.h
//  Implementation of the Class DataNodeSyncManager
//  Created on:      28-Jun-2004 09:40:02 AM
///////////////////////////////////////////////////////////

#if !defined(DataNodeSyncManager_1B993F4D_6709_466b_B984_C5195B7BF1C4__INCLUDED_)
#define DataNodeSyncManager_1B993F4D_6709_466b_B984_C5195B7BF1C4__INCLUDED_

#include <map>
#include <string>

#include "bus/scada/proxy_library/src/SynchronisationManager.h"

/**
 * This class manages the list of data points (local and remote) that are inuse by
 * the factory.  A dedicated class is used to manage the sharing of this resource
 * between the synchronisation classes and the DataNodeProxy (for write
 * functions)  Subscription is also handled in this class to assist with unit testing
 * @version 1.0
 * @created 28-Jun-2004 09:40:02 AM
 */
namespace TA_Base_Bus
{
	class DataNodeSyncManager : public SynchronisationManager
	{
		public:

		DataNodeSyncManager();
		virtual ~DataNodeSyncManager();
		
		void setCallback( DataNodeProxyManager& updateCallback );

	protected:

		 /**
          * This function will create a new agent interface.  It will use a direct
          * interface if the data point is local, else it will attempt to use the bulk
          * polling approach.  If this fails then it will default to the original event
          * driven interface.
          */
        virtual boost::shared_ptr<AgentSynchronisationInterface> createAgentInterface(TA_Base_Core::IEntityDataPtr entityData);

	private:

		DataNodeProxyManager*				m_callback;
	};
};
#endif // !defined(DataNodeSyncManager_1B993F4D_6709_466b_B984_C5195B7BF1C4__INCLUDED_)

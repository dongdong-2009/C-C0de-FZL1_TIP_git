/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeProxyManager.h $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by:  $Author: yong.liu $
  *
  */
///////////////////////////////////////////////////////////
//  DataNodeProxyManager.h
//  Implementation of the Class DataNodeProxyManager
//  Created on:      28-Jun-2004 09:39:36 AM
///////////////////////////////////////////////////////////

#if !defined(DataNodeProxyManager_0C5992F9_1E43_470b_B25B_B88B060429B6__INCLUDED_)
#define DataNodeProxyManager_0C5992F9_1E43_470b_B25B_B88B060429B6__INCLUDED_

#include "bus/scada/proxy_library/src/ObserverUpdateQueueProcessor.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataNodeProxyManager.h"
#include "bus/scada/proxy_library/src/IProxySmartPtrCallback.h"
#include "bus/scada/common_library/src/ThreadsafeMap.h"
#include "bus/generic_agent/src/IEntity.h"

#include "core/threads/src/QueueProcessor.h"
#include "core/naming/src/INamedObject.h"    // CorbaName definition
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

#include <map>

/**
 * This class will be responsible for managing the collection of data points and
 * notifying the subscribers of any change.
 * @version 1.0
 * @created 28-Jun-2004 09:39:36 AM
 */

namespace TA_Base_Bus
{
	class DataNodeSyncManager;
	class DataNodeProxy;
	class IDataNodeProxy;
	class IEntityUpdateEventProcessor;
    class IProxyObserverNotifier;

    class DnProxyDynamicSyncUpdate : public ProxyDynamicSyncUpdate
    {
	public:
        DnSyncUpdate detail;
    };
    
   
	//TD15939, Use this class as a queue processor to avoid blocking much time of caller
	class DataNodeProxyManager : public IProxySmartPtrCallback,
								 public IScadaProxyManager
	{

		public:

			DataNodeProxyManager(DataNodeSyncManager& syncManager,
       						     ObserverUpdateQueueProcessor& observerUpdateQueueProcessor);
			virtual ~DataNodeProxyManager();

		public:

            void registerDataNodeEntityData( TA_Base_Core::IEntityDataPtr EntityData );
			void getProxySmartPointer(unsigned long entityKey, TA_Base_Core::DataNodeEntityDataPtr entityData,
										DataNodeProxySmartPtr & dataNodeProxySmartPtr, IEntityUpdateEventProcessor& observer);

			//void setMonitorMode();

			//void setControlMode();

			//
			// IDataNodeProxySmartPtrCallback interface
			//

			// Used by DataNodeSmartPtr to notify library of destruction/copy
			virtual void onCreateProxy( IProxyObserverNotifier* observerNotifier,
										IEntityUpdateEventProcessor& observer );
			virtual void onCopyProxy( IProxyObserverNotifier* observerNotifier,
									  IEntityUpdateEventProcessor& observer );
			virtual void onDeleteProxy( IProxyObserverNotifier* observerNotifier,
										IEntityUpdateEventProcessor& observer );
			
         //************************************
         // Synchronisation Update Interfaces *
         //************************************

			/**
			 * Data Node Agent Interface not available
			 */
			virtual void onDataNodeAgentInterfaceNotAvailable (unsigned long entityKey);

			/**
			 * Data Node config load from DB required
			 */
			virtual void onloadDataNodeConfiguration ( unsigned long entityKey );

			//TD15939, to add update into queue
			virtual void insertUpdateQueue(boost::shared_ptr<DnProxyDynamicSyncUpdate> dataNodeUpdate);

		protected:

            boost::shared_ptr<DataNodeProxy> addProxy (unsigned long entityKey, TA_Base_Core::DataNodeEntityDataPtr entityData);

			virtual void processUpdateEvent(boost::shared_ptr<ProxyDynamicSyncUpdate> updateValue);

		private:

			typedef ThreadsafeMap< unsigned long, DataNodeProxy >  ProxyMap;
            typedef ThreadsafeMap< unsigned long, TA_Base_Core::DataNodeEntityData> DnEntityMap;

			ProxyMap								m_proxyMap;
            DnEntityMap                             m_entityMap;
            
			DataNodeSyncManager&					m_syncManager;
			ObserverUpdateQueueProcessor&					m_observerUpdateQueueProcessor;
	};
};
#endif // !defined(DataNodeProxyManager_0C5992F9_1E43_470b_B25B_B88B060429B6__INCLUDED_)

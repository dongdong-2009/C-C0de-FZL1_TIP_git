/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/ScadaProxyManager.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  * Template for a singleton queue object. Example 
  * implementations are DataPointWriteQueue and 
  * DataPointAlarmQueue.
  *
  *
  */

#ifndef SCADAPROXYMANAGER_H
#define SCADAPROXYMANAGER_H

#include <string>
#include <map>

#include "bus/scada/proxy_library/src/DataNodeSyncManager.h"
#include "bus/scada/proxy_library/src/DataPointSyncManager.h"
#include "bus/scada/proxy_library/src/SynchronisationManager.h"

#include "core/naming/src/INamedObject.h"   // For CorbaName structure
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


namespace TA_Base_Bus
{
    // Forward declarations
    class DataPointProxyManager;
    class DataNodeProxyManager;
    class IEntity;
    class IEntityUpdateEventProcessor;
    class IDataNodeProxy;
    class IDataPointProxy;
    class DataPointProxySmartPtr;
    class DataNodeProxySmartPtr;
    class ObserverUpdateQueueProcessor;

    class ScadaProxyManager
    {

    public:
           
		ScadaProxyManager(int numberOfWorkers = 0);
		virtual ~ScadaProxyManager();

	   /**
         *
         * registerLocalEntities
         *
         * Register map of local entities which may be accessed directly, avoiding the need
         * to go through the naming service.
         *
         */
         void registerLocalEntities(std::string agentName, const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap);
    

        /**
         *
         * registerEntityData
         *
         * Register map of local entitydata, avoiding the need
         * to go through database again.
         *
         */
         void registerEntityData(TA_Base_Core::IEntityDataPtr EntityData, const std::string& entityDataType);


        /**
         *
         * createDataPointProxy
         *
         * create a new monitored data point proxy and obtain a smart pointer to it.  Once the smart
         * pointer is destroyed the proxy will be automatically released from the library.
         *
         */
        void createDataPointProxy( unsigned long entityKey, 
                                                     TA_Base_Core::DataPointEntityDataPtr entityData, 
                                                     IEntityUpdateEventProcessor& eventProcessor,
													 DataPointProxySmartPtr & dataPointProxySmartPtr);

        /**
         *
         * createDataNodeProxy
         *
         * create a new monitored data node proxy and obtain a smart pointer to it.  Once the smart
         * pointer is destroyed the proxy will be automatically released from the library.
         *
         */
        void createDataNodeProxy( unsigned long entityKey, 
                                                   TA_Base_Core::DataNodeEntityDataPtr EntityData, 
                                                   IEntityUpdateEventProcessor& eventProcessor,
												   DataNodeProxySmartPtr &,
                                                   bool activateSummaries = true);
    
    protected:

        void cleanUp();
	private:
		friend class ScadaProxyFactory;
		void	startProcess();
		void	stopProcess();

    protected:

        ObserverUpdateQueueProcessor*                   m_observerUpdateQueueProcessor;
        DataPointSyncManager                            m_dataPointSyncManager;
        DataPointProxyManager*                          m_dataPointProxyManager;
        DataNodeSyncManager                             m_dataNodeSyncManager;
        DataNodeProxyManager*                           m_dataNodeProxyManager;


		TA_Base_Core::ReEntrantThreadLockable 			m_processLock;
		bool											m_isProcessing;
		
    };
}
#endif

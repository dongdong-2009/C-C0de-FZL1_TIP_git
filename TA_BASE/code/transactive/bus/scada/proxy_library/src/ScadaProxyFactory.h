/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/ScadaProxyFactory.h $
  * @author  J. Welton
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2015/01/21 14:56:06 $
  * Last modified by : $Author: haijun.li $
  *
  *
  * Template for a singleton queue object. Example 
  * implementations are DataPointWriteQueue and 
  * DataPointAlarmQueue.
  *
  *
  */

#ifndef SCADAPROXYFACTORY_H
#define SCADAPROXYFACTORY_H

#include <string>
#include <map>
#include "boost/shared_ptr.hpp"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/naming/src/INamedObject.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#define PARAM_SCADA_PROXY_CONSTRUCT_WO_START "ScadaProxyConstructWithoutStart"

namespace TA_Base_Bus
{
    // Forward declarations
	class ScadaProxyManager;
    class DataPointProxyManager;
    class DataNodeProxyManager;
    class IEntity;
    class IEntityUpdateEventProcessor;
    class IDataNodeProxy;
    class IDataPointProxy;
    class DataPointProxySmartPtr;
    class DataNodeProxySmartPtr;
    class ObserverUpdateQueueProcessor;

    class ScadaProxyFactory
    {

    public:
            

        /**
         * getInstance()
         *
         * Returns a pointer to the single instance
         * of ScadaProxyFactory, creating if necessary.
         *
         * @return  a pointer to a ScadaProxyFactory
         *
         */
         static ScadaProxyFactory& getInstance();


        /**
         * removeInstance()
         *
         * Terminates singleton instance of factory, releasing all resources associated
         * with it.
         *
         */
         static void removeInstance();


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
 								   IEntityUpdateEventProcessor& eventProcessor,
 								   DataPointProxySmartPtr &);

        /**
         *
         * createDataPointProxy
         *
         * create a new monitored data point proxy and obtain a smart pointer to it.  Once the smart
         * pointer is destroyed the proxy will be automatically released from the library.
         *
         */
        void createDataPointProxy( TA_Base_Core::DataPointEntityDataPtr entityData,
								   IEntityUpdateEventProcessor& eventProcessor,
								   DataPointProxySmartPtr&);

        /**
         *
         * createDataNodeProxy
         *
         * create a new monitored data node proxy and obtain a smart pointer to it.  Once the smart
         * pointer is destroyed the proxy will be automatically released from the library.
         *
         */
        void createDataNodeProxy( unsigned long entityKey,
                                                   IEntityUpdateEventProcessor& eventProcessor,
												   DataNodeProxySmartPtr&,
                                                   bool activateSummaries = true);
    
        /**
         *
         * createDataNodeProxy
         *
         * create a new monitored data node proxy and obtain a smart pointer to it.  Once the smart
         * pointer is destroyed the proxy will be automatically released from the library.
         *
         */
        void createDataNodeProxy( TA_Base_Core::DataNodeEntityDataPtr entityData,
                                                   IEntityUpdateEventProcessor& eventProcessor,
												   DataNodeProxySmartPtr & dataNodeProxySmartPtr,
                                                   bool activateSummaries = true);
    
        void	setProxiesToMonitorMode();
        void 	setProxiesToControlMode();
		void	setOperationMode(TA_Base_Core::EOperationMode mode);
		/**
         *
         * stopProcessing
         *
         * Stop ScadaProxyFactory's inner threads
         *
         */
		void stopProcessing();
    protected:

        ScadaProxyFactory();
        virtual ~ScadaProxyFactory();

        // Singleton
        static ScadaProxyFactory *                      m_singleton;
        static unsigned int                             m_refCounter;
        static TA_Base_Core::ReEntrantThreadLockable    m_singletonLock;
        
        ScadaProxyManager*								m_scadaProxyManager;
		TA_Base_Core::EOperationMode					m_currentMode;
    };
}
#endif

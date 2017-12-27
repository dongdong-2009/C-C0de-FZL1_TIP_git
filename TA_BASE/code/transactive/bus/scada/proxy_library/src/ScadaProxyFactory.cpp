/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/ScadaProxyFactory.cpp $
  * @author  A. Parker
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2015/01/21 14:56:06 $
  * Last modified by : $Author: haijun.li $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyManager.h"
#include "bus/scada/proxy_library/src/DataPointProxyManager.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataNodeProxyManager.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{

    ScadaProxyFactory*              ScadaProxyFactory::m_singleton = NULL;
    unsigned int                    ScadaProxyFactory::m_refCounter = 0;
    TA_Base_Core::ReEntrantThreadLockable ScadaProxyFactory::m_singletonLock;

    ScadaProxyFactory::ScadaProxyFactory()
	:m_scadaProxyManager (NULL),
	 m_currentMode(TA_Base_Core::Standby)
    {
        int numberOfWorkers = 5;
        if ( true == TA_Base_Core::RunParams::getInstance().isSet ( RPARAM_SCADA_OBSWORKERS ) )
        {
            numberOfWorkers = atoi ( 
				TA_Base_Core::RunParams::getInstance().get( RPARAM_SCADA_OBSWORKERS ).c_str() );
        }

		if (numberOfWorkers < 1)
		{
			numberOfWorkers = 1;
		}
		
        m_scadaProxyManager = new ScadaProxyManager( numberOfWorkers);

		//for compatibility purpose
		if(false == TA_Base_Core::RunParams::getInstance().isSet(PARAM_SCADA_PROXY_CONSTRUCT_WO_START) &&
		   0 != TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_STANDBY) )
		{
			this->setOperationMode(TA_Base_Core::Control);
		}
    }


    ScadaProxyFactory::~ScadaProxyFactory()
    {
        delete m_scadaProxyManager;
        m_scadaProxyManager = NULL;
    }
    
    /**
     * ScadaProxyFactory::getInstance()
     *
     * Returns a pointer to the single instance
     * of SingeltonQueue, creating if necessary.
     *
     * @return  a pointer to the ScadaProxyFactory
     *
     */
    ScadaProxyFactory& ScadaProxyFactory::getInstance()
    {
        TA_THREADGUARD( m_singletonLock );

        if ( m_singleton == NULL )
        {
            m_singleton = new ScadaProxyFactory();
        }

        // increment reference counter
        m_refCounter++;

        return *m_singleton;
    }   

    void ScadaProxyFactory::removeInstance()
    {
        TA_THREADGUARD( m_singletonLock );

        TA_ASSERT(m_refCounter > 0, "m_refCounter <= 0");

        // decrement reference counter
        --m_refCounter;

        // if not referenced anywhere
        if ( 0 == m_refCounter )
        {
            if ( m_singleton != NULL )
            {
                delete m_singleton;
                m_singleton = NULL;
            }
        }
    }


    void ScadaProxyFactory::registerLocalEntities(std::string agentName, const std::map<unsigned long,IEntity*>* entityMap)
    {
        m_scadaProxyManager->registerLocalEntities( agentName, entityMap );
    }
    void ScadaProxyFactory::registerEntityData(TA_Base_Core::IEntityDataPtr EntityData, const std::string& entityDataType)
    {
        m_scadaProxyManager->registerEntityData( EntityData, entityDataType );
    }
    

    void ScadaProxyFactory::createDataPointProxy(unsigned long entityKey, 
		                                         IEntityUpdateEventProcessor& observer,
												 DataPointProxySmartPtr & dataPointProxySmartPtr)
    {
        TA_Base_Core::DataPointEntityDataPtr nullData;
		m_scadaProxyManager->createDataPointProxy(entityKey, nullData, observer, dataPointProxySmartPtr);
    }

    void ScadaProxyFactory::createDataPointProxy(TA_Base_Core::DataPointEntityDataPtr entityData, 
		IEntityUpdateEventProcessor& observer,
		DataPointProxySmartPtr & dataPointProxySmartPtr)
    {
        TA_ASSERT(entityData.get() != NULL, "input DataPointEntityData is NULL");

        m_scadaProxyManager->createDataPointProxy(entityData->getKey(), entityData, observer, dataPointProxySmartPtr);
    }
    

    void ScadaProxyFactory::createDataNodeProxy( unsigned long entityKey,
                                                                  IEntityUpdateEventProcessor& observer,
																  DataNodeProxySmartPtr & dataNodeProxySmartPtr,
                                                                  bool activateSummaries )
    {
        TA_Base_Core::DataNodeEntityDataPtr nullData;
        m_scadaProxyManager->createDataNodeProxy(entityKey, nullData, observer, dataNodeProxySmartPtr);
    }

    void ScadaProxyFactory::createDataNodeProxy( TA_Base_Core::DataNodeEntityDataPtr entityData,
        IEntityUpdateEventProcessor& observer,
		DataNodeProxySmartPtr & dataNodeProxySmartPtr, 
        bool activateSummaries )
    {
        TA_ASSERT(entityData.get() != NULL, "input DataNodeEntityData is NULL");
        
        m_scadaProxyManager->createDataNodeProxy(entityData->getKey(), entityData, observer, dataNodeProxySmartPtr);
    }
    
    void ScadaProxyFactory::setProxiesToMonitorMode()
    {
        //do nothing
        this->setOperationMode(TA_Base_Core::Monitor);
    }


    void ScadaProxyFactory::setProxiesToControlMode()
    {
        //do nothing
        this->setOperationMode(TA_Base_Core::Control);
    }

	void ScadaProxyFactory::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		if(m_currentMode == mode)
		{
			return;
		}
		
		switch(mode)
		{
			case TA_Base_Core::Control:
			case TA_Base_Core::Monitor:
				m_scadaProxyManager->startProcess();
				break;
			case TA_Base_Core::Standby:
				m_scadaProxyManager->stopProcess();
				break;
			default:
				//todo->log
				break;
		}
		m_currentMode = mode;
	}
	
	
	void ScadaProxyFactory::stopProcessing()
	{
		if (NULL != m_scadaProxyManager)
		{
			m_scadaProxyManager->stopProcess();
		}
	}
};

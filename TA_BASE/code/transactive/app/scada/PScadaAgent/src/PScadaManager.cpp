/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/PScadaAgent/src/PScadaManager.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The PScadaManager object is responsible for managing PScadaUnits' running mode 
  * And processing PScadaUnit' poll/write data
  *
  */

#include "app/scada/PScadaAgent/src/PScadaManager.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{
    PScadaManager::PScadaManager()
        :
        m_agentAssetName(""),
        m_stationName(""),
        m_pScadaUnit(NULL)
    {
		FUNCTION_ENTRY("PScadaManager::PScadaManager");
		m_qualityStatus.clear();
		FUNCTION_EXIT("PScadaManager::PScadaManager");
   }
    

    PScadaManager::~PScadaManager()
    {
		FUNCTION_ENTRY("PScadaManager::~PScadaManager");
		FUNCTION_EXIT("PScadaManager::~PScadaManager");
   }
        

    TA_Base_Bus::IEntity* PScadaManager::createPScadaUnitEntity ( TA_Base_Core::IEntityDataPtr EntityData )
    {
		FUNCTION_ENTRY("createPScadaUnitEntity");
       // cast the EntityData to RTUEntityData
        TA_Base_Core::RTUEntityDataPtr pscadaEntityData = boost::dynamic_pointer_cast<TA_Base_Core::RTUEntityData>(EntityData);
        
        TA_ASSERT( NULL != pscadaEntityData.get(), "pscadaEntityData is NULL in createPScadaUnitEntity" );
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating PScadaUnit entity for [%s]...", pscadaEntityData->getName().c_str());
        // create PScadaUnit entity
		PScadaUnit* pScadaUnit;
		if (m_pScadaUnit == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Start to Create a new PScadaUnit");
			pScadaUnit = new PScadaUnit(*this, pscadaEntityData);

			TA_Base_Bus::IEntity* pEntity = pScadaUnit;
			pEntity->start();

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Create PScadaUnit[%p] for [%s]", pScadaUnit,pScadaUnit->getName().c_str());
			m_pScadaUnit = pScadaUnit;
			//TD10586
			m_pscadaOutputWorker.addPScadaUnit(*pScadaUnit);
			//jinmin++
			m_pscadaPollingWorker.addPScadaUnit(*pScadaUnit);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Add PScadaUnit [%s] finished", pScadaUnit->getName().c_str());
		}
		else
		{
			m_pScadaUnit->addLink(pscadaEntityData);
			pScadaUnit = m_pScadaUnit;
		}

		FUNCTION_EXIT("createPScadaUnitEntity");

        return pScadaUnit;
    }

    void PScadaManager::setAgentAssetName(std::string& assetName)
    {
		FUNCTION_ENTRY("setAgentAssetName");
        m_agentAssetName = assetName;
		FUNCTION_EXIT("setAgentAssetName");
   }

    std::string& PScadaManager::getAgentAssetName()
    {
		FUNCTION_ENTRY("getAgentAssetName");
		FUNCTION_EXIT("getAgentAssetName");
       return m_agentAssetName;
    }

    void PScadaManager::setStationName(std::string& stationName)
    {
		FUNCTION_ENTRY("setStationName");
		m_stationName = stationName;
		FUNCTION_EXIT("setStationName");
    }

    std::string& PScadaManager::getStationName()
    {
		FUNCTION_ENTRY("getStationName");
		FUNCTION_EXIT("getStationName");
       return m_stationName;
    }
    
    bool PScadaManager::updatePScadaUnitConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent)
    {
		FUNCTION_ENTRY("updatePScadaConfiguration");
        if (updateEvent.getKey() == m_pScadaUnit->getPKey())
        {
			if (m_pScadaUnit != NULL)
			{
				m_pScadaUnit->updatePScadaUnitConfiguration(updateEvent);
			}

            // return true to indicate no need to go further processing
            return true;
        }

		FUNCTION_EXIT("updatePScadaConfiguration");
       // not PScadaUnit configuration update
        return false;
    }

    bool PScadaManager::addDataPoint(TA_Base_Bus::DataPoint* dp)
    {
		FUNCTION_ENTRY("addDataPoint");
        TA_ASSERT( NULL != dp, "datapoint is NULL" );

        unsigned long i = 0;
        bool isAdded = false;

        //Add output data point to RTU to be processed by StationSystemOutputWirter
        if (dp->isWriteable())
        {
			if (m_pScadaUnit != NULL)
			{
				// output data points are in both rtus, so ingore the return value
				m_pScadaUnit->addDataPoint(dp);
			}

			FUNCTION_EXIT("addDataPoint");
            return true;
        }
        else
        {
			if (m_pScadaUnit != NULL)
			{
			   isAdded = m_pScadaUnit->addDataPoint(dp);
			}
			FUNCTION_EXIT("addDataPoint");
            return isAdded;
        }
    }
        
    bool PScadaManager::removeDataPoint(TA_Base_Bus::DataPoint* dp)
    {
		FUNCTION_ENTRY("removeDataPoint");
        TA_ASSERT( NULL != dp, "datapoint is NULL" );

        unsigned long i = 0;

        //remove data point to PScadaUnit to be processed by StationSystemOutputWirter
		if (m_pScadaUnit != NULL)
		{
			m_pScadaUnit->removeDataPoint(dp);
		}
		FUNCTION_EXIT("removeDataPoint");
        return true;
    }
        
    void PScadaManager::setToControl()
    {
		FUNCTION_ENTRY("setToControl");
		if (m_pScadaUnit != NULL)
		{
			m_pScadaUnit->setToControlMode();
			m_pscadaOutputWorker.start();
			m_pscadaPollingWorker.setInServiceState(true);
		}
		FUNCTION_EXIT("setToControl");
   }

    void PScadaManager::setToMonitor()
    {
		FUNCTION_ENTRY("setToMonitor");
		if (m_pScadaUnit != NULL)
		{
			m_pScadaUnit->setToMonitorMode();
			// before stop output thread, need to clear unsent write requests
			m_pscadaOutputWorker.clearWriteQueue();
			m_pscadaOutputWorker.terminateAndWait();
			m_pscadaPollingWorker.setInServiceState(false);
		}
		FUNCTION_EXIT("setToMonitor");

    }

	void PScadaManager::stopProcess()
	{
		FUNCTION_ENTRY("stopProcess");
		if (m_pScadaUnit != NULL)
		{
			m_pScadaUnit->stop();
		}
		m_pscadaOutputWorker.clearWriteQueue();
		m_pscadaOutputWorker.terminateAndWait();
		m_pscadaPollingWorker.setInServiceState(false);
		FUNCTION_EXIT("stopProcess");
	}  
}

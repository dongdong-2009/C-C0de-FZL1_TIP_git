/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/PScadaAgent/src/PScadaManager.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The RTUManager object is responsible for managing PScadaUnit' running mode 
  * And processing PScadaUnit' poll/write data
  *
  */

#if !defined(AFX_PSCADAMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
#define AFX_PSCADAMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "app/scada/PScadaAgent/src/PScadaUnit.h"
#include "app/scada/PScadaAgent/src/PScadaOutputWorker.h"
#include "app/scada/PScadaAgent/src/PScadaPollingWorker.h"



namespace TA_Base_Bus
{
    class IEntity;
    class DataPoint;
};

namespace TA_Base_Core
{
    class IEntityData;
    class ConfigUpdateDetails;
};

namespace TA_IRS_App
{
    class PScadaUnit;
    
    class PScadaManager
    {
    public:
        PScadaManager();
        virtual ~PScadaManager();
        
        // for PScadaAgent
        TA_Base_Bus::IEntity* createPScadaUnitEntity(TA_Base_Core::IEntityDataPtr EntityData);
        bool updatePScadaUnitConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent);
        bool addDataPoint(TA_Base_Bus::DataPoint* dp);
        bool removeDataPoint(TA_Base_Bus::DataPoint* dp);

        // response to agent running mode
        void setToControl();
        void setToMonitor();
		void stopProcess();

        void setAgentAssetName(std::string& assetName);
        std::string& getAgentAssetName();
        void setStationName(std::string& stationName);
        std::string& getStationName();
            
    private:
        // internal utility functions

    private:
        // global parameters
        std::string					m_agentAssetName;
        std::string					m_stationName;
		std::vector<TA_Base_Bus::EQualityStatus> m_qualityStatus;

        // an event processor for datapoint write request
        PScadaOutputWorker     m_pscadaOutputWorker;

		// a plling data thread
		PScadaPollingWorker	   m_pscadaPollingWorker;

   
        // enhence to unlimited number of rtus
        PScadaUnit*            m_pScadaUnit;
//        TA_Base_Core::ReEntrantThreadLockable   m_dpProcessorLock;
    };
}

#endif // !defined(AFX_PSCADAMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)

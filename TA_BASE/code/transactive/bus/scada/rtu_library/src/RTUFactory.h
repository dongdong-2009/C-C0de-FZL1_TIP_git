/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTUFactory.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2017/02/13 14:56:06 $
  * Last modified by : $Author: Dhanshri $
  *
  *
  * Template for a singleton queue object. Example 
  * implementations are DataPointWriteQueue and 
  * DataPointAlarmQueue.
  *
  *
  */

#ifndef RTUFACTORY_H
#define RTUFACTORY_H

#include <string>
#include <map>
#include <vector>

#include "boost/shared_ptr.hpp"
#include "core/threads/src/LFThreadPoolManager.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
//#include "core/naming/src/INamedObject.h"
//#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
//#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_Base_Core
{
    class ConfigUpdateDetails;
	class IThreadPoolExecuteItem;
}

namespace TA_Base_Bus
{
    // Forward declarations
	class BaseRTUManager;
    class IEntity;
    class DataPoint;

    class RTUFactory
    {
    public:
        /**
         * getInstance()
         *
         * Returns a pointer to the single instance
         * of RTUFactory, creating if necessary.
         *
         * @return  a pointer to a RTUFactory
         *
         */
         static RTUFactory& getInstance();


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
         * createRTUWorkers
         *
         * create a new monitored data node proxy and obtain a smart pointer to it.  Once the smart
         * pointer is destroyed the proxy will be automatically released from the library.
         *
         */
        void	createRTUWorkers();
		void	createRTUEntity(TA_Base_Core::IEntityDataPtr EntityData);
		void	addStationEntities(TA_Base_Core::IEntityDataPtr EntityData);
		void	addDataPointList(std::map < unsigned long, TA_Base_Bus::DataPoint *> &dataPointList);
		void	addDataPoint(DataPoint * dp);

        // set running parameter
        void	setAgentAssetName(std::string& assetName);
        void	setStationName(std::string& stationName);
        void	setIsRTUSwitchingDisabled(bool isDisabled);
        void	setAnalogueDpUpdateInterval(int analogueDpUpdateInterval);

        void	setRTUToMonitorMode();
        void 	setRTUToControlMode();
		void	updateDataPointConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent, std::map < unsigned long, TA_Base_Bus::DataPoint *> &dataPointList);
		void	updateRTUConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent);

	protected:

        RTUFactory();
        virtual ~RTUFactory();

        // Singleton
        static RTUFactory *								m_singleton;
        static unsigned int                             m_refCounter;
        static TA_Base_Core::ReEntrantThreadLockable    m_singletonLock;

        typedef std::map<int, BaseRTUManager*> RTUManagerMap;
        RTUManagerMap									m_rtuManagers;
		// Make member variable m_rtuManagers thread-safe as multiple threads create agent entities.e.g. RTUEntity
        TA_Base_Core::ReEntrantThreadLockable   m_RTUManagerLock;

		// Map item: <Station EK, vec[StationSystemEntityData]>
		std::map< unsigned long, std::vector<TA_Base_Core::IEntityDataPtr> > m_LocalStationSystemEntityMap;
		TA_Base_Core::ReEntrantThreadLockable   m_StnSysLock;

		TA_Base_Core::EOperationMode					m_currentMode;
    };

	class CreateDPWorkItem : public TA_Base_Core::IThreadPoolExecuteItem
	{
	public:
		CreateDPWorkItem(RTUFactory& agent, DataPoint* dp, TA_Base_Core::WorkItemExeSync* workSync);
		void executeWorkItem();
	private:
		RTUFactory&						m_parent;
		DataPoint*						m_dp;
		TA_Base_Core::WorkItemExeSync*  m_sync;
	};
}
#endif

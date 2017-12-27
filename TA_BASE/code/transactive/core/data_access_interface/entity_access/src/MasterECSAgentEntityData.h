/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MasterECSAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * MasterECSAgentEntityData implements the interface IMasterECSAgentEntityData to provide read-only access
  * to MasterECSAgent entity data.
  */

#ifndef MasterECSAgent_ENTITY_DATA_H
#define MasterECSAgent_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IMasterECSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class MasterECSAgentEntityData :  public virtual IMasterECSAgentEntityData,  public virtual EntityData
    {
	public:
		MasterECSAgentEntityData();
        MasterECSAgentEntityData ( unsigned long key );

        virtual ~MasterECSAgentEntityData();

		//
		// operations required by IEntityData
		//
   
        /**
         * clone
         *
         * Returns a pointer to a new RTUEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the object.
         */
        static IEntityData* clone ( unsigned long key);

        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        /**
         * getType
         *
         * Returns the entity type of this entity (i.e. "Camera").
         *
         * @return The entity type of this entity as a std::string
         */
        std::string getType();
		static std::string getStaticType();

        virtual unsigned long       getWorkerThreadsSleepTimeInMSecs();
        virtual unsigned long       getMaxModeExecutionTimeInSecs();
        virtual unsigned long       getSelectionTimeoutInSecs();
        virtual unsigned long       getNormalMasterMode();
        virtual std::string         getSpareAtsAgentName();

        virtual std::string         getMyZonesCongestionToken();
        virtual std::string         getMyZonesEmergencyToken();
        //virtual std::string         getMyZonesCongestionAppliedToken(); //Not required
        virtual std::string         getMyZonesEmergencyAppliedToken();
        virtual std::string         getMyZonesEvacuationSelectedToken();
        virtual std::string         getMyZonesAirflowSelectedToken();
        virtual std::string         getMyZonesEvacuationAppliedToken();
        virtual std::string         getMyZonesAirflowAppliedToken();
        virtual std::string         getMyModeStatusToken();
        virtual std::string         getMyModeInProgressToken();

        virtual std::string         getMasterModeLocksSqlToken();
        virtual std::string         getSelectedMasterModesSqlToken();
        virtual std::string         getSelectedOverridingStationModesSqlToken();
        virtual std::string         getLastAppliedMasterModesSqlToken();
        virtual std::string         getLastAppliedStationModesSqlToken();
        virtual std::string         getLocalModesSqlToken();

		virtual std::string			getAssetName();

    private:
        MasterECSAgentEntityData ( const MasterECSAgentEntityData & obj );
		MasterECSAgentEntityData & operator= ( const MasterECSAgentEntityData &);

        //
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string WORKER_THREADS_SLEEP_TIME_IN_MSECS;
        static const std::string MAX_MODE_EXECUTION_TIME_IN_SECS;
        static const std::string SELECTION_TIMEOUT_IN_SECS;
        static const std::string NORMAL_MASTER_MODE;
        static const std::string MY_ZONES_CONGESTION_TOKEN;
        static const std::string MY_ZONES_EMERGENCY_TOKEN;
        //static const std::string MY_ZONES_CONGESTION_APPLIED_TOKEN; // not required
        static const std::string MY_ZONES_EMERGENCY_APPLIED_TOKEN;
        static const std::string MY_ZONES_EVACUATION_SELECTED_TOKEN;
        static const std::string MY_ZONES_AIRFLOW_SELECTED_TOKEN;
        static const std::string MY_ZONES_EVACUATION_APPLIED_TOKEN;
        static const std::string MY_ZONES_AIRFLOW_APPLIED_TOKEN;
        static const std::string MY_MODE_STATUS_TOKEN;
        static const std::string MY_MODE_IN_PROGRESS_TOKEN;
        static const std::string MASTER_MODE_LOCKS_SQL_TOKEN;
        static const std::string SELECTED_MASTER_MODES_SQL_TOKEN;
        static const std::string SELECTED_OVERRIDING_STATION_MODES_TOKEN;
        static const std::string LAST_APPLIED_MASTER_MODES_SQL_TOKEN;
        static const std::string LAST_APPLIED_STATION_MODES_SQL_TOKEN;
        static const std::string LOCAL_MODES_SQL_TOKEN;
        static const std::string SPARE_ATS_AGENT_NAME;
		static const std::string ASSET_NAME;

        unsigned long       m_workerThreadsSleepTimeInMSecs;
        unsigned long       m_maxModeExecutionTimeInSecs;
        unsigned long       m_selectionTimeoutInSecs;
        unsigned long       m_normalMasterMode;
        std::string         m_spareAtsAgentName;

        std::string         m_myZonesCongestionToken;
        std::string         m_myZonesEmergencyToken;
        //std::string         m_myZonesCongestionAppliedToken; //Not required now
        std::string         m_myZonesEmergencyAppliedToken;
        std::string         m_myZonesEvacuationSelectedToken;
        std::string         m_myZonesAirflowSelectedToken;
        std::string         m_myZonesEvacuationAppliedToken;
        std::string         m_myZonesAirflowAppliedToken;
        std::string         m_myModeStatusToken;
        std::string         m_myModeInProgressToken;

        std::string         m_masterModeLocksSqlToken;
        std::string         m_selectedMasterModesSqlToken;
        std::string         m_selectedOverridingStationModesSqlToken;
        std::string         m_lastAppliedMasterModesSqlToken;
        std::string         m_lastAppliedStationModesSqlToken;
        std::string         m_localModesSqlToken;
		std::string			m_assetName;
        
    };
    
    typedef boost::shared_ptr<MasterECSAgentEntityData> MasterECSAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/StationECSAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * StationECSAgentEntityData implements the interface IStationECSAgentEntityData to provide read-only access
  * to StationECSAgent entity data.
  */

#ifndef StationECSAgent_ENTITY_DATA_H
#define StationECSAgent_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IStationECSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class StationECSAgentEntityData :  public virtual IStationECSAgentEntityData,  public virtual EntityData
    {
	public:

		StationECSAgentEntityData();
        StationECSAgentEntityData ( unsigned long key );

        virtual ~StationECSAgentEntityData();

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

		//
		// operations required by the IStationECSAgentEntityData
		//
        virtual unsigned long       getWorkerThreadsSleepTimeInMSecs();
        virtual unsigned long       getMaxModeExecutionTimeInSecs();
        virtual unsigned long       getNormalStationMode();
        virtual unsigned long       getIscsAlivePollTimeInSecs();
        virtual std::string         getSpareAtsAgentName();
        
        virtual std::string         getMyZonesCongestionToken();
        virtual std::string         getMyZonesEmergencyToken();
        virtual std::string         getMyMasterModeLockToken();
        virtual std::string         getMyMasterModeLockControlToken();
        virtual std::string         getMySelectedMasterModeToken();
        virtual std::string         getMySelectedOverridingStationModeToken();
        virtual std::string         getMyLastAppliedMasterModeToken();
        virtual std::string         getMyLastAppliedStationModeToken();

        virtual std::string         getModeStatusSqlToken();
        virtual std::string         getModeControlSqlToken();
        virtual std::string         getModeInProgressSqlToken();
        virtual std::string         getLocalModeSqlToken();
        virtual std::string         getIscsAliveSqlToken();  
        virtual std::string         getAutoModeSqlToken();
        virtual std::string         getAssetName();

    private:

        StationECSAgentEntityData ( const StationECSAgentEntityData & obj );
		StationECSAgentEntityData & operator= ( const StationECSAgentEntityData &);
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string WORKER_THREADS_SLEEP_TIME_IN_MSECS;
        static const std::string MAX_MODE_EXECUTION_TIME_IN_SECS;
        static const std::string NORMAL_STATION_MODE;
        static const std::string ISCS_ALIVE_POLL_TIME_IN_SECS;
        
        static const std::string MY_ZONES_CONGESTION_TOKEN;
        static const std::string MY_ZONES_EMERGENCY_TOKEN;
        static const std::string MY_MASTER_MODE_LOCK_TOKEN;
        static const std::string MY_MASTER_MODE_LOCK_CONTROL_TOKEN;
        static const std::string MY_SELECTED_MASTER_MODE_TOKEN;
        static const std::string MY_SELECTED_OVERRIDING_STATION_MODE_TOKEN;
        static const std::string MY_LAST_APPLIED_MASTER_MODE_TOKEN;
        static const std::string MY_LAST_APPLIED_STATION_MODE_TOKEN;

        static const std::string MODE_STATUS_SQL_TOKEN;
        static const std::string MODE_CONTROL_SQL_TOKEN;
        static const std::string MODE_IN_PROGRESS_SQL_TOKEN;
        static const std::string LOCAL_MODE_SQL_TOKEN;
        static const std::string ISCS_ALIVE_SQL_TOKEN;
        static const std::string AUTO_MODE_SQL_TOKEN;
        static const std::string SPARE_ATS_AGENT_NAME;

		static const std::string ASSET_NAME;
    };
    
    typedef boost::shared_ptr<StationECSAgentEntityData> StationECSAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif

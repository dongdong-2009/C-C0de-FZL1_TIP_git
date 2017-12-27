/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/PMSAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PMSAgentEntityData implements the interface IPMSAgentEntityData to provide read-only access
  * to PMSAgent entity data.
  */

#ifndef PMSAGENT_ENTITY_DATA_H
#define PMSAGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IPMSAgentEntityData.h"

namespace TA_Base_Core
{
    class PMSAgentEntityData : public virtual IPMSAgentEntityData, public virtual EntityData
    {
	public:

		PMSAgentEntityData();
        PMSAgentEntityData( unsigned long key );

        virtual ~PMSAgentEntityData();

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
		// operations required by the IPMSAgentEntityData
		//

        /**
         * getScanTimeMSecs
         *
         * Returns the scan time in msecs for PMSAgent entity
         *
         * @return The integer value presenting the scan time in msecs
         */
        virtual int getScanTimeMSecs();


        /**
         * getIsRTUSwitchingDisabled
         *
         * Returns true if the switching of RTU is to be disabled in PMSAgent,
		 * false ortherwise
         *
         */
        virtual bool getIsRTUSwitchingDisabled();


        /**
         * getStationName
         *
         * Returns the name of the Station the PMSAgent entity is configured to monitor
         *
         * @return The name of the Station the PMSAgent entity is configured to monitor
         */
        virtual const std::string getStationName();
        
        /**
         * getProxyDpHealthMonitoringPeriodSecs
         *
         * Returns the period in secs to check for the health status of the proxy dp
         *
         */
        virtual int getProxyDpHealthMonitoringPeriodSecs();

		virtual std::string getAssetName();

       //TD16169
		/**
         * getAnalogueDataPointUpdateInterval
         *
         * Returns the analogue datapoint update interval in msecs for PMSAgent entity
         *
         * @return The integer value presenting the analogue datapoint update interval
         */
        virtual int getAnalogueDataPointUpdateInterval();

    private:
		          
        PMSAgentEntityData( const PMSAgentEntityData & obj );
		PMSAgentEntityData & operator= ( const PMSAgentEntityData &);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

    public:
        static const std::string PMSAGENT_ENTITY_TYPE;
		static const std::string SCANTIME;
		static const std::string RTU_SWITCHING_DISABLED;
		static const std::string STATIONNAME;
		static const std::string PROXY_DP_HEALTH_MONITORING_PERIOD_SECS;
		static const std::string ASSET_NAME;
		static const std::string ANALOGUE_DP_UPDATE_INTERVAL;  //TD16169
    };
    
    typedef boost::shared_ptr<PMSAgentEntityData> PMSAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif

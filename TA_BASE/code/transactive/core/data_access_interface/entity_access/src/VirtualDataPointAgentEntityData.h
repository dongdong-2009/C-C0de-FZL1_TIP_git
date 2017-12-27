/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VirtualDataPointAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * VirtualDataPointAgentEntityData provide read-only access
  * to VirtualDataPointAgent entity data.
  */

#ifndef VIRTUAL_DATAPOINT_AGENT_ENTITY_DATA_H
#define VIRTUAL_DATAPOINT_AGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class VirtualDataPointAgentEntityData : public EntityData
    {
	public:

		VirtualDataPointAgentEntityData();
        VirtualDataPointAgentEntityData ( unsigned long key );

        virtual ~VirtualDataPointAgentEntityData();

		//
		// operations required by EntityData
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
		// operations required by the VirtualDataPointAgentEntityData
		//

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

    private:

        VirtualDataPointAgentEntityData ( const VirtualDataPointAgentEntityData & obj );
		VirtualDataPointAgentEntityData & operator= ( const VirtualDataPointAgentEntityData &);

    public:
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string VIRTUAL_DATAPOINT_AGENT_ENTITY_TYPE;
		static const std::string STATIONNAME;
		static const std::string PROXY_DP_HEALTH_MONITORING_PERIOD_SECS;
		static const std::string ASSET_NAME;

    };
    
    typedef boost::shared_ptr<VirtualDataPointAgentEntityData> VirtualDataPointAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif

/**
  * The source code in this file is the property of ST Electronics and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/PEnergyAgentEntityData.h $
  * @author Liu Yong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PEnergyAgentEntityData implements the interface IEntityData to provide read-only access
  * to PEnergyAgent entity data.
  */

#ifndef PENERGY_AGENT_ENTITY_DATA_H
#define PENERGY_AGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class PEnergyAgentEntityData : public virtual EntityData
    {
	public:

		PEnergyAgentEntityData();
        PEnergyAgentEntityData( unsigned long key );

        virtual ~PEnergyAgentEntityData();

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

        /**
         * getStationName
         *
         * Returns the name of the Station the PEnergyAgent entity is configured to monitor
         *
         * @return The name of the Station the PEnergyAgent entity is configured to monitor
         */
        virtual const std::string getStationName();
        
		virtual const std::string getAssetName();

    private:
		          
        PEnergyAgentEntityData( const PEnergyAgentEntityData & obj );
		PEnergyAgentEntityData & operator= ( const PEnergyAgentEntityData &);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

    public:
		static const std::string STATIONNAME;
		static const std::string ASSET_NAME;
    };
    
    typedef boost::shared_ptr<PEnergyAgentEntityData> PEnergyAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif

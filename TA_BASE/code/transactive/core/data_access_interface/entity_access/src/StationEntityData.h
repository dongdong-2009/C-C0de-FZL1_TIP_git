/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/StationEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * StationEntityData implements the interface IStationEntityData to provide read-only access
  * to Station entity data.
  */

#ifndef STATION_ENTITY_DATA_H
#define STATION_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IStationEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class StationEntityData : public EntityData
    {
	public:

		StationEntityData();
        StationEntityData ( unsigned long key );

        virtual ~StationEntityData();

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

    private:
		          
        StationEntityData ( const StationEntityData & obj ){};
		StationEntityData & operator= ( const StationEntityData &){};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
    };
    
    typedef boost::shared_ptr<StationEntityData> StationEntityDataPtr;
} //close namespace TA_Base_Core

#endif

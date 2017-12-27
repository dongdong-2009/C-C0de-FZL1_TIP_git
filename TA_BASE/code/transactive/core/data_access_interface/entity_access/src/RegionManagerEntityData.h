#ifndef REGION_MANAGER_ENTITY_DATA
#define REGION_MANAGER_ENTITY_DATA

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RegionManagerEntityData.h $
  * @author Bart Golab
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface for Region Manager entity configuration access.
  */

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class RegionManagerEntityData : public EntityData
    {

    public:
        /**
         * RegionManagerEntityData()
         *
         * Constructs an empty RegionManagerEntityData object.
         * Only getType() will be able to be called.
         */
        RegionManagerEntityData();

        /**
         * RegionManagerEntityData()
         *
         * Constructs a new RegionManagerEntityData object with the specified key.
         *
         * @param key The key to a RegionManager entity in the database.
         */
        RegionManagerEntityData( unsigned long key );
        virtual ~RegionManagerEntityData();

        /**
         * invalidate()
         *
         * Marks the data contained by this entity as invalid. A subsequent call to one  
         * of the getter methods will cause all the data to be reloaded from the database.
         */
        virtual void invalidate();

        /**
         * getType()
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type should not be 
         * loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

        /**
         * clone()
         *
         * Returns a pointer to a new RegionManagerEntityData object that has been initialised 
         * with the specified key.
         *
         * @param key The key with which to construct the RegionManagerEntityData object.
         */
        static IEntityData* clone(unsigned long key);
    };
    
    typedef boost::shared_ptr<RegionManagerEntityData> RegionManagerEntityDataPtr;
}

#endif // REGION_MANAGER_ENTITY_DATA

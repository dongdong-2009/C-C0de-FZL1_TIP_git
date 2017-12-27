#ifndef RIGHTSAGENTENTITYDATA_H_INCLUDED
#define RIGHTSAGENTENTITYDATA_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RightsAgentEntityData.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  *
  */

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class RightsAgentEntityData : public EntityData
    {

    public:
        RightsAgentEntityData() {}
        RightsAgentEntityData(unsigned long key);

        virtual ~RightsAgentEntityData();

    public:

        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

        /**
         * clone
         *
         * Returns a pointer to a new RightsAgentEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the RightsAgentEntityData object.
         */
        static IEntityData* clone(unsigned long key);
        
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

		virtual std::string getAssetName();

	private:

        RightsAgentEntityData( const RightsAgentEntityData&);            
		RightsAgentEntityData& operator=(const RightsAgentEntityData&);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string ASSET_NAME;
    };
    
    typedef boost::shared_ptr<RightsAgentEntityData> RightsAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif

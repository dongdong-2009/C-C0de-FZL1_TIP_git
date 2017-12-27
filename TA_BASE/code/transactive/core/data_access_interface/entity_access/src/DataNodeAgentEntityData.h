/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DataNodeAgentEntityData.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * DataNodeAgentEntityData provide read-only access to DataNodeAgent entity data.
  */
  
#ifndef DATANODE_AGENT_ENTITY_DATA_H
#define DATANODE_AGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class DataNodeAgentEntityData : public EntityData
    {
	public:

		DataNodeAgentEntityData();
        DataNodeAgentEntityData ( unsigned long key );

        virtual ~DataNodeAgentEntityData();

		virtual std::string getAssetName();

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


    private:

        DataNodeAgentEntityData ( const DataNodeAgentEntityData & obj );
		DataNodeAgentEntityData & operator= ( const DataNodeAgentEntityData &);

    private:
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string ASSET_NAME;

    };
    
    typedef boost::shared_ptr<DataNodeAgentEntityData> DataNodeAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif //DATANODE_AGENT_ENTITY_DATA_H

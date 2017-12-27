/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TrainAgentEntityData.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * TrainAgentEntityData provide read-only access to DataNodeAgent entity data.
  */
  
#ifndef TRAIN_AGENT_ENTITY_DATA_H
#define TRAIN_AGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    /**
     * @author adamr
     * @version 1.0
     * @created 15-Jan-2008 12:21:05 PM
     */
    class TrainAgentEntityData : public TA_Base_Core::EntityData
    {

    public:

        /**
         * Destructor
         */
        TrainAgentEntityData();


        /**
         * 
         * @param key
         */
        TrainAgentEntityData(unsigned long key);


        /**
         * Destructor
         */
        virtual ~TrainAgentEntityData();


        /**
         * clone  Returns a pointer to a new RTUEntityData object that has been initialised with the specified key. 
         * 
         * @param key    The key with which to construct the object.
         */
        static IEntityData* clone(unsigned long key);


        /**
         * invalidate  Mark the data contained by this entity as invalid. The next call to get...() following a call to invalidate() will cause all the data to be reloaded from the database.
         */
        void invalidate();


        /**
         * getType  Returns the entity type of this entity (i.e. "Camera"). 
         * @return The entity type of this entity as a std::string
         */
        std::string getType();


        static std::string getStaticType();


        virtual std::string getAssetName();
        
        /**
		 * Gets the agent origin - the origin value to put in the train protocol messages
		 */
		virtual unsigned long getAgentOrign();

    private:

        /**
         * 
         * @param obj
         */
        TrainAgentEntityData(const TrainAgentEntityData& obj){}


        /**
         * 
         * @param prm1
         */
        TrainAgentEntityData & operator=(const TrainAgentEntityData& prm1){return  *this;}
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string ASSET_NAME;
		static const std::string AgentOrigin;

    };
    
    typedef boost::shared_ptr<TrainAgentEntityData> TrainAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif //TRAIN_AGENT_ENTITY_DATA_H


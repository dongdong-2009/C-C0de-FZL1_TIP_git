/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/PlanAgentData.h $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface.
  */

#ifndef PLAN_AGENT_DATA_H_INCLUDED
#define PLAN_AGENT_DATA_H_INCLUDED

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class PlanAgentData : public EntityData
    {

    public:
        PlanAgentData() {}
        PlanAgentData(unsigned long key);

        virtual ~PlanAgentData();

    public:

        /**
         * getActivePlanExpiryTime
         *
         * Returns the expiry time for active plan instances that have finished execution.
         *
         * @return Active plan expiry time in minutes.
         */
        int getActivePlanExpiryTime();

        /**
         * getActivePlanThreadPoolSize
         *
         * Returns the number of threads used to service plan execution requests.
         * This is effectively the number of plan instances that can be running at 
         * any one time. This is an optional parameter, if not present the default
         * value is returned.
         *
         * @return Executing plan thread pool size. Default = 100
         */
        unsigned long getActivePlanThreadPoolSize();

		   /**
         * getActivePlanPausedSleepTime
         *
         * Returns the number of miliseconds a paused-plan thread  
         * is configured to sleep before checking for requests
			* specificaly introduced to prevent CPU starvation 
			* when a large number of paused (> 70) plans are present
			* in the system
			*
			* @return sleep time for paused plans , Default = 10000 miliseconds
         */	
		  unsigned long getActivePlanPausedSleepTime ();

		  virtual std::string getAssetName();
        
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
         * Returns a pointer to a new PlanAgentData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the PlanAgentData object.
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

        static const std::string ACTIVEPLAN_EXPIRYTIME;
        static const std::string ACTIVEPLAN_THREAD_POOL_SIZE;
		static const std::string ACTIVEPLAN_PAUSED_SLEEP_TIME;
		static const std::string ASSET_NAME;
		  
	private:

        PlanAgentData( const PlanAgentData&);            
		PlanAgentData& operator=(const PlanAgentData&);

        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;

    };
    
    typedef boost::shared_ptr<PlanAgentData> PlanAgentDataPtr;
} //close namespace TA_Base_Core

#endif // PLAN_AGENT_DATA_H_INCLUDED

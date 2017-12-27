/**
  * The source code in this file is the property of 
  * Combuilder PTE LTD and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/EventAgentEntityData.h $
  * @author Noel R. Tajanlangit
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/24 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Class definition for EventAgentEntityData
  */

#ifndef __EVENT_AGENT_ENTITY_DATA_H_INCLUDED__
#define __EVENT_AGENT_ENTITY_DATA_H_INCLUDED__

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include <string>


namespace TA_Base_Core
{
	class EventAgentEntityData : public EntityData
	{
	public: // Methods
		/** 
		 * Default Constructor
		 */
		EventAgentEntityData();
		
		/** 
		 * Constructor
		 *
		 * @param ulKey unique identifier for this entity
		 */
		EventAgentEntityData(unsigned long ulkey);

		/** 
		 * Destructor
		 */
		virtual ~EventAgentEntityData();

		/** 
		 * invalidate
		 *
		 * Marks data as invalid so reload could trigger on the next request of data
		 */
		virtual void invalidate();

		/** 
		 * getType
		 *
		 * Returns the type of this entity
		 */
		virtual std::string getType();

		/** 
		 * getStaticType
		 *
		 * Static function which returns the type of this entity
		 */
		static std::string getStaticType();

		/** 
		 * clone
		 *
		 * duplicates the entity with the provide ID
		 *
		 * @param entity key to clone
		 */
		static IEntityData* clone(unsigned long key);

		/** 
		 * getAssetName
		 *
		 * retrieves the asset name for this entity
		 */
		virtual std::string getAssetName();

		/** 
		 * getDbRefreshInterval
		 *
		 * retrieves the database refresh interval configuration for this entity
		 */
		virtual unsigned long getDbRefreshInterval();

		/** 
		 * getAssetName
		 *
		 * retrieves the submit interval configuration for this entity
		 */
		virtual unsigned long getEVSubmitInterval();

		/** 
		 * getMaxCacheSize
		 *
		 * retrieves the maximum cache size configuration for this entity
		 */
		virtual unsigned long getMaxCacheSize();

		/** 
		 * getAgentRefreshInterval
		 *
		 * retrieves the agent synchronization refresh interval configuration for this entity
		 */
		virtual unsigned long getAgentRefreshInterval();

		/** 
		 * getPropagationBatchSize
		 *
		 * retrieves the propagation batch size configuration for this entity
		 */
		virtual unsigned long getPropagationBatchSize();

	private: // Methods
	private: // Variables
		static const std::string ENTITY_TYPE;
		static const std::string ASSET_NAME;
		static const std::string DATABASE_REFRESH_INTERVAL;
		static const std::string EVENT_SUBMIT_INTERVAL;
		static const std::string MAX_CACHE_SIZE;
		static const std::string AGENT_REFRESH_INTERVAL;
		static const std::string PROPAGATE_BATCH_SIZE;
	};	

	typedef boost::shared_ptr<EventAgentEntityData> EventAgentEntityDataPtr;
}

#endif //__EVENT_AGENT_ENTITY_DATA_H_INCLUDED__


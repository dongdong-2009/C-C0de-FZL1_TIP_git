#ifndef DUTYAGENTENTITYDATA_H_INCLUDED
#define DUTYAGENTENTITYDATA_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DutyAgentEntityData.h $
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
    class DutyAgentEntityData : public EntityData
    {

    public:
        DutyAgentEntityData() {}
        DutyAgentEntityData(unsigned long key);

        virtual ~DutyAgentEntityData();

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
         * getUncontrolledTimeout
         *
		 * Returns the number of seconds that a Region or Subsystem can remain uncontrolled
		 * without causing an alarm to be raised by the Duty Agent.
         */
		unsigned long getUncontrolledTimeout();

		/**
         * getPolicyType
         *
		 * Returns the type of policy the Duty Agent should use. At the time of writing
		 * there were two valid policies: "Rail" and "Road"
         */
		std::string getPolicyType();

		std::string getAssetName();

        /**
         * clone
         *
         * Returns a pointer to a new DutyAgentEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the DutyAgentEntityData object.
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

    protected:


	private:

        DutyAgentEntityData( const DutyAgentEntityData&);            
		DutyAgentEntityData& operator=(const DutyAgentEntityData&);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

    public:
		static const std::string UNCONTROLLED_TIMEOUT_NAME;
		static const std::string POLICY_TYPE_NAME;
		static const std::string RAIL_POLICY_VALUE;
		static const std::string ROAD_POLICY_VALUE;
		static const std::string ASSET_NAME;
    };
    
    typedef boost::shared_ptr<DutyAgentEntityData> DutyAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(Camera_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)

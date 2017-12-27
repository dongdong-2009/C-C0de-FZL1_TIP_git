#ifndef DUTY_MANAGER_ENTITY_DATA
#define DUTY_MANAGER_ENTITY_DATA

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DutyManagerEntityData.h $
  * @author Bart Golab
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface for Duty Manager entity configuration access.
  */

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class DutyManagerEntityData : public EntityData
    {

    public:
        /**
         * DutyManagerEntityData()
         *
         * Constructs an empty DutyManagerEntityData object.
         * Only getType() will be able to be called.
         */
        DutyManagerEntityData();

        /**
         * DutyManagerEntityData()
         *
         * Constructs a new DutyManagerEntityData object with the specified key.
         *
         * @param key The key to a Duty Manager entity in the database.
         */
        DutyManagerEntityData( unsigned long key );
        virtual ~DutyManagerEntityData();

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
         * Returns a pointer to a new DutyManagerEntityData object that has been initialised 
         * with the specified key.
         *
         * @param key The key with which to construct the DutyManagerEntityData object.
         */
        static IEntityData* clone(unsigned long key);


        /**
         * useLocationLabel()
         *
         * Returns true if the Duty Manager is to refer to regions as "locations".
         * Reloads parameter data from the database if necessary.
         *
         * @exception   See reload().
         */
        bool useLocationLabel();


        /**
          * getNormalDutyColour
          *
          * Returns the data stored in the 'NormalDutyColour' parameter.
          *
          * @return std::string The colour to use for subsystems in normal state.
          */
        std::string getNormalDutyColour();


        /**
          * getDelegatedDutyColour
          *
          * Returns the data stored in the 'DelegatedDutyColour' parameter.
          *
          * @return std::string The colour to use for the subsystems in delegated state.
          */
        std::string getDelegatedDutyColour();


        /**
          * getDegradedDutyColour
          *
          * Returns the data stored in the 'DegradedDutyColour' parameter.
          *
          * @return std::string The colour to use for the subsystems in degraded state.
          */
        std::string getDegradedDutyColour();


        /**
          * getUnallocatedSubsystemColour
          *
          * Returns the data stored in the 'UnallocatedSubsystemColour' parameter.
          *
          * @return std::string The colour to use for the subsystems that aren't present in a region.
          */
        std::string getUnallocatedSubsystemColour();

        /**
          * getCorbaTimeoutSeconds
          *
          * Returns the data stored in the 'CorbaTimeoutSeconds' parameter.
          *
          * @return unsigned long for corbaTimeout of dutyManager exclude AccessControlPopupTimeout.
          */
        unsigned long getCorbaTimeoutSeconds();

    protected:

    private:
        std::string m_normalDutyColour;
        std::string m_delegatedDutyColour;
        std::string m_degradedDutyColour;
        std::string m_unallocatedSubsystemColour;
        unsigned long m_corbaTimeoutSeconds;

	public:
		static const unsigned long USE_DEFAULT_TIMEOUT;
    };
    
    typedef boost::shared_ptr<DutyManagerEntityData> DutyManagerEntityDataPtr;
}

#endif // REGION_MANAGER_ENTITY_DATA

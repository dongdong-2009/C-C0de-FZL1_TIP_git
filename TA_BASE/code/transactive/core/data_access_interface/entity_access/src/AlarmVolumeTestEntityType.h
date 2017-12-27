/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AlarmVolumeTestEntityType.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implementation for Alarm Volume Testing. Entities of this type contian no useful data.
  */

#if !defined(ALARMVOLUMEENTITYTYPE_H)
#define ALARMVOLUMEENTITYTYPE_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class AlarmVolumeTestEntityType : public EntityData
    {

    public:
        /**
         * AlarmVolumeTestEntityType (constructor)
         *
         * Constructs a new AlarmVolumeTestEntityType object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        AlarmVolumeTestEntityType() {};

        /**
         * AlarmVolumeTestEntityType (constructor)
         *
         * Constructs a new AlarmVolumeTestEntityType object with the specified key.
         *
         * @param key The key to this AlarmVolumeTestEntityType in the database.
         */
        AlarmVolumeTestEntityType(unsigned long key);
        virtual ~AlarmVolumeTestEntityType();

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
        std::string getType();
		static std::string getStaticType();

        /**
         * clone
         *
         * Returns a pointer to a new AlarmVolumeTestEntityType object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the AlarmVolumeTestEntityType object.
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
        //
        // Disable copy constructor and assignment operator
        //
        AlarmVolumeTestEntityType( const AlarmVolumeTestEntityType& theAlarmVolumeTestEntityType );
        AlarmVolumeTestEntityType& operator=( const AlarmVolumeTestEntityType& theAlarmVolumeTestEntityType );

        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;
    };

    typedef boost::shared_ptr<AlarmVolumeTestEntityType> AlarmVolumeTestEntityTypePtr;
} //close namespace TA_Base_Core

#endif // !defined(ALARMVOLUMEENTITYTYPE_H)

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AlarmRaiserEntityData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *
  * This class implements the AlarmRaiserEntityData class.
  *
  */

#ifndef ALARM_RAISER_ENTITY_DATA
#define ALARM_RAISER_ENTITY_DATA

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"

namespace TA_Base_Core
{
    class EntityHelper;
    class AlarmRaiserEntityData : public EntityData
    {
        
    public:
        
       /**
        * AlarmRaiserEntityData (constructor)
        *
        * Constructs a new AlarmRaiserEntityData object that contains only the static values. This
        * includes type, so the getType() method can be called without any further action.
        */
        AlarmRaiserEntityData();
        
       /**
        * AlarmRaiserEntityType (constructor)
        *
        * Constructs a new AlarmRaiserEntityType object with the specified key.
        *
        * @param key The key to this AlarmRaiserEntityType in the database.
        */
        AlarmRaiserEntityData( unsigned long key );
        
        virtual ~AlarmRaiserEntityData();
        
       /**
        * invalidate
        *
        * Mark the data contained by this console as invalid. The next call to get...() 
        * following a call to invalidate() will cause all the data to be reloaded from
        * the database.
        */
        virtual void invalidate();
        
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
        * Returns a pointer to a new Console object that has been initialised with the 
        * specified key.
        *
        * @param key The key with which to construct the Console object.
        */
        static IEntityData* clone(unsigned long key);
        
       /**
        * getUserId
        *
        * Returns the User Id Parameter Value
        */
        virtual unsigned long getUserId();
        
       /**
        * getProfileId
        *
        * Returns the Profile Id Parameter Value
        */
        virtual unsigned long getProfileId();
        
       /**
        * getWorkStationId
        *
        * Returns the Work Station Id Parameter Value
        */
        virtual unsigned long getWorkStationId();
        
       /**
        * getPassword
        *
        * Returns the Password Parameter Value
        */
        virtual std::string getPassword();
        
    protected:
        
    private:
        //
        // Copy constructor and assignment operator are not used
        //
        AlarmRaiserEntityData( const AlarmRaiserEntityData& theAlarmRaiserEntityData) {};            
		AlarmRaiserEntityData& operator=(const AlarmRaiserEntityData &) {return *this;};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string USER_ID;
        static const std::string PROFILE_ID;
        static const std::string WORK_STATION_ID;
        static const std::string PASSWORD;
        bool m_isValidData;
        unsigned long m_userId;
        unsigned long m_profileId;
        unsigned long m_workStationId;
        std::string m_password;
    };
    
    typedef boost::shared_ptr<AlarmRaiserEntityData> AlarmRaiserEntityDataPtr;
}

#endif // ALARM_RAISER_ENTITY_DATA
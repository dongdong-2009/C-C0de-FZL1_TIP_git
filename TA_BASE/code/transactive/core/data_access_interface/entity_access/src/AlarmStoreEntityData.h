/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AlarmStoreEntityData.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * AlarmStoreEntityData is a concrete implementation of IAlarmStoreEntityData, which is in turn an implementation of
  * IEntityData. It proves read-only access to alarm store data, and a specific framework
  * for alarm store parameters.
  *
  */

#if !defined(Alarm_Store_Entity_Data_INCLUDED_)
#define Alarm_Store_Entity_Data_INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class AlarmStoreEntityData : public EntityData
    {

    public:
        /**
         * AlarmStoreEntityData (constructor)
         *
         * Constructs a new AlarmStoreEntityData object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        AlarmStoreEntityData() {};

        /**
         * AlarmStoreEntityData (constructor)
         *
         * Constructs a new AlarmStoreEntityData object with the specified key.
         *
         * @param key The key to this AlarmStoreEntity in the database.
         */
        AlarmStoreEntityData(unsigned long key);

        virtual ~AlarmStoreEntityData();

    public:

		//**********************
		// Entity Data Methods *
		//**********************

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
         * getType
         *
         * Returns the console type of this console (i.e. "Console").
         *
         * @return The console type of this console as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

        /**
         * invalidate
         *
         * Mark the data contained by this console as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    protected:


    private:
        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;

    private:
        //
        // Copy constructor and assignment operator are not used
        //
        AlarmStoreEntityData( const AlarmStoreEntityData& theStore) {};            
		AlarmStoreEntityData& operator=(const AlarmStoreEntityData &) {};

   };

    typedef boost::shared_ptr<AlarmStoreEntityData> AlarmStoreEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(Alarm_Store_Entity_Data_INCLUDED_)

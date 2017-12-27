/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MmsAlarmSubmitterEntityData.h $
  * @author C. DeWolfe
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface.
  */

#ifndef MMS_ALARM_SUB_ENTITY_DATA_H_INCLUDED
#define MMS_ALARM_SUB_ENTITY_DATA_H_INCLUDED

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{

    class  MmsAlarmSubmitterEntityData : public EntityData
    {

    public:

         MmsAlarmSubmitterEntityData() {}
         MmsAlarmSubmitterEntityData(unsigned long key);

        virtual ~MmsAlarmSubmitterEntityData();

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
         * Returns a pointer to a new  MmsAlarmSubmitterEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the  MmsAlarmSubmitterEntityData object.
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

	private:

         MmsAlarmSubmitterEntityData( const  MmsAlarmSubmitterEntityData&);            
		 MmsAlarmSubmitterEntityData& operator=(const  MmsAlarmSubmitterEntityData&);
        
         // 
         // The entity type is fixed for all instances of this class.
         // 
         static const std::string ENTITY_TYPE;

    };
    
    typedef boost::shared_ptr<MmsAlarmSubmitterEntityData> MmsAlarmSubmitterEntityDataPtr;
} //end namespace TA_Base_Core

#endif // MMS_ALARM_SUB_ENTITY_DATA_H_INCLUDED

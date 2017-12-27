/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MmsPeriodicEntityData.h $
  * @author C. DeWolfe
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface.
  */

#ifndef MMS_PERIODIC_ENTITY_DATA_H_INCLUDED
#define MMS_PERIODIC_ENTITY_DATA_H_INCLUDED

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class MmsPeriodicEntityData : public EntityData
    {

    public:

        MmsPeriodicEntityData() {}
        MmsPeriodicEntityData(unsigned long key);

        virtual ~MmsPeriodicEntityData();

    public:

        /**
         * getScanPeriodMinutes
         *
         * @return the value of the SCAN_PERIOD_MIN param. This parameter indicates
		 * how often we should check to see if any periodic mantenance messages
		 * are due to be sent
         */
		unsigned int getScanPeriodMinutes();

        /**
         * getBackLogCheckPeriodHours
         *
         * @return the value of the BACKLOG_CHECK_PERIOD_HOURS param. 
		 * This parameter indicates how far back in time we should check to 
		 * see if need to send any maintenance messages we have missed 
		 * while the MMS Agent was down
         */
		unsigned int getBackLogCheckPeriodHours();

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
         * Returns a pointer to a new MmsPeriodicEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the MmsPeriodicEntityData object.
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

        MmsPeriodicEntityData( const MmsPeriodicEntityData&);            
		MmsPeriodicEntityData& operator=(const MmsPeriodicEntityData&);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

    public:
		static const std::string SCAN_PERIOD_MIN;
		static const std::string BACKLOG_CHECK_PERIOD_HOURS;
    };
    
    typedef boost::shared_ptr<MmsPeriodicEntityData> MmsPeriodicEntityDataPtr;
} //end namespace TA_Base_Core

#endif // MMS_PERIODIC_ENTITY_DATA_H_INCLUDED

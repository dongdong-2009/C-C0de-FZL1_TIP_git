/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/PlanManagerEntityData.h $
  * @author Bart Golab
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface.
  */

#ifndef PLAN_MANAGER_ENTITY_DATA
#define PLAN_MANAGER_ENTITY_DATA

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class PlanManagerEntityData : public EntityData
    {

    public:
        /**
         * PlanManagerEntityData()
         *
         * Constructs an empty PlanManagerEntityData object.
         * Only getType() will be able to be called.
         */
        PlanManagerEntityData();

        /**
         * PlanManagerEntityData()
         *
         * Constructs a new PlanManagerEntityData object with the specified key.
         *
         * @param key The key to a PlanManager entity in the database.
         */
        PlanManagerEntityData( unsigned long key );
        virtual ~PlanManagerEntityData();


        /** 
          * getFlowchartDirectory
          *
          * Gets the flowchart root directory that the flowcharts will reside.
          *
          *
          * @return the flowchart root directory that the flowcharts will reside.
          */
        std::string getFlowchartDirectory();

        /** 
          * getReportDirectory
          *
          * Gets the report report template file path.
          *
          * @return the report template file path.
          */
        std::string getReportTemplate();

        /** 
          * getExportDirectory
          *
          * Gets the default export directory
          *
          * @return the default directory file path 
          */
		std::string getExportDirectory();

        /** 
          * getMaxCategoryCount
          *
          * Gets maximize count of category
          *
          * @return the maximize count of category.
          */
        std::string getMaxCategoryDepth();

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
         * Returns a pointer to a new PlanManagerEntityData object that has been initialised 
         * with the specified key.
         *
         * @param key The key with which to construct the PlanManagerEntityData object.
         */
        static IEntityData* clone(unsigned long key);


    protected:
        /**
         * reload
         *
         * (Re)loads the parameter data from the database.
         *
         * @exception DatabaseException If there is a database error.
         * @exception DataException If a parameter value cannot be converted to the required
         *            type
         */

    private:
        PlanManagerEntityData( const PlanManagerEntityData & obj );
		PlanManagerEntityData & operator= ( const PlanManagerEntityData &);
    };
    
    typedef boost::shared_ptr<PlanManagerEntityData> PlanManagerEntityDataPtr;
}

#endif // PLAN_MANAGER_ENTITY_DATA

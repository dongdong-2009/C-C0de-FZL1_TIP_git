/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/EquipmentStatusViewerEntityData.h $
  * @author Bart Golab
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface for the Equipment Status Viewer entity.
  */

#ifndef EQUIPMENT_STATUS_VIEWER_ENTITY_DATA
#define EQUIPMENT_STATUS_VIEWER_ENTITY_DATA

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class EquipmentStatusViewerEntityData : public EntityData
    {

    public:
        /**
         * EquipmentStatusViewerEntityData()
         *
         * Constructs an empty EquipmentStatusViewerEntityData object.
         * Only getType() will be able to be called.
         */
        EquipmentStatusViewerEntityData();

        /**
         * EquipmentStatusViewerEntityData()
         *
         * Constructs a new EquipmentStatusViewerEntityData object with the specified key.
         *
         * @param key The key to a EquipmentStatusViewer entity in the database.
         */
        EquipmentStatusViewerEntityData( unsigned long key );
        virtual ~EquipmentStatusViewerEntityData();


        /** 
          * getExportDirectory
          *
          * Gets the target location for exported report files.
          *
          * @return Report export directory path.
          */
        std::string getExportDirectory();


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
         * Returns a pointer to a new EquipmentStatusViewerEntityData object that has been initialised 
         * with the specified key.
         *
         * @param key The key with which to construct the EquipmentStatusViewerEntityData object.
         */
        static IEntityData* clone(unsigned long key);


    private:
        //
        // Disable copy constructor and assignment operator
        //
        EquipmentStatusViewerEntityData ( const EquipmentStatusViewerEntityData & obj ){};
		EquipmentStatusViewerEntityData & operator= ( const EquipmentStatusViewerEntityData &){};

        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;
        static const std::string EXPORT_DIRECTORY;

    };
    
    typedef boost::shared_ptr<EquipmentStatusViewerEntityData> EquipmentStatusViewerEntityDataPtr;
}

#endif // EQUIPMENT_STATUS_VIEWER_ENTITY_DATA

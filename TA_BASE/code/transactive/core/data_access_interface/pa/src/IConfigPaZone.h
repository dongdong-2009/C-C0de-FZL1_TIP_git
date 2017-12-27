/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/IConfigPaZone.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IConfigPaZone - This is the interface supported by objects that allow configuration of
  * PaZones. It provides methods for retrieving, settings, and writing data for an PaZone object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigPaZone_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigPaZone_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/pa/src/IPaZone.h"

namespace TA_Base_Core
{

    class IConfigPaZone : public virtual TA_Base_Core::IConfigItem, public virtual IPaZone
    {

    public:

        virtual ~IConfigPaZone() {};

        virtual void setLocationKey(const unsigned long locationKey) =0;
        virtual void setId(const unsigned long id) =0;
        virtual void setLabel(const std::string& label) =0;
        // No longer allowed to set the area (this data fetched from entity table)
        //virtual void setArea(const std::string& area) =0;
        virtual void setStnEntityName(const std::string& name) =0;
        virtual void setOccEntityName(const std::string& name) =0;               
        //virtual void setStatusEntityKey(const unsigned long statusEntityKey) =0;
        //virtual void setAssociatedEquipmentEntity( const std::string& entityName, bool isAssociated) =0;

        /**
         * getDateCreated
         *
         * Returns the date created for this PaZone
         *
         * @return The date created for this PaZone as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this PaZone.
         *
         * @return The date modified for this PaZone as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;

        
        
        /**
         * deleteThisObject
         *
         * Removes this PaZone from the database. 
         * The calling application MUST then delete this PaZone object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaZone was initially loaded from the database
         *      OR     - writePaZoneData() has already been called
         *      This PaZone has not been deleted
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

        /**
         * createPaZoneHelperRecords
         *
         * Creates a set of Pa Zone Helper entries in the
         *  PA_ZONE_HELPER table, to associate this
         *  Pa Zone with various entities
         *
         * @param entityEquipmentKeys the keys of all the entities
         *      (ENTITY.NAME) to link to this zone
         *
         * @exception DataException if there already exists
         *          an association between this zone and any of the input entities
         *          (meaning a duplicate Zone / Equipment name entry exists in database)
         *          or this zone object doesn't exist (ie no key)
         *
         * @exception DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */        
        //virtual void createPaZoneHelperRecords(const std::vector<std::string>& entityEquipmentKeys) = 0;
    };
} //close namespace TA_Core
#endif // !defined(IConfigPaZone_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/IConfigPaTisMessageMap.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IConfigPaTisMessageMap - This is the interface supported by objects that allow configuration of
  * PaTisMessageMaps. It provides methods for retrieving, settings, and writing data for an PaTisMessageMap object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigPaTisMessageMap_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigPaTisMessageMap_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/pa/src/IPaTisMessageMap.h"

namespace TA_Base_Core
{

    class IConfigPaTisMessageMap : public virtual TA_Base_Core::IConfigItem, public virtual IPaTisMessageMap
    {

    public:

        virtual ~IConfigPaTisMessageMap() {};

        virtual void setPaDvaMessageKey(const unsigned long id) =0;
        virtual void setTisMessageTag(const unsigned long tisMessageTag) = 0;
        virtual void setTisLibrarySection(const unsigned long tisLibrarySection) = 0;
        virtual void setTisLibraryVersion(const unsigned long version) =0;


        /**
         * getDateCreated
         *
         * Returns the date created for this PaTisMessageMap
         *
         * @return The date created for this PaTisMessageMap as a time_t.
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
         * Returns the date modified for this PaTisMessageMap.
         *
         * @return The date modified for this PaTisMessageMap as a time_t.
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
         * Removes this PaTisMessageMap from the database. 
         * The calling application MUST then delete this PaTisMessageMap object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaTisMessageMap was initially loaded from the database
         *      OR     - writePaTisMessageMapData() has already been called
         *      This PaTisMessageMap has not been deleted
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

    };
} //close namespace TA_Base_Core
#endif // !defined(IConfigPaTisMessageMap_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)

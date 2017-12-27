/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/IConfigPaStation.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IConfigPaStation - This is the interface supported by objects that allow configuration of
  * PaStations. It provides methods for retrieving, settings, and writing data for an PaStation object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigPaStation_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigPaStation_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/pa/src/IPaStation.h"

namespace TA_Base_Core
{

    class IConfigPaStation : public virtual TA_Base_Core::IConfigItem, public virtual IPaStation
    {

    public:

        virtual ~IConfigPaStation() {};
        virtual void setKey(const unsigned long id)=0;
        virtual void setLocationKey(const unsigned long locationKey) =0;

        /**
         * getDateCreated
         *
         * Returns the date created for this PaStation
         *
         * @return The date created for this PaStation as a time_t.
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
         * Returns the date modified for this PaStation.
         *
         * @return The date modified for this PaStation as a time_t.
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
         * Removes this PaStation from the database. 
         * The calling application MUST then delete this PaStation object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaStation was initially loaded from the database
         *      OR     - writePaStationData() has already been called
         *      This PaStation has not been deleted
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

        /**
         * getModifiedKey
         *
         * @return the modified (if applicable) primary key, but if the
         *          key hasn't been modified, returns the actual primary key
         *
         * @see setKey
         */
        virtual unsigned long getModifiedKey() = 0;

    };
} //close namespace TA_Base_Core
#endif // !defined(IConfigPaStation_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)

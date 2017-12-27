/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/pa/src/IConfigPaFasTriggeredBroadcast.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * IConfigPaFasTriggeredBroadcast - This is the interface supported by objects that allow configuration of
  * PaFasTriggeredBroadcasts. It provides methods for retrieving, settings, and writing data for an PaFasTriggeredBroadcast object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigPaFasTriggeredBroadcast_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigPaFasTriggeredBroadcast_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/pa/src/IPaFasTriggeredBroadcast.h"

namespace TA_Base_Core
{

    class IConfigPaFasTriggeredBroadcast : public virtual TA_Base_Core::IConfigItem, public virtual IPaFasTriggeredBroadcast
    {

    public:

        virtual ~IConfigPaFasTriggeredBroadcast() {};


        virtual void setFasAlarmEntityKey(unsigned long ulEntityKey) =0;
        virtual void setLocationKey(unsigned long locationKey) =0;
        virtual void setPaCyclicTimes(unsigned long ulCyclicTimes) =0;
        virtual void setPaZoneGroupKey(unsigned long key) =0;
        virtual void setPaDvaMessageKey(unsigned long key) =0;
        virtual void setIsEnabledParameter(bool isEnabled) =0;

        /**
         * getDateCreated
         *
         * Returns the date created for this PaFasTriggeredBroadcast
         *
         * @return The date created for this PaFasTriggeredBroadcast as a time_t.
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
         * Returns the date modified for this PaFasTriggeredBroadcast.
         *
         * @return The date modified for this PaFasTriggeredBroadcast as a time_t.
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
         * Removes this PaFasTriggeredBroadcast from the database. 
         * The calling application MUST then delete this PaFasTriggeredBroadcast object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaFasTriggeredBroadcast was initially loaded from the database
         *      OR     - writePaFasTriggeredBroadcastData() has already been called
         *      This PaFasTriggeredBroadcast has not been deleted
         */
        virtual void deleteThisObject() = 0;

    };
} //close namespace TA_Base_Core
#endif // !defined(IConfigPaFasTriggeredBroadcast_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)

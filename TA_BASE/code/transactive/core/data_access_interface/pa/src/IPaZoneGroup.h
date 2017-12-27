/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/IPaZoneGroup.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IPaZoneGroup is an interface to a PaZoneGroup object.  It allows the PaZoneGroup object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IPaZoneGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IPaZoneGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IPaZone;

    class IPaZoneGroup : public virtual TA_Base_Core::IItem
    {

    public:

        virtual unsigned long getKey() =0;
        virtual unsigned long getLocationKey() =0;
        virtual std::string getLabel() =0;
        virtual unsigned long getIsEventGroup() =0;

        /**
         * getAssociatedPaZones
         *
         * Returns the PaZones associated with this Pa Zone Group
         *
         * @return The PaZones in this group
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual std::vector<IPaZone*> getAssociatedPaZones() = 0;


        virtual ~IPaZoneGroup() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IPaZoneGroup*> IPaZoneGroups;
    typedef IPaZoneGroups::iterator    IPaZoneGroupsIt;

} //close namespace TA_Base_Core


#endif // !defined(IPaZoneGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)

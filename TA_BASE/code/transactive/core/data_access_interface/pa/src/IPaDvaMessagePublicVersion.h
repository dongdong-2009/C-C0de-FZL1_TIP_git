/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/pa/src/IPaDvaMessagePublicVersion.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * IPaDvaMessagePublicVersion is an interface to a PaDvaMessagePublicVersion object.  It allows the PaDvaMessagePublicVersion object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  */

#if !defined IPaDvaMessagePublicVersion_H
#define IPaDvaMessagePublicVersion_H

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IPaDvaMessagePublicVersion;

    class IPaDvaMessagePublicVersion : public virtual TA_Base_Core::IItem
    {
    public:
        enum EVersionColumn
        {
            PUBLIC_ADHOC1,
            PUBLIC_ADHOC2,
            PUBLIC_ADHOC3,
            PUBLIC_ADHOC4,
            PUBLIC_ADHOC5,
            PUBLIC_ADHOC6,
            PUBLIC_PRERECORDED,

            VERSION_COLUMN_MAX
        };

        enum ELocation
        {
            DEPOT,
            OCC,
            STATION
        };

        virtual unsigned long getKey() =0;
        virtual unsigned long getLocationKey() =0;
        virtual std::string getMessageVersion(IPaDvaMessagePublicVersion::ELocation location, unsigned long stationDvaMessageId) = 0;
        virtual std::string getVersionData(EVersionColumn column) = 0;

        virtual ~IPaDvaMessagePublicVersion() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IPaDvaMessagePublicVersion*> IPaDvaMessagePublicVersions;
    typedef IPaDvaMessagePublicVersions::iterator    IPaDvaMessagePublicVersionsIt;

} //close namespace TA_Base_Core


#endif // !defined IPaDvaMessagePublicVersion_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/pa/src/IPaFasTriggeredBroadcast.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * IPaFasTriggeredBroadcast is an interface to a PaFasTriggeredBroadcast object.  It allows the PaFasTriggeredBroadcast object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  */

#if !defined(IPaFasTriggeredBroadcast_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IPaFasTriggeredBroadcast_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IPaFasTriggeredBroadcast;

    class IPaFasTriggeredBroadcast : public virtual TA_Base_Core::IItem
    {

    public:

        virtual unsigned long getKey() =0;
        virtual unsigned long getFasAlarmEntityKey() =0;
        virtual unsigned long getLocationKey() =0;
        virtual unsigned long getPaCyclicTimes() =0;
        virtual unsigned long getPaZoneGroupKey() =0;
        virtual unsigned long getPaDvaMessageKey() =0;
        virtual bool          getIsEnabledParameter() =0;

        virtual ~IPaFasTriggeredBroadcast() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IPaFasTriggeredBroadcast*> IPaFasTriggeredBroadcasts;
    typedef IPaFasTriggeredBroadcasts::iterator    IPaFasTriggeredBroadcastsIt;

} //close namespace TA_Base_Core


#endif // !defined(IPaFasTriggeredBroadcast_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)

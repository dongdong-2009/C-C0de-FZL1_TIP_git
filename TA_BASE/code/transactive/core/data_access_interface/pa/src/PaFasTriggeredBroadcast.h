 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/pa/src/PaFasTriggeredBroadcast.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * PaFasTriggeredBroadcast is an implementation of IPaFasTriggeredBroadcast. It holds the data specific to an PaFasTriggeredBroadcast entry
  * in the database, and allows read-only access to that data.
  *
  */


#if !defined(PaFasTriggeredBroadcast_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define PaFasTriggeredBroadcast_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/pa/src/PaFasTriggeredBroadcastHelper.h"
#include "core/data_access_interface/pa/src/IPaFasTriggeredBroadcast.h"

namespace TA_Base_Core
{


    class PaFasTriggeredBroadcast : public IPaFasTriggeredBroadcast
    {

    public:



        /**
         * PaFasTriggeredBroadcast (constructor)
         *
         * Construct an PaFasTriggeredBroadcast class based around a key.
         *
         * @param key The key of this PaFasTriggeredBroadcast in the database
         */
        PaFasTriggeredBroadcast(const unsigned long key);

        /**
         * PaFasTriggeredBroadcast (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaFasTriggeredBroadcastHelper::PaFasTriggeredBroadcastHelper(IData*)
         *
         */
        PaFasTriggeredBroadcast(unsigned long row, TA_Base_Core::IData& data);

        virtual ~PaFasTriggeredBroadcast();

        virtual unsigned long getKey();
 
        virtual unsigned long getFasAlarmEntityKey();
        virtual unsigned long getLocationKey();
        virtual unsigned long getPaCyclicTimes();
        virtual unsigned long getPaZoneGroupKey();
        virtual unsigned long getPaDvaMessageKey();
        virtual bool          getIsEnabledParameter();
        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  
         * For the PA ATS Triggered Broadcast this is 
         * just the key in a string format e.g. "PA ATS Triggered Broadcast 1".
         *
         * @return The key description for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

        /**
         * invalidate
         *
         * Make the data contained by this PaFasTriggeredBroadcast as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


    private:

        // Assignment PaFasTriggeredBroadcast not used so it is made private
		PaFasTriggeredBroadcast& operator=(const PaFasTriggeredBroadcast &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the PaFasTriggeredBroadcastHelper
        // has only been written for ConfigLocation objects and will not copy the PaFasTriggeredBroadcast helper
        // for a read-only object (ie this one).
        PaFasTriggeredBroadcast( const PaFasTriggeredBroadcast& thePaFasTriggeredBroadcast);  

        TA_Base_Core::PaFasTriggeredBroadcastHelper * m_PaFasTriggeredBroadcastHelper;
    };
} // closes TA_Base_Core

#endif // !defined(PaFasTriggeredBroadcast_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

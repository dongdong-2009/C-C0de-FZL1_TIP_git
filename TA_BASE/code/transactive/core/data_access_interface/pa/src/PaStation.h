 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaStation.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaStation is an implementation of IPaStation. It holds the data specific to an PaStation entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(PaStation_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define PaStation_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/pa/src/PaStationHelper.h"
#include "core/data_access_interface/pa/src/IPaStation.h"

namespace TA_Base_Core
{


    class PaStation : public IPaStation
    {

    public:



        /**
         * PaStation (constructor)
         *
         * Construct an PaStation class based around a key.
         *
         * @param key The key of this PaStation in the database
         */
        PaStation(const unsigned long key);

        /**
         * PaStation (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaStationHelper::PaStationHelper(IData*)
         *
         */
        PaStation(unsigned long row, TA_Base_Core::IData& data);


        virtual ~PaStation();

        virtual unsigned long getKey();
        virtual unsigned long getLocationKey();

         
        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA Station this is 
         * just the key in a string format e.g. "PA Station 1".
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
         * Make the data contained by this PaStation as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


    private:

        // Assignment PaStation not used so it is made private
		PaStation& operator=(const PaStation &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the PaStationHelper
        // has only been written for ConfigLocation objects and will not copy the PaStation helper
        // for a read-only object (ie this one).
        PaStation( const PaStation& thePaStation);  


        TA_Base_Core::PaStationHelper * m_paStationHelper;
    };
} // closes TA_Base_Core

#endif // !defined(PaStation_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

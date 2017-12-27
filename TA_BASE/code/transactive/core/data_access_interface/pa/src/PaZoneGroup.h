 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaZoneGroup.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaZoneGroup is an implementation of IPaZoneGroup. It holds the data specific to an PaZoneGroup entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(PaZoneGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define PaZoneGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/pa/src/PaZoneGroupHelper.h"
#include "core/data_access_interface/pa/src/IPaZoneGroup.h"

namespace TA_Base_Core
{


    class PaZoneGroup : public IPaZoneGroup
    {

    public:



        /**
         * PaZoneGroup (constructor)
         *
         * Construct an PaZoneGroup class based around a key.
         *
         * @param key The key of this PaZoneGroup in the database
         */
        PaZoneGroup(const unsigned long key);

        /**
         * PaZoneGroup (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaZoneGroupHelper::PaZoneHelper(IData*)
         *
         */
        PaZoneGroup(unsigned long row, TA_Base_Core::IData& data);


        virtual ~PaZoneGroup();

        virtual unsigned long getKey();
        virtual unsigned long getLocationKey();
        virtual std::string getLabel();
        virtual unsigned long getIsEventGroup();
         
        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA Zone Group this is 
         * just the key in a string format e.g. "PA Zone Group 1".
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
		virtual std::vector<IPaZone*> getAssociatedPaZones();

        /**
         * invalidate
         *
         * Make the data contained by this PaZoneGroup as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


    private:

        // Assignment PaZoneGroup not used so it is made private
		PaZoneGroup& operator=(const PaZoneGroup &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the PaZoneGroupHelper
        // has only been written for ConfigLocation objects and will not copy the PaZoneGroup helper
        // for a read-only object (ie this one).
        PaZoneGroup( const PaZoneGroup& thePaZoneGroup);  


        TA_Base_Core::PaZoneGroupHelper * m_paZoneGroupHelper;
    };
} // closes TA_Base_Core

#endif // !defined(PaZoneGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaZone.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaZone is an implementation of IPaZone. It holds the data specific to an PaZone entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(PaZone_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define PaZone_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/pa/src/PaZoneHelper.h"
#include "core/data_access_interface/pa/src/IPaZone.h"

namespace TA_Base_Core
{


    class PaZone : public IPaZone
    {

    public:



        /**
         * PaZone (constructor)
         *
         * Construct an PaZone class based around a key.
         *
         * @param key The key of this PaZone in the database
         */
        PaZone(const unsigned long key);

        /**
         * PaZone (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaZoneHelper::PaZoneHelper(IData*)
         *
         */
        PaZone(unsigned long row, TA_Base_Core::IData& data);

        virtual ~PaZone();

        virtual unsigned long getKey();
        virtual unsigned long getLocationKey();
        virtual unsigned long getId();
        virtual std::string getLabel();
        virtual std::string getStnEntityName();
        virtual std::string getOccEntityName();
        //virtual unsigned long getStatusEntityKey();
        //virtual std::list<std::string> getAssociatedEquipmentEntities();
         
        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA Zone this is 
         * just the key in a string format e.g. "PA Zone 1".
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
         * Make the data contained by this PaZone as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();
        
    private:

        // Assignment PaZone not used so it is made private
		PaZone& operator=(const PaZone &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the PaZoneHelper
        // has only been written for ConfigLocation objects and will not copy the PaZone helper
        // for a read-only object (ie this one).
        PaZone( const PaZone& thePaZone);  


        PaZoneHelper * m_paZoneHelper;
    };
} // closes TA_Base_Core

#endif // !defined(PaZone_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTisMessageMap.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaTisMessageMap is an implementation of IPaTisMessageMap. It holds the data specific to an PaTisMessageMap entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(PaTisMessageMap_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define PaTisMessageMap_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/pa/src/PaTisMessageMapHelper.h"
#include "core/data_access_interface/pa/src/IPaTisMessageMap.h"

namespace TA_Base_Core
{


    class PaTisMessageMap : public IPaTisMessageMap
    {

    public:

        /**
         * PaTisMessageMap (constructor)
         *
         * Construct an PaTisMessageMap class based around a key.
         *
         * @param key The key of this PaTisMessageMap in the database
         */
        PaTisMessageMap(const unsigned long key);

        /**
         * PaTisMessageMap (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaTisMessageMapHelper::PaTisMessageMapHelper(IData*)
         *
         */
        PaTisMessageMap(unsigned long row, TA_Base_Core::IData& data);


        virtual ~PaTisMessageMap();

        virtual unsigned long getKey();
        virtual unsigned long getPaDvaMessageKey();
        virtual unsigned long getTisMessageTag();
        virtual unsigned long getTisLibrarySection();
        virtual unsigned long getTisLibraryVersion();
         
        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA DVA Messages this is 
         * just the key in a string format e.g. "PA Dva Message 1".
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
         * Make the data contained by this PaTisMessageMap as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


    private:

        // Assignment PaTisMessageMap not used so it is made private
		PaTisMessageMap& operator=(const PaTisMessageMap &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the PaTisMessageMapHelper
        // has only been written for ConfigLocation objects and will not copy the PaTisMessageMap helper
        // for a read-only object (ie this one).
        PaTisMessageMap( const PaTisMessageMap& thePaTisMessageMap);  


        TA_Base_Core::PaTisMessageMapHelper * m_paTisMessageMapHelper;
    };
} // closes TA_Base_Core

#endif // !defined(PaTisMessageMap_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

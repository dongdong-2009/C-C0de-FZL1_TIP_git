/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IItem.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This interface should be implemented by all items that fit a standard format. This is so applications can make the
  * most of reusing code for common database objects.
  */

#if !defined(IItem_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IItem_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <boost/shared_ptr.hpp>

namespace TA_Base_Core
{

    class IItem
    {
    public:
	
        virtual ~IItem() { };


        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey() =0;


        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName() =0;
		

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef boost::shared_ptr<IItem> IItemPtr;

} //close namespace TA_Base_Core

#endif // !defined(IItem_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)

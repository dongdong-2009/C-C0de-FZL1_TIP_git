#if !defined(ArchiveOnline_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ArchiveOnline_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveOnline.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ArchiveOnline is an implementation of IArchiveOnline. It holds the data specific to a 
  * ArchiveOnline entry in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnlineHelper.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveOnline.h"

namespace TA_Base_Core
{
    class ArchiveOnline : public IArchiveOnline
    {
    public:
        /**
         * ArchiveOnline
         *
         * Standard constructer that creates a ArchiveOnline object with the given key.
         *
         * @param       unsigned long key 
         *              The unique key of this ArchiveOnline.
         * 
         * @exception   ArchiveException
         *              Thrown if the ArchiveOnlineHelper cannot be created.
         */

        ArchiveOnline( unsigned long key );

        /**
         * ~ArchiveOnline
         *
         * Standard destructor.
         */

        virtual ~ArchiveOnline() {};

        /**
         * getKey
         *
         * Returns the key for this ArchiveOnline entry.
         *
         * Preconditions:   Either - this ArchiveOnline entry was initially 
         *                           loaded from the database
         *                  OR     - writeArchiveOnlineData() has already been called
         *
         * @return      unsigned long
         *              The key for this ArchiveOnline.
         */

	    virtual unsigned long getKey();

        /**
         * getOnlinePeriodInDays
         *
         * Returns the online period for this ArchiveOnline entry.
         * This is the number of days that data will be kept in a table
         * that is archived from when the archive query column is filled 
         * in to when the data is removed from the system.
         *
         * Preconditions:   Either - this ArchiveOnline entry was initially 
         *                           loaded from the database
         *                  OR     - writeArchiveOnlineData() has already been called
         *
         * @return      unsigned short
         *              The online period of the ArchiveOnline entry.
         */

        virtual unsigned short getOnlinePeriodInDays();

        /**
         * getDataExpiryInDays
         *
         * Returns the number of days that restored data will be kept in a table
         * before the system deletes it.
         *
         * Preconditions:   Either - this ArchiveOnline entry was initially 
         *                           loaded from the database
         *                  OR     - writeArchiveOnlineData() has already been called
         *
         * @return      unsigned short
         *              The data expiry period of the ArchiveOnline entry.
         */

        virtual unsigned short getDataExpiryInDays();

        /**
         * invalidate
         *
         * Mark the data contained by this ArchiveOnline as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    0 != m_archiveTableHelper.
         */

	    virtual void invalidate();

    private:
        //
        // Disable copy constructor and assignment operator.
        //

        ArchiveOnline( const ArchiveOnline& theArchiveOnline);  
  		ArchiveOnline& operator=(const ArchiveOnline &);

        //
        // This is the object that does all the work.
        //
    
        ArchiveOnlineHelper m_archiveOnlineHelper;
    };

} // closes TA_Base_Core

#endif // !defined(ArchiveOnline_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

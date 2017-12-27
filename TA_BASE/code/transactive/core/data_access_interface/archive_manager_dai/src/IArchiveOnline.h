#if !defined(IArchiveOnline_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define IArchiveOnline_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/IArchiveOnline.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IArchiveOnline is the interface to an object that holds the data specific to a 
  * ArchiveOnline entry in the database, and allows read-only access to that data.
  */

namespace TA_Base_Core
{
    class IArchiveOnline
    {
    public:
        /**
         * ~IArchiveOnline
         *
         * Standard destructor.
         */

        virtual ~IArchiveOnline() {};

        /**
         * getKey
         *
         * Returns the key for this IArchiveOnline entry.
         *
         * Preconditions:   Either - this IArchiveOnline entry was initially 
         *                           loaded from the database
         *                  OR     - writeIArchiveOnlineData() has already been called
         *
         * @return      unsigned long
         *              The key for this IArchiveOnline.
         */

	    virtual unsigned long getKey() = 0;

        /**
         * getOnlinePeriodInDays
         *
         * Returns the online period for this IArchiveOnline entry.
         * This is the number of days that data will be kept in a table
         * that is archived from when the archive query column is filled 
         * in to when the data is removed from the system.
         *
         * Preconditions:   Either - this IArchiveOnline entry was initially 
         *                           loaded from the database
         *                  OR     - writeIArchiveOnlineData() has already been called
         *
         * @return      unsigned short
         *              The online period of the IArchiveOnline entry.
         */

        virtual unsigned short getOnlinePeriodInDays() = 0;

        /**
         * getDataExpiryInDays
         *
         * Returns the number of days that restored data will be kept in a table
         * before the system deletes it.
         *
         * Preconditions:   Either - this IArchiveOnline entry was initially 
         *                           loaded from the database
         *                  OR     - writeIArchiveOnlineData() has already been called
         *
         * @return      unsigned short
         *              The data expiry period of the IArchiveOnline entry.
         */

        virtual unsigned short getDataExpiryInDays() = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this IArchiveOnline as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    0 != m_archiveTableHelper.
         */

	    virtual void invalidate() = 0;
    };

} // closes TA_Base_Core

#endif // !defined(IArchiveOnline_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

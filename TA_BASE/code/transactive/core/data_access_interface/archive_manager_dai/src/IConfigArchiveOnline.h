#if !defined(IConfigArchiveOnline_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigArchiveOnline_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/IConfigArchiveOnline.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IConfigArchiveOnline is the interface to an object that holds the data specific to a 
  * ArchiveOnline entry in the database, and allows read and write access to that data.
  */

#include "core/data_access_interface/archive_manager_dai/src/IArchiveOnline.h"

namespace TA_Base_Core
{
    class IConfigArchiveOnline : public IArchiveOnline
    {
    public:
        /**
         * ~IConfigArchiveOnline
         *
         * Standard destructor.
         */

        virtual ~IConfigArchiveOnline() {};

        /**
         * getKey
         *
         * Returns the key for this IConfigArchiveOnline entry.
         *
         * Preconditions:   Either - this IConfigArchiveOnline entry was initially 
         *                           loaded from the database
         *                  OR     - writeIConfigArchiveOnlineData() has already been called
         *
         * @return      unsigned long
         *              The key for this IConfigArchiveOnline.
         */

	    virtual unsigned long getKey() = 0;

        /**
         * getOnlinePeriodInDays
         *
         * Returns the online period for this IConfigArchiveOnline entry.
         * This is the number of days that data will be kept in a table
         * that is archived from when the archive query column is filled 
         * in to when the data is removed from the system.
         *
         * Preconditions:   Either - this IConfigArchiveOnline entry was initially 
         *                           loaded from the database
         *                  OR     - writeIConfigArchiveOnlineData() has already been called
         *
         * @return      unsigned short
         *              The online period of the IConfigArchiveOnline entry.
         */

        virtual unsigned short getOnlinePeriodInDays() = 0;

        /**
         * setOnlinePeriodInDays
         *
         * Sets the online period for this IConfigArchiveOnline entry.
         * This is the number of days that data will be kept in a table
         * that is archived from when the archive query column is filled 
         * in to when the data is removed from the system.
         *
         * Preconditions:   Either - this IConfigArchiveOnline entry was initially 
         *                           loaded from the database
         *                  OR     - writeIConfigArchiveOnlineData() has already been called
         *
         * @param       unsigned short onlinePeriodInDays 
         *              The online period of the IConfigArchiveOnline entry.
         */

        virtual void setOnlinePeriodInDays( unsigned short onlinePeriodInDays ) = 0;

        /**
         * getDataExpiryInDays
         *
         * Returns the number of days that restored data will be kept in a table
         * before the system deletes it.
         *
         * Preconditions:   Either - this IConfigArchiveOnline entry was initially 
         *                           loaded from the database
         *                  OR     - writeIConfigArchiveOnlineData() has already been called
         *
         * @return      unsigned short
         *              The data expiry period of the IConfigArchiveOnline entry.
         */

        virtual unsigned short getDataExpiryInDays() = 0;

        /**
         * setDataExpiryInDays
         *
         * Sets the number of days that restored data will be kept in a table
         * before the system deletes it.
         *
         * Preconditions:   Either - this IConfigArchiveOnline entry was initially 
         *                           loaded from the database
         *                  OR     - writeIConfigArchiveOnlineData() has already been called
         *
         * @param       unsigned short dataExpiryInDays
         *              The data expiry period of the IConfigArchiveOnline entry.
         */

        virtual void setDataExpiryInDays( unsigned short dataExpiryInDays ) = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this IConfigArchiveOnline as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    0 != m_archiveTableHelper.
         */

	    virtual void invalidate() = 0;

        /**
         * applyChanges
         *
	     * This will apply all changes made to the database.
         *
         * @exception   DatabaseException 
         *              Thrown if there is a problem accessing the data to the database.
         * @exception   DataException 
         *              Thrown if there is more than one ArchiveOnline object.
         * @exception   DataConfigurationException 
         *              Thrown if the data contained in the ArchiveOnline object
         *              is not sufficent to create an entry in the database.
	     */

	    virtual void applyChanges() = 0;
    };

} // closes TA_Base_Core

#endif // !defined(IConfigArchiveOnline_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)

#if !defined(ConfigArchiveOnline_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigArchiveOnline_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ConfigArchiveOnline.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigArchiveOnline is a concrete implementation of IConfigArchiveOnline, which is in turn 
  * an implementation of IArchiveOnline. It is to be used by the Configuration Editor, 
  * and other applications that wish to change the database content for ArchiveOnlines.
  */

#include "core/data_access_interface/archive_manager_dai/src/IConfigArchiveOnline.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnlineHelper.h"

namespace TA_Base_Core
{
    class ConfigArchiveOnline : public IConfigArchiveOnline
    {
    public:
        /**
         * ConfigArchiveOnline
         *
         * Standard constructer that creates a configurable ArchiveOnline object with the given key.
         * This is used when creating a *new* ArchiveOnline - that is, one that does not yet exist 
         * in the database.
         *
         * @param       unsigned long key 
         *              The unique key of this ArchiveOnline.
         * 
         * @exception   ArchiveException
         *              Thrown if the ArchiveOnlineHelper cannot be created.
         */

        ConfigArchiveOnline( unsigned long key );

        /**
         * ConfigArchiveOnline
         *
         * Standard constructer that creates a configurable ArchiveOnline object with the given key.
         * This is used when creating a *new* ArchiveOnline - that is, one that does not yet exist 
         * in the database.
         *
         * @exception   ArchiveException
         *              Thrown if the ArchiveOnlineHelper cannot be created.
         */

        ConfigArchiveOnline();

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

	    virtual unsigned long getKey();

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

        virtual unsigned short getOnlinePeriodInDays();

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

        virtual void setOnlinePeriodInDays( unsigned short onlinePeriodInDays );

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

        virtual unsigned short getDataExpiryInDays();

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

        virtual void setDataExpiryInDays( unsigned short dataExpiryInDays );

        /**
         * invalidate
         *
         * Mark the data contained by this IConfigArchiveOnline as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    0 != m_archiveTableHelper.
         */

	    virtual void invalidate();

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

	    virtual void applyChanges();
        
    private:
        //
        // Disable copy constructor and assignment operator.
        //

        ConfigArchiveOnline( const ConfigArchiveOnline& theConfigArchiveOnline );  
  		ConfigArchiveOnline& operator=( const ConfigArchiveOnline & );

        //
        // This is the object that does all the work.
        //
        
        ArchiveOnlineHelper m_archiveOnlineHelper;
    };

} // closes TA_Base_Core

#endif // !defined(ConfigArchiveOnline_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)

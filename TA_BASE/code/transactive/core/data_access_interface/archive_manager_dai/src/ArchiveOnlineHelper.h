#if !defined(ArchiveOnlineHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define ArchiveOnlineHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveOnlineHelper.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ArchiveOnlineHelper is an object that is held by ArchiveOnline and ConfigArchiveOnline objects. 
  * It contains all data used by ArchiveOnlines, and manipulation methods for the data. 
  * It helps avoid re-writing code for both ArchiveOnline and ConfigArchiveOnline.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

namespace TA_Base_Core
{
    class ArchiveOnlineHelper
    {
    public:
        /**
         * ArchiveOnlineHelper
         * 
         * This constructor creates a new ArchiveOnlineHelper for configuration.
         */

       ArchiveOnlineHelper();
   
       /**
         * ArchiveOnlineHelper
         * 
         * This constructor creates a new ArchiveOnlineHelper for the specified key.
         *
         * @param       unsigned long archiveTableKey
         *              The database (entity) key for this ArchiveOnline.
         */

        ArchiveOnlineHelper( unsigned long archiveTableKey );

        /**
          * ArchiveOnlineHelper
          *
          * Standard copy constructor.
          */

        ArchiveOnlineHelper( const ArchiveOnlineHelper& theArchiveOnlineHelper );

        /**
         * ~ArchiveOnlineHelper
         *
         * Standard destructor.
         */

        virtual ~ArchiveOnlineHelper() {};

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

	    unsigned long getKey();

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

        unsigned short getOnlinePeriodInDays();

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

        void setOnlinePeriodInDays( unsigned short onlinePeriodInDays );

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

        unsigned short getDataExpiryInDays();

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

        void setDataExpiryInDays( unsigned short dataExpiryInDays );

        /**
         * invalidate
         *
         * Mark the data contained by this IConfigArchiveOnline as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    0 != m_archiveTableHelper.
         */

	    void invalidate();

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

	    void applyChanges();

    private:
        //
        // Disable the assignment operator.
        //

		ArchiveOnlineHelper& operator=(const ArchiveOnlineHelper &);
        
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * Precondition:    Either - this ArchiveOnline was initially loaded from the database
         *                  OR     - writeArchiveOnlineData() has already been called.
         */

	    void reload();

        //
        // Flag to indicate if the data needs reloading.
        //

	    bool m_isValidData;

        // 
        // Flag to indicate if this is a new object.
        //

        bool m_isNew;

        //
        // The ArchiveOnline database key and associated periods.
        //

        unsigned long m_key;
        unsigned short m_onlinePeriodInDays;
        unsigned short m_dataExpiryInDays;
    };

} // closes TA_Base_Core

#endif // !defined(ArchiveOnlineHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)

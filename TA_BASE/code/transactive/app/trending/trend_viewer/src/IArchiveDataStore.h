#if !defined(IArchiveDataStore_AD1B740D_007C_495b_BA98_C4A4A040939D__INCLUDED_)
#define IArchiveDataStore_AD1B740D_007C_495b_BA98_C4A4A040939D__INCLUDED_

/**
  * IArchiveDataStore.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/IArchiveDataStore.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides a contract between the GUI and an actual Archive Data Store. The dialog will
  * call methods on this interface to load the acrhive data dates and to determine whether data
  * between specific dates have been archived.
  *
  */

#include <string>

namespace TA_Base_App
{
    //
    // IArchiveDataStore
    //
    class IArchiveDataStore
    {

    public:

        /**
          * ~IArchiveDataStore
          * 
          * Destructor.
          *
          */
        virtual ~IArchiveDataStore() {}

        /**
          * loadData
          * 
          * Loads all the dates that have had archived data restored.
          *
          */
        virtual void loadData() =0;
        
        /**
          * getUnrestoredDates
          * 
          * Retrieve the dates between a date range that have not been restored.
          *
          * @param startDate - start of date range
          * @param endDate - end of date range
          *
          * @return string - dates that have not been restored in a string format.
          */
        virtual std::string getUnrestoredDates(time_t startDate, time_t endDate) =0;
        
        /**
          * isAllDataRestored
          * 
          * Check if all the dates between a date range have been restored.
          *
          * @param startDate - start of date range
          * @param endDate - end of date range
          */
        virtual bool isAllDataRestored(time_t startDate, time_t endDate) =0;

        /**
          * getDatabaseStorageDays
          * 
          * Get the number of days data is kept online
          *
          * @return number of days
          */
        virtual unsigned short getDatabaseStorageDays() const =0;
    };
}

#endif // !defined(IArchiveDataStore_AD1B740D_007C_495b_BA98_C4A4A040939D__INCLUDED_)

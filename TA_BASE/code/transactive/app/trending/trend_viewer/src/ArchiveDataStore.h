#if !defined(AFX_ArchiveDataStore__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_ArchiveDataStore__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * ArchiveDataStore.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/ArchiveDataStore.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class is a loader for archived data dates and keeps a store of these dates.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 


#include <string>
#include <vector>

#include "app/trending/trend_viewer/src/IArchiveDataStore.h"


namespace TA_Base_App
{
    
    //
    // IGraph
    //
    class ArchiveDataStore : public IArchiveDataStore
    {
    public:
        
        /**
          * ArchiveDataStore
          * 
          * Constructor.
          */
        ArchiveDataStore();

        /**
          * ~ArchiveDataStore
          * 
          * Destructor.
          */
        virtual ~ArchiveDataStore() {}

         /**
          * loadData
          * 
          * Loads all the dates that have had archived data restored.
          *
          */
        virtual void loadData();
        
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
        virtual std::string getUnrestoredDates(time_t startDate, time_t endDate);
        
        /**
          * isAllDataRestored
          * 
          * Check if all the dates between a date range have been restored.
          *
          * @param startDate - start of date range
          * @param endDate - end of date range
          */
        virtual bool isAllDataRestored(time_t startDate, time_t endDate);

    public:

        /**
          * getDatabaseStorageDays
          * 
          * Get the number of days data is kept online
          *
          * @return number of days
          */
        virtual unsigned short getDatabaseStorageDays() const
        {
            return m_databaseStorageDays;
        }

    private:

        ArchiveDataStore( const ArchiveDataStore& theArchiveDataStore);
        ArchiveDataStore& operator=(const ArchiveDataStore &);
        
    private:

        std::vector<time_t> m_restoredDates;    // Store of archived data dates that have been restored

        unsigned short m_databaseStorageDays;   // Number of days data is stored in database before it's archived
    };
}

#endif // !defined(AFX_ArchiveDataStore__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)





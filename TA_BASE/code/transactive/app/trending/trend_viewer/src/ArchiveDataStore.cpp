#if !defined(AFX_ArchiveDataStoreCpp__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_ArchiveDataStoreCpp__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * ArchiveDataStore.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/ArchiveDataStore.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class is a loader for archived data dates and keeps a store of these dates.
  *
  */

#include <sstream>
#include <algorithm>

#include "app/trending/trend_viewer/src/ArchiveDataStore.h"
#include "app/trending/trend_viewer/src/Globals.h"

#include "core/data_access_interface/archive_manager_dai/src/ArchiveInventoryAccessFactory.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveInventory.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveOnline.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveOnlineAccessFactory.h"

#include "core/utilities/src/FunctionEntry.h"
#include "core/utilities/src/DebugUtil.h"


namespace // unnamed namepace
{
    //
    // appendDates
    //
    void appendDates(std::string& dateString, time_t startDate, time_t endDate)
    {
        if(dateString.empty() == false)
        {
            dateString += ", ";
        }

        struct tm* newStartTime;
        newStartTime = localtime(&startDate);

        std::ostringstream str;
        str << newStartTime->tm_mday << "/" << (newStartTime->tm_mon + 1) << "/" << (newStartTime->tm_year + 1900);

        if(startDate != endDate)
        {
            struct tm* newEndTime;
            newEndTime = localtime(&endDate);

            str << " - " << newEndTime->tm_mday << "/" << (newEndTime->tm_mon + 1) << "/" << (newEndTime->tm_year + 1900);
        }

        dateString += str.str();
    }
}


namespace TA_Base_App
{

    //
    // constructor
    //
    ArchiveDataStore::ArchiveDataStore()
    {
        try
        {
            std::auto_ptr<TA_Base_Core::IArchiveOnline> archiveOnline(TA_Base_Core::ArchiveOnlineAccessFactory::getInstance().getArchiveOnlineEntry(false));
            m_databaseStorageDays = archiveOnline->getOnlinePeriodInDays();
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not retrieve archive online period.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Setting database storage days to default.");

            m_databaseStorageDays = MAX_DATABASE_STORAGE_DAYS;
        }
    }


    //
    // loadData
    //
    void ArchiveDataStore::loadData()
    {
        FUNCTION_ENTRY("loadData");

        std::auto_ptr<TA_Base_Core::IArchiveInventory> archiveInventory(TA_Base_Core::ArchiveInventoryAccessFactory::getInstance().getArchiveInventory());
        TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap archiveInventoryItems = archiveInventory->getAllArchiveInventoryItems();
        
        TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap::iterator it = archiveInventoryItems.begin();
        for(; it != archiveInventoryItems.end(); ++it)
        {
            m_restoredDates.push_back(it->second.dataDate);
        }

        FUNCTION_EXIT;
    }


    //
    // getUnrestoredDates
    //
    std::string ArchiveDataStore::getUnrestoredDates(time_t startDate, time_t endDate)
    {
        FUNCTION_ENTRY("getUnrestoredDates");

        std::string unrestoredDates("");
        time_t firstSavedDate = 0;
        time_t lastSavedDate = 0;

        time_t date = startDate;
        while(static_cast<unsigned long>(date) < endDate + SECONDS_IN_DAY)
        {
            std::vector<time_t>::iterator it = std::find(m_restoredDates.begin(), m_restoredDates.end(), date);
            
            if(it == m_restoredDates.end())
            {
                if(firstSavedDate == 0)
                {
                    firstSavedDate = date;
                    lastSavedDate = date;
                }
                else
                {
                    if(lastSavedDate + SECONDS_IN_DAY == static_cast<unsigned long>(date))
                    {
                        // liborm 06/01/2005 - fix for TD#8005
                        // added equal to in comparison
                        if(date >= endDate)
                        {
                            // This is the last date
                            appendDates(unrestoredDates, firstSavedDate, lastSavedDate);
                        }
                        
                        lastSavedDate = date;
                    }
                    else
                    {
                        appendDates(unrestoredDates, firstSavedDate, lastSavedDate);

                        firstSavedDate = date;
                        lastSavedDate = date;
                    }
                }
            }

            date += SECONDS_IN_DAY;
        }

        FUNCTION_EXIT;
        return unrestoredDates;
    }


    //
    // isAllDataRestored
    //
    bool ArchiveDataStore::isAllDataRestored(time_t startDate, time_t endDate)
    {
        FUNCTION_ENTRY("isAllDataRestored");
        
        time_t date = startDate;
        while(static_cast<unsigned long>(date) != endDate + SECONDS_IN_DAY)
        {
            std::vector<time_t>::iterator it = std::find(m_restoredDates.begin(), m_restoredDates.end(), date);
            
            if(it == m_restoredDates.end())
            {
                FUNCTION_EXIT;
                return false;
            }

            date += SECONDS_IN_DAY;
        }

        FUNCTION_EXIT;
        return false;
    }

}  

#endif // !defined(AFX_ArchiveDataStoreCpp__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)





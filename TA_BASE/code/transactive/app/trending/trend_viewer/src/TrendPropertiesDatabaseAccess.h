#if !defined(AFX_TREND_PROPERTIES_DATABASE_ACCESS_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_TREND_PROPERTIES_DATABASE_ACCESS_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * TrendPropertiesDatabaseAccess.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendPropertiesDatabaseAccess.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides database access for the Trend Templates.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 

#include <string>
#include <map>

#include "app/trending/trend_viewer/src/TrendPropertiesStore.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"


namespace TA_Base_App
{
    //
    // TrendPropertiesDatabaseAccess
    //
    class TrendPropertiesDatabaseAccess  
    {
    public:

        struct UpdateDetails
        {
            bool isNewItem;                            // This indicates if the item is a new one
            bool isDelete;                             // This indicates if the item was deleted
            unsigned long keyOfChangedItem;            // This is the key of the item that changed
            std::string nameOfChangedItem;             // This is the name of the item that changed
            //ConfigUpdateType configUpdateMessage;      // This is which config update we should send (ie Entity, Operator etc)
            TA_Base_Core::ItemChanges changes;              // This is a vector of all changes made.
        };
        
        /**
          * TrendPropertiesStore
          * 
          * Constructor.
          *
          */
        TrendPropertiesDatabaseAccess() {}

        /**
          * ~TrendPropertiesStore
          * 
          * Destructor.
          *
          */
        ~TrendPropertiesDatabaseAccess() {}

        /**
          * loadTrendProperties
          * 
          * Load the trend templates from the database.
          *
          * @param trendTemplates - map to store the Trend Properties loaded from the database
          *
          * @exception TrendViewerException - thrown when there is a problem loading the properties
          * from the database
          */
        void loadTrendProperties(std::map<std::string, TrendProperties>& trendTemplates);

        /**
          * addTrendPropertiesToDatabase
          * 
          * Add a new trend template to the database.
          *
          * @param trendProperties - trend template to add to the database
          *
          * @exception TrendViewerException - thrown when there is a problem adding the properties
          * to the database
          */
        void addTrendPropertiesToDatabase(const TrendProperties& trendProperties);

        /**
          * removeTrendPropertiesFromDatabase
          * 
          * Delete a trend template from the database.
          *
          * @param name - name of the template to remove
          *
          * @exception TrendViewerException - thrown when there is a problem deleting the properties
          * from the database
          */
        void removeTrendPropertiesFromDatabase(const std::string& name);

        /**
          * modifyTrendProperties
          * 
          * Modify a trend template in the database
          *
          * @exception TrendViewerException - thrown when there is a problem updating the properties
          * in the database
          */
        void modifyTrendProperties(const std::string& name, const TrendProperties& trendProperties);

    private:

        TrendPropertiesDatabaseAccess(const TrendPropertiesDatabaseAccess&) {}
        TrendPropertiesDatabaseAccess& operator=(const TrendPropertiesDatabaseAccess&) {}

        void sendMessagesForTheChange(const UpdateDetails& updateDetails);
        void sendAuditMessages(const UpdateDetails& updateDetails);
        void sendOnlineUpdateMessage(const UpdateDetails& updateDetails);

    private:

        static const int EVENT_DETAILS_FIELD_SIZE;
    };
}

#endif // !defined(AFX_TREND_PROPERTIES_DATABASE_ACCESS_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)





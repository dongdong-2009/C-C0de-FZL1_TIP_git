#if !defined(AFX_TREND_PROPERTIES_STORE_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_TREND_PROPERTIES_STORE_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * TrendPropertiesStore.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendPropertiesStore.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class maintains a store of all the trend templates available in the database.
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

#include "app/trending/trend_viewer/src/stdafx.h"

//#include "core/synchronisation/src/NonReEntrantThreadLockable.h" //TD17935
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/trends/src/ItemInfoDetails.h"

namespace TA_Base_App
{
    enum EGraphType
    {
        LINE,
        BAR,
        COMPARISON,
        XY,
    };

    struct TrendProperties
    {
        std::string name;
        EGraphType graphType;
        std::string description;
        time_t startDateTime;
        time_t endDateTime;
        unsigned long samplePeriod;
        bool live;
        std::map<std::string, TA_Base_Core::ItemInfo> items;
        std::string extraInfo;
        time_t lastModified;
        bool modificationsUnsaved;
        bool isNew;
    };
    
    typedef std::map<std::string, TrendProperties> TrendPropertiesMap;

    //
    // TrendPropertiesStore
    //
    class TrendPropertiesStore  
    {
    public:
        
        /**
          * TrendPropertiesStore
          * 
          * Constructor.
          *
          */
        TrendPropertiesStore();
        
        /**
          * ~TrendPropertiesStore
          * 
          * Destructor.
          *
          */
        virtual ~TrendPropertiesStore() {}

        /**
          * getAllTrendProperties
          * 
          * Retrieve properties for all trend templates.
          *
          * @return TrendPropertiesMap - map containing all of the trend templates
          */
        const TrendPropertiesMap& getAllTrendProperties();

        /**
          * getTrendProperties
          * 
          * Retrieve properties for one template.
          *
          * @param name - template name
          *
          * @return TrendProperties - structure containing all the properties of the template
          *
          * @exception TrendViewerException - thrown if template doesn't exist
          */
        TrendProperties getTrendProperties(const std::string& name);

        /**
          * addTrendProperties
          * 
          * Add properties for one template to the store.
          *
          * @param name - template name
          * @param TrendProperties - structure containing all the properties of the template
          *
          * @exception TrendViewerException - template of the same name already exists
          */
        void addTrendProperties(const std::string& name, const TrendProperties& trendProperties);

        /**
          * removeTrendProperties
          * 
          * Remove properties for one template from the store.
          * @param lastModified - last modified time retrieved from the database before modifications were made
          *
          * @param name - template name
          *
          */
        void removeTrendProperties(const std::string& name, time_t lastModified);

        /**
          * forceRemoveTrendProperties
          * 
          * Remove properties for one template from the store. There are no checks to see whether modifications
          * have been made to the trend before it is removed.
          *
          * @param name - template name
          *
          */
        void forceRemoveTrendProperties(const std::string& name);

        /**
          * modifyTrendProperties
          * 
          * Modify properties for one template in the store. If the template doesn't exist in the store,
          * it is added.
          *
          * @param name - template name
          * @param TrendProperties - structure containing all the properties of the template
          * @param lastModified - last modified time retrieved from the database before modifications were made
          *
          */
        void modifyTrendProperties(const std::string& name, const TrendProperties& trendProperties, time_t lastModified);

        /**
          * forceModifyTrendProperties
          * 
          * Modify properties for one template in the store. If the template doesn't exist in the store,
          * it is added. There are no checks to see whether the trend has been over written.
          *
          * @param name - template name
          * @param TrendProperties - structure containing all the properties of the template
          *
          */
        void forceModifyTrendProperties(const std::string& name, const TrendProperties& trendProperties);

        /**
          * getGraphType
          * 
          * Convert an index into an EGraphType
          *
          * @param index - index number representing graph type
          *
          * @return EGraphType - the enumerated representation
          */
        static EGraphType getGraphType(int index);

        /**
          * getGraphTypeAsString
          * 
          * Convert an EGraphType into a string
          *
          * @param graphType - enumerated representation of graph type
          *
          * @return string - string representation
          */
        static std::string getGraphTypeAsString(TA_Base_App::EGraphType graphType);

        /**
          * getGraphType
          * 
          * Convert a string into an EGraphType
          *
          * @param graphType - string representation of graph type
          *
          * @return EGraphType - the enumerated representation
          */
        static TA_Base_App::EGraphType getGraphType(const std::string& graphType);


    private:

        TrendPropertiesStore(const TrendPropertiesStore&) {}
        TrendPropertiesStore& operator=(const TrendPropertiesStore&) {}

        void loadProperties();

    private:

        TrendPropertiesMap m_trendTemplates;

    };
}


#endif // !defined(AFX_TREND_PROPERTIES_STORE_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)





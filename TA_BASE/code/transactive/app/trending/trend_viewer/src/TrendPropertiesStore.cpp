#if !defined(AFX_TREND_PROPERTIES_STORE_CPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_TREND_PROPERTIES_STORE_CPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * TrendPropertiesStore.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendPropertiesStore.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class maintains a store of all the trend templates available in the database.
  *
  */

#include "app/trending/trend_viewer/src/TrendPropertiesStore.h"
#include "app/trending/trend_viewer/src/TrendPropertiesDatabaseAccess.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TrendViewerException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/data_access_interface/trends/src/globals.h"


using TA_Base_Core::DebugUtil;


namespace TA_Base_App
{
    //
    // TrendPropertiesStore
    //
    TrendPropertiesStore::TrendPropertiesStore()
    {
        FUNCTION_ENTRY("TrendPropertiesStore");

        //loadProperties();

        FUNCTION_EXIT;
    }

    //
    // getAllTrendProperties
    //
    const TrendPropertiesMap& TrendPropertiesStore::getAllTrendProperties()
    {
        // Reload trend templates just in case something new has been added/removed
        loadProperties();

        return m_trendTemplates;
    }


    //
    // getTrendProperties
    //
    TrendProperties TrendPropertiesStore::getTrendProperties(const std::string& name)
    {
        FUNCTION_ENTRY("getTrendProperties");

        // Reload trend templates just in case something new has been added/removed
        loadProperties();

        std::map<std::string, TrendProperties>::const_iterator it = m_trendTemplates.find(name);
        
        if(it != m_trendTemplates.end())
        {
            FUNCTION_EXIT;
            return it->second;
        }

        // If execution reaches here, means the template name wasn't found
        TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEMPLATE_NOT_EXIST));
    }


    //
    // addTrendProperties
    //
    void TrendPropertiesStore::addTrendProperties(const std::string& name, const TrendProperties& trendProperties)
    {
        FUNCTION_ENTRY("addTrendProperties");

        // Reload trend templates just in case something new has been added/removed
        loadProperties();

        std::map<std::string, TrendProperties>::iterator it = m_trendTemplates.find(name);

        if(it != m_trendTemplates.end())
        {
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEMPLATE_ALREADY_EXISTS));
        }
        else
        {
            m_trendTemplates[name] = trendProperties;

            // Add to database
            TrendPropertiesDatabaseAccess databaseAccess;

            try
            {
                databaseAccess.addTrendPropertiesToDatabase(trendProperties);
            }
            catch(const TA_Base_Core::DataException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
                TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, ex.what()));
            }
            catch(const TA_Base_Core::DatabaseException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
                TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, ex.what()));
            }
        }

        FUNCTION_EXIT;
    }

    
    //
    // removeTrendProperties
    //
    void TrendPropertiesStore::removeTrendProperties(const std::string& name, time_t lastModified)
    {
        FUNCTION_ENTRY("removeTrendProperties");

        // Reload trend templates just in case something new has been added/removed
        loadProperties();

        std::map<std::string, TrendProperties>::iterator it = m_trendTemplates.find(name);

        // Check if the template has been modified since these modifications were made
        if(it->second.lastModified > lastModified)
        {
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEMPLATE_MODIFIED));
        }

        forceRemoveTrendProperties(name);

        FUNCTION_EXIT;
    }

    
    //
    // modifyTrendProperties
    //
    void TrendPropertiesStore::modifyTrendProperties(const std::string& name, const TrendProperties& trendProperties, time_t lastModified)
    {
        FUNCTION_ENTRY("modifyTrendProperties");

        // Reload trend templates just in case something new has been added/removed
        loadProperties();

        std::map<std::string, TrendProperties>::iterator it = m_trendTemplates.find(name);

        // Check if the template has been modified since these modifications were made
        if(it->second.lastModified > lastModified)
        {
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEMPLATE_MODIFIED));
        }

        forceModifyTrendProperties(name, trendProperties);
        
        FUNCTION_EXIT;
    }


    //
    // forceModifyTrendProperties
    //
    void TrendPropertiesStore::forceModifyTrendProperties(const std::string& name, const TrendProperties& trendProperties)
    {
        FUNCTION_ENTRY("modifyTrendProperties");

        // Reload trend templates just in case something new has been added/removed
        loadProperties();

        std::map<std::string, TrendProperties>::iterator it = m_trendTemplates.find(name);

        if(it == m_trendTemplates.end())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Template: %s was not found for modification. Adding new template.", name.c_str());
        }

        m_trendTemplates[name] = trendProperties;

        // Modify record in database
        TrendPropertiesDatabaseAccess databaseAccess;

        try
        {
            databaseAccess.modifyTrendProperties(name, trendProperties);
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, ex.what()));
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, ex.what()));
        }
        
        FUNCTION_EXIT;
    }


    //
    // forceRemoveTrendProperties
    //
    void TrendPropertiesStore::forceRemoveTrendProperties(const std::string& name)
    {
        FUNCTION_ENTRY("forceRemoveTrendProperties");

        // Reload trend templates just in case something new has been added/removed
        loadProperties();

        std::map<std::string, TrendProperties>::iterator it = m_trendTemplates.find(name);

        if(it == m_trendTemplates.end())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Template: %s was not found for removal.", name.c_str());
        }
        else
        {
            m_trendTemplates.erase(it);

            // Remove from database
            TrendPropertiesDatabaseAccess databaseAccess;

            try
            {
                databaseAccess.removeTrendPropertiesFromDatabase(name);
            }
            catch(const TA_Base_Core::DataException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
                TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, ex.what()));
            }
            catch(const TA_Base_Core::DatabaseException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
                TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, ex.what()));
            }
        }

        FUNCTION_EXIT;
    }


    //
    // getGraphType
    //
    TA_Base_App::EGraphType TrendPropertiesStore::getGraphType(int index)
    {
        switch(index)
        {
            case 0:  return TA_Base_App::LINE;
               
            case 1:  return TA_Base_App::BAR;

            case 2:  return TA_Base_App::COMPARISON;

            case 3:  return TA_Base_App::XY;

            default: TA_ASSERT(false, "Invalid graph type index");
        }

        // If execution reaches here, means the index wasn't valid
        std::ostringstream str;
        str << "There is no valid graph type for index " << index;
        TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::GRAPH_TYPE_INVALID, str.str()));
    }


    //
    // getGraphTypeAsString
    //
    std::string TrendPropertiesStore::getGraphTypeAsString(TA_Base_App::EGraphType graphType)
    {
        switch(graphType)
        {
            case TA_Base_App::LINE: return GRAPH_TYPES[TA_Base_App::LINE];

            case TA_Base_App::BAR: return GRAPH_TYPES[TA_Base_App::BAR];

            case TA_Base_App::COMPARISON: return GRAPH_TYPES[TA_Base_App::COMPARISON];

            case TA_Base_App::XY: return GRAPH_TYPES[TA_Base_App::XY];

            default: TA_ASSERT(false, "Invalid graph type");
        }

        // If execution reaches here, means the graph type wasn't valid
        std::ostringstream str;
        str << "Graph type is invalid.";
        TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::GRAPH_TYPE_INVALID, str.str()));
    }


    //
    // getGraphType
    //
    TA_Base_App::EGraphType TrendPropertiesStore::getGraphType(const std::string& graphType)
    {
        if(_strcmpi(graphType.c_str(), (GRAPH_TYPES[TA_Base_App::LINE]).c_str()) == 0)
        {
            return TA_Base_App::LINE;
        }
        else if (_strcmpi(graphType.c_str(), (GRAPH_TYPES[TA_Base_App::BAR]).c_str()) == 0)
        {
            return TA_Base_App::BAR;
        }
        else if (_strcmpi(graphType.c_str(), (GRAPH_TYPES[TA_Base_App::COMPARISON]).c_str()) == 0)
        {
            return TA_Base_App::COMPARISON;
        }
        else if (_strcmpi(graphType.c_str(), (GRAPH_TYPES[TA_Base_App::XY]).c_str()) == 0)
        {
            return TA_Base_App::XY;
        }
        else
        {
           TA_ASSERT(false, "Invalid graph type index");
        }

        // If execution reaches here, means the graph type wasn't valid
        std::ostringstream str;
        str << "There is no valid graph type with the string representation: " << graphType;
        TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::GRAPH_TYPE_INVALID, str.str()));
    }

    //
    // loadProperties
    //
    void TrendPropertiesStore::loadProperties()
    {
        FUNCTION_ENTRY("loadProperties");

        TrendPropertiesDatabaseAccess propertiesDbAccess;
        propertiesDbAccess.loadTrendProperties(m_trendTemplates);
        
        FUNCTION_EXIT;
    }

}



#endif // !defined(AFX_TREND_PROPERTIES_STORE_CPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)

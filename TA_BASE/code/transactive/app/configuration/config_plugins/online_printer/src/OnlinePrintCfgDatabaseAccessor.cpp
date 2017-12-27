/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/online_printer/src/OnlinePrintCfgDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve online printer information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/online_printer/src/StdAfx.h"
#include "app/configuration/config_plugins/online_printer/src/OnlinePrintCfgDatabaseAccessor.h"
#include "app/configuration/config_plugins/online_printer/src/NewLocation.h"


#include "core/data_access_interface/src/IConfigOnlinePrintCfg.h"
#include "core/data_access_interface/src/OnlinePrintCfgAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;
using namespace TA_Base_Core;

namespace TA_Base_App
{
    OnlinePrintCfgDatabaseAccessor::OnlinePrintCfgDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configItems.clear();
    }


    OnlinePrintCfgDatabaseAccessor::~OnlinePrintCfgDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end(); ++it)
            {
                if( (it->second) != NULL )
                {
                    delete it->second;
                    it->second = NULL;
                }
            }
            m_configItems.clear();
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void OnlinePrintCfgDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the items and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (ConfigItemsIt assocToDelete = m_configItems.begin(); assocToDelete != m_configItems.end(); ++assocToDelete)
        {
            if( (assocToDelete->second) != NULL )
            {
                delete assocToDelete->second;
                assocToDelete->second = NULL;
            }
        }
        m_configItems.clear();


        // Now reload all
		TA_Base_Core::IOnlinePrintCfgs items = TA_Base_Core::OnlinePrintCfgAccessFactory::getInstance().getAllOnlinePrintCfgs(true);

        // And insert them into our map. At the same time cast them to IConfigOnlinePrintCfg*
        // instead of IOnlinePrintCfg*.
        for (TA_Base_Core::IOnlinePrintCfgsIt it = items.begin(); it != items.end(); ++it)
        {
            TA_Base_Core::IOnlinePrintCfg* item = *it;
			TA_Base_Core::IConfigOnlinePrintCfg* configItem = dynamic_cast<TA_Base_Core::IConfigOnlinePrintCfg*>(item);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getUniqueIdentifier();
                m_configItems[key] = configItem;
            }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> OnlinePrintCfgDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        std::multimap<CString, unsigned long> ret;

        for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end(); ++it)
        {
            if (it->second != NULL)
            {
                //ret.insert(std::map<CString, unsigned long>::value_type(
                  //  it->second->getLocationName().c_str(), it->first));
				ret.insert(std::multimap<CString, unsigned long>::value_type(
					it->second->getLocationDisplayName().c_str(), it->first));
            }
        }

        return ret;
    }


    std::map<unsigned long,OnlinePrintCfgDatabaseAccessor::OnlinePrintCfgRecord> OnlinePrintCfgDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,OnlinePrintCfgRecord> details; 

        // The maximum range is the number of locations times by 2. This is because we want to step for each
        // Location while it is loaded and then again while each Location is added to the list.
        progress.SetRange( 0, m_configItems.size() * 2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.

        CString buffer;
        for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end(); ++it)
        {
            if (it->second != NULL)
            {
                OnlinePrintCfgRecord record;

				record.location = it->second->getLocation();
                record.defaultPrinter = it->second->getDefaultPrinter().c_str();
                
                details.insert( std::map<unsigned long, OnlinePrintCfgRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    OnlinePrintCfgDatabaseAccessor::OnlinePrintCfgRecord OnlinePrintCfgDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the record matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the record with identifier %lu",identifier);
            FUNCTION_EXIT;
            OnlinePrintCfgRecord record;
            return record;
        }

        OnlinePrintCfgRecord record;

		record.location = matching->second->getLocation();
        record.defaultPrinter = matching->second->getDefaultPrinter().c_str();
 
        FUNCTION_EXIT;
        return record;
    }


    TA_Base_Core::IConfigItem* OnlinePrintCfgDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the record matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the record with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void OnlinePrintCfgDatabaseAccessor::deleteItem(unsigned long key)
    {
    }


    TA_Base_Core::IConfigItem* OnlinePrintCfgDatabaseAccessor::newItem()
    {
        CNewLocation loc;
        if (!loc.canCreateNew())
        {
            MessageBox(NULL, "All locations have been defined!", "Error", MB_OK);
        }
        else
        {
            if (IDOK == loc.DoModal())
            {
                return loc.m_newItem;
            }
        }

        return NULL;
    }


    TA_Base_Core::IConfigItem* OnlinePrintCfgDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        return NULL;
    }


    bool OnlinePrintCfgDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(itemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end();  ++it)
        {
            if( (it->second != NULL) && (it->second->hasChanged() || it->second->isNew()) )
            {
                // We just add a single empty item if we know a change has been made. We don't list each
                // individual change
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }
}

    

    

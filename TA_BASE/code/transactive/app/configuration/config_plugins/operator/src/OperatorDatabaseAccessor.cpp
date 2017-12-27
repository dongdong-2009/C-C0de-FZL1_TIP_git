/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/operator/src/OperatorDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve Operator information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/Operator/src/StdAfx.h"
#include "app/configuration/config_plugins/Operator/src/OperatorDatabaseAccessor.h"

#include <algorithm>
#include <cctype>

#include "core/data_access_interface/src/IConfigOperator.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    static std::string UNALLOCATED = "<unallocated>";

    OperatorDatabaseAccessor::OperatorDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    OperatorDatabaseAccessor::~OperatorDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedOperators::iterator iter = m_operators.begin(); iter != m_operators.end(); ++iter)
            {
                if( (iter->second) != NULL )
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void OperatorDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the Operators and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedOperators::iterator operatorToDelete = m_operators.begin(); operatorToDelete != m_operators.end(); ++operatorToDelete)
        {
            if( (operatorToDelete->second) != NULL )
            {
                delete operatorToDelete->second;
                operatorToDelete->second = NULL;
            }
        }
        m_operators.clear();


        // Now reload all the Operators
        std::vector<TA_Base_Core::IOperator*> operators = TA_Base_Core::OperatorAccessFactory::getInstance().getAllOperators( true);

        // And insert the operators into our map. At the same time cast them to IConfigOperator*
        // instead of IOperatorData*.
        for (std::vector<TA_Base_Core::IOperator*>::iterator iter = operators.begin();
             iter != operators.end();
             ++iter
             )
        {
            TA_Base_Core::IConfigOperator* castOperator = dynamic_cast<TA_Base_Core::IConfigOperator*>(*iter);
            if( castOperator != NULL )
            {
                std::string name = castOperator->getName();
                std::transform(name.begin(), name.end(), name.begin(), ::tolower);

                if (name == UNALLOCATED)
                {
                    delete castOperator;
                    castOperator = NULL;
                }
                else
                {
                    m_operators.insert( LoadedOperators::value_type(castOperator->getUniqueIdentifier(), castOperator) );
                }
            }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> OperatorDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::multimap<CString,unsigned long> names; 

        // The maximum range is the number of operators times by 2. This is because we want to step for each
        // Operator while it is loaded and then again while each Operator is added to the list.
        progress.SetRange( 0, m_operators.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedOperators::iterator iter = m_operators.begin();
             iter != m_operators.end();
             ++iter)
        {
            if (iter->second != NULL)
            {
                names.insert( std::multimap<CString, unsigned long>::value_type( iter->second->getName().c_str(), iter->second->getUniqueIdentifier() ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return names;
    }


    TA_Base_Core::IConfigItem* OperatorDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the Operator matching the key passed in.
        LoadedOperators::iterator matching = m_operators.find(key);
        if (matching == m_operators.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Operator with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void OperatorDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the Operator matching the key passed in.
        LoadedOperators::iterator matching = m_operators.find(key);
        if (matching == m_operators.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Operator with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored Operator was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisObject();
        }
        delete matching->second;
        matching->second = NULL;
        m_operators.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* OperatorDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigOperator* newOperator = TA_Base_Core::OperatorAccessFactory::getInstance().createOperator();

        m_operators.insert( LoadedOperators::value_type( newOperator->getUniqueIdentifier(), newOperator ) );

        return newOperator;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* OperatorDatabaseAccessor::copyItem(const unsigned long idOfOperatorToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigOperator* newOperator = NULL;

        // Attempt to find the Operator matching the key passed in.
        LoadedOperators::iterator matching = m_operators.find(idOfOperatorToCopy);

        TA_ASSERT(matching != m_operators.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigOperator stored in the map");

        newOperator = TA_Base_Core::OperatorAccessFactory::getInstance().copyOperator(matching->second);

        if (newOperator != NULL)
        {
            m_operators.insert( LoadedOperators::value_type( newOperator->getUniqueIdentifier(), newOperator ) );
        }
        
        return newOperator;

        FUNCTION_EXIT;
    }


    bool OperatorDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& operatorsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(operatorsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedOperators::iterator iter = m_operators.begin(); iter != m_operators.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    operatorsNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this Operator that has not had it's changes applied yet");
                    operatorsNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this Operator that has not had it's changes applied yet");
                    operatorsNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !operatorsNotApplied.empty();
    }
}

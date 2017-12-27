/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigOnlinePrintCfg.cpp $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 00:07:03 $
  * Last modified by: $Author: huirong.luo $
  * 
  * ConfigOnlinePrintCfg is a concrete implementation of IConfigOnlinePrintCfg, which is in turn an implementation
  * of IOnlinePrintCfg. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for OnlinePrintCfgs.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/src/ConfigOnlinePrintCfg.h"
#include "core/data_access_interface/src/OnlinePrintCfgHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string DEFAULT_PRINTER = "DEF_PRINTER";
    
    long ConfigOnlinePrintCfg::s_nextAvailableIdentifier = 0;


    ConfigOnlinePrintCfg::ConfigOnlinePrintCfg()
     : m_OnlinePrintCfgHelper( new OnlinePrintCfgHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }


    ConfigOnlinePrintCfg::ConfigOnlinePrintCfg(const unsigned long idKey)
     : m_OnlinePrintCfgHelper( new OnlinePrintCfgHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigOnlinePrintCfg::ConfigOnlinePrintCfg( const ConfigOnlinePrintCfg& theConfigOnlinePrintCfg)
     : m_OnlinePrintCfgHelper( new OnlinePrintCfgHelper( *(theConfigOnlinePrintCfg.m_OnlinePrintCfgHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigOnlinePrintCfg::ConfigOnlinePrintCfg(unsigned long row, TA_Base_Core::IData& data)
     : m_OnlinePrintCfgHelper( new OnlinePrintCfgHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigOnlinePrintCfg::~ConfigOnlinePrintCfg()
    {
        try
        {
            delete m_OnlinePrintCfgHelper;
            m_OnlinePrintCfgHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigOnlinePrintCfg::isNew()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL,"The OnlinePrintCfgHelper pointer is null.");

        return m_OnlinePrintCfgHelper->isNew();
    }

    void ConfigOnlinePrintCfg::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    void ConfigOnlinePrintCfg::setDefaultPrinter(const std::string& pPrinter)
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL,"The OnlinePrintCfgHelper pointer is null.");
        
        updateOnlinePrintCfgChanges(DEFAULT_PRINTER, m_OnlinePrintCfgHelper->getDefaultPrinter().c_str(), pPrinter.c_str());

        m_OnlinePrintCfgHelper->setDefaultPrinter(pPrinter);
    }
    
    void ConfigOnlinePrintCfg::setLocation(unsigned long pLocation)
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL,"The OnlinePrintCfgHelper pointer is null.");
        m_OnlinePrintCfgHelper->setLocation(pLocation);
    }

    void ConfigOnlinePrintCfg::setLocationName(const std::string& pLocationName)
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL,"The OnlinePrintCfgHelper pointer is null.");
        m_OnlinePrintCfgHelper->setLocationName(pLocationName);
    }

    unsigned long ConfigOnlinePrintCfg::getKey()
    {
        return getLocation();
    }

    std::string ConfigOnlinePrintCfg::getDefaultPrinter()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL,"The OnlinePrintCfgHelper pointer is null.");
        return m_OnlinePrintCfgHelper->getDefaultPrinter();
    }


    unsigned long ConfigOnlinePrintCfg::getLocation()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL,"The OnlinePrintCfgHelper pointer is null.");
        return m_OnlinePrintCfgHelper->getLocation();
    }

    std::string ConfigOnlinePrintCfg::getLocationName()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL,"The OnlinePrintCfgHelper pointer is null.");
        return m_OnlinePrintCfgHelper->getLocationName();
    }
	
    std::string ConfigOnlinePrintCfg::getLocationDisplayName()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL,"The OnlinePrintCfgHelper pointer is null.");
        return m_OnlinePrintCfgHelper->getLocationDisplayName();
    }

    std::string ConfigOnlinePrintCfg::getName()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL,"The OnlinePrintCfgHelper pointer is null.");

        std::ostringstream name;
		name << m_OnlinePrintCfgHelper->getDefaultPrinter();
        name << " - ";
		name << m_OnlinePrintCfgHelper->getLocation();

        return name.str();
    }
 
	
    void ConfigOnlinePrintCfg::invalidate()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL,"The OnlinePrintCfgHelper pointer is null.");

        m_OnlinePrintCfgHelper->invalidate();
        m_OnlinePrintCfgChanges.clear();

    }


    void ConfigOnlinePrintCfg::deleteThisObject(bool cascade)
    {
        TA_Assert(0);
    }


    void ConfigOnlinePrintCfg::applyChanges()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL, "applyChanges() has been called after OnlinePrintCfg was deleted");

        m_OnlinePrintCfgHelper->writeOnlinePrintCfgData();

        m_OnlinePrintCfgChanges.clear();
    }


    void ConfigOnlinePrintCfg::updateOnlinePrintCfgChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateOnlinePrintCfgChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_OnlinePrintCfgChanges.find(name);

        if (matching != m_OnlinePrintCfgChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_OnlinePrintCfgChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            TA_Base_Core::Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_OnlinePrintCfgChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }
    
} // closes TA_Core


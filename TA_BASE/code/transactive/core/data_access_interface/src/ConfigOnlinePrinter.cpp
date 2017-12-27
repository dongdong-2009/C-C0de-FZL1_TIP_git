/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigOnlinePrinter.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigOnlinePrinter is a concrete implementation of IConfigOnlinePrinter, which is in turn an implementation
  * of IOnlinePrinter. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for OnlinePrinters.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/src/ConfigOnlinePrinter.h"
#include "core/data_access_interface/src/OnlinePrinterHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string PRINTER          = "Printer";
    static const std::string SUBSYSTEM        = "Subsystem";
    static const std::string ISALARM          = "IsAlarm";
    
    long ConfigOnlinePrinter::s_nextAvailableIdentifier = 0;


    ConfigOnlinePrinter::ConfigOnlinePrinter()
     : m_OnlinePrinterHelper( new OnlinePrinterHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }


    ConfigOnlinePrinter::ConfigOnlinePrinter(const unsigned long idKey)
     : m_OnlinePrinterHelper( new OnlinePrinterHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }


    ConfigOnlinePrinter::ConfigOnlinePrinter( const ConfigOnlinePrinter& theConfigOnlinePrinter)
     : m_OnlinePrinterHelper( new OnlinePrinterHelper( *(theConfigOnlinePrinter.m_OnlinePrinterHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigOnlinePrinter::ConfigOnlinePrinter(unsigned long row, TA_Base_Core::IData& data)
     : m_OnlinePrinterHelper( new OnlinePrinterHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigOnlinePrinter::~ConfigOnlinePrinter()
    {
        try
        {
            delete m_OnlinePrinterHelper;
            m_OnlinePrinterHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigOnlinePrinter::isNew()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");

        return m_OnlinePrinterHelper->isNew();
    }

    void ConfigOnlinePrinter::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    void ConfigOnlinePrinter::setPrinter(const std::string& pPrinter)
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        
        updateOnlinePrinterChanges(PRINTER, m_OnlinePrinterHelper->getPrinter().c_str(), pPrinter.c_str());

        m_OnlinePrinterHelper->setPrinter(pPrinter);
    }
    
    void ConfigOnlinePrinter::setSubsystem(unsigned long pSubsystem)
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        m_OnlinePrinterHelper->setSubsystem(pSubsystem);
    }

    void ConfigOnlinePrinter::setLocation(unsigned long pLocation)
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        m_OnlinePrinterHelper->setLocation(pLocation);
    }

    void ConfigOnlinePrinter::setIsAlarm(bool pIsAlarm)
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        m_OnlinePrinterHelper->setIsAlarm(pIsAlarm);
    }

	unsigned long ConfigOnlinePrinter::getKey()
	{
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getKey();
    }

    std::string ConfigOnlinePrinter::getPrinter()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getPrinter();
    }


    unsigned long ConfigOnlinePrinter::getLocation()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getLocation();
    }

    unsigned long ConfigOnlinePrinter::getSubsystem()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getSubsystem();
    }

    bool ConfigOnlinePrinter::getIsAlarm()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getIsAlarm();
    }

    std::string ConfigOnlinePrinter::getName()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");

        std::ostringstream name;
		name << m_OnlinePrinterHelper->getPrinter();
        name << " - ";
		name << m_OnlinePrinterHelper->getSubsystem();

        return name.str();
    }
 
	
    void ConfigOnlinePrinter::invalidate()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");

        m_OnlinePrinterHelper->invalidate();
        m_OnlinePrinterChanges.clear();

    }


    void ConfigOnlinePrinter::deleteThisObject(bool cascade)
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL,"The OnlinePrinterHelper pointer is null.");

        // Delete the OnlinePrinter from the database
        m_OnlinePrinterHelper->deleteOnlinePrinter(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete OnlinePrinterHelper pointer m_OnlinePrinterHelper");

        delete m_OnlinePrinterHelper;
        m_OnlinePrinterHelper = NULL;

    }


    void ConfigOnlinePrinter::applyChanges()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL, "applyChanges() has been called after OnlinePrinter was deleted");

        m_OnlinePrinterHelper->writeOnlinePrinterData();

        m_OnlinePrinterChanges.clear();
    }


    void ConfigOnlinePrinter::updateOnlinePrinterChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateOnlinePrinterChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_OnlinePrinterChanges.find(name);

        if (matching != m_OnlinePrinterChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_OnlinePrinterChanges.erase(matching);
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
            m_OnlinePrinterChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
} // closes TA_Core


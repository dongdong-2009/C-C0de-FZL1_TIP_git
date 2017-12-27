/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigGlobalParameter.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * ConfigGlobalParameter is an implementation of IConfigGlobalParameter. It holds the data specific to a GlobalParameter entry
  * in the database, and allows read-write access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/src/ConfigGlobalParameter.h"
#include "core/data_access_interface/src/GlobalParameterHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    long ConfigGlobalParameter::s_nextAvailableIdentifier = 0;

    const std::string ConfigGlobalParameter::NAME           = "Name";
    const std::string ConfigGlobalParameter::VALUE          = "Value";
    const std::string ConfigGlobalParameter::SUBSYSTEMKEY   = "Subsystem Key";


    ConfigGlobalParameter::ConfigGlobalParameter()
        : m_GlobalParameterHelper( new GlobalParameterHelper() ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigGlobalParameter::ConfigGlobalParameter( const unsigned long key )
		: m_GlobalParameterHelper( new GlobalParameterHelper(key) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

	ConfigGlobalParameter::ConfigGlobalParameter(const unsigned long row, TA_Base_Core::IData& data)
		: m_GlobalParameterHelper( new GlobalParameterHelper(row, data) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

	ConfigGlobalParameter::ConfigGlobalParameter( const ConfigGlobalParameter& theGlobalParameter)
        : m_GlobalParameterHelper( new GlobalParameterHelper(*(theGlobalParameter.m_GlobalParameterHelper)) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
	}


    ConfigGlobalParameter::~ConfigGlobalParameter()
    {
        try
        {
            if (m_GlobalParameterHelper != NULL)
            {
                delete m_GlobalParameterHelper;
                m_GlobalParameterHelper = NULL;
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigGlobalParameter::isNew()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->isNew();
    }


    unsigned long ConfigGlobalParameter::getKey()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getKey();
    }


    std::string ConfigGlobalParameter::getName()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getName();
    }


	std::string ConfigGlobalParameter::getValue()
	{
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getValue();
	}


	unsigned long ConfigGlobalParameter::getSubsystemKey()
	{
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getSubsystemKey();
	}


    time_t ConfigGlobalParameter::getDateCreated()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getDateCreated();
    }


    time_t ConfigGlobalParameter::getDateModified()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getDateModified();
    }


    void ConfigGlobalParameter::invalidate()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        m_GlobalParameterHelper->invalidate();
        m_GlobalParameterChanges.clear();
    }


    void ConfigGlobalParameter::applyChanges()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        m_GlobalParameterHelper->applyChanges();

        m_GlobalParameterChanges.clear();
    }

    
    void ConfigGlobalParameter::deleteThisGlobalParameter()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        m_GlobalParameterHelper->deleteThisGlobalParameter();
    }


    void ConfigGlobalParameter::setName(const std::string& name)
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        updateGlobalParameterChanges(NAME,m_GlobalParameterHelper->getName(), name);

        m_GlobalParameterHelper->setName(name);
    }


    void ConfigGlobalParameter::setValue(const std::string& value)
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        updateGlobalParameterChanges(VALUE,m_GlobalParameterHelper->getValue(), value);

        m_GlobalParameterHelper->setValue(value);
    }


    void ConfigGlobalParameter::setSubsystemKey(unsigned long subsystemKey)
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        char buf1[64] = {0}, buf2[64] = {0};
        sprintf(buf1,"%ul", m_GlobalParameterHelper->getSubsystemKey());
        sprintf(buf2,"%ul", subsystemKey);

        updateGlobalParameterChanges(SUBSYSTEMKEY,buf1, buf2);

        m_GlobalParameterHelper->setSubsystemKey(subsystemKey);
    }


    void ConfigGlobalParameter::updateGlobalParameterChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateGlobalParameterChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_GlobalParameterChanges.find(name);

        if (matching != m_GlobalParameterChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_GlobalParameterChanges.erase(matching);
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
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_GlobalParameterChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

} // closes TA_Base_Core

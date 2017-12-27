/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigEquipReport.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/src/ConfigEquipReport.h"
#include "core/data_access_interface/src/EquipReportHelper.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	long ConfigEquipReport::s_nextAvailableIdentifier = 0;

    const std::string	ConfigEquipReport::NAME				= "Name";
    const std::string	ConfigEquipReport::LOCATIONID		= "0";
	const std::string	ConfigEquipReport::ASSETFILTER		= "";
	const std::string	ConfigEquipReport::ASSETFILTERFLAG	= "0";

    ConfigEquipReport::ConfigEquipReport()
        : m_EquipReportHelper( new EquipReportHelper() ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigEquipReport::ConfigEquipReport( std::string& key )
		: m_EquipReportHelper( new EquipReportHelper(key) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


	ConfigEquipReport::ConfigEquipReport( const ConfigEquipReport& theEquipReport)
        : m_EquipReportHelper( new EquipReportHelper(*(theEquipReport.m_EquipReportHelper)) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
	}


	ConfigEquipReport::ConfigEquipReport(	std::string& key, 
											const std::string& name, 
											const unsigned long locationId)
		: m_EquipReportHelper( new EquipReportHelper(key, name, locationId) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
	}

	ConfigEquipReport::ConfigEquipReport(const unsigned long row, TA_Base_Core::IData& data)
		: m_EquipReportHelper( new EquipReportHelper(row, data) ),
		  m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
	}	

    ConfigEquipReport::~ConfigEquipReport()
    {
        if (m_EquipReportHelper != NULL)
        {
            delete m_EquipReportHelper;
            m_EquipReportHelper = NULL;
        }
    }


    bool ConfigEquipReport::isNew()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->isNew();
    }

    std::string  ConfigEquipReport::getId()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getId();
    }
    
    unsigned long ConfigEquipReport::getKey()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getKey();
    }


    std::string ConfigEquipReport::getName()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getName();
    }

	std::string ConfigEquipReport::getReportTypeFilter()
    {        
        return m_EquipReportHelper->getReportTypeFilter();
    }

	
	bool ConfigEquipReport::getReportTypeFilterFlag()
    {        
        return m_EquipReportHelper->getReportTypeFilterFlag();
    }


	std::string ConfigEquipReport::getAssetFilter()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getAssetFilter();
    }

	
	bool ConfigEquipReport::getAssetFilterFlag()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getAssetFilterFlag();
    }

	std::string ConfigEquipReport::getDescFilter()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getDescFilter();
    }

	
	bool ConfigEquipReport::getDescFilterFlag()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getDescFilterFlag();
    }

	std::string ConfigEquipReport::getDPNameFilter()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getDPNameFilter();
    }

	
	bool ConfigEquipReport::getDPNameFilterFlag()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getDPNameFilterFlag();
    }

	std::string ConfigEquipReport::getDPTypeFilter()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getDPTypeFilter();
    }

	
	bool ConfigEquipReport::getDPTypeFilterFlag()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getDPTypeFilterFlag();
    }

	std::string ConfigEquipReport::getDPValueFilter()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getDPValueFilter();
    }

	
	bool ConfigEquipReport::getDPValueFilterFlag()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getDPValueFilterFlag();
    }

	bool ConfigEquipReport::getScanInhibitFlag()
	{
		return m_EquipReportHelper->getScanInhibitFlag();
	}
	
	bool ConfigEquipReport::getAlarmInhibitFlag()
	{
		return m_EquipReportHelper->getAlarmInhibitFlag();
	}

	bool ConfigEquipReport::getControlInhibitFlag()
	{
		return m_EquipReportHelper->getControlInhibitFlag();
	}

	bool ConfigEquipReport::getMmsInhibitFlag()
	{
		return m_EquipReportHelper->getMmsInhibitFlag();
	}

	bool ConfigEquipReport::getTaggedFlag()
	{
		return m_EquipReportHelper->getTaggedFlag();
	}

	bool ConfigEquipReport::getTaggedPTWFlag()
	{
		return m_EquipReportHelper->getTaggedPTWFlag();
	}

	bool ConfigEquipReport::getOverriddenFlag()
	{
		return m_EquipReportHelper->getOverriddenFlag();
	}

	bool ConfigEquipReport::getStatusFilterFlag()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getStatusFilterFlag();
    }

	std::string ConfigEquipReport::getStatusChangeByFilter()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getStatusChangeByFilter();
    }

	
	bool ConfigEquipReport::getStatusChangeByFilterFlag()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getStatusChangeByFilterFlag();
    }

	std::string ConfigEquipReport::getTimestampFilter()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getTimestampFilter();
    }

	
	bool ConfigEquipReport::getTimestampFilterFlag()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getTimestampFilterFlag();
    }

	unsigned long ConfigEquipReport::getLocationId()
	{
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getLocationId();
	}

    unsigned long ConfigEquipReport::getProfile()
	{
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportHelper->getProfile();
	}

    void ConfigEquipReport::invalidate()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        m_EquipReportHelper->invalidate();
        m_EquipReportChanges.clear();
    }


    void ConfigEquipReport::applyChanges()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        m_EquipReportHelper->applyChanges();
        m_EquipReportChanges.clear();
    }

    
    void ConfigEquipReport::deleteThisEquipReport()
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        m_EquipReportHelper->deleteThisEquipReport();
    }


    void ConfigEquipReport::setName(const std::string& name)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        updateEquipReportChanges(NAME, m_EquipReportHelper->getName(), name);

        m_EquipReportHelper->setName(name);
    }

	void ConfigEquipReport::setReportTypeFilter(const std::string& reportFilter)
    {        
        m_EquipReportHelper->setReportTypeFilter(reportFilter);
    }

	void ConfigEquipReport::setReportTypeFilterFlag(const bool flag)
    {
        m_EquipReportHelper->setReportTypeFilterFlag(flag);
    }


	void ConfigEquipReport::setAssetFilter(const std::string& assetFilter)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        updateEquipReportChanges(ASSETFILTER, m_EquipReportHelper->getAssetFilter(), assetFilter);

        m_EquipReportHelper->setAssetFilter(assetFilter);
    }

	void ConfigEquipReport::setAssetFilterFlag(const bool flag)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

//        updateEquipReportChanges(ASSETFILTERFLAG, m_EquipReportHelper->getAssetFilterFlag(), assetFilterFlag);

        m_EquipReportHelper->setAssetFilterFlag(flag);
    }

	void ConfigEquipReport::setDescFilter(const std::string& descFilter)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        //updateEquipReportChanges(ASSETFILTER, m_EquipReportHelper->getAssetFilter(), assetFilter);

        m_EquipReportHelper->setDescFilter(descFilter);
    }

	void ConfigEquipReport::setDescFilterFlag(const bool flag)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

//        updateEquipReportChanges(ASSETFILTERFLAG, m_EquipReportHelper->getAssetFilterFlag(), assetFilterFlag);

        m_EquipReportHelper->setDescFilterFlag(flag);
    }

	void ConfigEquipReport::setDPNameFilter(const std::string& dpNameFilter)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        //updateEquipReportChanges(ASSETFILTER, m_EquipReportHelper->getAssetFilter(), assetFilter);

        m_EquipReportHelper->setDPNameFilter(dpNameFilter);
    }

	void ConfigEquipReport::setDPNameFilterFlag(const bool flag)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

//        updateEquipReportChanges(ASSETFILTERFLAG, m_EquipReportHelper->getAssetFilterFlag(), assetFilterFlag);

        m_EquipReportHelper->setDPNameFilterFlag(flag);
    }

	void ConfigEquipReport::setDPTypeFilter(const std::string& dpTypeFilter)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        //updateEquipReportChanges(ASSETFILTER, m_EquipReportHelper->getAssetFilter(), assetFilter);

        m_EquipReportHelper->setDPTypeFilter(dpTypeFilter);
    }

	void ConfigEquipReport::setDPTypeFilterFlag(const bool flag)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

//        updateEquipReportChanges(ASSETFILTERFLAG, m_EquipReportHelper->getAssetFilterFlag(), assetFilterFlag);

        m_EquipReportHelper->setDPTypeFilterFlag(flag);
    }

	void ConfigEquipReport::setDPValueFilter(const std::string& dpValueFilter)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        //updateEquipReportChanges(ASSETFILTER, m_EquipReportHelper->getAssetFilter(), assetFilter);

        m_EquipReportHelper->setDPValueFilter(dpValueFilter);
    }

	void ConfigEquipReport::setDPValueFilterFlag(const bool flag)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

//        updateEquipReportChanges(ASSETFILTERFLAG, m_EquipReportHelper->getAssetFilterFlag(), assetFilterFlag);

        m_EquipReportHelper->setDPValueFilterFlag(flag);
    }
	
	void ConfigEquipReport::setStatusFilterFlag(const bool flag)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

//        updateEquipReportChanges(ASSETFILTERFLAG, m_EquipReportHelper->getAssetFilterFlag(), assetFilterFlag);

        m_EquipReportHelper->setStatusFilterFlag(flag);
    }

	void ConfigEquipReport::setScanInhibitFlag(const bool flag)
    {
        m_EquipReportHelper->setScanInhibitFlag(flag);
    }

	void ConfigEquipReport::setAlarmInhibitFlag(const bool flag)
    {
        m_EquipReportHelper->setAlarmInhibitFlag(flag);
    }

	void ConfigEquipReport::setControlInhibitFlag(const bool flag)
    {
        m_EquipReportHelper->setControlInhibitFlag(flag);
    }

	void ConfigEquipReport::setMmsInhibitFlag(const bool flag)
    {
        m_EquipReportHelper->setMmsInhibitFlag(flag);
    }

	void ConfigEquipReport::setTaggedFlag(const bool flag)
    {
        m_EquipReportHelper->setTaggedFlag(flag);
    }

	void ConfigEquipReport::setTaggedPTWFlag(const bool flag)
    {
        m_EquipReportHelper->setTaggedPTWFlag(flag);
    }

	void ConfigEquipReport::setOverriddenFlag(const bool flag)
    {
        m_EquipReportHelper->setOverriddenFlag(flag);
    }

	void ConfigEquipReport::setStatusChangeByFilter(const std::string& statusFilter)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        //updateEquipReportChanges(ASSETFILTER, m_EquipReportHelper->getAssetFilter(), assetFilter);

        m_EquipReportHelper->setStatusChangeByFilter(statusFilter);
    }

	void ConfigEquipReport::setStatusChangeByFilterFlag(const bool flag)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

//        updateEquipReportChanges(ASSETFILTERFLAG, m_EquipReportHelper->getAssetFilterFlag(), assetFilterFlag);

        m_EquipReportHelper->setStatusChangeByFilterFlag(flag);
    }

	void ConfigEquipReport::setTimestampFilter(const std::string& timestampFilter)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

        //updateEquipReportChanges(ASSETFILTER, m_EquipReportHelper->getAssetFilter(), assetFilter);

        m_EquipReportHelper->setTimestampFilter(timestampFilter);
    }

	void ConfigEquipReport::setTimestampFilterFlag(const bool flag)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

//        updateEquipReportChanges(ASSETFILTERFLAG, m_EquipReportHelper->getAssetFilterFlag(), assetFilterFlag);

        m_EquipReportHelper->setTimestampFilterFlag(flag);
    }

    void ConfigEquipReport::setLocationId(const unsigned long locationId)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

 //       updateEquipReportChanges(LOCATIONID, "" << m_EquipReportHelper->getLocationId(), "" << locationId);

        m_EquipReportHelper->setLocationId(locationId);
    }

	void ConfigEquipReport::setProfile(const unsigned long profile)
    {
        TA_ASSERT(m_EquipReportHelper != NULL, "The equipment report helper is NULL");

 //       updateEquipReportChanges(LOCATIONID, "" << m_EquipReportHelper->getLocationId(), "" << locationId);

        m_EquipReportHelper->setProfile(profile);
    }

    void ConfigEquipReport::updateEquipReportChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateLocationChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_EquipReportChanges.find(name);

        if (matching != m_EquipReportChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_EquipReportChanges.erase(matching);
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
            m_EquipReportChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

} //TA_Base_Core

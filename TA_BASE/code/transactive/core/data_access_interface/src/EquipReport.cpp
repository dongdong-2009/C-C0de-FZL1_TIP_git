/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EquipReport.cpp $
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

#include "core/data_access_interface/src/EquipReport.h"
#include "core/data_access_interface/src/EquipReportHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

	EquipReport::EquipReport(std::string& key)
		: m_EquipReportHelper(new EquipReportHelper(key))
    {
    }


	EquipReport::EquipReport(std::string& key, 
							 const std::string& name,
							 const unsigned long locationId)
		: m_EquipReportHelper(new EquipReportHelper(key, name, locationId))
    {
    }

	EquipReport::EquipReport(const unsigned long row, TA_Base_Core::IData& data)
		: m_EquipReportHelper(new EquipReportHelper(row, data))
    {
    }

	EquipReport::~EquipReport()
	{	
        if (m_EquipReportHelper != NULL)
        {
            delete m_EquipReportHelper;
            m_EquipReportHelper = NULL;
        }
	}


	void EquipReport::invalidate()
	{
        m_EquipReportHelper->invalidate();
    }

	// report type filter
	bool EquipReport::getReportTypeFilterFlag()
	{
		return m_EquipReportHelper->getReportTypeFilterFlag();
	}


	std::string EquipReport::getReportTypeFilter()
	{
		return m_EquipReportHelper->getReportTypeFilter();
	}


	// asset filter
	bool EquipReport::getAssetFilterFlag()
	{
		return m_EquipReportHelper->getAssetFilterFlag();
	}


	std::string EquipReport::getAssetFilter()
	{
		return m_EquipReportHelper->getAssetFilter();
	}

	// description filter
	bool EquipReport::getDescFilterFlag()
	{
		return m_EquipReportHelper->getDescFilterFlag();
	}


	std::string EquipReport::getDescFilter()
	{
		return m_EquipReportHelper->getDescFilter();
	}

	// Data Point Name filter
	bool EquipReport::getDPNameFilterFlag()
	{
		return m_EquipReportHelper->getDPNameFilterFlag();
	}


	std::string EquipReport::getDPNameFilter()
	{
		return m_EquipReportHelper->getDPNameFilter();
	}

	// Data Point Type filter
	bool EquipReport::getDPTypeFilterFlag()
	{
		return m_EquipReportHelper->getDPTypeFilterFlag();
	}


	std::string EquipReport::getDPTypeFilter()
	{
		return m_EquipReportHelper->getDPTypeFilter();
	}

	// Data Point Value filter
	bool EquipReport::getDPValueFilterFlag()
	{
		return m_EquipReportHelper->getDPValueFilterFlag();
	}


	std::string EquipReport::getDPValueFilter()
	{
		return m_EquipReportHelper->getDPValueFilter();
	}

	// Status filter
	bool EquipReport::getStatusFilterFlag()
	{
		return m_EquipReportHelper->getStatusFilterFlag();
	}


	bool EquipReport::getScanInhibitFlag()
	{
		return m_EquipReportHelper->getScanInhibitFlag();
	}

	bool EquipReport::getAlarmInhibitFlag()
	{
		return m_EquipReportHelper->getAlarmInhibitFlag();
	}

	bool EquipReport::getControlInhibitFlag()
	{
		return m_EquipReportHelper->getControlInhibitFlag();
	}

	bool EquipReport::getMmsInhibitFlag()
	{
		return m_EquipReportHelper->getMmsInhibitFlag();
	}

	bool EquipReport::getTaggedFlag()
	{
		return m_EquipReportHelper->getTaggedFlag();
	}

	bool EquipReport::getTaggedPTWFlag()
	{
		return m_EquipReportHelper->getTaggedPTWFlag();
	}

	bool EquipReport::getOverriddenFlag()
	{
		return m_EquipReportHelper->getOverriddenFlag();
	}

	// Status filter
	bool EquipReport::getStatusChangeByFilterFlag()
	{
		return m_EquipReportHelper->getStatusChangeByFilterFlag();
	}


	std::string EquipReport::getStatusChangeByFilter()
	{
		return m_EquipReportHelper->getStatusChangeByFilter();
	}



	// Timestamp filter
	bool EquipReport::getTimestampFilterFlag()
	{
		return m_EquipReportHelper->getTimestampFilterFlag();
	}


	std::string EquipReport::getTimestampFilter()
	{
		return m_EquipReportHelper->getTimestampFilter();
	}

	unsigned long EquipReport::getKey()
	{
		return m_EquipReportHelper->getKey();
	}

	std::string EquipReport::getId()
	{
		return m_EquipReportHelper->getId();
	}

	std::string EquipReport::getName()
	{
		return m_EquipReportHelper->getName();
	}

	unsigned long EquipReport::getProfile()
	{
		return m_EquipReportHelper->getProfile();
	}

	unsigned long EquipReport::getLocationId()
	{
        return m_EquipReportHelper->getLocationId();
	}
} // TA_Base_Core


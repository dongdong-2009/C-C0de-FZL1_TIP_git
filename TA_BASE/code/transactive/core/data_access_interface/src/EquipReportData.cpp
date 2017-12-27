/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EquipReportData.cpp $
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

#include "core/data_access_interface/src/EquipReportData.h"
#include "core/data_access_interface/src/EquipReportDataHelper.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

	EquipReportData::EquipReportData(std::string& key)
		: m_EquipReportDataHelper(new EquipReportDataHelper(key))
    {
    }


	EquipReportData::EquipReportData(std::string& key, 							 
									 std::string& reportKey)
		: m_EquipReportDataHelper(new EquipReportDataHelper(key, reportKey))
    {
    }

	EquipReportData::EquipReportData(const unsigned long row, TA_Base_Core::IData& data)
		: m_EquipReportDataHelper(new EquipReportDataHelper(row, data))
    {
    }

	EquipReportData::~EquipReportData()
	{	
        if (m_EquipReportDataHelper != NULL)
        {
			delete m_EquipReportDataHelper;
            m_EquipReportDataHelper = NULL;
        }
	}


	void EquipReportData::invalidate()
	{
        m_EquipReportDataHelper->invalidate();
    }

	
	unsigned long EquipReportData::getKey()
	{
		return m_EquipReportDataHelper->getKey();
	}

    std::string  EquipReportData::getId()
    {
		return m_EquipReportDataHelper->getId();
    }

    std::string EquipReportData::getReportKey()
	{
		return m_EquipReportDataHelper->getReportKey();
	}

	unsigned long EquipReportData::getDataNodeKey()
	{
		return m_EquipReportDataHelper->getDataNodeKey();
	}

	unsigned long EquipReportData::getDataPointKey()
	{
		return m_EquipReportDataHelper->getDataPointKey();
	}
	

} // TA_Base_Core


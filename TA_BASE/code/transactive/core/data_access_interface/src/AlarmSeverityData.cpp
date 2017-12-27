/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmSeverityData.cpp $
 * @author:  Andy Siow
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * AlarmSeverityData is an implementation of IAlarmSeverityData. It holds the data specific to a Alarm
 * severity entry in the database.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/src/AlarmSeverityData.h"
#include "core/data_access_interface/src/AlarmSeverityHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    AlarmSeverityData::AlarmSeverityData( const unsigned long key )
		:
        m_helper( new AlarmSeverityHelper(key) )
    {
    }

    AlarmSeverityData::AlarmSeverityData(const unsigned long row, TA_Base_Core::IData& data)
		:
        m_helper( new AlarmSeverityHelper(row, data) )
    {
    }

    AlarmSeverityData::~AlarmSeverityData()
    {
        try
        {
            delete m_helper;
            m_helper = NULL;
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "Unknown exception caught in destructor");
        }
    }


    unsigned long AlarmSeverityData::getKey()
    {
        return m_helper->getKey();
    }


    std::string AlarmSeverityData::getName()
    {
        return m_helper->getName();
    }


	unsigned long AlarmSeverityData::getSeverityColourKey(EColourType type)
	{
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

		return m_helper->getSeverityColourKey(type);
	}


    time_t AlarmSeverityData::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        return m_helper->getDateCreated();
    }


    time_t AlarmSeverityData::getDateModified()
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        return m_helper->getDateModified();

    }

    void AlarmSeverityData::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmSeverityData::invalidate" );
		m_helper->invalidate();
    }



} // closes TA_Base_Core

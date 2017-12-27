/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/AlarmPlanAssocData.cpp $
 * @author:  Cameron Rochester
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * AlarmPlanAssocData is an implementation of IAlarmPlanAssocData. It holds the data specific to 
 * alarm plan associations.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/plans/src/AlarmPlanAssocData.h"
#include "core/data_access_interface/plans/src/AlarmPlanAssocHelper.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    AlarmPlanAssocData::AlarmPlanAssocData(unsigned long key, 
			                               unsigned long entityKey,
			                               unsigned long entityTypeKey,
			                               unsigned long alarmType,
			                               const std::string& planPath,
                                           time_t dateCreated,
                                           time_t dateModified)
        : m_helper( new AlarmPlanAssocHelper(key,entityKey,entityTypeKey,alarmType,planPath,dateCreated,dateModified) )
    {
    }


    AlarmPlanAssocData::AlarmPlanAssocData( unsigned long key )
        : m_helper( new AlarmPlanAssocHelper(key) )
    {
    }


	AlarmPlanAssocData::~AlarmPlanAssocData( )
    {
		if ( NULL != m_helper )
		{
			delete m_helper;
			m_helper = NULL;
		}
    }


	unsigned long AlarmPlanAssocData::getKey()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getKey();
    }
	
	
	std::string AlarmPlanAssocData::getName()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getName();
    }

    
    unsigned long AlarmPlanAssocData::getAlarmType()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getAlarmType();
    }


	unsigned long AlarmPlanAssocData::getEntityKey()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getEntityKey();
    }


	unsigned long AlarmPlanAssocData::getEntityTypeKey()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getEntityTypeKey();
    }


	std::string AlarmPlanAssocData::getPlanPath()
	{
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        return m_helper->getPlanPath();
	}


	bool AlarmPlanAssocData::match( const unsigned long& entityKey, 
			const unsigned long& entityTypeKey,
			const unsigned long& alarmType)
	{
		TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");

        if( m_helper->getEntityKey() != 0 )
        {
            if( entityKey == m_helper->getEntityKey() )
            {
                if( m_helper->getAlarmType() != 0 )
                { 
                    if( alarmType == m_helper->getAlarmType() )
                    {
                        return true;
                    }
                }
                else
                {
                    return true;
                }
            }
            return false;
        }

        if( 0 != m_helper->getAlarmType())
        {
            if( m_helper->getAlarmType() ==  alarmType ) 
            {
                if( m_helper->getEntityTypeKey() != 0 )
                {
                    if( m_helper->getEntityTypeKey() == entityTypeKey )
                    {
                        return true;
                    }
                }
                else
                {
                    return true;
                }
            }
            return false;
        }

        if( m_helper->getEntityTypeKey() != 0 )
        {
            if( m_helper->getEntityTypeKey() == entityTypeKey )
            {
                return true;
            }
        }
        return false;
	}


    void AlarmPlanAssocData::invalidate()
    {
        TA_ASSERT(m_helper != NULL,"The AlarmPlanAssocHelper pointer is null.");
        m_helper->invalidate();
    }

} // closes TA_Base_Core



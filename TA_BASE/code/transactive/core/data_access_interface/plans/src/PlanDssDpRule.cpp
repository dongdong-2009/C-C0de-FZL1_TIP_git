 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/PlanDssDpRule.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PlanDssDpRule is an implementation of IPlanDssDpRule. It holds the data specific to an PlanDssDpRule entry
  * in the database, and allows read-only access to that data.
  *
  */


#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/plans/src/PlanDssDpRuleHelper.h"
#include "core/data_access_interface/plans/src/PlanDssDpRule.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    PlanDssDpRule::PlanDssDpRule(const unsigned long key) : 
    m_PlanDssDpRuleHelper ( new PlanDssDpRuleHelper(key))
    {
    }

    PlanDssDpRule::PlanDssDpRule(unsigned long row, TA_Base_Core::IData& data) : 
    m_PlanDssDpRuleHelper(new PlanDssDpRuleHelper(row, data))
    {
    }

    PlanDssDpRule::~PlanDssDpRule() 
	{
        delete m_PlanDssDpRuleHelper;
        m_PlanDssDpRuleHelper=NULL;
    }

    unsigned long PlanDssDpRule::getKey()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getKey();
    }

    unsigned long PlanDssDpRule::getLocationKey()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getLocationKey();
    }

    void PlanDssDpRule::invalidate()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        m_PlanDssDpRuleHelper->invalidate();
    }

    std::string PlanDssDpRule::getName()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        std::ostringstream name;
		name << "Plan DSS rule " << m_PlanDssDpRuleHelper->getKey();

        return name.str();
    }

    unsigned long PlanDssDpRule::getTriggerEntityKey()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getTriggerEntityKey();
    }

    unsigned long PlanDssDpRule::getActivePlanKey()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getActivePlanKey();
    }

    unsigned long PlanDssDpRule::getMinimumInterval()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getMinimumInterval();
    }

    unsigned long PlanDssDpRule::getTolerantTimeForActive()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getTolerantTimeForActive();
    }

    unsigned long PlanDssDpRule::getValidStartTimeInDay()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidStartTimeInDay();
    }

    unsigned long PlanDssDpRule::getValidEndTimeInDay()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidEndTimeInDay();
    }

    bool PlanDssDpRule::getValidSunday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidSunday();
    }

    bool PlanDssDpRule::getValidMonday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidMonday();
    }

    bool PlanDssDpRule::getValidTuesday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidTuesday();
    }

    bool PlanDssDpRule::getValidWednesday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidWednesday();
    }

    bool PlanDssDpRule::getValidThursday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidThursday();
    }

    bool PlanDssDpRule::getValidFriday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidFriday();
    }

    bool PlanDssDpRule::getValidSaturday()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getValidSaturday();
    }

    bool PlanDssDpRule::getActivateValue()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getActivateValue();
    }

    bool PlanDssDpRule::getNeedDeactivate()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getNeedDeactivate();
    }

    bool PlanDssDpRule::getAllowMultiple()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getAllowMultiple();
    }

    bool PlanDssDpRule::getEnable()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getEnable();
    }

    int PlanDssDpRule::getDenyMultipleStrategy()
    {
        if ( m_PlanDssDpRuleHelper == NULL )
        {
            DataException expWhat( "The PlanDssDpRuleHelper pointer is null.", DataException::INVALID_VALUE, "" );
            TA_THROW( expWhat );
        }

        return m_PlanDssDpRuleHelper->getDenyMultipleStrategy();
    }

} // closes TA_Base_Core

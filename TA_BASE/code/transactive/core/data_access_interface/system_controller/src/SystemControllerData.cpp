/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/SystemControllerData.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/10/18 11:29:58 $
  * Last modified by:  $Author: Ouyang $
  *
  * <description>
  *
  */
#include "SystemControllerData.h"
#include "SystemControllerHelper.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    SystemControllerData::SystemControllerData(unsigned long key)
    {
        m_helper = new SystemControllerHelper(key);
    }

    SystemControllerData::~SystemControllerData()
    {
        delete m_helper;
		m_helper = NULL;
    }

    unsigned long SystemControllerData::getKey()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getKey();
    }

    std::string SystemControllerData::getName()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getName();
    }

    unsigned long SystemControllerData::getAgentPollPeriod()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getAgentPollPeriod();
    }

    unsigned long SystemControllerData::getHeartbeatSendPeriod()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getHeartbeatSendPeriod();
    }

    unsigned long SystemControllerData::getHeartbeatSendNumber()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getHeartbeatSendNumber();
    }

    unsigned long SystemControllerData::getAgentResponseTimeout() 
    {
        TA_ASSERT(m_helper != NULL, "The SystemControllerHelper pointer is null.");
        return m_helper->getAgentResponseTimeout();
    }

    unsigned long SystemControllerData::getServerEntityKey()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getServerEntityKey();
    }

    unsigned long SystemControllerData::getGroupId()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getGroupId();
    }

    std::string SystemControllerData::getGroupByName()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getGroupName( m_helper->getGroupId() );
    }

    std::string SystemControllerData::getDebugLogDir()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getDebugLogDir();
    }

    std::string SystemControllerData::getBinDir()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        return m_helper->getBinDir();
    }

    void SystemControllerData::invalidate()
    {
        TA_ASSERT(m_helper != NULL,"The SystemControllerHelper pointer is null.");
        m_helper->invalidate();
    }


} // namespace TA_Base_Core

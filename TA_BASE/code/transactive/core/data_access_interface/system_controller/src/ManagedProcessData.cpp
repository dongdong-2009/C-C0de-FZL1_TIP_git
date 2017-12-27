/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/ManagedProcessData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#include "ManagedProcessData.h"
#include "ManagedProcessHelper.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    ManagedProcessData::ManagedProcessData(unsigned long systemControllerKey,
                                           unsigned long managedProcessKey)
    {
        m_helper = new ManagedProcessHelper(systemControllerKey, managedProcessKey);
    }

    ManagedProcessData::~ManagedProcessData()
    {
        delete m_helper;
        m_helper = NULL;
    }

    unsigned long ManagedProcessData::getSystemControllerKey()
    {
        TA_ASSERT(m_helper != NULL, "The ManagedProcessHelper pointer is null.");
        return m_helper->getSystemControllerKey();
    }

    unsigned long ManagedProcessData::getManagedProcessKey()
    {
        TA_ASSERT(m_helper != NULL, "The ManagedProcessHelper pointer is null.");
        return m_helper->getManagedProcessKey();
    }

    EOperationMode ManagedProcessData::getDefaultOperatingMode()
    {
        TA_ASSERT(m_helper != NULL, "The ManagedProcessHelper pointer is null.");
        return m_helper->getDefaultOperatingMode();
    }

	bool ManagedProcessData::isStandbyProcess()
	{
		TA_ASSERT(m_helper != NULL, "The ManagedProcessHelper pointer is null.");
		return m_helper->IsStandbyProcess();
	}
	bool ManagedProcessData::hasStandbyProcess()
	{
		TA_ASSERT(m_helper != NULL, "The ManagedProcessHelper pointer is null.");
		return m_helper->hasStandbyProcess();
	}
	std::string ManagedProcessData::getMonitorGroupName()
	{
		TA_ASSERT(m_helper != NULL, "The ManagedProcessHelper pointer is null.");
		return m_helper->getMonitorGroupName();
	}
	int ManagedProcessData::getStartLag()
	{
		TA_ASSERT(m_helper != NULL, "The ManagedProcessHelper pointer is null.");
		return m_helper->getStartLag();
	}
} // namespace TA_Base_Core

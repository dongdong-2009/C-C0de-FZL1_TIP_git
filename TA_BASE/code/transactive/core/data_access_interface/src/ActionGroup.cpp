/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ActionGroup.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 12:15:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * ActionGroup is an implementation of IActionGroup. It holds the data specific to an ActionGroup entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#pragma warning(disable:4284)
#endif

#include "core/data_access_interface/src/ActionGroup.h"
#include "core/data_access_interface/src/ActionGroupHelper.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    ActionGroup::ActionGroup( const unsigned long key, const std::string& name )
		: m_actionGroupHelper( new ActionGroupHelper(key,name) )
    {
    }

	ActionGroup::ActionGroup(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& actions)
		: m_actionGroupHelper( new ActionGroupHelper(row, data, actions) )
	{
	}

    ActionGroup::~ActionGroup()
    {
        try
        {
            if (m_actionGroupHelper != NULL)
            {
                delete m_actionGroupHelper;
                m_actionGroupHelper = NULL;
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    unsigned long ActionGroup::getKey()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->getKey();
    }


    std::string ActionGroup::getName()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->getName();
    }

    std::string ActionGroup::getDisplayName()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->getDisplayName();
    }

    bool ActionGroup::isControlType()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->isControlType();
    }


    void ActionGroup::invalidate()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        m_actionGroupHelper->invalidate();
    }

} // closes TA_Base_Core

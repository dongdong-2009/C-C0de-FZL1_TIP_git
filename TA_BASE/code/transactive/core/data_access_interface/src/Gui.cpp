/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Gui.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Gui is an implementation of IGui. It holds the data specific to an gui entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/src/Gui.h"
#include "core/data_access_interface/src/GuiHelper.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    Gui::Gui(const unsigned long key)
    {
        m_guiHelper = new GuiHelper(key);
    }

	Gui::Gui(const unsigned long row, TA_Base_Core::IData& data)
		: m_guiHelper( new GuiHelper(row, data) )
	{
	}

    Gui::~Gui()
    {
        try
        {
            if (m_guiHelper != NULL)
            {
                delete m_guiHelper;
                m_guiHelper=NULL;
            }
        }
        catch( ... )
        {
        }
    }


    unsigned long Gui::getKey()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        return m_guiHelper->getKey();
    }


    std::string Gui::getName()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        return m_guiHelper->getName();
    }


    std::string Gui::getFullName()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        return m_guiHelper->getFullName();
    }


    std::string Gui::getExecutable()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        return m_guiHelper->getExecutable();
    }


    std::string Gui::getHelpFilePath()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        return m_guiHelper->getHelpFilePath();
    }

    
    bool Gui::isIconDisplayed()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        return m_guiHelper->isIconDisplayed();
    }


    bool Gui::areInstancesLimited()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        return m_guiHelper->areInstancesLimited();
    }


    bool Gui::shouldReposition()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        return m_guiHelper->shouldReposition();
    }


    bool Gui::shouldPassParameters()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        return m_guiHelper->shouldPassParameters();
    }


    bool Gui::shouldTerminateOnLogout()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        return m_guiHelper->shouldTerminateOnLogout();
    }


    void Gui::invalidate()
    {
        TA_ASSERT(m_guiHelper != NULL,"The GuiHelper pointer is null.");

        m_guiHelper->invalidate();
    }

} // closes TA_Base_Core

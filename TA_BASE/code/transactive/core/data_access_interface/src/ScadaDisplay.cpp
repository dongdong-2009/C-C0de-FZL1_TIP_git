/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ScadaDisplay.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ScadaDisplay is an implementation of IScadaDisplay. It holds the data specific to a scada display
  * entry in the database and allows read-only access to that data.
  */

#include "core/data_access_interface/src/ScadaDisplay.h"
#include "core/data_access_interface/src/ScadaDisplayHelper.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    ScadaDisplay::ScadaDisplay(const unsigned long key)
		: m_displayHelper( new ScadaDisplayHelper(key) )
    {
    }

	ScadaDisplay::ScadaDisplay(const unsigned long row, TA_Base_Core::IData& data)
		: m_displayHelper( new ScadaDisplayHelper(row, data) )
	{
	}
    
    ScadaDisplay::~ScadaDisplay() 
	{
        delete m_displayHelper;
        m_displayHelper=NULL;
    }


    unsigned long ScadaDisplay::getKey()
    {
        TA_ASSERT(m_displayHelper != NULL,"The ScadaDisplayHelper pointer is null.");
        return m_displayHelper->getKey();
    }


    std::string ScadaDisplay::getName()
    {
        TA_ASSERT(m_displayHelper != NULL,"The ScadaDisplayHelper pointer is null.");
        return m_displayHelper->getName();
    }


    std::string ScadaDisplay::getPath()
    {
        TA_ASSERT(m_displayHelper != NULL,"The ScadaDisplayHelper pointer is null.");
        return m_displayHelper->getPath();
    }


	unsigned long ScadaDisplay::getLocation()
	{
        TA_ASSERT(m_displayHelper != NULL,"The ScadaDisplayHelper pointer is null.");
		return m_displayHelper->getLocation();
	}


    std::string ScadaDisplay::getLocationName()
    {
        TA_ASSERT(m_displayHelper != NULL,"The ScadaDisplayHelper pointer is null.");
		return m_displayHelper->getLocationName();
    }


    time_t ScadaDisplay::getDateCreated()
    {
        TA_ASSERT(m_displayHelper != NULL,"The ScadaDisplayHelper pointer is null.");
		return m_displayHelper->getDateCreated();
    }


    time_t ScadaDisplay::getDateModified()
    {
        TA_ASSERT(m_displayHelper != NULL,"The ScadaDisplayHelper pointer is null.");
		return m_displayHelper->getDateModified();
    }


    void ScadaDisplay::invalidate()
    {
        TA_ASSERT(m_displayHelper != NULL,"The ScadaDisplayHelper pointer is null.");
        m_displayHelper->invalidate();
    }

} // closes TA_Base_Core

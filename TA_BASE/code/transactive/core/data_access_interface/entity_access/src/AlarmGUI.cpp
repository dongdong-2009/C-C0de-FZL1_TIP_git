/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AlarmGUI.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/AlarmGUI.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    const std::string AlarmGUI::ENTITY_TYPE = "AlarmManager";
    const std::string AlarmGUI::m_alarmCloseWindowParameter = "WindowClose";
	const std::string AlarmGUI::m_exportAlarmDirPathParameter = "ExportFilePath";
	//TD17888 marc_bugfix
	const std::string AlarmGUI::m_exportDirectoryParameter = "ExportDirectory";

    AlarmGUI::AlarmGUI()
    {}

    AlarmGUI::AlarmGUI( unsigned long key ) :
		EntityData( key, getStaticType() )
    {
    }

    void AlarmGUI::invalidate()
    {
        getHelper()->invalidate();
    }

    AlarmGUI::~AlarmGUI()
    {
    }

    std::string AlarmGUI::getType()
    {
        return getStaticType();
    }

    std::string AlarmGUI::getStaticType()
    {
        return ENTITY_TYPE;
    }

    TA_Base_Core::IEntityData* AlarmGUI::clone(unsigned long key)
    {
        return( new AlarmGUI(key));
    }

    bool AlarmGUI::getWindowCloseOption()
    {

        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");

        std::string windowClose("");
		windowClose = getHelper()->getParameter(m_alarmCloseWindowParameter);
        
        if(windowClose == "1")
        {
            return true;
        }
        else if(windowClose == "0" || windowClose == "")
        {
            return false;
        }

        return false;
    }


	std::string AlarmGUI::getExportAlarmDirectory()
	{
		TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		return getHelper()->getParameter(m_exportAlarmDirPathParameter);
	}
	
	std::string AlarmGUI::getExportDirectory()
	{
		TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		return getHelper()->getParameter(m_exportDirectoryParameter);
	}
	
}

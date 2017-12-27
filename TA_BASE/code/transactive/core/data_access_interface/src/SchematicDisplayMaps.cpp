/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/src/SchematicDisplayMaps.cpp $
* @author:  Karen Graham
* @version: $Revision: #5 $
*
* Last modification: $DateTime: 2011/11/16 18:23:22 $
* Last modified by:  $Author: hui.wang $
* 
* ReportFile is an implementation of IReportFile. It holds the data specific to an ReportFile entry
* in the database, and allows read-only access to that data.
*/

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/SchematicDisplayMaps.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/uuid/src/TAuuid.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_Core
{
	SchematicDisplayMaps::SchematicDisplayMaps( int nSubTypeID, const std::string& strStartupDisplay, const std::string& strSubTypeLable )
	{
		m_nSubTypeIDValue = 0;
		m_strStartupDisplayValue.clear();
		m_strSubTypeLableValue.clear();

		m_nSubTypeIDValue = nSubTypeID;
		m_strStartupDisplayValue = strStartupDisplay;
		m_strSubTypeLableValue = strSubTypeLable;
	}

	SchematicDisplayMaps::~SchematicDisplayMaps()
	{
		m_nSubTypeIDValue = 0;
		m_strStartupDisplayValue.clear();
		m_strSubTypeLableValue.clear();
	}


	int SchematicDisplayMaps::getSubTypeID()
	{
		return m_nSubTypeIDValue;
	}

	std::string SchematicDisplayMaps::getStartUpDisPlay()
	{
		return m_strStartupDisplayValue;
	}

	std::string SchematicDisplayMaps::getSubTypeLabel()
	{
		return m_strSubTypeLableValue;
	}		

} // closes TA_Base_Core

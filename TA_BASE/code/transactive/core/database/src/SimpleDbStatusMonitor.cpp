/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/SimpleDbStatusMonitor.cpp $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
* <description>
*
*/
//
// SimpleDbStatusMonitor.cpp
//
// Wrapper for the OCIAccessStatusMonitor class which (hopefully) takes all the pain
// out of database access. 
//
// This is created for use in SimpleDbStatusChecker. The current intplementation of SimpleDb is unfit for
// Database status checking since we need a faster way to determine when database connection is down
//


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "DBException.h"
#include "SimpleDbStatusMonitor.h"
#include "OCIAccessStatusMonitor.h"

using TA_Base_Core::DebugUtil;
namespace TA_Base_Core
{
	
	SimpleDbStatusMonitor::SimpleDbStatusMonitor(const char* pszDbType,
		const char* connection, 
		const char* user, 
		const char* passwd,
		const char* hostname)
		: SimpleDb(pszDbType, connection, user, passwd, hostname)
	{
		FUNCTION_ENTRY("SimpleDbStatusMonitor");
		
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
			"SimpleDbStatusMonitor::SimpleDbStatusMonitor(): connection=%s, user=%s, passwd=%s, hostname=%s", connection, user, passwd, hostname);
		
		// Create a string from the database connection details.
		// This will be used to identify the OCIConnection object.
		m_connectionStr = pszDbType;
		
		m_connectionStr += ":";
		
		m_connectionStr += connection;
		
		m_connectionStr += ":";
		
		m_connectionStr += user;
		
		m_connectionStr += ":";
		
		m_connectionStr += passwd;
		
		m_connectionStr += ":";
		
		m_connectionStr += hostname;
		
		// The final parameter is false to indicate that the connection is
		// not being created as part of auto-reconnect.
		OCIAccessStatusMonitor::getInstance().createConnection(m_connectionStr, pszDbType, connection, user, passwd, hostname, false );
		
		FUNCTION_EXIT;
	}
	
	SimpleDbStatusMonitor::~SimpleDbStatusMonitor()
	{
		FUNCTION_ENTRY("~SimpleDbStatusMonitor");

		FUNCTION_EXIT;
	}
	
	void SimpleDbStatusMonitor::Open()
	{
		FUNCTION_ENTRY("Open");
		
		OCIAccessStatusMonitor::getInstance().open( m_connectionStr );
		
		FUNCTION_EXIT;
	}
	
	void SimpleDbStatusMonitor::Close()
	{
		FUNCTION_ENTRY("Close");
		
		OCIAccessStatusMonitor::getInstance().close( m_connectionStr );
		
		FUNCTION_EXIT;
	}
	
	void SimpleDbStatusMonitor::CloseAll()
	{
		FUNCTION_ENTRY("CloseAll");
		
		OCIAccessStatusMonitor::getInstance().closeAll( m_connectionStr );
		
		FUNCTION_EXIT;
	}
	
	void SimpleDbStatusMonitor::ParseConnectStr(const std::string& str,                  // IN
		std::vector<std::string>& components,    // OUT
		char delimiter)                          // IN
	{
		//reuse parsing of connection string by oci access
		SimpleDb::ParseConnectStr( str, components, delimiter );
	}
	
}
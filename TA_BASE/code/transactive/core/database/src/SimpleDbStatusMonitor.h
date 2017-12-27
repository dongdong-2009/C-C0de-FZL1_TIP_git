/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/SimpleDbStatusMonitor.h $
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
// SimpleDbStatusMonitor.h
//
// Wrapper for the OCIAccess class which (hopefully) takes all the pain
// out of database access. Note that it is probably not very efficient.
//
// Author: Bruce Fountain 24-Jan-2001
//

#ifndef SIMPLEDBSTATUSMONITOR_H_INCLUDED
#define SIMPLEDBSTATUSMONITOR_H_INCLUDED

#if defined( WIN32 )
#pragma warning ( disable : 4786 )
#endif // defined( WIN32 )

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>
#include <string>
#include "SimpleDb.h" //reuse all functionalities that can be used in simple db

namespace TA_Base_Core
{

	class SimpleDbStatusMonitor: public TA_Base_Core::SimpleDb
	{
	public:
		// You must specify the connection on construction, because this is
		// required by OCIAccess. The connection string is the name assigned
		// to your connection via Net8EasyConfig (eg "TCMS")
		//
		// NOTE: the hostname parameter is optional on Windows, but required
		//       on Solaris
		//
		// Bruce Fountain (21-Mar-2001): actually, I don't think the hostname
		//      is required at all if the listener is configured properly on
		//      solaris
		SimpleDbStatusMonitor(const char* pszDbType, const char* connection, const char* user, const char* passwd,
			const char* hostname = "");
		virtual ~SimpleDbStatusMonitor();
		
		// A useful method to parse a colon-separated database connection
		// string (eg "TCMS:COM:COM") and return a vector of the component
		// strings
		static void ParseConnectStr(const std::string& str,					// IN
			std::vector<std::string>& components,   // OUT
			char delimiter = ':');                  // IN
		
		// You MUST open the connection before calling Exec(), otherwise you
		// will get an assertion. You can close the connnection yourself or
		// let the destructor do this
		virtual void Open();
		void Close();
		virtual void CloseAll();
		

	private:
		
		std::string m_connectionStr;
	};
	
}
#endif // SIMPLEDBSTATUSMONITOR_H_INCLUDED



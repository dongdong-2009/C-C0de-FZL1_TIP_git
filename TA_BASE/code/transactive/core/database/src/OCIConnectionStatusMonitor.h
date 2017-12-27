/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/OCIConnectionStatusMonitor.h $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
* This class is used by OCIAccess to store database connection 
* information and maintain the associated connection handles.  
* If the database automatically reconnects a database with the same name, username and 
* password, this class will connect to the new database on the first exec
* failure.
* 
*/

#ifndef OCICONNECTIONSTATUSMONITOR_H
#define OCICONNECTIONSTATUSMONITOR_H

#ifdef WIN32
#pragma warning(disable: 4290) // C++ exception specification ignored
#endif // WIN32

#include <string>
#include <omnithread.h>  // This must be included before oci.h to avoid redefinition errors.
// It is usually located in $ORACLE_HOME/rdbms/demo
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
//#include "OCIAccess.h"
#include "DBException.h"
#include "OCIConnection.h"

namespace TA_Base_Core
{

	class OCIConnectionStatusMonitor: private OCIConnection
	{
	public:
		
    /**
	* OCIConnectionStatusMonitor
	* 
	* Sole constructor that stores the database connection details and 
	* allocates handles to perform transactions with the database.
	* Note: this method can be unsuccessful, so checkIsValid() should be 
	* called afterwards to check that construction was successful.
	* 
	* @param connectionStr databaseName:userName:password:hostName
	* @param databaseName  the name of the database to connect to
	* @param userName      the name of the user for the session
	* @param password      the name of the password for the session
	* @param hostName      the name of the host that the database is on
		*/
		OCIConnectionStatusMonitor( const std::string& connectionStr, 
			const std::string& databaseName, 
			const std::string& userName, 
			const std::string& password, 
			const std::string& hostName = "" );
		
		
			/**
			* ~OCIConnectionStatusMonitor
			* 
			* Sole standard destructor. Frees the handles allocated in the constructor.
		*/
		virtual ~OCIConnectionStatusMonitor();
		
	};
	
}

#endif // OCICONNECTIONSTATUSMONITOR_H




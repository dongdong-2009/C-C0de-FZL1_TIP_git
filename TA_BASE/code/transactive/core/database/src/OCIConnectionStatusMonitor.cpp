/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/OCIConnectionStatusMonitor.cpp $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
* This class is used by OCIAccess to store database connection 
* information and maintain the associated connection handles.  
* If the database automatically reconnects to a database with the same name, username and 
* password, this class will connect to the new database on the first exec
* failure.
* 
*/

#if defined( WIN32 )
#pragma warning ( disable : 4786 )
#endif // defined( WIN32 )

#include "core/synchronisation/src/ThreadGuard.h"
#include "OCIConnection.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "OCIConnectionStatusMonitor.h"

namespace TA_Base_Core
{
	using TA_Base_Core::ThreadGuard;
	using TA_Base_Core::DebugUtil;
	using TA_Base_Core::Thread;
	
	////////////////////////////////////////////////////////////////////////////////
	//                                                                            //
	//                         Construction/Destruction                           //
	//                                                                            //
	////////////////////////////////////////////////////////////////////////////////
	
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
	OCIConnectionStatusMonitor::OCIConnectionStatusMonitor( const std::string& connectionStr, 
		const std::string& databaseName, 
		const std::string& userName, 
		const std::string& password, 
		const std::string& hostName /*= ""*/ )
		: OCIConnection(connectionStr, databaseName, userName, password, hostName)
	{

	}
	
	
	/**
	* ~OCIConnectionStatusMonitor
	* 
	* Sole standard destructor.  Frees the handles allocated in the constructor.
	*/
	OCIConnectionStatusMonitor::~OCIConnectionStatusMonitor()
	{

	}

}



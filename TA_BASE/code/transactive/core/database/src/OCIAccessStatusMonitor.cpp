/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/OCIAccessStatusMonitor.cpp $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
* OCIAccess is a singleton that manages all database connections
* using the ORACLE OCI interface.
*/

#if defined( WIN32 )
#pragma warning ( disable : 4786 )
#endif // defined( WIN32 )

#include <sstream>
#include <time.h>
#include <algorithm>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "OCIConnection.h"
#include "ConnectionReaper.h"
#include "OCIAccessStatusMonitor.h"
#include "OCIConnectionStatusMonitor.h"

namespace TA_Base_Core
{
	
	
	using TA_Base_Core::ThreadGuard;
	using TA_Base_Core::DebugUtil;
	
	// Definition of static member variables.
	TA_Base_Core::ReEntrantThreadLockable OCIAccessStatusMonitor::m_sLock;
	OCIAccessStatusMonitor* OCIAccessStatusMonitor::m_instance = NULL;
	
	
	////////////////////////////////////////////////////////////////////////////////
	//                                                                            //
	//                         Construction/Destruction                           //
	//                                                                            //
	////////////////////////////////////////////////////////////////////////////////
	
	
	/**
	* getInstance
	* 
	* OCIAccessStatusMonitor is a singleton, so this method returns a reference to the active
	* OCIAccessStatusMonitor object and creates a new OCIAccessStatusMonitor if one does not exist already.
	*
	* @return  a reference to the active OCIAccessStatusMonitor singleton object
	*/
	OCIAccessStatusMonitor& OCIAccessStatusMonitor::getInstance()
	{
		// TD11793: The OCIAccessStatusMonitor instance was being deleted while it was still in use
		// with the old singleton implementation. Using a lock and a heap bound object
		// should avoid this situation.
		TA_Base_Core::ThreadGuard guard( m_sLock );
		
		if ( NULL == m_instance )
		{
			m_instance = new OCIAccessStatusMonitor();
		}
		
		return *m_instance;
	}
	
	/**
	* removeInstance
	* 
	* OCIAccessStatusMonitor is a singleton, so it wont delete itself
	* this should be called in the cleanup of your app (called by generic gui and generic agent)
	*
	*/
	void OCIAccessStatusMonitor::removeInstance()
	{
		TA_Base_Core::ThreadGuard guard( m_sLock );
		
		if ( m_instance != NULL )
		{
			delete m_instance;
			m_instance = 0;
		}
	}
	
	/**
	* OCIAccessStatusMonitor
	* 
	* Sole constructor.  This class is a singleton, hence the constructor
	* is private.  Use getInstance() to obtain a reference to the OCIAccessStatusMonitor
	* object.
	*/
	OCIAccessStatusMonitor::OCIAccessStatusMonitor()
	{
		//no need for a reaper as connection will be closed and opened
	}
	
	/**
	* ~OCIAccessStatusMonitor
	* 
	* Sole destructor that deletes all of the OCIConnection objects.
	*/
	OCIAccessStatusMonitor::~OCIAccessStatusMonitor()
	{

	}
	
	
}
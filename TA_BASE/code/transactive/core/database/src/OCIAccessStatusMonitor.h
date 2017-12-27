/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/OCIAccessStatusMonitor.h $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
* OCIAccess is a singleton that manages all database connections
* using the ORACLE OCI interface.
*/

#ifndef OCIACCESSSTATUSMONITOR_H
#define OCIACCESSSTATUSMONITOR_H

#if defined( WIN32 )
#pragma warning ( disable : 4786 )
#endif // defined( WIN32 )

#include <string>
#include <vector>
#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "CommonType.h"
#include "SimpleDb.h"
#include "DbServerLocator.h"



namespace TA_Base_Core
{    
	class OCIConnection;
	class OCIConnectionStatusMonitor;
	
	class OCIAccessStatusMonitor : public DbServerLocator
	{
	public:
		
    /**
	* getInstance
	* 
	* OCIAccessStatusMonitor is a singleton, so this method returns a reference to the 
	* active OCIAccessStatusMonitor object and creates a new OCIAccessStatusMonitor if one does 
	* not exist already.
	*
	* @return  a reference to the active OCIAccessStatusMonitor singleton object
		*/
		static OCIAccessStatusMonitor& getInstance();
		
		
		/**
		* removeInstance
		* 
		* OCIAccessStatusMonitor is a singleton, so it wont delete itself
		* this should be called in the cleanup of your app (called by generic gui and generic agent)
		*
		*/
		static void removeInstance();
		
		
		/**
		* ~OCIAccessStatusMonitor
		* 
		* Sole destructor that deletes all of the OCIConnection objects.
		*/
		virtual ~OCIAccessStatusMonitor();
	private:
		
		/**
		* OCIAccessStatusMonitor
		* 
		* Sole constructor.  This class is a singleton, hence the constructor
		* is private.  Use getInstance() to obtain a reference to the OCIAccessStatusMonitor
		* object.
		*/
		OCIAccessStatusMonitor();
		
		// Variables for implementing the singleton.
		static OCIAccessStatusMonitor* m_instance;
		static TA_Base_Core::ReEntrantThreadLockable m_sLock;
	};
}

#endif // OCIACCESSSTATUSMONITOR_H




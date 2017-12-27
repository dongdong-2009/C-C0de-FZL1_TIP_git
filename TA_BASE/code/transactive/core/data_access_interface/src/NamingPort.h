// NamingPort.h: interface for the NamingPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAMINGPORT_H__B22EFC47_AFBF_417A_90FE_000123456789__INCLUDED_)
#define AFX_NAMINGPORT_H__B22EFC47_AFBF_417A_90FE_000123456789__INCLUDED_

/**
  * 
  * NamingPort is an implementation of INamingPort. It holds the data specific to a agent entry
  * in the database, and allows read-only access to that data.
  *
  * This class will access to the following DB tables: SN_SYSTEM_CONTROLLER, SN_MANAGED_PROCESS, ENTITY, NAMING_PORTS.
  * This class is added for CL-21230
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/src/INamingPort.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class NamingPort : public INamingPort
	{
	public:
		// constructor
		NamingPort(const std::string agentName);
		/**
		 * getName
		 *
		 * Returns the Agent Name
		 *
		 * @return The agent name as std::string
		 */
		virtual std::string getName();

		/**
		 * getPrimaryPort
		 *
		 * Returns the primary naming service port of the Agent which default operating mode is configured to CONTROL
		 *
		 * @return The port of naming service as an unsigned long
		 */
		virtual unsigned long getPrimaryPort();

		/**
		 * getSecondaryPort
		 *
		 * Returns the primary naming service port of the Agent which default operating mode is configured to MONITOR
		 *
		 * @return The port of naming service as an unsigned long
		 */
		virtual unsigned long getSecondaryPort();

		/**
		 * getPrimaryHostname
		 *
		 * Returns the primary host name where the default control mode agent is running in.
		 *
		 * @return The primary host name as std::string
		 */
		virtual std::string getPrimaryHostname();

		/**
		 * getSecondaryHostname
		 *
		 * Returns the primary host name where the default monitor mode agent is running in.
		 *
		 * @return The primary host name as std::string
		 */
		virtual std::string getSecondaryHostname();

		/**
		 * getNsPort
		 *
		 * Returns the naming service port for the specified hostname and agent name
		 *
		 * @param hostname - the hostname in which the agent running
		 *
		 * @return The naming service port as unsigned long
		 */
		virtual unsigned long getNsPort(const std::string hostname);


	private:
		void loadDataFromDb();

		bool m_isValidData;
		std::string m_agentName;
		std::string m_primaryHostname;
		std::string m_secondaryHostname;
		
		unsigned long m_primaryPort;
		unsigned long m_secondaryPort;

		ReEntrantThreadLockable m_dataLoadLock;

	private:
		// Assignment operator not used so it is made private
		NamingPort& operator=(const NamingPort&);
		NamingPort(const NamingPort&);
	};
} // closes TA_Base_Core

#endif // !defined(AFX_NAMINGPORT_H__B22EFC47_AFBF_417A_90FE_000123456789__INCLUDED_)

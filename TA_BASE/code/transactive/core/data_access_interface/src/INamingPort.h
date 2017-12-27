// INamingPort.h: interface for the INamingPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INAMINGPORT_H__B22EFC47_AFBF_417A_90FE_65E8CF778789__INCLUDED_)
#define AFX_INAMINGPORT_H__B22EFC47_AFBF_417A_90FE_65E8CF778789__INCLUDED_

/**
  * 
  * INamingPort is an interface to a Naming object. It allows the action object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  * This interface is added for CL-21230
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

namespace TA_Base_Core
{
	class INamingPort  
	{
	public:
		/**
		 * getName
		 *
		 * Returns the Agent Name
		 *
		 * @return The agent name as std::string
		 */
		virtual std::string getName() = 0;

		/**
		 * getPrimaryPort
		 *
		 * Returns the primary naming service port of the Agent which default operating mode is configured to CONTROL
		 *
		 * @return The port of naming service as an unsigned long
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. PRIMARI_PORT should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
		 */
		virtual unsigned long getPrimaryPort() = 0;

		/**
		 * getSecondaryPort
		 *
		 * Returns the primary naming service port of the Agent which default operating mode is configured to MONITOR
		 *
		 * @return The port of naming service as an unsigned long
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. SECONDARY_PORT should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
		 */
		virtual unsigned long getSecondaryPort() = 0;

		/**
		 * getPrimaryHostname
		 *
		 * Returns the primary host name where the default control mode agent is running in.
		 *
		 * @return The primary host name as std::string
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. HOSTNAME should be an 
         *            string), or if the wrong ammount of data is retrieved.
		*/
		virtual std::string getPrimaryHostname() = 0;

		/**
		 * getSecondaryHostname
		 *
		 * Returns the primary host name where the default monitor mode agent is running in.
		 *
		 * @return The primary host name as std::string
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. HOSTNAME should be an 
         *            string), or if the wrong ammount of data is retrieved.
		 */
		virtual std::string getSecondaryHostname() = 0;

		/**
		 * getNsPort
		 *
		 * Returns the naming service port for the specified hostname and agent name
		 *
		 * @param hostname - the hostname in which the agent running
		 *
		 * @return The naming service port as unsigned long
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. NSPORT should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
		 */
		virtual unsigned long getNsPort(const std::string hostname) = 0;
	};
} //close namespace TA_Base_Core

#endif // !defined(AFX_INAMINGPORT_H__B22EFC47_AFBF_417A_90FE_65E8CF778789__INCLUDED_)

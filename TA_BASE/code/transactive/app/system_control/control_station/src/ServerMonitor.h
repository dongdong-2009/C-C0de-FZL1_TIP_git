/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ServerMonitor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This will monitor the first server listed in the HostNames list. If this server goes down
  * then it will alert the ProcessManager so each application can be told.
  * 
  * This class is a singleton
  */


#if !defined(ServerMonitor_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_)
#define ServerMonitor_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/sockets/src/RawDatagramSocket.h"

#include "app/system_control/control_station/src/PingableItem.h"


class ProcessManager;

class ServerMonitor : public TA_Base_Core::Thread
{
public:

    /**
    * Destructor
    */
    virtual ~ServerMonitor();


    /**
    * getInstance
    *
    * @return ServerMonitor& - returns a single instance of this object
    */
    static ServerMonitor& getInstance();


    /**
    * run
    *
	* This will block until terminate() is called.
    * NOTE: do not call this method directly. It needs to be public so that it can be called from
    * the thread-spawning function
	*/
	void run();


	/**
    * terminate
    *
	* The terminate() method will cause run() to return. 
    * NOTE: probably not a good idea to call this method directly - call terminateAndWait() instead
	*/
	void terminate();


    /**
     * isContactable
     *
     * @return bool - Indicates if the server is contactable. Returns false if the server cannot be
     *                contacted and true if it can.
     */
    bool isContactable() const
    {
        return m_isContactable;
    }


    /**
     * setProcessManager
     *
     * @param ProcessManager* - The object to callback when we have a change in the status of the server.
     */
    void setProcessManager( ProcessManager* processManager )
    {
        m_processManager = processManager;
    }


	static void cleanUp();

private:
   
    // Made private as this is a singleton
    ServerMonitor( );
	ServerMonitor( const ServerMonitor& theServerMonitor);
	ServerMonitor& operator=(const ServerMonitor& pm);


    /**
    * isTerminating
    *
    * This should be called to determine if we are terminating. run() should use
    * this to determine if it should exit it's loop. This uses the m_isTerminating
    * member variable. This method should be used instead of accessing the variable
    * directly so it can be thread locked correctly.
    *
    * @return true indicates the thread should terminate
    *         false indicates the thread can keep running
    */
    bool isTerminating();


    /**
    * setIsTerminating
    *
    * This should be called by terminate to indicate that the run() method should
    * return as soon as possible. This uses the m_isTerminating
    * member variable. This method should be used instead of accessing the variable
    * directly so it can be thread locked correctly.
    *
    * @param bool - This indicates if we should terminate. true indicates the run()
    *               method should return as soon as possible.
    */
    void setIsTerminating(bool isTerminating);


    /**
    * pollHosts
    *
    * This sends the packets to ping the hosts.
    *
    * @param pingableItems - The items to ping. Each host is represented by a PingableItem.
    *
    */
    void pollHosts(std::vector<PingableItem*>& pingableItems);

    /**
    * doPostPollUpdate
    *
    * This method is called following each call to pollHosts. 
    * The checks the alarm policy state for each pingable item and notifes
    * the system if there has been any change in state.
    *
    * @param pingableItems - The items that have been pinged. Each host is represented by a PingableItem.
    */
    void doPostPollUpdate(std::vector<PingableItem*>& pingableItems);

    /**
    * retrievePingParameters
    *
    * Retrieves the ping parameters from the entity configuration parameters.
    * If these can't be retrieved, they will be set as the default values.
    */
    void retrievePingParameters();

    /**
    * createPingableItemsFromHostnames
    *
    * This method parses the Control Station's HostNames parameter to retrieve the server names
    * and creates a pingable item for each server.
    *
    * @param pingableItems - Vector of pingable item pointers. Each host is represented by a PingableItem.
    */
    void createPingableItemsFromHostnames(std::vector<PingableItem*>& pingableItems);

    // the one and only class instance
    static ServerMonitor* s_instance;

	// protect singleton creation
	static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

    // The process manager to callback if we find an application has terminated
    ProcessManager* m_processManager;

    // true if the thread is terminating
    bool m_isTerminating;

    // protect the m_isTerminating member
    TA_Base_Core::NonReEntrantThreadLockable m_terminateLock;

    // true if able to contact any of the hosts in the host names list
    bool m_isContactable;

    // Number of times to ping a server without success before officially
    // declaring it unpingable
	unsigned int m_pingRetries;

	// ICMP datagrams are "unreliable"...
	// This value dictates how many times during each ping cycle, i.e.
	// one call to pollHosts(), each item is, at a maximum, pinged.
	// It is impotant to note that the Control Station parameter:
	// ControlStation::PING_RETRIES_PARAM is NOT THIS PARAM;
	// see comments on m_pingRetries above
	static const unsigned int PING_RETRIES_PER_CYCLE;

	// Maximum length of time to wait after sending response before timing out (in seconds)
	unsigned int m_pingTimeoutSeconds;

    // This datamember is used to identify received datagrams as ones 
	// that we sent
	unsigned short m_pid;
#ifdef WIN32
	WSAData winsockData;
#endif // WIN32
    //The socket that we use to send pings on
    TA_Base_Core::RawDatagramSocket m_socket;
};

#endif // !defined(ServerMonitor_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_)

#ifndef GENERIC_SERVER_SIMULATOR_COMMS_OBSERVER_H
#define GENERIC_SERVER_SIMULATOR_COMMS_OBSERVER_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssCommsObserver.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/GssCommon.h"

#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_Bus
{
    /**
      * GssCommsObserver allows the GssSimulationModule to delegate incoming socket
      * connections.  Each new connection is passed to a new GssCommsObserver object.
      *
      * GssCommsObserver holds a member observed socket, and acts as an interface
      * (a.k.a. Fascade) for objects to access useful operations on that socket.
      */
    class GssCommsObserver : public virtual TA_Base_Core::ITcpSocketObserver
    {
    public:

        // Typedef for classes used
		//
		// The socket type is constructed by TcpObservedSocket, and hence must be
		// of the most derived type that you wish to use.
		//
        typedef TA_Base_Core::TcpNonblockingSocket SocketType;		

        typedef TA_Base_Core::TcpObservedSocket<SocketType> ObservedSocketType;

        typedef TA_Base_Core::TcpServerSocket< ObservedSocketType > ServerSocketType;
        
        //we have no need for a default constructor.
        GssCommsObserver(ObservedSocketType * socket);
        
        virtual ~GssCommsObserver();
        
        
        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                          ITcpObserver methods                            //
        //                                                                          //
        //          NOTE: These methods are all already virtual by virtue of        //
        //                being derived from ITcpObserver. Not all pure methods     //
        //                of that class have been implemented here. See the         //
        //                examples directory for typical uses that a generic        //
        //                framework cannot predict for you.                         //
        //                                                                          //
        //                The main method you still need to override is:            //
        //                processReceivedData()                                     //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////



        /**
          * writeFailed
          * 
          * This method must be implemented to inherit ITcpSocketObserver.  Any time a 
          * query to write to the observed socket fails, this method is called.  It allows
          * the user to include any cleanup they require after a failed write.
          *
          */
        void writeFailed();


        /**
          * connectionLost
          * 
          * This method must be implemented to inherit ITcpSocketObserver.  This method is called
          * whenever the observed socket loses it's connection to the remote socket.
          *
          * @return bool continueToObserve returns true if the socket should continue to be observed
          *            and returns false if the socket should no longer be observed.
          *
          */
        bool connectionLost();


        /**
          * connectionEstablished
          *
          * This method must be implemented to inherit ITcpSocketObserver. it
          * is called after a connection is successfully established.
          */
        void connectionEstablished();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                         CommsObserver methods                            //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        
        /**
          * getSocketId
          */
        virtual int getSocketId() const;

        /**
          * getSocketClientName
          *
          * @return std::string the client name, possibly just the ipaddress
          */
        virtual std::string getSocketClientName() const;


        /**
          * stillConnected
          *
          * indicates if the observed socket is still connected to it's remote host.
          * Returns a data member that is updated by connectionLost() and
          * connectionEstablished().  Does NOT force a call to 
          * m_clientSocket->stillConnected().
          *
          * @return bool true if the socket is still connected, false otherwise.
          */
        virtual bool stillConnected() const;

        /** getSocket
          *
          * Accessor for the socket being observed. Most socket access should
          * use the fascade methods getSocketId() and getSocketClientName()
          *
          * @return the observed socket.
          */
        ObservedSocketType * getSocket() { return m_clientSocket; }

    protected:
        /** getVerbosity
          *
          * Accessor for the private level-of-verboseness data member
          */
        int getVerbosity() const;

    private:
        /// the observed socket
        ObservedSocketType * m_clientSocket;

        /// indicates if the observed socket is still connected.    
        bool m_stillConnected;

        /// cached client name, retrieved by background Thread at construction
        std::string m_cachedClientName;

        ///
        /// allows the client name discovery thread to  set the client name
        void setSocketClientName(const std::string & name);

        ///
        /// How likely it is that the comms observer will print stuff to stdout
        int m_verbosity;

        ///
        friend class GssCommsObserverClientNameDiscoveryThread;
    };


    /**
      * A background thread run only once, at GssCommsObserver construction,
      * to discover the socket's client name. This sometimes otherwise takes
      * an extortionately long time to retrieve during a call to execStatus.
      */
    class GssCommsObserverClientNameDiscoveryThread
        : public virtual TA_Base_Core::Thread
    {

    public:
        GssCommsObserverClientNameDiscoveryThread( GssCommsObserver * gco );

        /** The run method must be implemented in any class that inherits from
          * TA_Base_Core::Thread.  */
        void run();


        /** The terminate method must be implemented in any class that inherits
          * from TA_Base_Core::Thread.  */
        void terminate();

    private:
        ///
        /// The parent comms observer
        GssCommsObserver * m_commsObserver;

        ///
        ///
        volatile bool m_threadIsRunning;

        ///
        ///
        volatile bool m_doThreadTerminate;

    }; // class GssCommsObserverClientNameDiscoveryThread

} // namespace TA_Base_Bus
#endif // ifndef(GENERIC_SERVER_SIMULATOR_COMMS_OBSERVER_H)


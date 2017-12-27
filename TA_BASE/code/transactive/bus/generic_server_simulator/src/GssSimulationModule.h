#ifndef GENERIC_SERVER_SIMULATOR_SIMULATION_MODULE_H
#define GENERIC_SERVER_SIMULATOR_SIMULATION_MODULE_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssSimulationModule.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Declaration file for the GssSimulationModule class
  */

#include <vector>
#include <string>
#include <list>

#include "bus/generic_server_simulator/src/ISimulationModule.h"
#include "bus/generic_server_simulator/src/GssCommsObserver.h"
#include "bus/generic_server_simulator/src/GssCommon.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


namespace TA_Base_Bus
{
	class GssCommandProcessor;

    /**
      * GssSimulationModule implements ISimulationModule, for use with the generic simulator.
      */
    class GssSimulationModule
    : public virtual TA_Base_Core::Thread
    , public ISimulationModule
    {

    public:
        typedef std::list<std::string> PortList;

		typedef std::vector< GssCommsObserver * > CommsObserverList;

        //Construction & Destruction
        GssSimulationModule(const std::string &name, const std::string & host, const PortList & ports);
        GssSimulationModule(const std::string &name);
        virtual ~GssSimulationModule();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                      TA_Base_Core::Thread methods                             //
        //                                                                          //
        //          NOTE: These methods are all already virtual by virtue of        //
        //                being derived from TA_Base_Core::Thread                        //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * run
          * 
          * The run method must be implemented in any class that inherits from
          * TA_Base_Core::Thread. 
          */
        void run();


        /**
          * terminate
          * 
          * The terminate method must be implemented in any class that inherits
          * from TA_Base_Core::Thread.
          */
        void terminate();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                         ISimulationModule methods                        //
        //                                                                          //
        //          NOTE: These methods are all already virtual by virtue of        //
        //                being derived from ISimulationModule                      //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * startSim
          * 
          * startSim is called whenever the "start" command is issued by the
          * simulator user. Any initialisation that needs to be taken care of
          * before the simulation starts for the first time is done in this method.
          *
          */
        void startSim();


        /**
          * stoptSim
          * 
          * stopSim is called whenever the "stop" command is issued by the
          * simulator user.  Any processing that needs to be taken care of as
          * the simulation finishes is done in this method.
          */
        void stopSim();


        /**
          * restartSim
          * 
          * restartSim is called whenever the "start" command is issued by the
          * simulator user.  Any initialisation that needs to be taken care of
          * before the simulation restarts is done in this method.
          *
          */
        void restartSim();


        /**
          * destroySim
          * 
          * startSim is called whenever the "quit" command is issued by the
          * simulator user.  Any processing that needs to be taken care of
          * before the simulator is destroyed should be completed in this method.
          *
          */
        void destroySim();    


        /**
          * getName
          * 
          * returns the name that the simulator module was instantiated with.
          *
          * @return std::string Name
          */
        std::string getName();


        /**
          * getSimDescription
          * 
          * returns the description of the simulation module, for help messages.
          *
          * @return the value of the simulatorDescription
          */
        virtual std::string getSimDescription();


        /**
          * getCommandProcessor
          * 
          * returns a pointer to the derived GssCommandProcessor
          */
        TA_Base_Bus::ISimulationCommandProcessor& getCommandProcessor();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                       GssSimulationModule methods                        //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * createCommsObserver
          * 
          * Used by run() to create a CommsObserver when a new socket connection
		  * is accepted.
          *
          * @return the new'd derived GssCommsObserver class instance.
          *
          */
		virtual GssCommsObserver * createCommsObserver( GssCommsObserver::ObservedSocketType * socket ) = 0;

        /**
          * execStatus
          * 
          * Prints the current status of the GssSimulationModule. 
          *
          * @return true if the command is executed sucessfully.
          *
          */
        virtual bool execStatus();


        /**
          * setNewConnectionWaitTime
          *
          * Time in milliseconds between when run() checks for new socket
          * connection attempts
          *
          * @param t the time in milliseconds between checks.
          */
        virtual void setNewConnectionWaitTime(int t);

        
        /**
          * setConnectionLimit
          *
          * Sets the maximum number of clients that may connect to this
          * simulation module.
          *
          * @param limit the number to set the limit to
          */
        virtual void setConnectionLimit(int limit);

        
        /**
          * createCommandProcessor
          *
          * @return the new GssCommandProcessor (or subclass)
          */
        virtual TA_Base_Bus::GssCommandProcessor * GssSimulationModule::createCommandProcessor();
        

    protected:
        /** getVerbosity
          *
          * Accessor for the private level-of-verboseness data member
          */
        int getVerbosity() const;

        /** preLoop
          *
          * One of three hook functions. Called within run(), before any of the
          * sockets are checked for accepting a connection.
          *
          * Useful for subclasses that require regularly timed actions, before
          * any sockets have been connected
          */
        virtual void preLoop() {};

        /** midLoop
          *
          * One of three hook functions. Called within run(), after the sockets
          * are checked for accepting a connection, but before current comms
          * observers are checked for still being connected.
          *
          * Useful for subclasses that require regularly timed actions, after
          * any sockets have been connected
          */
        virtual void midLoop() {};

        /** endLoop
          *
          * One of three hook functions. Called within run(), after the sockets
          * are checked for accepting a connection, and after current comms
          * observers are checked for still being connected.
          *
          * Useful for subclasses that require regularly timed actions, after
          * comms observers have been ascertained as stable.
          */
        virtual void endLoop() {};

        /**
          * getCommsObserverVector
          * 
          * returns the vector of CommsObserver objects held by this class.
          * Each CommsObserver monitors a connected client socket.
          *
          * @return Const ref of the vector of CommsObserver objects held by the
          *         simulator.
          */
        const CommsObserverList & getCommsObservers();


        /**
          * addCommsObserver
          * 
          * adds the passed in CommsObserver to the vector of CommsObserver objects held by the 
          * simulator
          *
          * @param CommsObserver* com A pointer to the CommsObserver object to be added
          *
          */
        virtual void addCommsObserver( GssCommsObserver * com); 


        /**
          * removeCommsObserver
          * 
          * removes the passed in CommsObserver from the vector of CommsObserver objects held by the 
          * simulator
          *
          * @param com A pointer to the CommsObserver object to be removed
          *
          */
        virtual void removeCommsObserver( GssCommsObserver * com );


    private:

        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                    GssSimulationModule data members                      //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /// The name of this SimulationModule
        std::string m_name;

        /// The GssCommandProcessor
        TA_Base_Bus::ISimulationCommandProcessor * m_commandProcessor;

        /// flag used as a semaphore
        volatile bool m_threadIsRunning;

        /// flag used as a semaphore to tell run() to stop
        volatile bool m_doThreadTerminate;

        /// Limit on the number of clients that are allowed to connect.
        /// Less than 1 indicates no limit. Zero is default. Set using
        /// setConnectionLimit()
        int m_connectionLimit;

        /// The ports listened to
        PortList m_socketPorts;

        /// The host used by the socket. This is the address that the socket
        /// observer binds to.
        std::string m_socketHost;

		/// The number of milliseconds that the ::run() method waits between
		/// checking for socket connection attempts. Set in the constructor
        /// to 250, can be set using setNewConnectionWaitTime()
		int m_newConnectionWaitTime;

        /// a vector which holds an observed socket for each connection that
        /// is accepted.
        CommsObserverList m_commsObservers;

        ///
        /// Lock for a ThreadGuard to only allow one thread at a time to
        /// access the comms observers.
        TA_Base_Core::ReEntrantThreadLockable m_observerLock;

        ///
        /// How likely it is that the module will print stuff to stdout
        int m_verbosity;

    }; // class GssSimulationModule

} // namespace TA_Base_Bus

#endif // GENERIC_SERVER_SIMULATOR_SIMULATION_MODULE_H

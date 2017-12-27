#ifndef TelnetCommsObserver_H
#define TelnetCommsObserver_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/TelnetCommsObserver.h $
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

#include "bus/generic_server_simulator/src/GssCommsObserver.h"
#include "bus/generic_server_simulator/src/TelnetCommon.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <map>

namespace TA_Base_Bus
{
    /**
      * TelnetCommsObserver allows the GssSimulationModule to delegate incoming socket
      * connections.  Each new connection is passed to a new TelnetCommsObserver object.
      *
      * TelnetCommsObserver makes a reasonable attempt to observe the 
      * implementation approach to option negotiation as discussed in RFC 1143
      */
    class TelnetCommsObserver : public GssCommsObserver
    {
    public:

        //we have no need for a default constructor.
        TelnetCommsObserver(GssCommsObserver::ObservedSocketType * socket);
        
        virtual ~TelnetCommsObserver();
        
        
        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                          ITcpObserver methods                            //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * processReceivedData
          *
          * Do something with the received stream of data
          *
          * @param pData the tcp data stream
          * @param pSocket the ISocket that received the data
          */
        void processReceivedData( std::vector<unsigned char> & pData, TA_Base_Core::ISocket * pSocket );

        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                        TelnetCommsObserver methods                    //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////


        /** initiateNegotiations
          *
          * This method is called by TelnetModule after a connection is
		  * successfully established. Used here to initiate telnet option
		  * negotiation.
          */
        virtual void initiateNegotiations();
        
    protected:
        /**
          *
          */
        virtual void addToLineBuffer(unsigned char c);

        /**
          *
          */
        virtual void addToSubOptionNegotiationBuffer(unsigned char c);

        /**
          *
          */
        virtual std::vector<unsigned char>::const_iterator processTelnetCommand(std::vector<unsigned char>::const_iterator & iter);

        /**
          *
          */
        virtual std::vector<unsigned char>::const_iterator processAnsiSequence(std::vector<unsigned char>::const_iterator & iter);

        /**
          *
          */
        virtual std::vector<unsigned char>::const_iterator processTelnetOption(std::vector<unsigned char>::const_iterator & iter);

        /**
          *
          */
        virtual void processLine(const TelnetBuffer & line);

        /**
          *
          */
        const ByteVector & getBuffer() const;

        /** Called when the telnet BRK command is received */
        virtual void doBreak();

        /** Called when the telnet IP command is received */
        virtual void doInterruptProcess();

        /** Called when the telnet AO command is received */
        virtual void doAbortOutput();

        /** Called when the telnet AYT command is received */
        virtual void doAreYouThere();

        /** Called when the telnet EC command is received */
        virtual void doEraseCharacter();

        /** Called when the telnet EL command is received */
        virtual void doEraseLine();

        /** Called when the telnet GA command is received */
        virtual void doGoAhead();

        /** Called when the telnet DM command is received */
        virtual void doDataMark();

        /** Called when the telnet EOR command is received */
        virtual void doEndOfRecord();

        /** Called when the telnet SB command is received */
        virtual void doStartSubOptionNegotiation();

        /** Called when the telnet SE command is received */
        virtual void doEndSubOptionNegotiation();

        /** Called during doEndSubOptionNegotiation(), to process the buffer
          * that has arrived for suboption negotiation.  This default method
          * calls methods to process the NAOCRD, WindowSize, TerminalType and
          * RemoteFlowControl sub-option negotiations.  Override those methods
          * if you wish to provide handing of those sub-options, otherwise
          * override this method to handle all other sub-options.  Make sure to
          * call this method if you don't handle a sub-option in your overriden
          * method.
          *
          * @param subOptionData the buffer containing the sub-option data.
          *        Does not contain the IAC/SB or IAC/SE bytes.
          */
        virtual void processSubOption(const ByteVector & subOptionData);

        /** Handles the NAOCRD sub-option negotiation
          * 
          * @param subOptionData the buffer containing the sub-option data.
          *        Does not contain the IAC/SB or IAC/SE bytes.
          */
        virtual void doNAOCRDSubOption(const ByteVector & subOptionData);

        /** Handles the WindowSize (NAWS) sub-option negotiation
          * 
          * @param subOptionData the buffer containing the sub-option data.
          *        Does not contain the IAC/SB or IAC/SE bytes.
          */
        virtual void doWindowSizeSubOption(const ByteVector & subOptionData);

        /** Handles the TerminalType sub-option negotiation
          * 
          * @param subOptionData the buffer containing the sub-option data.
          *        Does not contain the IAC/SB or IAC/SE bytes.
          */
        virtual void doTerminalTypeSubOption(const ByteVector & subOptionData);

        /** Handles the Remote Flow Control sub-option negotiation
          * 
          * @param subOptionData the buffer containing the sub-option data.
          *        Does not contain the IAC/SB or IAC/SE bytes.
          */
        virtual void doRemoteFlowControlSubOption(const ByteVector & subOptionData);

    private:
        /** Sends a telnet IAC WONT command to the remote host to either refuse
          * the option, or to commence negotiating to not discuss the option
          *
          * @param option the TelnetOption to send a telnet IAC WONT command for
          */
        void sendLocalWontOptionNegotiation( TelnetOption option );

        /** Sends a telnet IAC WILL command to the remote host to either agree
          * to the option, or to commence negotiating discussing the option
          *
          * @param option the TelnetOption to send a telnet IAC WILL command for
          */
        void sendLocalWillOptionNegotiation( TelnetOption option );

        /** Sends a telnet IAC DONT command to the remote host to either refuse
          * the option, or to commence negotiating to not discuss the option
          *
          * @param option the TelnetOption to send a telnet IAC DONT command for
          */
        void sendRemoteDontOptionNegotiation( TelnetOption option );

        /** Sends a telnet IAC DO command to the remote host to either agree
          * to the option, or to commence negotiating discussing the option
          *
          * @param option the TelnetOption to send a telnet IAC DO command for
          */
        void sendRemoteDoOptionNegotiation( TelnetOption option );

        /** Wraps a common activity with logging
          *
          * @param command the TelnetCommand to send
          * @param extra any extra data to send (options, sub-options etc)
          */
        void sendTelnetCommand( TelnetCommand command, const TelnetBuffer & extra );

        /** Intended to wrap simple
          *
          *     m_[key] = value;
          *
          * assignments with some status transition logic.
          *
          * @param option the telnet option to set
          * @param status the status to set the option to
          * @param local  true if the option is local, false otherwise
          *
          * @return true if the status transition was valid and the option's
          *         status was able to be set as requested, false otherwise
          */
        bool setTelnetOption( TelnetOption option, TelnetOptionStatus status, bool local );

        /** 
          *
          */
        bool isOptionPending( TelnetOption option, bool local );

        /** 
          *
          */
        void sendNAOCRDSubOptionNegotiation();

        ///
        ///
        TelnetOptionMap m_remoteOptions;

        ///
        ///
        TelnetOptionMap m_localOptions;

        ///
        ///
        TA_Base_Core::ReEntrantThreadLockable m_optionsLock;

        ///
        /// Need the Echo option a little more at hand than in a map
        bool m_doEcho;

        ///
        /// Need the Go Ahead option a little more at hand than in a map
        bool m_doGoAhead;

        ///
        ///
        bool m_abortOutput;

        ///
        ///
        bool m_interruptProcess;

        ///
        ///
        bool m_inSubOptionNegotiation;

        ///
        /// command line buffer. Partly for support of EC and EL, partly
        /// because the GenericSimulator likes it that way
        TelnetBuffer m_buffer;
        
        ///
        ///
        ByteVector m_subOptionNegotiationBuffer;

        class NAOCRDSubOption
        {
        public:
            enum
            {
                DR              = 0,    // Data Receiver
                DS              = 1     // Data Sender
            }; // anonymous enumeration
        }; // class NAOCRDSubOption


        class FlowControlSubOption
        {
        public:
            enum
            {
                OFF             = 0,
                ON              = 1,
                RESTART_ANY     = 2,
                RESTART_XON     = 3
            }; // anonymous enumeration
        }; // class FlowControlSubOption

    }; // class TelnetCommsObserver

} // namespace TA_Base_Bus

#endif // ifndef TelnetCommsObserver_H

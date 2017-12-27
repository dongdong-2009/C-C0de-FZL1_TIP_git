/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/TelnetCommsObserver.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * TelnetCommsObserver allows the GssSimulationModule to delegate incoming socket
  * connections.  Each new connection is passed to a new TelnetCommsObserver object.
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include <iostream>
#include <algorithm>

#include "bus/generic_server_simulator/src/TelnetCommsObserver.h"
#include "bus/generic_server_simulator/src/GssUtility.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

using namespace TA_Base_Bus;

static TelnetOptionStatus command2status( Byte cmd );

TelnetCommsObserver::TelnetCommsObserver(GssCommsObserver::ObservedSocketType * socket)
: GssCommsObserver(socket)
, m_doEcho(false)   // ECHO is WON'T by default
, m_doGoAhead(true) // SUPPRESS GOAHEAD is WON'T by default
, m_abortOutput(false)
, m_interruptProcess(false)
, m_inSubOptionNegotiation(false)
{
    TA_Base_Core::ThreadGuard oguard(m_optionsLock);

    // set up telnet defaults (defined by the RFCs)
    m_remoteOptions[TelnetOption::TransmitBinary] = TelnetOptionStatus::Wont;
    m_remoteOptions[TelnetOption::Echo] = TelnetOptionStatus::Wont;
    m_remoteOptions[TelnetOption::SuppressGoAhead] = TelnetOptionStatus::Wont;
    m_remoteOptions[TelnetOption::Status] = TelnetOptionStatus::Wont;
    m_remoteOptions[TelnetOption::TimingMark] = TelnetOptionStatus::Wont;

    m_localOptions[TelnetOption::TransmitBinary] = TelnetOptionStatus::Wont;
    m_localOptions[TelnetOption::Echo] = TelnetOptionStatus::Wont;
    m_localOptions[TelnetOption::SuppressGoAhead] = TelnetOptionStatus::Wont;
    m_localOptions[TelnetOption::Status] = TelnetOptionStatus::Wont;
    m_localOptions[TelnetOption::TimingMark] = TelnetOptionStatus::Wont;
}

TelnetCommsObserver::~TelnetCommsObserver()
{
    m_buffer.erase();
}



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                            ITcpObserver methods                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
/**
  * processReceivedData
  *
  * Parses the received stream of data into zero or more valid table querys
  * and adds them to the message processor's queue.
  *
  * @param pData the tcp data stream
  * @param pSocket the ISocket that received the data
  */
void TelnetCommsObserver::processReceivedData
(   std::vector<unsigned char> & pData,
    TA_Base_Core::ISocket * pSocket
)
{
    FUNCTION_ENTRY("TelnetCommsObserver::processReceivedData");

    {
        std::string datahexstr;
        GssUtility::data2hex(pData, datahexstr);
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                TA_Base_Core::DebugUtil::DebugDebug, "Data Received: %s", datahexstr.c_str() );
    }

    std::vector<unsigned char>::const_iterator iter = pData.begin();

    while ( iter != pData.end() )
    {
        //
        // Notice that there is no "++iter" to increment the iterator at the
        // end of this while loop. That is because each case conditional clause
        // should do that increment appropriately
        if ( *iter == TelnetCommand::IAC)
        {
			std::vector<unsigned char>::const_iterator cmd_strt = iter;

            ++iter;
            iter = processTelnetCommand( iter );

            // if (getVerbosity() >= 6)
            {
				//
				// only want to std::cout the elements of the command that was
				// processed, not the whole pData buffer
				std::vector<unsigned char> cmd;
				for ( std::vector<unsigned char>::const_iterator cmd_element = cmd_strt ;
				      cmd_element != iter ;
					  cmd_element++
				    )
				{
					cmd.push_back( *cmd_element );
				}
                std::string hexstr;
                GssUtility::data2hex(cmd, hexstr);
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                        TA_Base_Core::DebugUtil::DebugDebug, "Processed Telnet Command: %s", hexstr.c_str() );
            }
        }
        else if ( m_inSubOptionNegotiation )
        {
            addToSubOptionNegotiationBuffer( *iter );
            iter++;
        }
//        else if ( *iter == 0x1B )
//        {
//            // Handle ANSI escape sequences
//            iter = processAnsiSequence( iter );
//        }
        else
        {
            addToLineBuffer( *iter );
            iter++;
        }
    }

    pData.clear();

    FUNCTION_EXIT;
}

void TelnetCommsObserver::initiateNegotiations()
{
    // Initiate telnet option negotiation.
    // Some of these confirm what should be defaults. Unfortunately, the MS
    // Telnet client implementation is broken, so these fix that.
    
    // Attempt to speak 8-bit in both directions
    // IAC WILL TRANSMIT-BINARY
    // sendLocalWillOptionNegotiation( TelnetOption::TransmitBinary );


    // IAC DO TRANSMIT-BINARY
    // sendRemoteDoOptionNegotiation( TelnetOption::TransmitBinary );

    // Request to have to send back typed characters. This is NOT supposed
    // to be the default, but the MS-Telnet is broken and assumes the opposite.
    // Worse still, if this telnet server sets the MS telnet client to local
    // echo on, it remembers for the next session, which will probably not be
    // appropriate
    // IAC WILL ECHO
    sendLocalWillOptionNegotiation( TelnetOption::Echo );

    // IAC WONT ECHO
    sendRemoteDontOptionNegotiation( TelnetOption::Echo );

    //
    // Request not to have to send GA (GoAhead).
    // IAC WILL SUPPRESS-GOAHEAD
    sendLocalWillOptionNegotiation( TelnetOption::SuppressGoAhead );

    //
    // Request to negotiate Output Carriage-Return Disposition.
    // IAC DO NAOCRD
    // sendRemoteDoOptionNegotiation( TelnetOption::NAOCRD );

}

void TelnetCommsObserver::addToSubOptionNegotiationBuffer(unsigned char c)
{
    // m_subOptionNegotiationBuffer += c;
    m_subOptionNegotiationBuffer.push_back(c);
}


void TelnetCommsObserver::addToLineBuffer(unsigned char c)
{
    // if ( (c=='\n') || (c=='\r') || (c==0x0D) || (c==0x0A) )
    if (c==0x0D) // supposed to be '\n'
    {
        if ( m_doEcho )
        {
            char ec[2] = { '\n', '\r' };
            getSocket()->write( ec, 2 );
        }

        // the std::cin.getline() requires that '\n' is the terminating char.
        // Thus, we put that, regardless of what we actually received
        m_buffer += '\n';

        //
        // otherwise, go ahead and process the line
        processLine(m_buffer);

        m_buffer.erase();
    }
    else
    {
        //
        // In ConsoleDetachUtility::detach_win the new stream is deliberately
        // created with mode _O_TEXT so as to convert CRLF to CR, but for some
        // reason this just doesn't happen. We don't want to translate CRLF
        // into multiple CR so we silently drop the apparently useless LF.
        switch (c)
        {
            case 0x0A:
            case 0x00:
                // ignore
                break;

            case 0x04: // ^D
                // kill this comms observer
                getSocket()->terminate();
                break;

            case 0x08: // backspace ^H
                doEraseCharacter();
                break;

            default:
                //
                // echo if the options require it
                if ( m_doEcho )
                {
                    char ec = c;
                    getSocket()->write( &ec, 1 );
                }

                m_buffer += c;
        }
    }
}

std::vector<unsigned char>::const_iterator TelnetCommsObserver::processTelnetCommand
(   std::vector<unsigned char>::const_iterator & iter
)
{
    Byte cmd = *iter;

    if ( cmd == TelnetCommand::NOP )
    {
        // No worries.
		iter++;
    }

    else if ( cmd == TelnetCommand::EOR )
    {
        iter++;
        doEndOfRecord();
    }
    else if ( cmd == TelnetCommand::DM )
    {
        iter++;
        doDataMark();
    }
    else if ( cmd == TelnetCommand::BRK )
    {
        iter++;
        doBreak();
    }
    else if ( cmd == TelnetCommand::IP )
    {
        iter++;
        doInterruptProcess();
    }
    else if ( cmd == TelnetCommand::AO )
    {
        iter++;
        doAbortOutput();
    }
    else if ( cmd == TelnetCommand::AYT )
    {
        iter++;
        doAreYouThere();
    }

    else if ( cmd == TelnetCommand::EC )
    {
        iter++;
        doEraseCharacter();
    }
    else if ( cmd == TelnetCommand::EL )
    {
        iter++;
        doEraseLine();
    }
    else if ( cmd == TelnetCommand::GA )
    {
        // um, ok. Generally a no-op
        iter++;
        doGoAhead();
    }

	//
	// Telnet Option commands
    else if ( cmd == TelnetCommand::WILL )
    {
        iter = processTelnetOption( iter );
    }
    else if ( cmd == TelnetCommand::WONT )
    {
        iter = processTelnetOption( iter );
    }
    else if ( cmd == TelnetCommand::DO )
    {
        iter = processTelnetOption( iter );
    }
    else if ( cmd == TelnetCommand::DONT )
    {
        iter = processTelnetOption( iter );
    }

    else if ( cmd == TelnetCommand::SB )
    {
        doStartSubOptionNegotiation();
    }
    else if ( cmd == TelnetCommand::SE )
    {
        doEndSubOptionNegotiation();
    }

    else if ( cmd == TelnetCommand::IAC )
    {
        // 
        // An IAC followed by and IAC is data of value 255
        if (m_inSubOptionNegotiation)
        {
            addToSubOptionNegotiationBuffer( 0xFF );
        }
        else
        {
            addToLineBuffer( 0xFF );
        }
		iter++;
    }

    return iter;
}

std::vector<unsigned char>::const_iterator TelnetCommsObserver::processAnsiSequence
(   std::vector<unsigned char>::const_iterator & iter
)
{
    std::vector<unsigned char> seq;

    while ( (*iter) != 0x7E )
    {
        seq.push_back(*iter);

        iter++;
    }

    {
        std::string hexstr;
        GssUtility::data2hex(seq, hexstr);
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "Processed Ansi Sequence: %s", hexstr.c_str());
    }

    return iter;
}

std::vector<unsigned char>::const_iterator TelnetCommsObserver::processTelnetOption
(   std::vector<unsigned char>::const_iterator & iter
)
{
    FUNCTION_ENTRY("TelnetCommsObserver::processTelnetOption");

    TelnetCommand cmd = *iter; iter++;
    TelnetOption  opt = *iter; iter++;

    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
        "Processing TelnetOption: %s (cmd=%s)",
        GssUtility::data2hex(opt.getChar()).c_str(),
        GssUtility::data2hex(cmd.getChar()).c_str() );

    switch ( cmd.value() )
    {
        //
        // If the remote end says it will, that's good.
        case TelnetCommand::WILL:
            if ( ! isOptionPending( opt, TelnetEndpoint::Remote ) )
            {
                if ( ( opt == TelnetOption::Echo )
                     || ( opt == TelnetOption::TransmitBinary )
                     || ( opt == TelnetOption::SuppressGoAhead )
                     || ( opt == TelnetOption::WindowSize )
                     || ( opt == TelnetOption::NAOCRD )
                   )
                {
                    // Happy to support the remote end doing things we know
                    // about
                    sendRemoteDoOptionNegotiation( opt );
                }
                else if ( opt == TelnetOption::TimingMark )
                {
                    // TimingMark should not be responded to
                }
                else
                {
                    // Refuse if we don't want to do what is being requested
                    sendRemoteDontOptionNegotiation( opt );
                }
            }
            else if ( opt == TelnetOption::NAOCRD )
            {
                // yipee! send sub-option negotiation
            }
            // set the option, regardless of the pending status.
            setTelnetOption( opt, cmd, TelnetEndpoint::Remote );
            break;


        //
        // Remote end refuses to something or other
        case TelnetCommand::WONT:
            if ( isOptionPending( opt, TelnetEndpoint::Remote ) )
            {
                //setTelnetOption( opt, cmd, TelnetEndpoint::Remote );
            }
            else
            {
                if ( !(opt == TelnetOption::TimingMark) )
                {
                    sendRemoteDontOptionNegotiation( opt );
                }
            }
            // set the option, regardless of the pending status.
            setTelnetOption( opt, cmd, TelnetEndpoint::Remote );
            break;


        //
        // Request or agreement to do...
        case TelnetCommand::DO:
            // only send the WILL command if the option request was not a response
            if ( ! isOptionPending( opt, TelnetEndpoint::Local ) )
            {
                //
                // We only support DOing the following options
                if ( ( opt == TelnetOption::Echo )
                     || ( opt == TelnetOption::TransmitBinary )
                     || ( opt == TelnetOption::SuppressGoAhead )
                     || ( opt == TelnetOption::TimingMark )
                     || ( opt == TelnetOption::WindowSize )
                     || ( opt == TelnetOption::RemoteFlowControl )
                     || ( opt == TelnetOption::TerminalType )
                     || ( opt == TelnetOption::NAOCRD )
                   )
                {
                    sendLocalWillOptionNegotiation( opt );
                }
                else
                {
                    //
                    // ( opt == TelnetOption::Status )
                    // ( opt == TelnetOption::ExtendedOptionsList )
                    // and anything else we don't understand
                    sendLocalWontOptionNegotiation( opt );
                }
            }
            // set the option, regardless of the pending status.
            setTelnetOption( opt, cmd, TelnetEndpoint::Local );
            break;


        //
        // Request from remote end or agreement to local end to NOT do...
        case TelnetCommand::DONT:
            if  ( isOptionPending( opt, TelnetEndpoint::Local ) )
            {
                setTelnetOption( opt, cmd, TelnetEndpoint::Local );
            }
            else
            {
                if (!( opt == TelnetOption::TimingMark ))
                {
                    sendLocalWontOptionNegotiation( opt );
                    setTelnetOption( opt, cmd, TelnetEndpoint::Local );
                }
            }
            break;

        default:
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                    TA_Base_Core::DebugUtil::DebugDebug,
                    "Unknown TelnetOption Command : %s (opt=%s)",
                    GssUtility::data2hex(cmd.getChar()).c_str(),
                    GssUtility::data2hex(opt.getChar()).c_str() );
            break;
    }

	return iter;

    FUNCTION_EXIT;
}

void TelnetCommsObserver::processLine(const TelnetBuffer & line)
{
    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
            TA_Base_Core::DebugUtil::DebugDebug, "Processed Line: %s", line.c_str() );
}

void TelnetCommsObserver::sendLocalWontOptionNegotiation( TelnetOption option )
{
    TA_Base_Core::ThreadGuard oguard(m_optionsLock);
    TelnetOptionMap::iterator optfnd = m_localOptions.find(option);
    if ( optfnd != m_localOptions.end() )
    {
		(*optfnd).second.setPending();
    }
    else
    {
        TelnetOptionStatus tos = TelnetOptionStatus::Will;
        tos.setPending();
        m_localOptions[option] = tos;
        // m_localOptions[option] = TelnetOptionStatus::Wont;
        // TelnetOptionMap::iterator optnew = m_localOptions.find(option);
		// (*optnew).second.setPending();
    }

    // don't want to set any options that aren't already. Yet
    TelnetBuffer optstr; optstr = option.getChar();
    sendTelnetCommand(TelnetCommand::WONT, optstr);
}

void TelnetCommsObserver::sendLocalWillOptionNegotiation( TelnetOption option )
{
    TA_Base_Core::ThreadGuard oguard(m_optionsLock);
    TelnetOptionMap::iterator optfnd = m_localOptions.find(option);
    if ( optfnd != m_localOptions.end() )
    {
		(*optfnd).second.setPending();
    }
    else
    {
        TelnetOptionStatus tos = TelnetOptionStatus::Dont;
        tos.setPending();
        m_localOptions[option] = tos;
        // m_localOptions[option] = TelnetOptionStatus::Wont;
        // TelnetOptionMap::iterator optnew = m_localOptions.find(option);
		// (*optnew).second.setPending();
    }

    // don't want to set any options that aren't already. Yet
    TelnetBuffer optstr; optstr = option.getChar();
    sendTelnetCommand(TelnetCommand::WILL, optstr);
}



void TelnetCommsObserver::sendRemoteDontOptionNegotiation( TelnetOption option )
{
    TA_Base_Core::ThreadGuard oguard(m_optionsLock);
    TelnetOptionMap::iterator optfnd = m_remoteOptions.find(option);
    if ( optfnd != m_remoteOptions.end() )
    {
		(*optfnd).second.setPending();
    }
    else
    {
        m_remoteOptions[option] = TelnetOptionStatus::Will;
        TelnetOptionMap::iterator optnew = m_remoteOptions.find(option);
		(*optnew).second.setPending();
    }

    // don't want to set any options that aren't already. Yet
    TelnetBuffer optstr; optstr = option.getChar();
    sendTelnetCommand(TelnetCommand::DONT, optstr);
}

void TelnetCommsObserver::sendRemoteDoOptionNegotiation( TelnetOption option )
{
    TA_Base_Core::ThreadGuard oguard(m_optionsLock);
    TelnetOptionMap::iterator optfnd = m_remoteOptions.find(option);
    if ( optfnd != m_remoteOptions.end() )
    {
		(*optfnd).second.setPending();
    }
    else
    {
        TelnetOptionStatus tos = TelnetOptionStatus::Wont;
        tos.setPending();
        m_remoteOptions[option] = tos;
        // m_remoteOptions[option] = TelnetOptionStatus::Wont;
        // TelnetOptionMap::iterator optnew = m_remoteOptions.find(option);
		// (*optnew).second.setPending();
    }

    // don't want to set any options that aren't already. Yet
    TelnetBuffer optstr; optstr = option.getChar();
    sendTelnetCommand(TelnetCommand::DO, optstr);
}


void TelnetCommsObserver::sendTelnetCommand( TelnetCommand command, const TelnetBuffer & extra )
{
    FUNCTION_ENTRY("TelnetCommsObserver::sendTelnetCommand");

    TelnetBuffer cmdstr;

    cmdstr = TelnetCommand(TelnetCommand::IAC).getChar();
    cmdstr+= command.getChar();
    cmdstr+= extra;

    getSocket()->write( reinterpret_cast<const char *>(cmdstr.c_str()), cmdstr.length() );

    //if (getVerbosity() >= 6)
    {
        std::string hexstr;
        GssUtility::data2hex(cmdstr, hexstr);
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                TA_Base_Core::DebugUtil::DebugDebug, "Wrote Telnet Command: %s", hexstr.c_str() );
    }

    FUNCTION_EXIT;
}

bool TelnetCommsObserver::isOptionPending( TelnetOption option, bool local )
{
    TA_Base_Core::ThreadGuard oguard(m_optionsLock);
    if (local)
    {

        TelnetOptionMap::iterator optfnd = m_localOptions.find(option);
        if (optfnd != m_localOptions.end())
        {
            return (*optfnd).second.isPending();
        }
    }
    else
    {
        TelnetOptionMap::iterator optfnd = m_remoteOptions.find(option);
        if (optfnd != m_remoteOptions.end())
        {
            return (*optfnd).second.isPending();
        }
    }
    
    // 
    // if all that fails then the option is not pending, as negotiation has
    // not even commenced.
    return false;
}

bool TelnetCommsObserver::setTelnetOption( TelnetOption option, TelnetOptionStatus status, bool local )
{
    TA_Base_Core::ThreadGuard oguard(m_optionsLock);
    //
    // options shouldn't be allowed to be of the Do/Don't variety, but instead
    // of disallowing them we morph them into the Will/Won't variety
    if ( status == TelnetOptionStatus::Do )
    {
        status = TelnetOptionStatus::Will;
    }
    else if ( status == TelnetOptionStatus::Dont )
    {
        status = TelnetOptionStatus::Wont;
    }

    if (local)
    {

        // 
		// can't just set the value, because it is in the map by value so
		// (*optfnd).first returns a const reference.  The solution seems to be
		// to erase the option, in which case there is nary little point in
		// finding it first, as erase will do that.  This solution also clears
		// the pending status of the option.
		m_localOptions.erase(option);
        m_localOptions[option] = status;

        if ( option == TelnetOption::SuppressGoAhead )
        {
            m_doGoAhead = (status == TelnetOptionStatus::Wont);
        }
        else if ( option == TelnetOption::Echo )
        {
            m_doEcho = (status == TelnetOptionStatus::Will);
        }
    }
    else
    {
		m_remoteOptions.erase(option);
        m_remoteOptions[option] = status;
    }
    
    return true;
}

void TelnetCommsObserver::doBreak()
{
}

void TelnetCommsObserver::doDataMark()
{
    // in normal mode, the DataMark is a no-op. In Urgent mode, it is
    // part of the Telnet "Synch" Signal.
    //
    // We don't get the Urgent Notification from the TcpSocket, but there
    // doesn't seem to be any circumstance under which a DM is sent other
    // than the Synch signal
    //
    // So: this is part of a Telnet Synch Signal, although generally because
    // the DM should go via urgent TCP, it should arrive before the IP Command
    //
    // TODO: implement this
    if (m_interruptProcess)
    {
    }
}

void TelnetCommsObserver::doEndOfRecord()
{
}

void TelnetCommsObserver::doInterruptProcess()
{
    m_interruptProcess = true;
}

void TelnetCommsObserver::doAbortOutput()
{
    m_abortOutput = true;
}

void TelnetCommsObserver::doAreYouThere()
{
    unsigned char aytstr[] = "Yes i am here.";

    getSocket()->write( reinterpret_cast<const char *>(aytstr),
                        strlen(reinterpret_cast<const char *>(aytstr)) );
    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
            TA_Base_Core::DebugUtil::DebugDebug, "Responded to \"Are You There\" Telnet Command" );
}

void TelnetCommsObserver::doEraseCharacter()
{
    if (! m_buffer.empty() )
    {
        m_buffer.erase( m_buffer.size()-1, 1 );
    }
}

void TelnetCommsObserver::doEraseLine()
{
    if (! m_buffer.empty() )
    {
        m_buffer.erase( 0, m_buffer.size() );
    }
}

void TelnetCommsObserver::doGoAhead()
{
}

void TelnetCommsObserver::doStartSubOptionNegotiation()
{
    FUNCTION_ENTRY("TelnetCommsObserver::doStartSubOptionNegotiation");
    m_inSubOptionNegotiation = true;
    FUNCTION_EXIT;
}

void TelnetCommsObserver::doEndSubOptionNegotiation()
{
    FUNCTION_ENTRY("TelnetCommsObserver::doEndSubOptionNegotiation");
    std::string hexstr;
    GssUtility::data2hex(m_subOptionNegotiationBuffer, hexstr);
    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
            TA_Base_Core::DebugUtil::DebugDebug, "Received sub-option negotiation data : %s", hexstr.c_str() );

    processSubOption(m_subOptionNegotiationBuffer);

    m_inSubOptionNegotiation = false;

    m_subOptionNegotiationBuffer.clear();
    FUNCTION_EXIT;
}

void TelnetCommsObserver::processSubOption(const ByteVector & subOptionData)
{
    FUNCTION_ENTRY("TelnetCommsObserver::processSubOption");
    switch ( subOptionData[0] )
    {
        case TelnetOption::NAOCRD:
            doNAOCRDSubOption(subOptionData);
            break;

        case TelnetOption::WindowSize:
            doWindowSizeSubOption(subOptionData);
            break;

        case TelnetOption::TerminalType:
            doTerminalTypeSubOption(subOptionData);
            break;

        case TelnetOption::RemoteFlowControl:
            doRemoteFlowControlSubOption(subOptionData);
            break;

        default:
            break;
    }
    FUNCTION_EXIT;
}

void TelnetCommsObserver::doNAOCRDSubOption(const ByteVector & subOptionData)
{
    FUNCTION_ENTRY("TelnetCommsObserver::doNAOCRDSubOption");

    std::string hexstr;
    GssUtility::data2hex(m_subOptionNegotiationBuffer, hexstr);

    if (subOptionData.size() == 5)
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                TA_Base_Core::DebugUtil::DebugDebug, "Good NAOCRD sub-option negotiation data : %s", hexstr.c_str() );

    }
    else
    {
        std::string hexstr;
        GssUtility::data2hex(subOptionData, hexstr);
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                TA_Base_Core::DebugUtil::DebugDebug, "BAD NAOCRD sub-option negotiation data : %s", hexstr.c_str() );
    }
    FUNCTION_EXIT;
}

void TelnetCommsObserver::doWindowSizeSubOption(const ByteVector & subOptionData)
{
    FUNCTION_ENTRY("TelnetCommsObserver::doWindowSizeSubOption");
    FUNCTION_EXIT;
}

void TelnetCommsObserver::doTerminalTypeSubOption(const ByteVector & subOptionData)
{
    FUNCTION_ENTRY("TelnetCommsObserver::doTerminalTypeSubOption");
    FUNCTION_EXIT;
}

void TelnetCommsObserver::doRemoteFlowControlSubOption(const ByteVector & subOptionData)
{
    FUNCTION_ENTRY("TelnetCommsObserver::doRemoteFlowControlSubOption");
    FUNCTION_EXIT;
}

void TelnetCommsObserver::sendNAOCRDSubOptionNegotiation()
{
    TelnetBuffer naocrd;
    Byte naocrd_[4] = { TelnetOption::NAOCRD, NAOCRDSubOption::DS, 255, 0 };
    naocrd.assign((Byte *)naocrd_);

    sendTelnetCommand(TelnetCommand::SB, naocrd);

    TelnetBuffer nothing;
    sendTelnetCommand(TelnetCommand::SE, nothing);
}



/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/BackgroundSimulationModule.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * Definition of the BackgroundSimulationModule class.
  */

#ifdef WIN32
#pragma warning (disable : 4786)

#include <io.h>

#else

#include <sys/types.h>
#include <sys/stat.h>

#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/BackgroundSimulationModule.h"
#include "bus/generic_server_simulator/src/BackgroundCommsObserver.h"

#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

// defined in ConsoleDetachUtility.cpp
extern int pipe_out[2];
extern int pipe_err[2];


BackgroundSimulationModule::BackgroundSimulationModule
(   const std::string & name,
    const std::string & host,
    const PortList & ports
)
: GssSimulationModule( name, host, ports )
{
}

BackgroundSimulationModule::~BackgroundSimulationModule()
{
}

GssCommsObserver * BackgroundSimulationModule::createCommsObserver( GssCommsObserver::ObservedSocketType * socket )
{
    setNewConnectionWaitTime(50);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "BackgroundSimulationModule::createCommsObserver() created observer");
    BackgroundCommsObserver * bco = new BackgroundCommsObserver( socket );

    //
    // Send initial telnet negotiations
    bco->initiateNegotiations();
    
    return bco;
}


std::string BackgroundSimulationModule::getSimDescription()
{
    return std::string( "Background Simulation Module" );
}


#if defined( WIN32 )

#define PipeRead(handle,buffer,count) _read(handle,buffer,count)
#define FileLength(handle) _filelength(handle)

#else // !defined( WIN32 )

#define PipeRead(handle,buffer,count) read(handle,buffer,count)
#define FileLength(handle) myfilelength(handle)

static int myfilelength( int handle )
{
    struct stat statbuffer;
    int result;
    result = fstat(handle, &statbuffer);
    if ( result == 0 )
    {
        return statbuffer.st_size;
    }

    return 0;
}

#endif // !defined( WIN32 )


static void write_socket
(   GssCommsObserver::ObservedSocketType * socket,
    char * buff,
    int bufflen,
    const int maxbufflen )
{
    // convert LF to CRLF, required for both Unix and MS telnet clients.
    // In ConsoleDetachUtility, we set the mode of the streams to _O_TEXT which
    // is supposed to do this CRLF translation for us, however it doesn't.

    char * start = buff;
    char * end = buff+bufflen;
    char * current = buff;

    char theCR = 0x0D;

    while ( current < end )
    {
        while ( ((*current) != 0x0A) && (current < end) )
        {
            current++;
        }

        if ((*current) == 0x0A)
        {
            socket->write(start, (current-start)+1);   
            socket->write((&theCR), 1);   

            ++current;
            start = current;
        }
        else // (current < end) ie. must have got to the end before a LF
        {
            socket->write(start, (current-start));   
        }
    }

}

void BackgroundSimulationModule::preLoop()
{
    const int bufflen=2048;
    char buffer[bufflen];
    int flen = 0;
    bool read_failed = false;
    int bytes_read;

    const CommsObserverList & observers = getCommsObservers();

    // optimised for one observer with lots of output
    while ( ( (flen = FileLength(pipe_out[ReadEnd])) > 0 ) && (!read_failed) )
    {
        if (observers.empty())
        {
            // Need to read from the pipe, even if no observers are present.
            // Otherwise, the buffer overflows
            bytes_read = PipeRead(pipe_out[ReadEnd],buffer,bufflen);
            read_failed = ( bytes_read == -1 );
        }
        else
        {
            bytes_read = PipeRead(pipe_out[ReadEnd],buffer,bufflen);
            read_failed = ( bytes_read == -1 );
            for ( CommsObserverList::const_iterator o_iter = observers.begin() ;
                  o_iter != observers.end() ;
                  o_iter++
                )
            {
                if (!read_failed)
                {
                    write_socket( (*o_iter)->getSocket(), buffer, bytes_read, bufflen);
                }
            }
        }
    }
    
    while ( ( (flen = FileLength(pipe_err[ReadEnd])) > 0 ) && (!read_failed) )
    {
        if (observers.empty())
        {
            // Need to read from the pipe, even if no observers are present.
            // Otherwise, the buffer overflows
            bytes_read = PipeRead(pipe_err[ReadEnd],buffer,bufflen);
            read_failed = ( bytes_read == -1 );
        }
        else
        {
            bytes_read = PipeRead(pipe_err[ReadEnd],buffer,bufflen);
            read_failed = ( bytes_read == -1 );
            for ( CommsObserverList::const_iterator o_iter = observers.begin() ;
                  o_iter != observers.end() ;
                  o_iter++
                )
            {
                if (!read_failed)
                {
                    write_socket( (*o_iter)->getSocket(), buffer, bytes_read, bufflen);
                }
            }
        }
    }
    
}

void BackgroundSimulationModule::addCommsObserver(GssCommsObserver * com)
{
    // do inherited stuff first
    GssSimulationModule::addCommsObserver(com);

    BackgroundCommsObserver * bco = dynamic_cast<BackgroundCommsObserver *>(com);

    if ( bco != 0 )
    {
        std::cout << "Hi." << std::endl;
        TelnetBuffer line;
        line.assign((const unsigned char *)"\n");
        bco->processLine(line);
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "WARN: comms observer not BackgroundCommsObserver !!");
    }
}



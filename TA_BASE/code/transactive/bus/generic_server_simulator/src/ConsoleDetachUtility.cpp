/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/ConsoleDetachUtility.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Definition/Implementation file for ConsoleDetachUtility class
  */
#ifdef WIN32
#pragma warning (disable : 4786)

// due to ACE and sockets and winsock problems, winsock2 must be included
// before windows.h
#include <winsock2.h>
#include <windows.h>
#include <io.h>

#endif // #ifdef WIN32

#include <fcntl.h>

#include "bus/generic_server_simulator/src/ConsoleDetachUtility.h"
#include "bus/generic_server_simulator/src/BackgroundSimulationModule.h"
#include "bus/generic_server_simulator/src/BackgroundCommsObserver.h"

// #include "core/utilities/src/RunParams.h"
// #include "core/utilities/src/DebugUtil.h"

#include <iostream>

using namespace TA_Base_Bus;

///
/// std::cin STDIN replacement pipe
int pipe_in[2] = {-1,-1};

///
/// std::cout STDOUT replacement pipe
int pipe_out[2] = {-1,-1};

///
/// std::cerr STDERR replacement pipe
int pipe_err[2] = {-1,-1};


int ConsoleDetachUtility::detach(const std::string & host, const std::string & port)
{
#if defined( WIN32 )
    return detach_win(host, port);
#else // !defined( WIN32 )
    return detach_unix(host, port);
#endif // !defined( WIN32 )
}


int ConsoleDetachUtility::detach_win(const std::string & host, const std::string & port)
{
#if defined( WIN32 )
    int detach_val = 0;

    GssSimulationModule::PortList ports;
    ports.push_back(port);

    //
    // The class which does the job of hanging around in the background being
    // the simulator command line is a GssSimulationModule, so we could 
    // instantiate it with the GssSimulationModuleFactory, or even through its
    // ModuleCreator.  However because we don't want to make it possible to
    // addmod the background simulation module unless there is real cause, ie.
    // we are detaching from the console, we don't normally register the module
    // creator with the ModuleFactory.  Using the ModuleCreator directly would
    // involve constructing a command line, which would then be parsed by the
    // ModuleCreator to get the values we already have; that is, we would be
    // beating about the bush. THUS we go direct to the object.
    BackgroundSimulationModule * bsm = new BackgroundSimulationModule("BackgroundCommandLine", host, ports);

    bsm->startSim();

    std::cout << "Successfully started Background Command Server" << std::endl;
    std::cout << "Redirecting input/output/error streams..." << std::endl;

    // open a pipe to replace the STDIN file/stream
    // It is 1024 chars to match the CommandMonitor getline call
    // _O_TEXT is the standard for the input stream, it maps CR to CR/LF
    if( _pipe( pipe_in, 1024, _O_TEXT ) == -1 )
    {
        return 1;
    }
    else
    {
        fflush(stdin);
        // have stdin now get from the read end of our pipe replacement
        _dup2(pipe_in[ReadEnd],0);
        // fclose(stdin);
    }


    // open a pipe to replace the STDOUT file/stream
    // It is 16384 chars to cause less loss of info
    // if( _pipe( pipe_out, 16384, _O_TEXT ) == -1 )
    if( _pipe( pipe_out, 16384, _O_TEXT ) == -1 )
    {
        return 1;
    }
    else
    {
        fflush(stdout);
        _dup2(pipe_out[WriteEnd],1);
        bool setmode_failure_W = (_setmode( pipe_out[WriteEnd], _O_TEXT) == -1 );
        bool setmode_failure_R = (_setmode( pipe_out[ReadEnd], _O_TEXT) == -1 );
    }

    if( _pipe( pipe_err, 16384, _O_TEXT ) == -1 )
    {
        return 1;
    }
    else
    {
        fflush(stderr);
        _dup2(pipe_err[WriteEnd],2);
        bool setmode_failure_W = (_setmode( pipe_err[WriteEnd], _O_TEXT) == -1 );
        bool setmode_failure_R = (_setmode( pipe_err[ReadEnd], _O_TEXT) == -1 );
    }


    BOOL is_free = FreeConsole();
    if ( ! is_free )
    {
        detach_val = GetLastError();
    }

    m_isDetached = true;

    return detach_val;

#endif // defined( WIN32 )

    return 0;
}


int ConsoleDetachUtility::detach_unix(const std::string & host, const std::string & port)
{

#if !defined( WIN32 )
/*
Here's some of the better references used in implementing this function:

http://www.erlenstar.demon.co.uk/unix/faq_2.html
http://www.erlenstar.demon.co.uk/unix/faq_3.html
http://www.erlenstar.demon.co.uk/unix/faq_8.html
http://www.aarnet.edu.au/~gdt/articles/2003-09-nowhere-daemon/daemon.html
http://www.opengroup.org/onlinepubs/009695399/functions/dup.html
http://www.opengroup.org/onlinepubs/009695399/functions/pipe.html
http://www.opengroup.org/onlinepubs/009695399/functions/setsid.html
http://www.gnu.org/software/libc/manual/html_node/Signal-Handling.html#Signal%20Handling

*/
    pid_t pid = fork();

    if (pid == -1)
    {
        // Here pid is -1, the fork failed. Some possible reasons are that the
        // system is out of process slots or virtual memory.
        perror("The fork failed!");
    }
    else if (pid > 0)
    {
        //
        // pid greater than zero is parent getting the child's pid
        std::cout << "fork() succeeded, child pid=" << pid << std::endl;
        std::cout << "_exit(0)ing..." << std::endl;
        _exit(0);
    }
    else
    {
        // pid of zero is the child
        // Here we're the child...what should we do?
        // ...
        // but after doing it, we should do something like:
        (void)umask((mode_t)0077);

        // Normally this is done so that an admin can unmount the directory
        // that the daemon was started from. In our case, that's not so good,
        // as we want to be able to continue logging (which occasionally
        // rolls files over). Thus, we don't really want to do this.
        // chdir("/");

    }

    m_isDetached = true;

    return 0;

#endif // #if !defined( WIN32 )

    return 0;

}


bool ConsoleDetachUtility::isDetached()
{
    return m_isDetached;
}

bool ConsoleDetachUtility::m_isDetached = false;


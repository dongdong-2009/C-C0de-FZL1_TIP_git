/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/ExecCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Definition/Implementation file for ExecCommand class
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/ExecCommand.h"
#include "bus/generic_server_simulator/src/CommandMonitor.h"
#include "bus/generic_server_simulator/src/GssUtility.h"

#include "core/utilities/src/DebugUtil.h"

#include <iostream>

using namespace TA_Base_Bus;


ExecCommand::ExecCommand(CommandOwner * owner, CommandMonitor * commandMonitor)
: LeafCommand(owner, "exec")
, m_commandMonitor(commandMonitor)
, m_thread(0)
{
}

ExecCommand::~ExecCommand()
{
    m_thread->terminateAndWait();
    delete m_thread;
}

bool ExecCommand::execute(TokenList & args, const std::string & argstr)
{
    if ( m_commandMonitor == 0 )
    {
        std::cerr << "No command monitor!!" << std::endl;
        return false;
    }

    //
    // basic argument checking
    if ((args.size() > 3) || (args.size() < 1))
    {
        std::cerr << "WARNING: incorrect number of parameters. Expected 1-3, got "
                  << args.size() << ": " << argstr << std::endl;
        return false;
    }
    
    std::string s;
    TokenList parameters;
    TokenList::const_iterator argiter = args.begin();

    bool ok = false;
    bool isStop = false;

    int  repeatVal=0;
    bool isRepeat = false;

    int  sleepVal = 0;
    bool isSleep = false;

    std::string filename;
    bool isFile = false;

    while ( argiter != args.end() )
    {
        s = *argiter;
        parameters.clear();
        GssUtility::tokenise(s.c_str(), parameters, "=");

        if (parameters.size() == 2)
        {
            std::string fieldstr = parameters.front();
            parameters.pop_front();

            std::string valuestr = parameters.front();
            parameters.pop_front();

            if (fieldstr == "repeat")
            {
                repeatVal = atoi(valuestr.c_str());
                isRepeat = true;
            }
            else if (fieldstr == "sleep")
            {
                sleepVal = atoi(valuestr.c_str());
                isSleep = true;
            }
            else if (fieldstr == "file")
            {
                filename = valuestr;
                isFile = true;
            }
            else
            {
                std::cerr << "ERROR: unrecognised exec argument: " << fieldstr << std::endl;
            }
            
        }
        else if ((parameters.size() == 1) && (args.size()==1))
        {
            //
            // if there is only one arg, and it doesn't have an '=' in it, it is
            // either a bare (non "file=...") file argument, or "stop"
            if (s=="stop")
            {
                isStop = true;
            }
            else
            {
                filename = s;
                isFile = true;
            }
        }
        else
        {
            //TODO log a warning
            std::cerr << "ERROR: Could not parse. Expected \"field=value\" pair, got: " << s << std::endl;
            return false;
        }

        argiter++;
    }

    if ( isStop )
    {
        std::cout << "Stopping exec thread..." << std::endl;
        m_thread->terminateAndWait();
        delete m_thread;
        m_thread = 0;
    }
    else if ((isRepeat) || (isSleep))
    {
        //
        // if repeat or sleep are specified, must specify all 3
        if (!isRepeat || !isSleep || !isFile)
        {
            std::cerr << "ERROR: must specify all 3 of repeat, freqency and file" << std::endl;
            return false;
        }
        
        //
        // only run one exec thread at a time
        if ( m_thread != 0)
        {
            if ( m_thread->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
            {
                std::cerr << "ERROR: exec already running: " << m_thread->getFilename() << std::endl;
                return false;
            }
            else if (( m_thread->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_TERMINATED )
                || ( m_thread->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_NEW ) )
            {
                // the thread has already terminated, but we need to call
                // terminateAndWait to fix the internal state.
                m_thread->terminateAndWait();
                m_thread->setParameters(m_commandMonitor, repeatVal, sleepVal, filename);
            }
            else
            {
                std::cerr << "ERROR: exec thread in unknown state, : " << m_thread->getFilename() << std::endl;
                return false;
            }
        }

        if (m_thread==0)
        {
            m_thread = new ExecThread(m_commandMonitor, repeatVal, sleepVal, filename);
        }

        m_thread->start();
    }
    else if (isFile)
    {
        ok = m_commandMonitor->executeScript(filename);

		if (ok)
		{
			std::cout << "finished " << getName() << ": " << filename << std::endl;
		}
		else
		{
			std::cerr << "ERROR: " << getName() << " failed: " << filename << std::endl;
		}

    }

    return ok;
}


void ExecCommand::usage(const std::string & name) const
{
    std::cout << name << " <[file=]filename> [repeat=n] [sleep=millisecs] [stop]" << std::endl;
}

ExecThread::ExecThread( CommandMonitor * commandMonitor, int repeat, int sleep_val, const std::string & filename )
: m_threadIsRunning( false )
, m_doThreadTerminate( false )
, m_commandMonitor(commandMonitor)
, m_repeat(repeat)
, m_sleep(sleep_val)
, m_filename(filename)
{
}


void ExecThread::terminate()
{
    //
    // only terminate if we haven't decided to already ourself
    if ( !m_doThreadTerminate )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ExecThread::terminate()");
        m_doThreadTerminate = true;
    }

}

void ExecThread::run()
{
    FUNCTION_ENTRY("ExecThread::run");

    m_threadIsRunning = true;
    int current_repeat = 0;

    while (!m_doThreadTerminate)
    {
        current_repeat++;

        try
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Script %s execution repeat #%d", m_filename.c_str(), current_repeat );

            //
            // we want to terminate if the script execution didn't succeed
            m_doThreadTerminate = !(m_commandMonitor->executeScript(m_filename));
        }
        catch ( ... )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Miscellaneous exception caught while executing script");
        }

        //
        // Wait for a few seconds before trying again
        if ( ((m_repeat > 0) && (current_repeat >= m_repeat)) || (m_doThreadTerminate))
        {
            break;
        }

        sleep(m_sleep);
    }

    m_threadIsRunning = false;

    terminate();

    FUNCTION_EXIT;
}

void ExecThread::setParameters( CommandMonitor * commandMonitor, int repeat, int sleep_val, const std::string & filename )
{
    m_commandMonitor = commandMonitor;
    m_repeat = repeat;
    m_sleep = sleep_val;
    m_filename = filename;
    m_doThreadTerminate = false;
}

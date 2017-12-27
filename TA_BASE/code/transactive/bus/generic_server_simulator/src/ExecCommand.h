#ifndef ExecCommand_H
#define ExecCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/ExecCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Declaration file for classes used by GssCommandProcessor to handle sub commands,
  * being 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/LeafCommand.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_Bus
{
    class CommandMonitor;
    class ExecThread;

    /** Implements the "exec" command
      * 
      * If a repeat and frequency are specified, runs the script repeatedly
      * in a background thread
      */
    class ExecCommand : public LeafCommand
    {
	public:
        /** 
          * 
          */
        ExecCommand(CommandOwner * owner, CommandMonitor * commandMonitor);

        /** 
          * 
          */
        ~ExecCommand();

        /** 
          * 
          */
        bool execute(TokenList & args, const std::string & argstr);

        /** 
          * 
          */
        void usage(const std::string & name) const;

    private:
        ///
        ///
		CommandMonitor * m_commandMonitor;
	
        ///
        ///
        ExecThread * m_thread;

	}; // class ExecCommand


    /**
      * A background thread run to implement repeatedly executing a script
      * file.
      */
    class ExecThread : public TA_Base_Core::Thread
    {

    public:
        ExecThread( CommandMonitor * commandMonitor, int repeat, int frequency, const std::string & filename );

        /** The run method must be implemented in any class that inherits from
          * TA_Base_Core::Thread.  */
        void run();

        /** The terminate method must be implemented in any class that inherits
          * from TA_Base_Core::Thread.  */
        void terminate();

        std::string getFilename() const;

        void setParameters( CommandMonitor * commandMonitor, int repeat, int frequency, const std::string & filename );

    private:
        ///
        ///
        volatile bool m_threadIsRunning;

        ///
        ///
        volatile bool m_doThreadTerminate;

        ///
        ///
		CommandMonitor * m_commandMonitor;
	
        ///
        ///
        int m_repeat;

        ///
        ///
        int m_sleep;

        ///
        ///
        std::string m_filename;
    }; // class ExecThread

    inline std::string ExecThread::getFilename() const
    {
        return m_filename;
    }

} // namespace TA_Base_Bus

#endif // #ifndef ExecCommand_H

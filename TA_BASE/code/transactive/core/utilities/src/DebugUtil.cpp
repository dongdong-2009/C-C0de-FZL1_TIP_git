/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/DebugUtil.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/07/19 16:49:01 $
  * Last modified by:  $Author: Ouyang $
  *
  * This file contains the implementation of the DebugUtil class.
  *
  */

#if defined( SOLARIS ) || defined( LINUX )
#include <pthread.h>                        // Enables thread ID to be included in the log
#endif                                      // defined( SOLARIS ) || defined( LINUX )

#if defined( FREEBSD )                      // Geoff at home
#include <sys/types.h>
#endif                                      // defined( FREEBSD )

#include <sys/timeb.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <sstream>
#include "core/utilities/src/DebugSETranslator.h"
#include "DebugUtil.h"
#include "GenericLogType.h"
#include "PreformattedLogType.h"
#include "Logger.h"
#include "TAAssert.h"
#include "NullOstream.h"
#include "core/exceptions/src/DebugEmptyArgsException.h"
#include "core/exceptions/src/AssertException.h"
#include "RunParams.h" 

#ifdef WIN32
    // For stack tracing api
    #ifndef STRACE_EXPORTS
        #define STRACE_EXPORTS
    #endif
    #include "cots/WinStackTrace/include/stack.h"

    // For pid
    #include <process.h>
    typedef int pid_t;
#else
    #include <unistd.h>
#endif

#ifdef _MSC_VER
    #pragma warning(disable: 4715)  // not all control paths return a value
#endif

#if defined( FREEBSD )                      // Geoff at home
extern "C" int ftime( struct timeb *tp);
#endif                                      // defined( FREEBSD )


namespace TA_Base_Core
{
    DebugUtil* DebugUtil::m_instance = 0;

    // If any new debug types are created insert it into this array, making sure that
    // the GenericLog type is the last element in the array.

	struct DebugTypeData
	{
		DebugUtil::EDebugType		debugType;
		DebugUtil::EDebugLevel		debugLevel;
		const char*		formatString;
	};

    DebugTypeData DebugTypeInfo[] =     
    {
        {DebugUtil::FunctionEntry,         DebugUtil::DebugTrace, "Function Entered: %s"},
        {DebugUtil::FunctionExit,          DebugUtil::DebugTrace, "Function Exited: %s"},
        {DebugUtil::ExceptionConstruction, DebugUtil::DebugError, "TA Exception Constructed: %s: %s"},
        {DebugUtil::ExceptionDetails,      DebugUtil::DebugError, "TA Exception Details: %s: %s"},
        {DebugUtil::ExceptionCatch,        DebugUtil::DebugError, "Exception Caught: %s: %s"},

        // This one (GenericLog) always remains at the end
        {DebugUtil::GenericLog, DebugUtil::DebugWarn, NULL},
    };


    DebugUtil& DebugUtil::getInstance()
    {
        // make this thread safe!

        // Fix for TD2689: m_lock was not being instantiated when DebugUtil was instantiated
        // from a static object. An accessor method is now used so that construction of the
        // lock is guaranteed.
        TA_Base_Core::ThreadGuard guard (getLock());

        if( m_instance == 0 )
        {
            m_instance = new DebugUtil();
        }
        
        return *m_instance;
    }

	void DebugUtil::removeInstance()
	{
		{
			TA_Base_Core::ThreadGuard guard (getLock());
		
			if( m_instance != NULL )
			{
				delete m_instance;
			}
		}

		TA_Base_Core::ReEntrantThreadLockable* lock = &(getLock());
		delete lock;
	}

    DebugUtil::~DebugUtil()
    {
        // Putting within try/catch so that pointers
        // will still be deleted if an exception is
        // thrown to prevent memory leaks

        try 
        {
            if ( m_outFile.is_open() )
            {
                m_outFile.flush();
                m_outFile.close();
            }
        }
        catch ( ... )
        {
        }

        delete m_logger;
        m_logger = NULL;
    }


    void DebugUtil::log(const std::string& file, int line, EDebugType type, ...)
    {
        TA_DEBUG_ASSERT ( !file.empty(), "File name not provided" );

        // Line number can be -1 if it is unknown (as is possibly the case
        // with the FunctionEntry class).
        TA_DEBUG_ASSERT ( line > 0 || line == -1, "Line number not provided" );

        TA_Base_Core::ThreadGuard guard (getLock());

        // Get the variable args

        va_list args;
        va_start ( args, type );

        try
        {
            // Ensure that args is pointing to something
            
            TA_DEBUG_ASSERT ( args != NULL, "Pointer to variable args is NULL" );

            // Determine the level of the log message. If the debug type is
            // GenericLog, the level SHOULD be the first argument in the 
            // variable argument.
            //
            // We retrieve the level as an int and then cast it to an enum 
            // when using it. This is because when this is run under Solaris, 
            // enums are converted to ints when passed through ... (variable 
            // argument). If we try to retrieve level as an enum here, it will 
            // throw an exception.
            
            int level;
            if ( type == GenericLog )
            {
                level = va_arg ( args, int );

                // Check that the level is valid - getStringDebugLevel calls
                // TA_DEBUG_ASSERT if the level does not exist

                std::string levelCheck = getStringDebugLevel ( ( EDebugLevel ) level );

                // Reset the arg pointer back to the beginning

                va_start ( args, type );
            }
            else
            {
                // Check that there are arguments

                char* firstArg = va_arg ( args, char* );
                
                TA_DEBUG_ASSERT ( firstArg != NULL, "Empty first argument");
                TA_DEBUG_ASSERT ( strcmp(firstArg, "") != 0, "Empty or incorrect first argument");

                // Don't catch the exception and rethrow the AssertException
                std::string temp(firstArg);
                
                // Reset the arg pointer back to the beginning

                va_start ( args, type );

                // Retrieve the level for the preformatted type

                level = m_logger->getLevel ( type );
            }

			m_lastMsgLevel = ( EDebugLevel ) level;

			// If logging is turned off for the level then do nothing
			if (preLogChecks(static_cast<EDebugLevel>(level)))
			{
				// Reset the arg pointer
				va_end ( args );
				return;
			}

            // If the current debug level is FATAL or ERROR we want to store any
            // lower level log messages to a cache. If the log message is FATAL
            // or ERROR then the cache will be printed along with the log message
            // to aid the developer in determining the cause of the problem.

            bool printCache;
            bool simpleLog = false;
            if ( m_currDebugLevel == DebugFatal || m_currDebugLevel == DebugError )
            {
                // Determine whether to print the cache and log message, or just
                // to store the message in the cache.

                if ( level <= m_currDebugLevel )
                {
                    // Log the message and print the cache

                    printCache = true;
                    logMessage( file, line, type, args, simpleLog, printCache );

                }
                else
                {
                    // Just add the message to the cache

                    m_logger->addToCache ( type, file, line, args, simpleLog );
                }
            }
            else
            {
                if ( level > m_currDebugLevel )
                {
                    // Level is too low, don't log anything
                }
                else
                {
                    // Just log the message 

                    printCache = false;
                    logMessage( file, line, type, args, simpleLog, printCache );
                }
            }

            // Reset the arg pointer

            va_end ( args );
        }
        catch ( TA_Base_Core::AssertException& dmce )
        {
            // This exception was caught from TA_DEBUG_ASSERT so throw it again, 
            // as these situations should not occur if LOG is used correctly.

            va_end ( args );
            throw dmce;
        }
        catch ( TA_Base_Core::DebugMsgConstructionException& dmce )
        {
            handleException ( "DebugMsgConstructionException", dmce, args );
        }
        catch ( TA_Base_Core::TransactiveException& tae )
        {
            handleException ( "Unknown", tae, args );
        }
        catch ( ... )
        {
            // Write to stderr that an unknown exception has been caught. It is
            // caught here rather than rethrowing it because it seems inconvenient
            // to do a try/catch everywhere LOG() is called.

            std::cerr << "DebugUtil::log(): Caught an unknown exception. Message will not be logged." << std::endl;
            
            // Reset the arg pointer
            va_end ( args );
        }

    }


    void DebugUtil::logGeneric(const std::string& file, int line, ...)
    {
        TA_DEBUG_ASSERT ( !file.empty(), "File name not provided" );

        // Line number can be -1 if it is unknown (as is possibly the case
        // with the FunctionEntry class).
        TA_DEBUG_ASSERT ( line > 0 || line == -1, "Line number not provided" );

        TA_Base_Core::ThreadGuard guard (getLock());

        // Get the variable args

        va_list args;
        va_start ( args, line );

        try
        {
            // Ensure that args is pointing to something
            
            TA_DEBUG_ASSERT ( args != NULL, "Pointer to variable args is NULL" );

            // Determine the level of the log message. If the debug type is
            // GenericLog, the level SHOULD be the first argument in the 
            // variable argument.
            //
            // We retrieve the level as an int and then cast it to an enum 
            // when using it. This is because when this is run under Solaris, 
            // enums are converted to ints when passed through ... (variable 
            // argument). If we try to retrieve level as an enum here, it will 
            // throw an exception.
            
            int level;
            level = va_arg ( args, int );

            // Check that the level is valid - getStringDebugLevel calls
            // TA_DEBUG_ASSERT if the level does not exist

            std::string levelCheck = getStringDebugLevel ( ( EDebugLevel ) level );

            // Reset the arg pointer back to the beginning

            va_start ( args, line );
            TA_DEBUG_ASSERT ( args != NULL, "Pointer to variable args is NULL" );

            m_lastMsgLevel = ( EDebugLevel ) level;

			// If logging is turned off for the level then do nothing
			if (preLogChecks(static_cast<EDebugLevel>(level)))
			{
				// Reset the arg pointer
				va_end ( args );
				return;
			}

            // If the current debug level is FATAL or ERROR we want to store any
            // lower level log messages to a cache. If the log message is FATAL
            // or ERROR then the cache will be printed along with the log message
            // to aid the developer in determining the cause of the problem.

            bool printCache;
            bool simpleLog = false;
            if ( m_currDebugLevel == DebugFatal || m_currDebugLevel == DebugError )
            {
                // Determine whether to print the cache and log message, or just
                // to store the message in the cache.

                if ( level <= m_currDebugLevel )
                {
                    // Log the message and print the cache

                    printCache = true;
                    logMessage( file, line, DebugUtil::GenericLog, args, simpleLog, printCache );

                }
                else
                {
                    // Just add the message to the cache

                    m_logger->addToCache ( DebugUtil::GenericLog, file, line, args, simpleLog );
                }
            }
            else
            {
                if ( level > m_currDebugLevel )
                {
                    // Level is too low, don't log anything
                }
                else
                {
                    // Just log the message 

                    printCache = false;
                    logMessage( file, line, DebugUtil::GenericLog, args, simpleLog, printCache );
                }
            }

            // Reset the arg pointer

            va_end ( args );
        }
        catch ( TA_Base_Core::AssertException& dmce )
        {
            // This exception was caught from TA_DEBUG_ASSERT so throw it again, 
            // as these situations should not occur if LOG is used correctly.

            va_end ( args );
            throw dmce;
        }
        catch ( TA_Base_Core::DebugMsgConstructionException& dmce )
        {
            handleException ( "DebugMsgConstructionException", dmce, args );
        }
        catch ( TA_Base_Core::DebugEmptyArgsException& deae )
        {
            handleException ( "DebugEpmtyArgsException", deae, args );
        }
        catch ( TA_Base_Core::TransactiveException& tae )
        {
            handleException ( "Unknown", tae, args );
        }
        catch ( ... )
        {
            // Write to stderr that an unknown exception has been caught. It is
            // caught here rather than rethrowing it because it seems inconvenient
            // to do a try/catch everywhere LOG() is called.

            std::cerr << "DebugUtil::logGeneric(): Caught an unknown exception. Message will not be logged" << std::endl;
            
            // Reset the arg pointer
            va_end ( args );
        }

    }

    void DebugUtil::logMore(const std::string& file, int line, ...)
    {
        TA_Base_Core::ThreadGuard guard (getLock());

		// If logging is turned off for the level then do nothing
		if (preLogChecks(m_lastMsgLevel))
		{
			return;
		}

        // Get the variable args

        va_list args;
        va_start ( args, line );

        try
        {
            TA_DEBUG_ASSERT ( args != NULL, "Pointer to variable args is NULL" );

            // First check that there are arguments

            char* firstArg = va_arg ( args, char* );
            TA_DEBUG_ASSERT ( firstArg != NULL, "Empty arguments");
            TA_DEBUG_ASSERT ( firstArg != "", "Empty arguments");

            // Reset the arg pointer back to the beginning
            va_start ( args, line );

            // If the current debug level is FATAL or ERROR we want to store any
            // lower level log messages to a cache. If the log message is FATAL
            // or ERROR then the cache will be printed along with the log message
            // to aid the developer in determining the cause of the problem.

            bool printCache;
            bool simpleLog = true;
            if ( m_currDebugLevel == DebugFatal || m_currDebugLevel == DebugError )
            {
                // Determine whether to print the cache and log message, or just
                // to store the message in the cache.

                if ( m_lastMsgLevel <= m_currDebugLevel )
                {
                    // Log the message and print the cache

                    printCache = true;
                    logMessage( NULL, -1, GenericLog, args, simpleLog, printCache );
                }
                else
                {
                    // Just store message in cache

                    m_logger->addToCache ( GenericLog, "", -1, args, simpleLog );
                }
            }
            else
            {
                if ( m_lastMsgLevel > m_currDebugLevel )
                {
                    // Level is too low, don't log anything
                }
                else
                {
                    // Just log the message

                    printCache = false;
                    logMessage( "", -1, GenericLog, args, simpleLog, printCache );
                }
            }

            // Reset the arg pointer

            va_end ( args );
        }
        catch ( TA_Base_Core::AssertException& dmce )
        {
            // This exception was caught from TA_DEBUG_ASSERT so throw it again, 
            // as these situations should not occur if LOG is used correctly.
            va_end ( args );
            throw dmce;
        }
        catch ( TA_Base_Core::DebugMsgConstructionException& dmce )
        {
            handleException ( "DebugMsgConstructionException", dmce, args );
        }
        catch ( TA_Base_Core::TransactiveException& tae )
        {
            handleException ( "Unknown", tae, args );
        }
        catch ( ... )
        {
            // Write to stderr that an unknown exception has been caught. It is
            // caught here rather than rethrowing it because it seems inconvenient
            // to do a try/catch everywhere LOG() is called.
            std::cerr << "DebugUtil::logMore(): Caught an unknown exception. Message will not be logged" << std::endl;
            
            // Reset the arg pointer
            va_end ( args );
        }
    }


    void DebugUtil::logStackTrace(CONTEXT* executionContext /* = NULL */)
    {
        std::string str;

        if (RunParams::getInstance().isSet(RPARAM_DEBUGLOGSTACKTRACE))
        {
            #ifdef WIN32
                struct StringTracer : public LineTracer
                {
                    StringTracer(std::string& str) : m_str(str) {}

                    // Implementation of LineTracer
                    virtual void Init() {}
                    virtual void Close() {}
                    virtual void TraceLine(PSTR Line)
                    {
                        m_str.append(Line);
                    }

                  std::string& m_str;
                };

                if (executionContext == NULL)
                {
                    TraceCurrentLocation(&StringTracer(str));
                }
                else
                {
                    StackTracer(executionContext, &StringTracer(str));
                }

            #else
                str = "DebugUtil::logStackTrace(CONTEXT*): NOT IMPLEMENTED";
            #endif // #ifdef WIN32
        }
        else
        {
            str = "DebugUtil::logStackTrace(CONTEXT*): Not logging stack trace";
        }

        logLargeString(SourceInfo, DebugInfo, str);
    }

    void DebugUtil::setFile(const char* name, int maxSize)
    {
        TA_Base_Core::ThreadGuard guard (getLock());

        if (maxSize)
        {
            setMaxSize(maxSize);
        }

        // Close file if currently open
		//TD17314 make sure the setFile won't make the log file size small than config
		if( !m_filename.empty() && m_filename == name && m_sizeInBytes < m_maxSizeInBytes)
		{
			return;
		}
        if (m_outFile.is_open())
        {
            m_outFile.flush();
            m_outFile.close();
        }

        if( name == NULL || name[0] == '\0' )
        {
            // Notify that messages will be printed to stdout

            std::cerr << "Cannot set file to empty string. Log messages will be written to stdout." << std::endl;
            m_filename = "";
            return;
        }


        // BCF 19-Nov-2001 PR1028:
        // If the file exists, then open the file for reading and count the number
        // of lines which the file already contains. This is not quite the same as
        // the usual meaning of nLines (which actually measure the number of
        // *entries*), but gives us a good enough estimate to work with - we just
        // want to make sure that the logfile doesn't grow without bound if the
        // process is continually restarted.
        //
        // The 1000 character buffer may seem excessive, but the file will turn
        // bad if a line is longer than the given buffer, so best to be sure
        //
        // It is pretty inefficient to scan the entire file just to determine its
        // size, but this should only happen once in the process's life. If anyone
        // has a better idea please feel free give it a go (I tried taking the
        // difference of the begin and end positions, but it didn't work)

		try
        {
			 m_sizeInBytes = 0;

			 // Store filename in class variable
			 m_filename = name;

			 // Used to check if a new file needs to be opened.
			 // setFile is only called externally so if m_filename
			 // is different from m_defaultFilename then a debug
			 // level specific file has been used in between.
			 m_defaultFilename = name;

			 std::string fileOpen = "";

			 ++m_fileCount;
			 
			 // Create the file. The filename will be the name with a number
			 // appended onto the end before the extension. The number is a 
			 // count of how many log files have been written to so far,
			 // for the default log file. Specific level files are handled
			 // seperately.  Eg. If the name is AlarmProcessor.log", the output file will 
			 // be set to "AlarmProcessor.1.log".
			 fileOpen = createFilename(m_fileCount);

			 // Check the size of the file
			 m_sizeInBytes = getFileSize(fileOpen);
			 
			 if (m_sizeInBytes == -1)
			 {
				 // Couldn't acertain size of file so message logged to cerr
				 return;
			 }
			 
			 // BCF 16-Nov-2001 PR1028:
			 // Open the file for appending (retain existing entries)
			 m_outFile.clear();
			 m_outFile.open(fileOpen.c_str(),std::ios::out|std::ios::app);
			 if (m_outFile.fail())
			 {
				 std::cerr << "DebugUtil::SetFile(): error opening file! - The reason is: "
					 << strerror( errno )
					 << std::endl;
				 m_outFile.clear();
			 }

             LOG0( SourceInfo, DebugUtil::DebugInfo, "\n"
                "\t**********************************************************************************************************************\n"
                "\t*                                                   START OF FILE                                                    *\n" 
                "\t**********************************************************************************************************************" );
        }
        catch ( ... )
        {
            // Write to stderr that an unknown exception has been caught. It is
            // caught here because a thread lock has been acquired so if an 
            // exception is thrown, the lock needs to be released.
			
            std::cerr << "Caught an unknown exception while attempting to set file."
                << " Messages will be logged to stdout." << std::endl;
        }
    }    


    struct DebugLevelNameValue
    {
        const char*             name;
        DebugUtil::EDebugLevel   value;
    };

    static DebugLevelNameValue nameValueMapping[] =
    {
		{ "FATAL",		DebugUtil::DebugFatal		},
        { "ERROR",		DebugUtil::DebugError		},
        { "WARNING",	DebugUtil::DebugWarn		},
        { "NORMAL",		DebugUtil::DebugNorm		},
        { "INFO",		DebugUtil::DebugInfo		},
        { "DEBUG",		DebugUtil::DebugDebug		},
        { "SQL",        DebugUtil::DebugSQL         },
        { "CORBA",      DebugUtil::DebugCORBA       },
        { "MSG",	    DebugUtil::DebugMsg	        },
        { "MSGPUB",	    DebugUtil::DebugMsgPub	    },
        { "PERIODIC",   DebugUtil::DebugPeriodic    },
        { "DATABASE",	DebugUtil::DebugDatabase	},
		{ "EXTERNAL",	DebugUtil::DebugExternal	},
		{ "TRACE",		DebugUtil::DebugTrace		},
        { NULL,			DebugUtil::DebugInfo		}
    };


    const char* DebugUtil::getStringDebugLevel(EDebugLevel level)
    {
        DebugUtil::EDebugLevel result = DebugUtil::DebugFatal;
        DebugLevelNameValue* nv = nameValueMapping;

        TA_DEBUG_ASSERT( nv != NULL, "Object is NULL" );

        while (nv->name != NULL)
        {
            if (level == nv->value)
            {
                return nv->name;
            }
            nv++;
        }

        // Should never come down here, but just in case
        TA_DEBUG_ASSERT ( false, "Undefined DebugLevel" );
        return "UNKNOWN";
    }


    DebugUtil::EDebugLevel DebugUtil::getDebugLevelFromString(const char* levelStr)
    {

        DebugUtil::EDebugLevel result = DebugUtil::DebugFatal;
        DebugLevelNameValue* nv = nameValueMapping;

        TA_DEBUG_ASSERT( nv != NULL, "Object is NULL" );

        while (nv->name != NULL)
        {
            if (strcmp(levelStr, nv->name) == 0)
            {
                result = nv->value;
                return result;
            }
            nv++;
        }

        // Should never come down here, but just in case
        TA_DEBUG_ASSERT ( false, "Debug String is invalid" );
    }   


    void DebugUtil::setMaxCacheSize(int maxCacheSize)
    {
        m_logger->setMaxCacheSize(maxCacheSize);
    }


	void DebugUtil::decodeDisabledLevels(const std::string& disabledString)
	{
		// Check the string is not empty or NULL
		TA_DEBUG_ASSERT(!disabledString.empty(), "String of debug levels to disable is empty");

		// Input string should be of the form 1,2,3,5...etc or
		// FATAL,ERROR,WARN,DEBUG..etc. or a combination of.


		// The getDebugLevelString function checks validity of
		// the debug levels specified in the input string.
		try
		{
			parseDebugLevelCmdLine(disabledString);
		}
		catch (...)
		{
			TA_DEBUG_ASSERT(false, "An error has occurred disabling debug log levels");

			return;
		}
	}


	void DebugUtil::decodeLevelFilenames(const std::string& levelFilenames)
	{
		TA_Base_Core::ThreadGuard guard (getLock());

		// Input string should be of the form 1,filename,2,filename...etc or
		// FATAL,filename,ERROR,filename..etc. or a combination of.  They
		// must be in order i.e. the first debug level will be matched with the
		// first filename and so on.

		// Check the string is not empty
		TA_DEBUG_ASSERT(!levelFilenames.empty(), "String of debug levels to disable is empty");

		try
		{
			parseDebugFileCmdLine(levelFilenames);
		}
		catch (...)
		{
			TA_DEBUG_ASSERT(false, "An error has occurred setting debug level filenames");

			return;
		}
	}


	void DebugUtil::encryptPidInFilenames(const std::string& usePidFlag)
	{
		TA_Base_Core::ThreadGuard guard (getLock());

		// Check the string is not empty
		TA_DEBUG_ASSERT(!usePidFlag.empty(), "String of PID flag is empty");
		TA_DEBUG_ASSERT(usePidFlag.c_str() != NULL, "String of PID flag is NULL");

		if (usePidFlag == "1" || usePidFlag == "true" || usePidFlag == "TRUE")
		{
			m_encodePidInFilenames = true;
		}
	}


    ////////////////// PRIVATE FUNCTIONS //////////////////////
    // Constructor
    //
    // NOTE: Typically the Linux boxes will be embedded processors with limited
    //       memory and disc space.  Therefore the length of the log files will
    //       nee to be somewhat smaller.
    //
    DebugUtil::DebugUtil()
        : m_currDebugLevel( DebugInfo ),
          m_lastMsgLevel( DebugDebug ),
          m_filename( "" ),
		  m_defaultFilename( "" ),
		  m_firstSetFile( true ),
		  m_encodePidInFilenames( false ),
    #if !defined( LINUX )               // See note above
          //m_maxSizeInBytes( 300000 ),
		  m_maxSizeInBytes( 500000 ),   //hyl: cl-17314
    #else                               // defined( LINUX )
          m_maxSizeInBytes( 100000 ),
    #endif                              // defined( LINUX )
		  m_sizeInBytes ( 0 ),
          m_maxFiles( 30 ),  //hyl changed from 20 to 30 for CL-17314
          m_fileCount( 0 ),
          m_logger ( NULL )
    {
        TA_DEBUG_ASSERT ( m_maxSizeInBytes != 0, "RPARAM_DEBUGFILEMAXSIZE == 0" );
        TA_DEBUG_ASSERT ( m_maxFiles != 0, "RPARAM_DEBUGMAXFILES == 0" );

        // Putting initialisation in try/catch block to 
        // prevent memory leak if LoadDebugTypes() throws
        // an exception

        try 
        {
            m_logger = new Logger;

            TA_DEBUG_ASSERT ( m_logger != NULL, "Object not created" );

            loadDebugTypes();

			// More levels can be added dynamically if required
			// Initialise storage vectors
			m_disabledLevels = std::vector<bool>(20, false);
			m_fileLevels = std::vector<bool>(20, false);
			m_levelFilenames = std::vector<std::string>(20, "");
			m_filenameNumber = std::vector<int>(20, -1);
        }
        catch ( ... )
        {
            std::cerr << "There were problems on initialisation. There will be no logging" << std::endl;

            delete m_logger;
            m_logger = NULL;
        }
    }


    void DebugUtil::swapFiles()
    {
        // This method is not thread locked because it is called by thread
        // locked methods.

        // Reset the line count (contains EOF character)

        m_sizeInBytes = 0; 
        if ( m_filename == "" )
        {
            return;
        }

        // m_fileCount started at 0 but was incremented in setFile. So
        // whatever m_fileCount is set to here is how many files there
        // are currently for the default file only.

		std::string newname;
		
		// Using the default file
		if (m_filename == m_defaultFilename)
		{
			++m_fileCount;
			
			// If we have reached our max files, then start
			// deleting the oldest file.
			
			if ( m_fileCount >= (m_maxFiles + 1) )
			{
				int rmFileCnt = m_fileCount - m_maxFiles;

                if ( 3 < rmFileCnt )
                {
                    std::string prevname = createFilename(rmFileCnt);

                    TA_DEBUG_ASSERT ( prevname.c_str() != NULL, "File string is NULL" );

                    // Delete the oldest file 

                    remove(prevname.c_str());
                }
			}
			
			// Close the file
			
			m_outFile.flush();
			m_outFile.close();
			
			/*
			* TD4889 - Once the file has been closed, clear its internal flags
			*          to ensure that the object is clean.
			*/
			m_outFile.clear();
			
			// Create the new file and append a new file count
			
			newname = createFilename(m_fileCount);

		}
		else
		// Using a specified file for the level
		{
			// Increment the file number, use the last message level
			// as an index, to avoid using a loop.
			++(m_filenameNumber[m_lastMsgLevel]);
			
			// If we have reached our max files, then start
			// deleting the oldest file.
			if ( m_filenameNumber[m_lastMsgLevel] >= (m_maxFiles + 1) )
			{
				int rmFileCnt = m_filenameNumber[m_lastMsgLevel] - m_maxFiles;

                if ( 3 < rmFileCnt )
                {
				    std::string prevname = createFilename(rmFileCnt);
    				
				    TA_DEBUG_ASSERT ( prevname.c_str() != NULL, "File string is NULL" );
    				
				    // Delete the oldest file 			
				    remove(prevname.c_str());
                }
			}
			
			// Close the file		
			m_outFile.flush();
			m_outFile.close();
			
			/*
			* TD4889 - Once the file has been closed, clear its internal flags
			*          to ensure that the object is clean.
			*/
			m_outFile.clear();
			
			// Create the new file and append a new file count
			newname = createFilename(m_filenameNumber[m_lastMsgLevel]);
		}

        //std::string newname = createFilename(m_fileCount);

        TA_DEBUG_ASSERT ( newname.c_str() != NULL, "File string is NULL" );

        m_outFile.open(newname.c_str(),std::ios::out|std::ios::trunc);
        if (m_outFile.fail())
        {
            std::cerr << "Error opening file '" << newname << "'! - The reason is: "
                      << strerror( errno )
                      << std::endl;   
        }

        // File will contain an EOF character so increment file count - and
        // even if it doesn't, the size is only an approximation anyway to
        // prevent the file growing without a limit, so it doesn't matter if
        // it is a few bytes out

        ++ m_sizeInBytes;
    }


    void DebugUtil::loadDebugTypes()
    {
        // Load GenericLog type

        GenericLogType* glogType = new GenericLogType;
        m_logger->addLogType ( *glogType );
        
        // Load Preformatted types

        const DebugTypeData* dtd = DebugTypeInfo;
        while ( dtd->debugType != GenericLog )
        {
            EDebugType debugType = dtd->debugType;
            EDebugLevel debugLevel = dtd->debugLevel;
            std::string debugFormat = dtd->formatString;

            ++dtd;

            try
            {
                PreformattedLogType* plogType = new PreformattedLogType( debugType, debugLevel, debugFormat );
                m_logger->addLogType ( *plogType );
            }
            catch ( ... )
            {
                std::cout << "Problems encounted while loading debug types. Not all types have been loaded" << std::endl;
            }
        }
    }


    void DebugUtil::logMessage
    (
        const std::string file, 
        int line, 
        EDebugType type, 
        va_list args, 
        bool simpleLog, 
        bool printCache
    )
    {
        // Can't do std::ostream outfile = std::cout as the solaris compiler
        // says the copy constructor is private. Use a flag instead.

        bool writeToCout = true;
        static bool outFileGood = true;

        if ( m_outFile.is_open() && outFileGood )
        {
            // If the current log file has reached its maximum
            // size then write to a new file.

            if ( m_sizeInBytes >= m_maxSizeInBytes )
            {
                swapFiles();
            }

            // Check that there are no problems with the outstream

            if ( !m_outFile.good() )
            {
                outFileGood = false;
                std::cerr << "Error logging message to file. Message will now be logged to stdout." << std::endl;

                // Close the file so all messages will be logged to stdout from now on.
                // We don't want some messages logged to stdout and some to the file.
                try
                {

                    /*
                     * TD4889: Only close the file if it has been openned,
                     *         otherwise don't bother.  The call to close()
                     *         is actually where this defect shows itself.
                     */
                    if ( true == m_outFile.is_open() )
                    {
                        m_outFile.close();
                        m_outFile.clear();
                    }
                }
                catch ( ... )
                {
                }

                m_filename = "";
            }
            else // out stream is fine - write to file instead of cout
            {
                writeToCout = false;
            }
        }

        if ( printCache )
        {
            // Print cached messages
			// Modification for compilation under VC++ 7
            int bytesAdded = 0;
			
			if (writeToCout)
			{
				bytesAdded = m_logger->logCache ( std::cout );
			}
			else
			{
				bytesAdded = m_logger->logCache ( m_outFile );
			}

            m_sizeInBytes += bytesAdded;
            m_logger->clearCache();
        }

        // Log message
		// Modification for compilation under VC++ 7
        int msgLen = 0;
			
		if (writeToCout)
		{
			msgLen = m_logger->log ( type, file, line, args, std::cout, simpleLog );
		}
		else
		{
			msgLen = m_logger->log ( type, file, line, args, m_outFile, simpleLog );
		}
        
        TA_DEBUG_ASSERT ( msgLen != 0, "Log message was an empty string" );
        
        m_sizeInBytes += msgLen;
    }


    void DebugUtil::handleException
    ( 
        const std::string& exceptionName, 
        const TA_Base_Core::TransactiveException& exception, 
        va_list& args 
    )   const
    {
        std::cerr << "Caught " 
                  << exceptionName.c_str() 
                  << " Exception: " 
                  << exception.what() 
                  << std::endl;
        std::cerr << "Message will not be logged." 
                  << std::endl;

        // Reset the arg pointer
        va_end ( args );
    }


    std::string DebugUtil::createFilename(int fileNum) const
    {
        std::ostringstream filename;

#if defined( WIN32 )
        pid_t pid = _getpid();
#else
        pid_t pid = getpid();
#endif
        
        // Find where the extension in the filename starts
        // (ie look for the last '.').
        std::string::size_type dotPos = m_filename.rfind('.');

        if (dotPos != std::string::npos)
        {
            // Found a '.'. Take a copy of what's after it.
            std::string extension = m_filename.substr(dotPos + 1, m_filename.length());

			if (m_encodePidInFilenames == false)
			{
				
				if (atoi(extension.c_str()) == 0 && extension != "0")
				{
					// Extension is a normal string, not a number.
					// (This is most likely the case for Windows.)
					
					// Add everything before the '.' to the filename.
					filename << m_filename.substr(0, dotPos);
					
					// Add the '.' and the number.
					filename << "." << fileNum;
					
					// Add the extension.
					filename << "." << extension;
				}
				else
				{
					// Extension is a number. (Most likely Solaris)
					// Just add the log number to the end.   
					filename << m_filename << "." << fileNum;
				}
				
			}
			else
			{			
				if (atoi(extension.c_str()) == 0 && extension != "0")
				{
					// Extension is a normal string, not a number.
					// (This is most likely the case for Windows.)
					
					// Add everything before the '.' to the filename.
					filename << m_filename.substr(0, dotPos);
					
					// Add the process id
					filename << "." << pid;
					
					// Add the '.' and the number.
					filename << "." << fileNum;
					
					// Add the extension.
					filename << "." << extension;
				}
				else
				{
					// Extension is a number. (Most likely Solaris)
					// Just add the log number to the end.   
					filename << m_filename << "." << pid << "." << fileNum;
				}
			}
        }
        else
        {
 			if (m_encodePidInFilenames == false)
			{
				// No extension at all. Add the log number to the end.
				filename << m_filename << "." << fileNum;
			}
			else
			{
				filename << m_filename << "." << pid;
			}
        }

        return filename.str();
    }


    TA_Base_Core::ReEntrantThreadLockable& DebugUtil::getLock()
    {
        // Fix for TD2689: m_lock was not being instantiated when DebugUtil was instantiated
        // from a static object. An accessor method is now used so that construction of the
        // lock is guaranteed.
        
        // Fix for TD2689: This defect was a sequencing problem - we have no control over the 
        // order of when the static objects are deleted. This defect was caused by the static
        // lock created below being deleted midway through DebugUtil::getInstance() when it was
        // passed to ThreadGuard constructor. By the time ThreadGuard called aquire() on the lock, 
        // it had been deleted and hence the pure virtual acquire() in IThreadLockable was being
        // called, aborting the program. A solution around this was to create the lock as a pointer,
        // as static simple datatype members (pointers, ints, chars) are not destructed, hence this 
        // problem will be overcome. A side affect is a memory leak when the singleton is destroyed, 
        // but this is small compared to the application crashing.
        static TA_Base_Core::ReEntrantThreadLockable* lock = new TA_Base_Core::ReEntrantThreadLockable;
        return *lock;
    }


	bool DebugUtil::isLoggingDisabledForLevel(EDebugLevel level)
	{
		if (!m_disabledLevels.empty())
		{
			return m_disabledLevels[static_cast<int>(level)];
		}

		// Value not found
		return false;
	}


	void DebugUtil::parseDebugLevelCmdLine(const std::string& valueString)
	{
		// Check the input string is not empty or NULL
		TA_DEBUG_ASSERT(!valueString.empty(), "Input string is empty");

		TA_Base_Core::ThreadGuard guard (getLock());

		// Input string should be of the form 1,2,3,5...etc or
		// FATAL,ERROR,WARN,DEBUG..etc. or a combination of.

		std::string::size_type pos = 0;
		std::string::size_type startPos = 0;
		std::string result = "";

		while (pos != std::string::npos)
		{
			// Find the first comma
			pos = valueString.find(',', startPos);

			if (pos != std::string::npos)
			{
				// Still more to come
				result = valueString.substr(startPos, (pos - startPos));
			}
			else
			{
				// End of string
				result = valueString.substr(startPos, valueString.length());				
			}

			// Check the first character in the string and handle it
			// depending on if it is a alphabetic character or a digit.
			// The getDebugLevelFromString function handles if the
			// input is valid or not, so we don't need to check here.
			if(isdigit(result[0]))
			{
				// Add disabled debug levels to vector
				m_disabledLevels[atoi(result.c_str())] = true;
			}
			else if(isalpha(result[0]))
			{
				// Add disabled debug level to vector
				m_disabledLevels[static_cast<int>(getDebugLevelFromString(result.c_str()))] = true;
			}
			else
			{
				// Should never get here
				TA_DEBUG_ASSERT(true, "Disabled debug level cmd line string is not formatted correctly");
			}

			// Clear result string
			result.erase();

			// Start next search after current comma position
			startPos = pos + 1;
		}
	}


	void DebugUtil::parseDebugFileCmdLine(const std::string& valueString)
	{
		// Check the input string is not empty or NULL
		TA_DEBUG_ASSERT(!valueString.empty(), "Input string is empty");

		TA_Base_Core::ThreadGuard guard (getLock());

		// Input string should be of the form 1,filename,2,filename...etc or
		// FATAL,filename,ERROR,filename..etc. or a combination of.  They
		// must be in order i.e. the first debug level will be matched with the
		// first filename and so on.


		std::string::size_type pos = 0;
		std::string::size_type startPos = 0;
		std::string result = "";
		std::string levelString = "";
		std::string filename = "";
		bool isDebugLevel = false;
		int decodedLevel = 0;

		while (pos != std::string::npos)
		{
			// Find the first comma
			pos = valueString.find(',', startPos);

			if (pos != std::string::npos)
			{
				// Still more to come
				result = valueString.substr(startPos, (pos - startPos));
			}
			else
			{
				// End of string
				result = valueString.substr(startPos, valueString.length());				
			}

			// Check the first character in the string and handle it
			// depending on if it is a alphabetic character or a digit.
			// The getDebugLevelFromString function handles if the
			// input is valid or not, so we don't need to check here.
			if(isdigit(result[0]))
			{
				// It can only be a debug level, filenames should not
				// start with a digit.
				decodedLevel = static_cast<int>(getDebugLevelFromString(
					getStringDebugLevel(
					static_cast<EDebugLevel>(atoi(result.c_str())))));

				// Set flag to true
				isDebugLevel = true;
			}
			// Can't assume the first character is alphabetical as
			// UNIX paths may start with a forward-slash '/'
			else
			{
				// Check if the token is a debug level or filename
				if (verifyStringIsDebugLevel(result))
				{
					// Result contains a debug level in text format
					decodedLevel = static_cast<int>(getDebugLevelFromString(result.c_str()));

					// Set flag to true
					isDebugLevel = true;
				}
				else
				{
					// Result contains a filename
					filename = result;
				}
			}

			if (isDebugLevel)
			{
				// Add level to container
				m_fileLevels[decodedLevel] = true;
			}
			else
			{
				for (unsigned int i = 0; i < m_levelFilenames.size(); ++i)
				{
					// if debug level flag set and filename is empty
					if (m_fileLevels[i] == true && m_levelFilenames[i] == "")
					{
						m_levelFilenames[i] = filename;

						m_filenameNumber[i] = 0;

						break;
					}
				}
			}
		
			// Reset local variables for next loop
			result.erase();
			levelString.erase();
			filename.erase();
			isDebugLevel = false;
			decodedLevel = 0;

			// Start next search after current comma position
			startPos = pos + 1;
		}

		// Check that all the levels have a corresponding filename
		for (unsigned int i = 0; i < m_levelFilenames.size(); ++i)
		{
			if (m_fileLevels[i] == true && m_levelFilenames[i] == "")
			{
				// If filename is blank reset all data for this level
				m_fileLevels[i] = false;
				m_levelFilenames[i] = "";
				m_filenameNumber[i] = -1;
			}
		}
	}


	bool DebugUtil::verifyStringIsDebugLevel(std::string& stringToVerify)
    {
		std::string compareString = "";
		int level = 0;

		// Use the fact that UNKNOWN will always be returned last
		// as the end of the search for a match
		while (0 != compareString.compare("UNKNOWN"))
		{
			// An exception will be thrown if a valid debug level is not
			// found, this is expected.
			try
			{
				// Get the string to represent the debug level
				compareString = getStringDebugLevel(static_cast<EDebugLevel>(level));
			}
			catch (...)
			{
				// Not a valid debug level so return
				return false;
			}

			// Compare the passed in string with the level string
			if (0 == stringToVerify.compare(compareString))
			{
				return true;
			}

			++level;
		}

		// Shouldn't get here
		return false;
	}


	int DebugUtil::countDebugLevels()
    {
		TA_Base_Core::ThreadGuard guard (getLock());

		std::string compareString = "";
		int level = 0;

		// Note: the last item which name is NULL
		while( nameValueMapping[level].name != NULL )
		{
			++level;
		}

		return level;


// 		// Use the fact that UNKNOWN will always be returned last
// 		// as the end of the search for a match
// 		while (0 != compareString.compare("UNKNOWN"))
// 		{
// 			// An exception will be thrown if a valid debug level is not
// 			// found, this is expected.
// 			try
// 			{
// 				// Get the string to represent the debug level
// 				compareString = getStringDebugLevel(static_cast<EDebugLevel>(level));
// 			}
// 			catch (...)
// 			{
// 				// Return debug level
// 				return (level - 1);
// 			}
// 
// 			++level;
// 		}
// 
// 		// Return debug level
// 		return (level - 1);
	}


	bool DebugUtil::preLogChecks(EDebugLevel level)
	{
		TA_Base_Core::ThreadGuard guard (getLock());

		if (isLoggingDisabledForLevel(level))
		{
			// Don't need to do anything so return true
			return true;
		}

		// OK to log message
		bool foundFile = false;
		int intLevel = static_cast<int>(level);

		// Check to see if a file has been specified for this debug level
		if (m_fileLevels[intLevel] == true)
		{
			foundFile = true;
			
			// Check to see if the filename needs changing
			if (m_levelFilenames[intLevel] != m_filename)
			{
				// Filename needs changing					
				transferToSpecifiedFile(m_levelFilenames[intLevel], intLevel);
				return false;
			}
		}

		if (foundFile == false)
		{
			// Check to see if the current filename
			// is not the same as the default
			if (m_filename != m_defaultFilename)
			{
				transferToDefaultFile();
				return false;
			}
		}

		return false;
	}


	void DebugUtil::transferToSpecifiedFile(std::string& filename, int index)
	{
		TA_DEBUG_ASSERT(!filename.empty(), "The debug level filename is empty");

        // Close file if currently open
        if (m_outFile.is_open())
        {
            m_outFile.flush();
            m_outFile.close();
        }

		// Check to see if the filename already exists
		// File number will be 0 if a file does not exist
		if (m_filenameNumber[index] == 0)
		{
			// Create and use a new file
			++(m_filenameNumber[index]);
			m_filename = filename;

			std::string fileOpen = "";
			fileOpen = createFilename(m_filenameNumber[index]);

			// Check the size of the file
			m_sizeInBytes = getFileSize(fileOpen);

			if (m_sizeInBytes == -1)
			{
				// Couldn't acertain size of file so message logged to cerr
				return;
			}
            
            // BCF 16-Nov-2001 PR1028:
            // Open the file for appending (retain existing entries)

            m_outFile.open(fileOpen.c_str(),std::ios::out|std::ios::app);
            if (m_outFile.fail())
            {
                std::cerr << "DebugUtil::SetFile(): error opening file! - The reason is: "
                          << strerror( errno )
                          << std::endl;
                m_outFile.clear();
            }
		}
		else
		{
			// File name already exists

			// Close the old open file
			if (m_outFile.is_open())
			{
				m_outFile.flush();
				m_outFile.close();
			}

			m_filename = filename;

			// Recreate the filename
			std::string fileOpen = "";
			fileOpen = createFilename(m_filenameNumber[index]);

			// Check the size of the file
			m_sizeInBytes = getFileSize(fileOpen);

			if (m_sizeInBytes == -1)
			{
				// Couldn't acertain size of file so message logged to cerr
				return;
			}
            
            // BCF 16-Nov-2001 PR1028:
            // Open the file for appending (retain existing entries)

            m_outFile.open(fileOpen.c_str(),std::ios::out|std::ios::app);
            if (m_outFile.fail())
            {
                std::cerr << "DebugUtil::SetFile(): error opening file! - The reason is: "
                          << strerror( errno )
                          << std::endl;
                m_outFile.clear();
            }
		}

	}

	void DebugUtil::transferToDefaultFile()
	{
        // Close file if currently open
        if (m_outFile.is_open())
        {
            m_outFile.flush();
            m_outFile.close();
        }

		// Create and use a new file
		m_filename = m_defaultFilename;
		
		std::string fileOpen = "";
		fileOpen = createFilename(m_fileCount);
		
		// Check the size of the file
		m_sizeInBytes = getFileSize(fileOpen);

		if (m_sizeInBytes == -1)
		{
			// Couldn't acertain size of file so message logged to cerr
			return;
		}
		
		// BCF 16-Nov-2001 PR1028:
		// Open the file for appending (retain existing entries)
		
		m_outFile.open(fileOpen.c_str(),std::ios::out|std::ios::app);
		if (m_outFile.fail())
		{
			std::cerr << "DebugUtil::SetFile(): error opening file! - The reason is: "
				<< strerror( errno )
				<< std::endl;
			m_outFile.clear();
		}
	}


	int DebugUtil::getFileSize(std::string& filename)
	{
		TA_DEBUG_ASSERT(!filename.empty(), "Filename is empty, cannot acertain size of file");

		// Check the size of the file
		std::ifstream inFile;
		inFile.open(filename.c_str());

		int fileSize = 0;

		if (!inFile.fail())
		{
			// Try to determine the size of the file
			
			#if defined (SOLARIS) || defined (LINUX)
				struct stat buf;
			#else
				struct _stat buf;
			#endif
				int result;
			
			#if defined (SOLARIS) || defined (LINUX)
				result = stat ( filename.c_str(), &buf );
			#else
				result = _stat ( filename.c_str(), &buf );
			#endif
			
			if( result != 0 )
			{
				// If we can't get the size of the file, don't want to risk 
				// writing to the file in case it exceeds the max size.
				// So write to stdout instead.
				
				std::cerr << "Cannot append to file: " << filename.c_str() 
					<< "Reason: Cannot determine size of log file." 
					<< "Log messages will be written to stdout" << std::endl;
				m_filename = "";
				return -1;
			}
			else
			{
				fileSize = buf.st_size;
			}

			inFile.clear();
			inFile.close();
		}

        return fileSize;
	}


    void DebugUtil::logLargeString(const std::string& file, int line, TA_Base_Core::DebugUtil::EDebugLevel level, const std::string& str)
    {
        // The continuation lines need to be output without being interleaved with 
        // calls to LOG from other threads.
		TA_Base_Core::ThreadGuard guard (getLock());

        unsigned int pos = 0;

        // The first line is output with the filename, linenumber prefix.
        // Unfortunately this always outputs a newline character.
        // We use "%s" because logMessage may contain % characters, depending on the omni logging level,
        // and this will cause FormatV() to fail on Windows.
        LOG(file, line, TA_Base_Core::DebugUtil::GenericLog, level, "%s", str.substr(pos, MAXLOGMESGSIZE).c_str());
        pos += MAXLOGMESGSIZE;

        // Subsequent lines are output without the prefix.
        while (pos < str.size())
        {
            TA_Base_Core::DebugUtil::getInstance().logMore(file, line, "%s", str.substr(pos, MAXLOGMESGSIZE).c_str());
            pos += MAXLOGMESGSIZE;
        }
    }

}




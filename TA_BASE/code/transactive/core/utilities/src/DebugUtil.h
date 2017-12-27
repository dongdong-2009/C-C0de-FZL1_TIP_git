#ifndef DEBUGUTIL_H_INCLUDED
#define DEBUGUTIL_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/DebugUtil.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if defined( WIN32 )
    #pragma warning( disable : 4786 )
#endif   // defined( WIN32 )

#include "FunctionEntry.h"
#include "TAAssert.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/TransactiveException.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <stdarg.h>

// From windows.h
typedef struct _CONTEXT CONTEXT;

namespace TA_Base_Core
{
    class Logger;

    class DebugUtil
    {
    public:

        // Debug levels are sequentially indexed starting at 0.  It is possible to iterate
        // through all debug levels by iterating through the integral sequence [ 0, countDebugLevels() ).
        // The current debug level determines the highest debugging level that will be logged.
        enum EDebugLevel
        {
            DebugFatal     = 0L,   // fatal error conditions
            DebugError,            // error conditions
            DebugWarn,             // warning conditions
            DebugNorm,             // normal conditions
            DebugInfo,             // informational messages
            DebugDebug,            // debug-level messages
            DebugSQL,              // SQL query strings
            DebugCORBA,            // for CORBA debug messages (only if other flags are enabled - see CorbaUtil)
            DebugMsg,              // for Messaging library debug messages
            DebugMsgPub,           // for Messaging library message sending and receiving debug messages
            DebugPeriodic,         // messages logged periodically by keepalives, etc
            DebugDatabase,         // data access (SimpleDB) messages (other than SQL query strings)
            DebugExternal,         // 3rd party (non-transactive) messages
            DebugTrace,            // Function Entry/Exit Messages
        };

        // If any new debug types are to be created, insert it
        // into this enum structure and add the level and format
        // string to the DebugTypeInfo array in the top of
        // the .cpp file. Also create a new macro for the debug
        // type, and a new function that is called by the macro.
        enum EDebugType
        {
            GenericLog,
            FunctionEntry,
            FunctionExit,
            ExceptionConstruction,
            ExceptionDetails,
            ExceptionCatch
        };

        /**
          * getInstance
          *
          * Returns a single static instance of DebugUtil
          *
          * @return The one and only DebugUtil instance
          */
        static DebugUtil& getInstance();

        static void removeInstance();

        /**
          * ~DebugUtil
          *
          * Destructor
          */
        virtual ~DebugUtil();

        /**
          * log
          *
          * Outputs a log message with the time, date, thread ID, debug level
          * and location details. Any strings passed into ... must be passed
          * in as a const char* not a std::string. This is because they can
          * only be read from ... as a char*.
          *
          * @param file Name of file
          * @param line Line number in file
          * @param type Debug type
          * @param ...  Variable arguments
          *
          * Pre: file is not an empty string
          *      line is greater than 0 or is -1 if unknown
          */
        void log(const std::string& file, int line, EDebugType type, ...);

        /**
          * logGeneric
          *
          * As above, but outputs GenericLogs ONLY. Therefore it does not
          * take a type parameter.
          *
          * @param file Name of file
          * @param line Line number in file
          * @param ...  Variable arguments
          *
          * Pre: file is not an empty string
          *      line is greater than 0 or is -1 if unknown
          */
        void logGeneric(const std::string& file, int line, ...);

        // HACK: DebugUtil's logging functions are only able to output strings shorter than
        // MAXLOGMESGSIZE for some reason.  This is a terrible limitation.  We will split
        // longer strings into a number of pieces for logging.  YUCK!
        // Also, we write the string WITHOUT formatting it (it may contain % characters that are
        // not intended to be format specifiers).
        void logLargeString(const std::string& file, int line, EDebugLevel level, const std::string& str);

        /**
          * logMore
          *
          * Outputs an indented log message without the time, date, thread ID,
          * debug level and location details. Any strings passed into ... must
          * be passed in as a const char* not a std::string. This is because
          * they can only be read from ... as a char*.
          *
          * The level of this message will be the same as the last log level.
          *
          * @param file Name of file
          * @param line Line number in file
          *
          * Pre: file is not an empty string
          *      line is greater than 0
          */
        void logMore(const std::string& file, int line, ...);

        // If RPARAM_DEBUGLOGSTACKTRACE is set, attempts to write the stack trace
        // of the execution point described by executionContext to the log file,
        // at log level DebugInfo.  The CONTEXT structure can be obtained in a
        // catch() handler for a structured exception (eg. WinStructuredException).
        // If the CONTEXT is NULL, the current excecution point will be traced.
        // NOT IMPLEMENTED on Solaris.
        void logStackTrace(CONTEXT* executionContext = NULL);

        /**
          * setFile
          *
          * Sets the log file to the file name specified.
          *
          * @param name Name of log file
          *
          * Pre: file is not an empty string
          */
        void setFile(const char* name, int maxSize = 0);
        void setFile(const std::string& name, int maxSize = 0) { setFile(name.c_str(), maxSize); }

        /**
          * getStringDebugLevel
          *
          * Convert enum representation of debug level to string.
          *
          * @param level Enum representation of debug level
          */
        static const char* getStringDebugLevel(EDebugLevel level);

        /**
          * getDebugLevelFromString
          *
          * Convert string to enum representation of debug level.
          *
          * @param level Enum representation of debug level
          */
        static EDebugLevel getDebugLevelFromString(const char* levelStr);
        static EDebugLevel getDebugLevelFromString(const std::string& levelStr) { return getDebugLevelFromString(levelStr.c_str()); }

        /**
          * getLevel
          *
          * Returns the current debug level
          *
          * @return Current debug level
          */
        EDebugLevel getLevel() const
        {
            return m_currDebugLevel;
        }

        /**
          * setLevel
          *
          * Sets the current debug level
          *
          * @param level New debug level
          */
        /*
        void setLevel(EDebugLevel level)
        {
            m_currDebugLevel = level;
        }
        */
        void setLevel(EDebugLevel level)
        {
            m_currDebugLevel = level;
            int MAX_LOG_FILE_SIZE = 10000000;

            if (level > DebugInfo && getMaxSize() < MAX_LOG_FILE_SIZE)
            {
                setMaxSize(MAX_LOG_FILE_SIZE);// extend to 10M size per log file
            }
        }

        /**
          * getMaxSize
          *
          * Returns the maximum size of the file in bytes
          *
          * @return Maximum size of file
          */
        int getMaxSize() const
        {
            return m_maxSizeInBytes;
        }

        /**
          * setMaxSize
          *
          * Sets the maximum size of the file
          *
          * @param maxSizeInBytes Maximum size of file in bytes
          */
        void setMaxSize(int maxSizeInBytes)
        {
            m_maxSizeInBytes = maxSizeInBytes;
        }

        /**
          * setMaxFiles
          *
          * Sets the maximum number of log files
          *
          * @param maxFiles Maximum number of log files
          */
        void setMaxFiles(int maxFiles)
        {
            m_maxFiles = maxFiles;
        }

        /**
          * setMaxCacheSize
          *
          * Sets the maximum number of messages allowed in the
          * cache.
          *
          * @param maxCache Maximum cache size.
          */
        void setMaxCacheSize(int maxCacheSize);

        /**
          * encryptPidInFilenames
          *
          * Adds the process Id (PID) to filenames so that on
          * restart of an application the old files are not overwrote.
          *
          * @param usePidFlag input string.
          */
        void encryptPidInFilenames(const std::string& usePidFlag);

        /**
          * decodeDisabledLevels
          *
          * Decodes the command line string into debug levels
          * for disabling.
          *
          * @param disabledString Input string.
          */
        void decodeDisabledLevels(const std::string& disabledString);

        /**
          * decodeLevelFilenames
          *
          * Decodes the command line string into a level and a
          * corresponding file to log too.
          *
          * @param disabledString Input string.
          */
        void decodeLevelFilenames(const std::string& levelFilenames);

        /**
          * countDebugLevels
          *
          * Used to dynamically count the number of debug levels.
          * Used by the SystemManager.
          *
          * @return int The number of debug levels.
          */
        int countDebugLevels();

    private:

        /**
          * DebugUtil
          *
          * Constructor. This is a singleton so getInstance() calls
          * the constructor.
          */
        DebugUtil();

        /**
          * swapFiles
          *
          * Changes the output log file to a file of the same name
          * with a incremented count appended to the end of the
          * file name.
          */
        void swapFiles();

        /**
          * loadDebugTypes
          *
          * Loads the preformatted debug types
          */
        void loadDebugTypes();

        /**
          * logMessage
          *
          * Outputs debug message to appropriate destination.
          *
          * @param file       Name of file
          * @param line       Line number in file
          * @param type       Debug type
          * @param args       Pointer to list of variable arguments
          * @param simpleLog  Flag to indicate simple/full log
          * @param printCache Flag to indicate whether to print contents of cache
          *
          * Pre: file is not an empty string
          *      line is greater than 0
          */
        void logMessage(const std::string file, int line, EDebugType type, va_list args, bool simpleLog, bool printCache);

        /**
          * handleException
          *
          * Handles a caught exception
          *
          * @param exceptionName Name of the exception caught
          * @param exception     The exception itself
          * @param args          Pointer to list of variable arguments
          */
        void handleException(const std::string& exceptionName, const TA_Base_Core::TransactiveException& exception, va_list& args) const;

        /**
          * createFilename
          *
          * Constructs a string containing the filename to log to. This is
          * simply the filename passed in to setFile(), but with a number
          * before the extension (eg "logfile.txt" becomes "logfile.3.txt").
          *
          * @param fileNum  The number to place in the filename before the
          *                 extension (3 in the above example).
          *
          * @return A string containing the filename to log to.
          *
          */
        std::string createFilename(int fileNum) const;

        /**
          * isLoggingDisabledForLevel
          *
          * Checks to see if logging is disabled for this debug level.
          *
          * @param level The debug level to against.
          */
        bool isLoggingDisabledForLevel(EDebugLevel level);

        /**
          * parseDebugLevelCmdLine
          *
          * Parses and tokenises the input string into EDebugLevels,
          * and stores them in the member vector.
          *
          * @param valueString Input string.
          *
          * Pre: valueString is of the form:
          *      1,2,4,5...etc or FATAL,ERROR,DEBUG,DATABASE...etc
          *      or a combination of 1,2,DEBUG,5...etc
          */
        void parseDebugLevelCmdLine(const std::string& valueString);

        /**
          * parseDebugFileCmdLine
          *
          * Parses and tokenises the input string into levels and
          * corresponding filenames.
          *
          * @param valueString Input string.
          *
          * Pre: valueString is of the form:
          *      1,filename,2,filename...etc or FATAL,filename,ERROR,filename...etc
          *      or a combination of 1,filename,DEBUG,filename...etc and
          *      they must be in order i.e. the first debug level will be
          *      matched with the first filename and so on.
          */
        void parseDebugFileCmdLine(const std::string& valueString);

        /**
          * verifyStringIsDebugLevel
          *
          * This checks that a given string is correspondent to a
          * debug level.  It is used to filter out passed in filenames
          * from the command line.
          *
          * @ param stringToVerify The string to be checked
          * @ return bool Flag to indicate if the input string
          *     corresponds to a debug level, true if it does
          */
        bool verifyStringIsDebugLevel(std::string& stringToVerify);

        /**
          * preLogChecks
          *
          * This checks if the debug level is disabled from logging
          * by calling isLoggingDisabledForLevel.  It also does
          * everything to ensure the correct file is used if a debug
          * level specific file has been specified.
          *
          * @param level The debug level to against.
          * @return bool Flag to indicate if the logging has been
          *     disabled for the particular level.  True if disabled.
          */
        bool preLogChecks(EDebugLevel level);

        /**
          * transferToSpecifiedFile
          *
          * This changes the output file to be the file specified
          * for this particular debug level.
          *
          * @param filename The output file.
          */
        void transferToSpecifiedFile(std::string& filename, int index);

        /**
          * transferToDefaultFile
          *
          * This changes the output file to be the default file.
          *
          */
        void transferToDefaultFile();

        /**
         * getFileSize
         *
         * This tries to acertiain the size of a file.
         *
         * @param filename The filename of the file.
         */
        int getFileSize(std::string& filename);

        // Fix for TD2689: m_lock was not being instantiated when DebugUtil was instantiated
        // from a static object. An accessor method is now used so that construction of the
        // lock is guaranteed.
        static TA_Base_Core::ReEntrantThreadLockable& getLock();

        static DebugUtil* m_instance;

        EDebugLevel m_currDebugLevel;
        EDebugLevel m_lastMsgLevel;

        std::string m_filename;
        std::string m_defaultFilename;

        std::fstream m_outFile;

        unsigned int m_maxSizeInBytes;
        unsigned int m_sizeInBytes;

        int m_maxFiles;
        int m_fileCount;

        bool m_firstSetFile;
        bool m_encodePidInFilenames;

        Logger* m_logger;

        // Vector to hold list of debug levels as EDebugLevel's whose
        // logging is to be disabled.
        std::vector<bool> m_disabledLevels;

        // Vector to hold the debug levels with specific filenames.
        std::vector<bool> m_fileLevels;

        // Vector to hold the debug level specific filenames.
        std::vector<std::string> m_levelFilenames;

        // Vector to hold the filename number
        std::vector<int> m_filenameNumber;
    };
}   // namespace TA_Base_Core

#include "DebugUtilMacros.h"

#ifndef NDEBUG

/**
  * TA_DEBUG_ASSERT
  *
  * This macro is the same as the TA_ASSERT macro (defined
  * in TAAssert.h but without the logging). We do not want
  * to log because TA_ASSERT uses LOG for logging. This
  * SHOULD NOT be used by anything else outside of
  * DebugUtil.
  *
  * Assert's are executed only in debug builds, not
  * release builds.
  *
  * DebugUtil will never abort if a TA_DEBUG_ABORT fails as
  * we don't want an application using DebugUtil to abort due
  * to the logging. Instead any exceptions will be caught
  * within DebugUtil itself and an error message written to
  * stderr.
  *
  * @param test   An expression to be tested.
  * @param info   A string outlining the problem if test is false.
  */
#define TA_DEBUG_ASSERT(test,info)                                              \
    {                                                                           \
        if (!(test))                                                            \
        {                                                                       \
            TA_DOBREAKACTION;                                                   \
            TA_DOASSERTIONACTION(__FILE__, __LINE__, test, info);               \
        }                                                                       \
    }

#else
#define TA_DEBUG_ASSERT
#endif // NDEBUG

#endif // DEBUGUTIL_H_INCLUDED

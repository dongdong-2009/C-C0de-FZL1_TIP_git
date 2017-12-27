#include <string>
#ifndef _LOG4CXX_LOGGER_H
#define _LOG4CXX_LOGGER_H

#if defined(_MSC_VER)
#pragma warning ( push )
#pragma warning ( disable: 4231 4251 4275 4786 )
#endif

namespace log4cxx
{
    class Logger
    {
    public:
		~Logger(){};
		Logger(){};
		static Logger * getLogger(std::string strLogName)
		{
			return NULL;
		}
	private:
		Logger * m_pInstance;
	};

	typedef Logger* LoggerPtr;
}


/**
Logs a message to a specified logger with a specified level.

@param logger the logger to be used.
@param level the level to log.
@param message the message string to log.
*/
#define LOG4CXX_LOG(logger, level, message) { \
        /*if (logger->isEnabledFor(level)) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(level, oss_.str(oss_ << message), LOG4CXX_LOCATION); }*/ }

/**
Logs a message to a specified logger with a specified level.

@param logger the logger to be used.
@param level the level to log.
@param message the message string to log in the internal encoding.
*/
#define LOG4CXX_LOGLS(logger, level, message) { \
        /*if (logger->isEnabledFor(level)) {\
           ::log4cxx::helpers::LogCharMessageBuffer oss_; \
           logger->forcedLog(level, oss_.str(oss_ << message), LOG4CXX_LOCATION); } }*/

/**
Logs a message to a specified logger with the DEBUG level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_DEBUG(logger, message) { \
       /* if (LOG4CXX_UNLIKELY(logger->isDebugEnabled())) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getDebug(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }*/}

/**
Logs a message to a specified logger with the TRACE level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_TRACE(logger, message) { \
       /* if (LOG4CXX_UNLIKELY(logger->isTraceEnabled())) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getTrace(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }*/}


/**
Logs a message to a specified logger with the INFO level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_INFO(logger, message) { \
        /*if (logger->isInfoEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getInfo(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }*/}

/**
Logs a message to a specified logger with the WARN level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_WARN(logger, message) { \
        /*if (logger->isWarnEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getWarn(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }*/}

/**
Logs a message to a specified logger with the ERROR level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_ERROR(logger, message) { \
        /*if (logger->isErrorEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getError(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }*/}

/**
Logs a error if the condition is not true.

@param logger the logger to be used.
@param condition condition
@param message the message string to log.
*/
#define LOG4CXX_ASSERT(logger, condition, message) { \
        /*if (!(condition) && logger->isErrorEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getError(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }*/}


/**
Logs a message to a specified logger with the FATAL level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define LOG4CXX_FATAL(logger, message) { \
        /*if (logger->isFatalEnabled()) {\
           ::log4cxx::helpers::MessageBuffer oss_; \
           logger->forcedLog(::log4cxx::Level::getFatal(), oss_.str(oss_ << message), LOG4CXX_LOCATION); }*/}

/**
Logs a localized message with no parameter.

@param logger the logger to be used.
@param level the level to log.
@param key the key to be searched in the resourceBundle of the logger.
*/
#define LOG4CXX_L7DLOG(logger, level, key) { \
        /*if (logger->isEnabledFor(level)) {\
        logger->l7dlog(level, key, LOG4CXX_LOCATION); }*/}

/**
Logs a localized message with one parameter.

@param logger the logger to be used.
@param level the level to log.
@param key the key to be searched in the resourceBundle of the logger.
@param p1 the unique parameter.
*/
#define LOG4CXX_L7DLOG1(logger, level, key, p1) { \
        /*if (logger->isEnabledFor(level)) {\
        logger->l7dlog(level, key, LOG4CXX_LOCATION, p1); }*/}

/**
Logs a localized message with two parameters.

@param logger the logger to be used.
@param level the level to log.
@param key the key to be searched in the resourceBundle of the logger.
@param p1 the first parameter.
@param p2 the second parameter.
*/
#define LOG4CXX_L7DLOG2(logger, level, key, p1, p2) { \
        /*if (logger->isEnabledFor(level)) {\
        logger->l7dlog(level, key, LOG4CXX_LOCATION, p1, p2); }*/}

/**
Logs a localized message with three parameters.

@param logger the logger to be used.
@param level the level to log.
@param key the key to be searched in the resourceBundle of the logger.
@param p1 the first parameter.
@param p2 the second parameter.
@param p3 the third parameter.
*/
#define LOG4CXX_L7DLOG3(logger, level, key, p1, p2, p3) { \
        /*if (logger->isEnabledFor(level)) {\
        logger->l7dlog(level, key, LOG4CXX_LOCATION, p1, p2, p3); }*/}

/**@}*/

#if defined(_MSC_VER)
#pragma warning ( pop )
#endif

#endif //_LOG4CXX_LOGGER_H

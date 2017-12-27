#ifndef SECURTY_CORBA_MACROS_H
#define SECURTY_CORBA_MACROS_H

#ifdef WIN32
// disable "warning C4290: C++ Exception Specification ignored"
#pragma warning(push)
#pragma warning(disable : 4290)
#endif // WIN32

#include "core/corba/src/CorbaUtil.h"

using namespace TA_Base_Core;

#define CORBA_CALL_RETURN_NO_TIMEOUT( result, namedObject, corbaInterface, parameters )\
    try\
    {\
		result = namedObject->corbaInterface parameters;\
    }\
    catch ( TA_Base_Core::OperationModeException& )\
    {\
		namedObject.clear();\
		result = namedObject->corbaInterface parameters;\
    }\
    catch ( CORBA::SystemException& ____ex )\
    {\
		std::string exStr = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ____ex );\
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", exStr.c_str() );\
		size_t found;\
		found=exStr.find("TRANSIENT_CallTimedout");\
		if(found == std::string::npos)\
		{\
			namedObject.clear();\
			result = namedObject->corbaInterface parameters;\
		}\
    }

#define CORBA_CALL_NO_TIMEOUT( namedObject, corbaInterface, parameters )\
    try\
    {\
		namedObject->corbaInterface parameters;\
    }\
    catch ( TA_Base_Core::OperationModeException& )\
    {\
		namedObject.clear();\
		namedObject->corbaInterface parameters;\
    }\
    catch ( CORBA::SystemException& ____ex )\
    {\
		std::string exStr = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ____ex );\
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", exStr.c_str() );\
		size_t found;\
		found=exStr.find("TRANSIENT_CallTimedout");\
		if(found == std::string::npos){\
			namedObject.clear();\
			namedObject->corbaInterface parameters;\
		}\
    }


#define CORBA_CALL_RETURN_TIMEOUT_NO_RETRY( result, namedObject, corbaInterface, parameters )\
    try\
	{\
		result = namedObject->corbaInterface parameters;\
	}\
    catch ( TA_Base_Core::OperationModeException& )\
	{\
		namedObject.clear();\
		result = namedObject->corbaInterface parameters;\
	}\
    catch ( CORBA::SystemException& ____ex )\
	{\
		std::string exStr = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ____ex );\
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", exStr.c_str() );\
		size_t found;\
		found=exStr.find("TRANSIENT_CallTimedout");\
		if(found == std::string::npos)\
		{\
			namedObject.clear();\
			result = namedObject->corbaInterface parameters;\
		}\
		else{\
		  throw;\
		}\
	}

#define CORBA_CALL_TIMEOUT_NO_RETRY( namedObject, corbaInterface, parameters )\
    try\
	{\
		namedObject->corbaInterface parameters;\
	}\
    catch ( TA_Base_Core::OperationModeException& )\
	{\
		namedObject.clear();\
		namedObject->corbaInterface parameters;\
	}\
    catch ( CORBA::SystemException& ____ex )\
	{\
		std::string exStr = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ____ex );\
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", exStr.c_str() );\
		size_t found;\
		found=exStr.find("TRANSIENT_CallTimedout");\
		if(found == std::string::npos){\
			namedObject.clear();\
			namedObject->corbaInterface parameters;\
		}\
		else{\
			throw;\
		}\
	}

#ifdef WIN32
#pragma warning(pop)
#endif // WIN32

#endif // NAMED_OBJECT_H

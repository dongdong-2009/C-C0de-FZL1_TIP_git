/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/ECSAgentExceptions.h $
  * @author J. Ebedes
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * CCTV classes. It is derived from TransActiveException.
  */


#if !defined(AFX_CCTVEXCEPTION_H__C4FD1942_47A0_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_CCTVEXCEPTION_H__C4FD1942_47A0_11D7_B150_0080C8E32E69__INCLUDED_


#include <string>																
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
    

    class InvalidECSConfigurationException : public TransactiveException
    {

    public:
	    
        InvalidECSConfigurationException() throw()
        : TransactiveException("Unspecified inconsistency detected in the ECS configuration")
        { }
	    
        InvalidECSConfigurationException(const char* msg) throw()
        : TransactiveException(msg)
        { }
	    
        virtual ~InvalidECSConfigurationException() throw()
        { }
    };



    class CachedMappingNotFoundException : public InvalidECSConfigurationException
    {

    public:
	    
        CachedMappingNotFoundException() throw() 
        : InvalidECSConfigurationException("Unspecified cached mapping not found")
        { }
	    
        CachedMappingNotFoundException(const char* msg) throw() 
        : InvalidECSConfigurationException(msg)
        { }
	    
        virtual ~CachedMappingNotFoundException() throw()
        { }
    };


    class InvalidECSDataPointValueException : public TransactiveException
    {

    public:
	    
        InvalidECSDataPointValueException() throw() : TransactiveException("Invalid ECS DataPoint value")
        { }
	    
        virtual ~InvalidECSDataPointValueException() throw()
        { }
    };


    class InvalidATSPacketException : public TransactiveException
    {

    public:
	    
        InvalidATSPacketException() throw() : TransactiveException("Invalid ATS packet data")
        { }

	    InvalidATSPacketException(const char* msg) throw() : TransactiveException(msg)
        { }

        virtual ~InvalidATSPacketException() throw()
        { }
    };

    class TrainIdNotFoundException : public TransactiveException
    {

    public:
	    
        TrainIdNotFoundException() throw() : TransactiveException("Train ID not found")
        { }

	    TrainIdNotFoundException(const char* msg) throw() : TransactiveException(msg)
        { }

        virtual ~TrainIdNotFoundException() throw()
        { }
    };

    //class InvalidDataPointWriteRequest : public TransactiveException
    //{
//
//    public:
//	    
//        InvalidDataPointWriteRequest() throw() : TransactiveException("Invalid DataPoint write request")
//        { }
//
//	    InvalidDataPointWriteRequest(const char* msg) throw() : TransactiveException(msg)
//        { }
//	    
//        virtual ~InvalidDataPointWriteRequest() throw()
//        { }
//    };

    
    class EcsMonitorModeException : public TransactiveException
    {

    public:
	    
        EcsMonitorModeException() throw()
        : TransactiveException("Operation attempted in monitor mode")
        { }
	    
        EcsMonitorModeException(const char* msg) throw()
        : TransactiveException(msg)
        { }
	    
        virtual ~EcsMonitorModeException() throw()
        { }
    };

}

#endif // !defined(AFX_CCTVEXCEPTION_H__C4FD1942_47A0_11D7_B150_0080C8E32E69__INCLUDED_)


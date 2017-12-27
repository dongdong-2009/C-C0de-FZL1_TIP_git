/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/PasExceptions.h $
  * @author J. Ebedes
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: 
  * 
  * This file outlines a simple exception class for use with
  * CCTV classes. It is derived from TransActiveException.
  */


#if !defined(PASEXCEPTIONS_H_INCLUDED)
#define PASEXCEPTIONS_H_INCLUDED


#include <string>																
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
    enum PASConnectionExceptionReason
    {
        DISCONNECTED,
        UNABLE_TO_CONNECT
    };

    class InvalidPasConfigurationException : public TransactiveException
    {

    public:
	    
        InvalidPasConfigurationException() throw()
        : TransactiveException("Unspecified inconsistency detected in the PAS configuration")
        { }
	    
        InvalidPasConfigurationException(const char* msg) throw()
        : TransactiveException(msg)
        { }
	    
        virtual ~InvalidPasConfigurationException() throw()
        { }
    };



    class CachedMappingNotFoundException : public InvalidPasConfigurationException
    {

    public:
	    
        CachedMappingNotFoundException() throw() 
        : InvalidPasConfigurationException("Unspecified cached mapping not found")
        { }
	    
        CachedMappingNotFoundException(const char* msg) throw() 
        : InvalidPasConfigurationException(msg)
        { }
	    
        virtual ~CachedMappingNotFoundException() throw()
        { }
    };


    class InvalidPasDataPointValueException : public TransactiveException
    {

    public:
	    
        InvalidPasDataPointValueException() throw() : TransactiveException("Invalid PAS datapoint value")
        { }
	    
        virtual ~InvalidPasDataPointValueException() throw()
        { }
    };


    class InvalidAtsPacketException : public TransactiveException
    {

    public:
	    
        InvalidAtsPacketException() throw() : TransactiveException("Invalid ATS packet data")
        { }

	    InvalidAtsPacketException(const char* msg) throw() : TransactiveException(msg)
        { }

        virtual ~InvalidAtsPacketException() throw()
        { }
    };

    class NoFreeAnnounceIdException : public TransactiveException
    {

    public:
	    
        NoFreeAnnounceIdException() throw() : TransactiveException("No available Announce Id found for the requested broadcast")
        { }

	    NoFreeAnnounceIdException(const char* msg) throw() : TransactiveException(msg)
        { }

        virtual ~NoFreeAnnounceIdException() throw()
        { }
    };

    class NoFreeMessageSequenceIdException : public TransactiveException
    {

    public:
	    
        NoFreeMessageSequenceIdException() throw() : TransactiveException("No available Message Sequence Id found for the requested broadcast")
        { }

	    NoFreeMessageSequenceIdException(const char* msg) throw() : TransactiveException(msg)
        { }

        virtual ~NoFreeMessageSequenceIdException() throw()
        { }
    };


    class PasConnectionException : public TransactiveException
    {

    public:
	    
        PasConnectionException(PASConnectionExceptionReason reason) throw() : TransactiveException("Unspecified PAS connection failure"),
            m_reason(reason)
        { }

	    PasConnectionException(const char* msg, PASConnectionExceptionReason reason) throw() : TransactiveException(msg),
            m_reason(reason)
        { }

        PasConnectionException() throw() : TransactiveException("Unspecified PAS connection failure"),
            m_reason(DISCONNECTED)
        { }

        PasConnectionException(const char *msg) throw() : TransactiveException(msg),
            m_reason(DISCONNECTED)
        { }

        virtual ~PasConnectionException() throw()
        { }

        PASConnectionExceptionReason getReason() const { return m_reason; }

    private:
        PASConnectionExceptionReason m_reason; // DISCONNECTED or UNABLE_TO_CONNECT
    };

    class PasWriteErrorException : public TransactiveException
    {

    public:
	    
        PasWriteErrorException(int errorValue) throw()
            : TransactiveException("Pas write returned an error state"),
              m_errorValue(errorValue)
        { }

	    PasWriteErrorException(int errorValue, const char* msg) throw()
            : TransactiveException(msg),
              m_errorValue(errorValue)
        { }

        virtual ~PasWriteErrorException() throw()
        { }

        int getErrorValue()
        {   return m_errorValue;    }

        int m_errorValue;
    };

    class InvalidPasBroadcastException : public TransactiveException
    {

    public:
	    
        InvalidPasBroadcastException() throw() : TransactiveException("Unspecified Invalid Pas Broadcast failure")
        { }

	    InvalidPasBroadcastException(const char* msg) throw() : TransactiveException(msg)
        { }

        virtual ~InvalidPasBroadcastException() throw()
        { }
    };

    
    class InvalidPasPacketException : public TransactiveException
    {

    public:
	    
        InvalidPasPacketException() throw() : TransactiveException("Malformed Pas Packet received")
        { }

	    InvalidPasPacketException(const char* msg) throw() : TransactiveException(msg)
        { }

        virtual ~InvalidPasPacketException() throw()
        { }
    };

    class CorruptInternalStateDataException : public TransactiveException
    {

    public:
	    
        CorruptInternalStateDataException() throw() : TransactiveException("Corrupt internal state data detected")
        { }

	    CorruptInternalStateDataException(const char* msg) throw() : TransactiveException(msg)
        { }

        virtual ~CorruptInternalStateDataException() throw()
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
}

#endif // !defined(PASEXCEPTIONS_H_INCLUDED)


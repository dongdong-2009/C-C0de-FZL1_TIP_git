/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/ModbusException.h $
  * @author:  A. Ivankovic
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:58:35 $
  * Last modified by:  $Author: haijun.li $
  * 
  * An exception that is thrown as a result of a Modbus protocol/communications
  * problem.
  */

#ifndef ModbusException_h
#define ModbusException_h

#include <string>

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Bus
{
    class ModbusException : public TA_Base_Core::TransactiveException  
    {
    public:

		static const char FUNCTION_TIMEOUT_ERR;
        static const char ILLEGAL_FUNCTION_ERR;
        static const char ILLEGAL_DATA_ADDRESS_ERR;
        static const char ILLEGAL_DATA_VALUE_ERR;
        static const char SLAVE_DEVICE_FAILURE_ERR;
        static const char ACKNOWLEDGE_ERR;
        static const char SLAVE_DEVICE_BUSY_ERR;
        static const char MEMORY_PARITY_ERR;
        static const char GATEWAY_PATH_UNAVAILABLE_ERR;
        static const char TARGET_FAILED_TO_RESPOND_ERR;

        enum EModbusFail    // Used to specify why a ModbusException was thrown
        {
			NO_EXCEPTION = 0,
            ILLEGAL_FUNCTION,
            ILLEGAL_DATA_ADDRESS,
            ILLEGAL_DATA_VALUE,
            SLAVE_DEVICE_FAILURE,
            ACKNOWLEDGE,
            SLAVE_DEVICE_BUSY,
            MEMORY_PARITY,
            GATEWAY_PATH_UNAVAILABLE,
            TARGET_FAILED_TO_RESPOND,

            // Values above this line comment match the const char error codes.

            INVALID_ADDRESS,
            INVALID_VALUE,
            INVALID_AND_MASK,
            INVALID_OR_MASK,
            BIT_ADDRESS_OUT_OF_RANGE,
            REPLY_PACKET_TOO_SHORT,
            REPLY_INCORRECT_SIZE,
            PACKET_BODY_MISSING,
            UNEXPECTED_FUNCTION_CODE,
            INVALID_REPLY_COUNT,
            CONNECTION_NOT_ENABLED,
			CONNECTION_CLOSED,
            REPLY_TIMEOUT,
            CRC_CALC_ERROR,
            NO_REQUEST_DATA,
            REPLY_HEADER_READ_FAILED,
            INVALID_TRANSACTION_ID,
            INVALID_PROTOCOL_ID,
            INCORRECT_DEVICE_ADDRESS,
            REPLY_PACKET_READ_ERROR,
            CONNECT_ERROR,
            SOCKET_WRITE_ERROR,
            SOCKET_READ_ERROR,
			SOCKET_SELECT_ERROR,
            SIZE_EXCEEDS_BUFFER,

			FAILED_SEND_TWO_FEPS,

			// unknown modbus exception

			UNKNOWN_MODBUS_EXCEPTION
        };


        /**
          * Constructor which takes an Modbus protocol error code as an argument.
          *
          * @param errorCode    A Modbus error code.
          */
        ModbusException( char errorCode ) throw();


        /** 
          * ModbusException
          *
          * Constructor which takes a message and a EModbusFail enum
          *
          * @param message  An exception message describing the problem.
          * @param type     The type of error / failure. This allows the
          *                 code catching the exception to deal with 
          *                 different types of ModbusExceptions differently.
          *
          */
        ModbusException( const std::string& message, const EModbusFail type) throw();

        /**
          * ~ModbusException
          *
          * Destructor.
          */
        virtual ~ModbusException() throw();

	    /**
	      * getFailType
	      * 
	      * This returns the type of failure that occurred. The value returned
	      * should allow the application to determine an appropriate course of
	      * action. The application should always retrieve this so it can 
	      * determine what to do next.
	      *
	      * @return EDataFail This indicates the type of failure that occurred.
	      *
	      */
        EModbusFail getFailType() const
        {
            return m_type;
        }

        /**
          * getDescription
          *
          * Converts a Modbus error code into a description string.
          *
          * @return             A description or NULL if the error code is unknown.
          * @param errorCode    A Modbus error code.
          */
        static std::string getDescription( char errorCode );

        /**
          * getType
          *
          * Converts a Modbus error code into an EModbusFail enum.
          *
          * @return             An EModbusFail enum matching the error code.
          * @param  errorCode   A Modbus error code.
          */
        static EModbusFail getType( char errorCode );

    private:
        EModbusFail m_type;     // The type of ModbusException
    };    
}

#endif // ModbusException_h

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/ModbusException.cpp $
  * @author:  A. Ivankovic
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:58:35 $
  * Last modified by:  $Author: haijun.li $
  * 
  * An exception that is thrown as a result of a Modbus protocol/communications
  * problem.
  */

#include    "ModbusException.h"
#include	"core/utilities/src/TAAssert.h"
#include    <sstream>
#include    <string>

#ifdef _WIN32
	#include <yvals.h>              // warning numbers get enabled in yvals.h 
	#pragma warning(disable: 4715)  // not all control paths return a value
#endif //_WIN32


using TA_Base_Core::TransactiveException;
using namespace std;

namespace TA_Base_Bus
{

	const char ModbusException::FUNCTION_TIMEOUT_ERR =                0x00;
    const char ModbusException::ILLEGAL_FUNCTION_ERR =                0x01;
	const char ModbusException::ILLEGAL_DATA_ADDRESS_ERR =            0x02;
	const char ModbusException::ILLEGAL_DATA_VALUE_ERR =              0x03;
    const char ModbusException::SLAVE_DEVICE_FAILURE_ERR =            0x04;
    const char ModbusException::ACKNOWLEDGE_ERR =                     0x05;
    const char ModbusException::SLAVE_DEVICE_BUSY_ERR =               0x06;
    const char ModbusException::MEMORY_PARITY_ERR =                   0x08;
    const char ModbusException::GATEWAY_PATH_UNAVAILABLE_ERR =        0x0A;
	const char ModbusException::TARGET_FAILED_TO_RESPOND_ERR =        0x0B;


    ModbusException::ModbusException( char errorCode ) throw()
        : TransactiveException( getDescription( errorCode ) ),
          m_type( getType(errorCode) )
    {
    } 


    ModbusException::ModbusException( const std::string& message, 
                                      const EModbusFail type) throw()
        : TransactiveException( message ), m_type( type )
    {
    }


    ModbusException::~ModbusException() throw()
    {
    }

    string ModbusException::getDescription( char errorCode )
    {
        stringstream err;

        switch ( errorCode )
        {
        case FUNCTION_TIMEOUT_ERR:
            return string( "The function code executed is timed out in the modbus device." );

        case ILLEGAL_FUNCTION_ERR:
            return string( "The function code requested is not supported in the modbus device." );

        case ILLEGAL_DATA_ADDRESS_ERR:
            return string( "The data address requested does not exist in the modbus device." );

        case ILLEGAL_DATA_VALUE_ERR:
            return string( "The data value is not valid." );

        case SLAVE_DEVICE_FAILURE_ERR:
            return string( "Unrecoverable error occurred in the modbus device." );

        case ACKNOWLEDGE_ERR:
            return string( "The command is in progress." );

        case SLAVE_DEVICE_BUSY_ERR:
            return string( "The modbus device is busy and the command can not be enacted." );

        case MEMORY_PARITY_ERR:
            return string( "A memory parity error occurred in the modbus device." );

        case GATEWAY_PATH_UNAVAILABLE_ERR:
            return string( "The modbus gateway could not connect to the modbus device." );

        case TARGET_FAILED_TO_RESPOND_ERR:
            return string( "The modbus gateway did not get a response from the modbus device." );

        default:
            err << "Unknown Modbus Error Code 0x" << hex << ( 0xff & int(errorCode) );
            return err.str();
        }
    }

    ModbusException::EModbusFail ModbusException::getType( char errorCode )
    {
        switch ( errorCode )
        {
        case FUNCTION_TIMEOUT_ERR:
            return REPLY_TIMEOUT;

        case ILLEGAL_FUNCTION_ERR:
            return ILLEGAL_FUNCTION;

        case ILLEGAL_DATA_ADDRESS_ERR:
            return ILLEGAL_DATA_ADDRESS;

        case ILLEGAL_DATA_VALUE_ERR:
            return ILLEGAL_DATA_VALUE;

        case SLAVE_DEVICE_FAILURE_ERR:
            return SLAVE_DEVICE_FAILURE;

        case ACKNOWLEDGE_ERR:
            return ACKNOWLEDGE;

        case SLAVE_DEVICE_BUSY_ERR:
            return SLAVE_DEVICE_BUSY;

        case MEMORY_PARITY_ERR:
            return MEMORY_PARITY;

        case GATEWAY_PATH_UNAVAILABLE_ERR:
            return GATEWAY_PATH_UNAVAILABLE;

        case TARGET_FAILED_TO_RESPOND_ERR:
            return TARGET_FAILED_TO_RESPOND;

		default:
			return UNKNOWN_MODBUS_EXCEPTION;
        }        
    }

}


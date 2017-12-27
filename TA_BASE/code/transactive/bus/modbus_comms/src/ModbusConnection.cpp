/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/ModbusConnection.cpp $
  * @author  A. Ivankovic
  * @version $Revision: #3 $
  *
  * Last modification : $DateTime: 2017/01/24 13:44:22 $
  * Last modified by : $Author: yong.liu $
  *
  */

#include <iostream>
#include <iomanip>
#include <sstream>

#include "bus/modbus_comms/src/ModbusConnection.h"
#include "bus/modbus_comms/src/ModbusException.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    const char ModbusConnection::READ_COILS_CODE =                  0x01;
    const char ModbusConnection::READ_DISCRETE_INPUTS_CODE =        0x02;
    const char ModbusConnection::READ_HOLDING_REGISTERS_CODE =      0x03;
    const char ModbusConnection::READ_INPUT_REGISTERS_CODE =        0x04;
    const char ModbusConnection::WRITE_SINGLE_REGISTER_CODE =       0x06;
    const char ModbusConnection::WRITE_MULTIPLE_COILS_CODE =        0x0F;
    const char ModbusConnection::WRITE_MULTIPLE_REGISTERS_CODE =    0x10;
    const char ModbusConnection::MASK_WRITE_REGISTER_CODE =         0x16;

    const int ModbusConnection::BITS_PER_BYTE = 8;

	const int ModbusConnection::BITS_PER_WORD = 8 * sizeof(WORD);
    const int ModbusConnection::MAX_BYTE_UNSIGNED_INTEGER = 255;


    ModbusConnection::ModbusConnection()
    :
    m_device( 0 ),
    m_enabled( true )
    {
    }


    ModbusConnection::~ModbusConnection()
    {
    }


    void ModbusConnection::enable()
    {
        m_enabled = true;
    }


    void ModbusConnection::disable()
    {
        m_enabled = false;
    }


    bool ModbusConnection::isEnabled() const
    {
        return m_enabled;
    }

     
    void ModbusConnection::readCoils( DataBlock<bool>& coils )
    {
        readDigitals( READ_COILS_CODE, coils );
    }

     
    void ModbusConnection::readDiscreteInputs( DataBlock<bool>& inputs )
    {
        readDigitals( READ_DISCRETE_INPUTS_CODE, inputs );
    }

    
    void ModbusConnection::readHoldingRegisters( DataBlock<WORD>& registers )
    {
        readRegisters( READ_HOLDING_REGISTERS_CODE, registers );
    }


    void ModbusConnection::readInputRegisters( DataBlock<WORD>& inputs )
    {
        readRegisters( READ_INPUT_REGISTERS_CODE, inputs );
    }


    void ModbusConnection::writeMultipleCoils( const DataBlock<bool>& coils )
    {
        performAddressCheck( coils.start() );

        std::string request;
        request += WRITE_MULTIPLE_COILS_CODE;
        appendWord( request, coils.start() );
        appendWord( request, coils.length() );

        int byteCount = coils.length() / BITS_PER_BYTE;
        if ( coils.length() % BITS_PER_BYTE > 0 )
        {
            ++byteCount;
        }
        appendByteCount( request, byteCount );

        // Now split out the bool info into the bytes to be sent.
        for( int i=0; i < byteCount; ++i )
        {
            char dataByte = 0;

            // Extract each bit from a data byte.
            for ( int j = 0; j < 8; ++j )
            {
                // Determine where in the coils input block this bit info is originating
                // from.
                int outputIndex = i * 8 + j + coils.start();

                // Only write the bit info if it is in the requested output
                // range. This will occur if the count is not a integer multiple
                // of 8. Also only perform an action if the coil is "true" since
                // the default bit setting is zero.
                if ( coils.isInRange( outputIndex ) && coils[outputIndex] )
                {
                    // Set the appropriate bit in the byte we are processing.
                    dataByte |= ( 0x01 << j );
                }
            }

            // Now add the byte which contains the state of 8 bool flags.
            request += LSB( dataByte );
        }

		writeRequest( request );

		// The expected reply consists of a function code, the start address word and
		// the coil count word.
		int expectedReplySize = sizeof(WRITE_MULTIPLE_COILS_CODE) + 2 * sizeof(WORD); 

		// TD# 3316
        std::string reply;
		readReply( reply, expectedReplySize );

        performWriteReplyChecks( WRITE_MULTIPLE_COILS_CODE, reply, coils.start(), coils.length() );
    }


    void ModbusConnection::writeMultipleRegisters( const DataBlock<WORD>& block )
    {
        performAddressCheck( block.start() );

        std::string request;
        request += WRITE_MULTIPLE_REGISTERS_CODE;
        appendWord( request, block.start() );
        appendWord( request, block.length() );            
        appendByteCount( request, block.length(), sizeof(WORD) );

        // Now write the register data to the packet.
        for( int i=0; i < block.length(); ++i )
        {
            request += MSB( block[ i + block.start() ] );
            request += LSB( block[ i + block.start() ] );
        }

        writeRequest( request );

		// The expected reply consists of a function code, the start address word and
		// the register count word.
		int expectedReplySize = sizeof(WRITE_MULTIPLE_REGISTERS_CODE) + 2 * sizeof(WORD); 

		// TD# 3316
        std::string reply;
        readReply( reply, expectedReplySize );

        performWriteReplyChecks( WRITE_MULTIPLE_REGISTERS_CODE, reply, block.start(), block.length() );
    }
	void ModbusConnection::writeMultipleRegisters( const std::string& block )
    {
		char* pBlock = (char*)block.c_str();

        //performAddressCheck( block.start() );
		int start = pBlock[2] << 8 && pBlock[3];
		int length = pBlock[4] << 8 && pBlock[5];

        std::string request;
        request += WRITE_MULTIPLE_REGISTERS_CODE;
		request += block;
        //appendWord( request, block.start() );
        //appendWord( request, block.length() );            
        //appendByteCount( request, block.length(), sizeof(WORD) );

        //// Now write the register data to the packet.
        //for( int i=0; i < block.length(); ++i )
        //{
        //    request += MSB( block[ i + block.start() ] );
        //    request += LSB( block[ i + block.start() ] );
        //}

        writeRequest( request );

		// The expected reply consists of a function code, the start address word and
		// the register count word.
		int expectedReplySize = sizeof(WRITE_MULTIPLE_REGISTERS_CODE) + 2 * sizeof(WORD); 

		// TD# 3316
        std::string reply;
        readReply( reply, expectedReplySize );

        performWriteReplyChecks( WRITE_MULTIPLE_REGISTERS_CODE, reply, start, length );
    }
	 void ModbusConnection::writeMultipleRegisters( const DataBlock<WORD>& block,WORD startAddress)
    {
        performAddressCheck( block.start() );		
		
        std::string request;
        request += WRITE_MULTIPLE_REGISTERS_CODE;
        appendWord( request, startAddress );
        appendWord( request, block.length() );            
        appendByteCount( request, block.length(), sizeof(WORD) );

        // Now write the register data to the packet.
        for( int i=0; i < block.length(); ++i )
        {
            request += MSB( block[ i + block.start() ] );
            request += LSB( block[ i + block.start() ] );
        }

        writeRequest( request );

		// The expected reply consists of a function code, the start address word and
		// the register count word.
		int expectedReplySize = sizeof(WRITE_MULTIPLE_REGISTERS_CODE) + 2 * sizeof(WORD); 

		// TD# 3316
        std::string reply;
        readReply( reply, expectedReplySize );

        performWriteReplyChecks( WRITE_MULTIPLE_REGISTERS_CODE, reply, block.start(), block.length() );
    }
     void ModbusConnection::writeMultipleRegisters( const DataBlock<BYTE>& block )
    {
        performAddressCheck( block.start() );

        std::string request;
        request += WRITE_MULTIPLE_REGISTERS_CODE;

		//start address
		request += block[0];
		request += block[1];

		//no of words
		request += block[2];
		request += block[3];
		
        //byte count in data packet
        request += block[4];

        // Now write the register data to the packet.
        for( int i=5; i < block.length(); ++i )
        {
            request +=  block[ i ] ;
            //request += LSB( block[ i + block.start() ] );
        }

        writeRequest( request );

		// The expected reply consists of a function code, the start address word and
		// the register count word.
		int expectedReplySize = sizeof(WRITE_MULTIPLE_REGISTERS_CODE) + 2 * sizeof(WORD); 

		// TD# 3316
        std::string reply;
        readReply( reply, expectedReplySize );

        performWriteReplyChecks( WRITE_MULTIPLE_REGISTERS_CODE, reply, block.start(), block.length() );
    }
   

    void ModbusConnection::writeHoldingRegister( int address, WORD value )
    {
        performAddressCheck( address );

        std::string request;
        request += WRITE_SINGLE_REGISTER_CODE;
        appendWord( request, address );
        appendWord( request, value );

        writeRequest( request );

		// The expected reply consists of a function code, the address word and
		// the value word.
		int expectedReplySize = sizeof(WRITE_SINGLE_REGISTER_CODE) + 2 * sizeof(WORD); 

		// TD# 3316
		std::string reply;
        readReply( reply, expectedReplySize );

        int offset = performCommonReplyChecks( WRITE_SINGLE_REGISTER_CODE, reply );

        WORD replyAddress = getWord( reply, offset );
        WORD replyValue = getWord( reply, offset );

        if ( replyAddress != address )
        {
            TA_THROW( ModbusException( "The reply's address does not match that requested.", ModbusException::INVALID_ADDRESS ) );
        }

        if ( replyValue != value )
        {
            TA_THROW( ModbusException( "The reply's value word does not match that requested.", ModbusException::INVALID_VALUE ) );
        }
    }


    void ModbusConnection::writeHoldingRegister( int address, WORD andMask, WORD orMask )
    {
        performAddressCheck( address );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Masking HR %d, AND = 0x%X, OR = 0x%X",
            address, andMask, orMask );

        std::string request;
        request += MASK_WRITE_REGISTER_CODE;
        appendWord( request, address );
        appendWord( request, andMask );
        appendWord( request, orMask );

        writeRequest( request );

		// The expected reply consists of a function code, the address word, the
		// AND mask value word and the OR mask word.
		int expectedReplySize = sizeof(MASK_WRITE_REGISTER_CODE) + 3 * sizeof(WORD); 

		// TD# 3316
		std::string reply;
        readReply( reply, expectedReplySize );

        int offset = performCommonReplyChecks( MASK_WRITE_REGISTER_CODE, reply );

        WORD replyAddress = getWord( reply, offset );
        WORD replyAndMask = getWord( reply, offset );
        WORD replyOrMask = getWord( reply, offset );

        if ( replyAddress != address )
        {
            TA_THROW( ModbusException( "The reply's address does not match that requested.", ModbusException::INVALID_ADDRESS ) );
        }

        if ( replyAndMask != andMask )
        {
            TA_THROW( ModbusException( "The reply's AND mask does not match that requested.", ModbusException::INVALID_AND_MASK ) );
        }

        if ( replyOrMask != orMask )
        {
            TA_THROW( ModbusException( "The reply's OR mask does not match that requested.", ModbusException::INVALID_OR_MASK ) );
        }
    }


    void ModbusConnection::writeHoldingRegisterBit( int address, int bit, bool value )
    {
        if ( bit < 1 || bit > BITS_PER_WORD )
        {
            TA_THROW( ModbusException( "The bit address is not in range", ModbusException::BIT_ADDRESS_OUT_OF_RANGE ) );
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Setting HR %d. %d to %s",
            address, bit, value ? "TRUE" : "FALSE" );

        WORD orMask = 0x01 << ( bit - 1 );
        WORD andMask = ~orMask;

		// If we're resetting the bit then make sure the OR mask is zero.

		if ( !value )
        {
            orMask = 0;
        }

        writeHoldingRegister( address, andMask, orMask );
    }

     
    void ModbusConnection::readDigitals( char functionCode, DataBlock<bool>& digitals )
    {
        TA_ASSERT( functionCode > 0, "The function code should be positive." );
        performAddressCheck( digitals.start() );

        std::string request;
        request += functionCode;
        appendWord( request, digitals.start() );
        appendWord( request, digitals.length() );

        writeRequest( request );
		/* a byte for every digital or a bit for every digital, according to modbus
		   protocol, it should be a bit for every digital, so the expected reply size
		   is wrong.*/

		// The expected reply consists of a function code, the byte count and a
		// byte for every digital.
		// int expectedReplySize = sizeof(functionCode) + sizeof(char) + digitals.length();

		int dataByteNum = digitals.length() / 8;
		if (digitals.length() % 8 != 0)
		{
			dataByteNum += 1;
		}

		int expectedReplySize = sizeof(functionCode) + sizeof(char) + dataByteNum;

		// TD# 3316
		std::string reply;
        readReply( reply, expectedReplySize );

        int offset = performCommonReplyChecks( functionCode, reply );

        int byteCount = ToInt( reply[ offset++ ] );

        if ( byteCount == 0 )
        {
            // A zero byte count implies that the byte count should be
            // derived from the requested data amount. This is an extension
            // to the Modbus protocol that some devices implement.
            byteCount = digitals.length() / BITS_PER_BYTE;
            if ( digitals.length() % BITS_PER_BYTE > 0 ) ++byteCount;
        }

        if ( offset + byteCount > (int) reply.size() )
        {
            TA_THROW( ModbusException( "Reply packet shorter than expected.", ModbusException::REPLY_PACKET_TOO_SHORT ) );
        }

        // Now split out the discrete bit info in the bytes received.
        for( int i = 0; i < byteCount; ++i )
        {
            char dataByte = reply[ offset++ ];

            // Extract each bit from a data byte.
            for( int j = 0; j < BITS_PER_BYTE; ++j )
            {
                // Determine where in the output array this bit info needs to be
                // written.
                int inputIndex = i * BITS_PER_BYTE + j + digitals.start();

                // Only write the bit info if it is in the requested output
                // range. This will occur if the count is not a integer multiple
                // of 8.
                if ( digitals.isInRange( inputIndex ) )
                {
                    // Determine if the bit is set and set the output byte
                    // accordingly i.e. one input bit maps to one output byte.
                    int bit = ( dataByte >> j ) & 0x01;
                    digitals.set( inputIndex, bit != 0 );
                }
            }
        }
    }

    
    void ModbusConnection::readRegisters( char functionCode, DataBlock<WORD>& registers )
    {
        TA_ASSERT( functionCode > 0, "The function code should be positive." );

        performAddressCheck( registers.start() );

        std::string request;
        request += functionCode;
        appendWord( request, registers.start() );
        appendWord( request, registers.length() );

		writeRequest( request );

		// The expected reply consists of a function code, the byte count and a
		// word for every register.
		int expectedReplySize = sizeof(functionCode) + sizeof(char) + registers.length() * sizeof(WORD); 

		// TD# 3316
		std::string reply;
		readReply( reply, expectedReplySize );

        int offset = performCommonReplyChecks( functionCode, reply );

		int byteCount = ToInt( reply[ offset++ ] );
        int expectedByteCount = registers.length() * sizeof(WORD) / sizeof(char);

        if ( byteCount == 0 )
        {
            // A zero byte count implies that the byte count should be
            // derived from the requested data amount. This is an extension
            // to the Modbus protocol that some devices implement.
            byteCount = expectedByteCount;
        }
        else if ( byteCount != expectedByteCount )
        {
            std::ostringstream err;
            err << "The reply contains " << byteCount << " data words when " << expectedByteCount 
                << " were expected.";
            
            TA_THROW( ModbusException( err.str(), ModbusException::REPLY_INCORRECT_SIZE ) );
        }

        if ( offset + byteCount > (int) reply.size() )
        {
            TA_THROW( ModbusException( "Reply packet shorter than expected.", ModbusException::REPLY_PACKET_TOO_SHORT ) );
        }

        for( int i = registers.start(); i <= registers.end(); ++i )
        {
            WORD word = getWord( reply, offset );
            registers.set( i, word );
        }
    }


    int ModbusConnection::performCommonReplyChecks( char functionCode, const std::string& reply )
    {
        TA_ASSERT( functionCode > 0, "The function code should be positive." );

        const unsigned int EXCEPTION_PACKET_SIZE = 2;

        if ( reply.size() == 0 )
        {
            TA_THROW( ModbusException( "The reply does not have a packet body.", ModbusException::PACKET_BODY_MISSING ) );
        }

        int offset = 0;        // The position of the function code in the reply packet.
        char replyFunctionCode = reply[ offset++ ];

        if ( replyFunctionCode != functionCode )
        {
            if ( ( replyFunctionCode & 0x7f ) == functionCode &&
                   reply.size() >= EXCEPTION_PACKET_SIZE )
            {
                // If the 8th bit is set in the reply function code, but the function
                // code value is otherwise the same then an exception error code follows
                // the function code defining the problem.
                char errorCode = reply[ offset++ ];

                throw ModbusException( errorCode );
            }
            else
            {
                std::stringstream err;
                err << std::hex
                    << "The reply packet function code 0x" << ToInt( replyFunctionCode )
                    << " does not equal the expected code 0x" << ToInt( functionCode ) << "."; 
                TA_THROW( ModbusException( err.str(), ModbusException::UNEXPECTED_FUNCTION_CODE ) );
            }
        }

        return offset;
    }


    int ModbusConnection::performWriteReplyChecks( char functionCode, const std::string & reply, int start, int count )
    {
        int offset = performCommonReplyChecks( functionCode, reply );
        int replyStart = getWord( reply, offset );

        if ( replyStart != start )
        {
            std::ostringstream err;
            err << "The write reply start address (" << replyStart << ") does not match that requested ("  << start << ").";
            
            TA_THROW( ModbusException( err.str(), ModbusException::INVALID_ADDRESS ) );
        }

        int replyCount = getWord( reply, offset );
        if ( replyCount != count )
        {
            std::stringstream err;
            err << "The write coils count reply (" << replyCount << ") does not match that requested (" << count << ").";
            TA_THROW( ModbusException( err.str(), ModbusException::INVALID_REPLY_COUNT ) );
        }

        return offset;
    }
    
    
    void ModbusConnection::appendByteCount( std::string & request, int dataCount, int elementSize )
    {
        int byteCount = dataCount * elementSize;
        
        if ( byteCount > MAX_BYTE_UNSIGNED_INTEGER )
        {
            // If the number of data bytes required can not be specified
            // in a single length byte, then set the length byte to zero
            // and get the Modbus server to infer the number of data
            // bytes from the length of the packet. This is not strictly
            // compatible with the Modbus protocol but allows for extensions
            // to the protocol implemented by non-Modicon devices.
            request += LSB( 0 );
        }
        else
        {
            request += LSB( byteCount );
        }
    }
    

    void ModbusConnection::appendWord( std::string & request, WORD word )
    {
        request += MSB( word );
        request += LSB( word );
    }


    WORD ModbusConnection::getWord( const std::string & packet, int & offset )
    {
		char hi = packet[ offset++ ];
		char lo = packet[ offset++ ];
        
        WORD word = getWord( hi, lo );

        return word;
    }


    void ModbusConnection::performAddressCheck( int address )
    {
        if ( address < 0 )
        {
            TA_THROW( ModbusException( "The start address requested is negative.", ModbusException::INVALID_ADDRESS ) );
        }

        if ( false == isEnabled() )
        {
            TA_THROW( ModbusException( "The connection is not enabled for communications.", ModbusException::CONNECTION_NOT_ENABLED ) );
        }
    }


	void ModbusConnection::hexDump( const char* prefix, const char* buffer, int length, bool fullDump )
	{
		static const int MAX_BYTES_LOGGED = 512;

		if ( true == fullDump || DebugUtil::getInstance().getLevel() >= DebugUtil::DebugDebug )
        {
            std::ostringstream message;
            message << prefix
                    << std::hex << std::uppercase << std::setfill('0');
        
            int nDumpLength = length;
            
            if ( false == fullDump )
            {
                nDumpLength = length < MAX_BYTES_LOGGED ? length : (MAX_BYTES_LOGGED - 2);
            }

            for( int i=0; i < nDumpLength; ++i )
            {
                unsigned int value = 0xff & (unsigned int) buffer[i];
                message << " " << std::setw(2) << value;
            }
        
            if ( length > MAX_BYTES_LOGGED )
            {
                unsigned int crc1 = buffer[ length - 2] & 0xff;
                unsigned int crc2 = buffer[ length - 1 ] & 0xff;
                message << " ... " << std::setw(2) << crc1 << " " << std::setw(2) << crc2;
            }

            LOG_GENERIC( SourceInfo, fullDump ? DebugUtil::DebugInfo : DebugUtil::DebugDebug, "%s",
                message.str().c_str() );
        }
	}
}

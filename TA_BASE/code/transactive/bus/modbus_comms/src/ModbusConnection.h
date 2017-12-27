/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/ModbusConnection.h $
  * @author  A. Ivankovic
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  * An abstract class that represents a connection to a Modbus device. Note that the
  * form of the connection is not mandated by this base class. Examples of forms
  * of Modbus connections include:
  * a) a connection that uses TCP/IP as the transport layer.
  * b) a connection that uses a serial point for communicating.
  * c) a connection that aggregates a set of connections to improve throughput.
  *
  * This class implements the aspects of the Modbus communication protocol that are
  * common across different connection types.
  *
  */

#ifndef ModbusConnection_h
#define ModbusConnection_h

#include <string>
#include "IModbus.h"

namespace TA_Base_Bus
{
    class ModbusConnection : public IModbus
    {

    public:
        /**
          * ModbusConnection
          *
          * Constructor.
          */
        ModbusConnection();

        /**
          * ~ModbusConnection
          *
          * Destructor.
          */
        virtual ~ModbusConnection();

        /**
          * getDeviceAddress
          *
          * Device address accessor.
          *
          * @return The address of the Modbus device this connection object 
          *         communicates to.
          */
        virtual int getDeviceAddress() const
		{
			return m_device;
		}

        /**
          * setDeviceAddress
          *
          * Device address mutator.
          *
          * @param address  The address of the Modbus device this connection communicates with.
          *
          */
        virtual void setDeviceAddress( int device )
		{
			m_device = device;
		}

        /**
          * enable
          *
          * Enables this object for Modbus device communications. This object
          * will subsequently initiate connections to the actual modbus device
          * as required to respond to communication method calls.
          */
        virtual void enable();

        /**
          * disable
          *
          * Disables Modbus communications via the object. All current connections
          * to Modbus devices are disconnected. Any subsequent communication method
          * calls on this object will fail (until it is enabled again). The disable
          * method is useful in redundancy scenarios where a IModbus instance is
          * being used as a "hot standby" but should not take up any Modbus device
          * connection resources.
          */
        virtual void disable();

        /**
          * isEnabled
          *
          * @return Whether this modbus connection instance is currently enabled.
          */
        virtual bool isEnabled() const;

        /**
          * readCoils
          *
          * Reads coil data from a Modbus device. Coil data is a Modbus protocol reference
          * to discrete digital outputs.
          *
          * @param coils    The data block of coils to be read.
          * @exception      A ModbusException is thrown if the Modbus protocol interaction
          *                 succeeded but an error code was returned in the protocol response.
          */
        virtual void readCoils( DataBlock<bool>& coils );

        /**
          * readDiscreteInputs
          *
          * Reads a discrete input data block from a Modbus device.
          *
          * @param inputs   The data block of discrete inputs to be read.    
          * @exception      A ModbusException is thrown if the Modbus protocol interaction
          *                 succeeded but an error code was returned in the protocol response.
          */
        virtual void readDiscreteInputs( DataBlock<bool>& inputs );

        /**
          * readHoldingRegisters
          *
          * Reads data from a holding register block in a Modbus device.
          *
          * @param registers    The holding register data block to be read.
          * @exception          A ModbusException is thrown if the Modbus protocol
          *                     interaction succeeded but an error code was returned 
          *                     in the protocol response.
          */
        virtual void readHoldingRegisters( DataBlock<WORD>& registers );

        /**
          * readInputRegisters
          *
          * Reads input data from the input register block in a Modbus device.
          *
          * @param inputs   The input register data block to be read.
          * @exception      A ModbusException is thrown if the Modbus protocol interaction
          *                 succeeded but an error code was returned in the protocol response..
          */
        virtual void readInputRegisters( DataBlock<WORD>& inputs );

        /**
          * writeMultipleCoils
          *
          * Writes discrete output data to a Modbus device. The term "coil" is a 
          * historical term referring to an individual digital output.
          *
          * @param coils    A data block of coil values to be written to the device.
          * @exception      A ModbusException is thrown if the Modbus protocol interaction
          *                 succeeded but an error code was returned in the protocol response.
          */
        virtual void writeMultipleCoils( const DataBlock<bool>& coils );

        /**
          * writeMultipleRegisters
          *
          * Writes a block of output data to the specified holding register block in a
          * Modbus device.
          *
          * @param registers    The holding register data block to be written to the device.
          * @exception          A ModbusException is thrown if the Modbus protocol
          *                     interaction succeeded but an error code was returned 
          *                     in the protocol response.
          */
        virtual void writeMultipleRegisters( const DataBlock<WORD>& registers );
		/**
          * writeMultipleRegisters
          *
          * Writes a block of output data to the specified holding register block in a
          * Modbus device.
          *
          * @param registers    The holding register data block to be written to the device.
          * @exception          A ModbusException is thrown if the Modbus protocol
          *                     interaction succeeded but an error code was returned 
          *                     in the protocol response.
          */
		virtual void writeMultipleRegisters( const std::string& registers );

	 /**
	  * writeMultipleRegisters
	  *
	  * Writes a block of output data to the specified holding register block in a
	  * Modbus device.
	  *
	  * @param registers	The holding register data block to be written to the device.
	  *                     its start address is not used
	  * @param startAddress the startaddress
	  * @exception			A ModbusException is thrown if the Modbus protocol
	  * 					interaction succeeded but an error code was returned 
	  * 					in the protocol response.
	  */
		virtual void writeMultipleRegisters( const DataBlock<WORD>& registers,WORD startAddress );

		/**
		*writeMultipleRegisters
		*
		*Writes a block of output data to the specified holding register block in a
		*Modbus Device
		* registers includes start address, number of words written,byte count and Force Data
		*/
		virtual void writeMultipleRegisters( const DataBlock<BYTE>& registers );

	

        /**
          * writeHoldingRegister
          *
          * Writes a word to the specified holding register.
          *
          * @param address  The address of the register to be written to.
          * @param value    The value to write.
          * @exception      A ModbusException is thrown if the Modbus protocol
          *                 interaction succeeded but an error code was returned 
          *                 in the protocol response.
          */
        virtual void writeHoldingRegister( int address, WORD value );

        /**
          * writeHoldingRegister
          *
          * Writes to the specified holding register but also applies a combination
          * of an AND mask and an OR mask during the write. The algorithm used
          * is as follows:
          * Result = (Current Contents AND And_Mask) OR (Or_Mask AND And_Mask)
          *
          * @param address  The address of the register to be written to.
          * @param andMask  The AND mask to use.
          * @param mask     The OR mask to use.
          * @exception      A ModbusException is thrown if the Modbus protocol
          *                 interaction succeeded but an error code was returned 
          *                 in the protocol response.
          */
        virtual void writeHoldingRegister( int address, WORD andMask, WORD orMask );

        /**
          * writeHoldingRegister
          *
          * Writes to a specific bit within a holding register. The bit address
          * ranges from 1 through to 16 inclusive. This functionality is implemented
          * using the masked register write Modbus protocol functionality.
          *
          * @param address  The address of the register to be written to.
          * @param bit      The bit number to set.
          * @param value    The value to set the bit to (true=1, false=0).
          * @exception      A ModbusException is thrown if the Modbus protocol
          *                 interaction succeeded but an error code was returned 
          *                 in the protocol response.
          */
        virtual void writeHoldingRegisterBit( int address, int bit, bool value );

    protected:
        /**
          * readDigitals
          *
          * Reads discrete digital data from a Modbus device. This can be either digital
          * inputs or output coils depending on the function code supplied as a parameter.
          *
          * @param code     The Modbus function code to use for the request.
          * @param digitals The digital data block to update with the read.
          * @exception      A ModbusException is thrown if the Modbus protocol interaction
          *                 succeeded but an error code was returned in the protocol response.
          */
        void readDigitals( char code, DataBlock<bool>& digitals  );

        /**
          * readRegisters
          *
          * Reads input data from a register block in a Modbus device. The register block
          * to read is defined by the function code parameter.
          *
          * @param code     The Modbus function code to use for the request.
          * @param output   The data block to update with the read.
          * @exception      A ModbusException is thrown if the Modbus protocol interaction
          *                 succeeded but an error code was returned in the protocol response.
          */
        void readRegisters( char code, DataBlock<WORD>& output );

        /**
          * writeRequest
          *
          * Writes Modbus request data to the communication connection.
          *
          * @param request  The data buffer defining the request.
          */
        virtual void writeRequest( const std::string& request ) = 0;

        /**
          * readReply
          *
          * Reads the reply to a Modbus request.
          *
          * @param reply        A buffer to store the reply data in. Note that the reply
          *                     data does not include any transport layer specific header
          *                     or footer data.

		  * @param expectedSize The expected number of bytes in the reply packet assuming

		  *                     the reply isn't an exception/error response.
          */
        virtual void readReply( std::string& reply, int expectedSize ) = 0;

        /**
          * performCommonReplyChecks
          *
          * Checks the supplied reply packet for:
          * - The expected function code.
          * - The standard error response.
          *
          * @return         The offset position in the reply packet directly after
          *                 the Modbus function code.
          * @param code     The Modbus function code expected in the reply.
          * @param reply    The reply packet to check.
          * @exception      A ModbusException is thrown if any of the checks fail.
          */
        int performCommonReplyChecks( char code, const std::string& reply );

        /**
          * performWriteReplyChecks
          *
          * Checks the supplied reply packet to a multiple write request for:
          * - The expected function code.
          * - The standard error response.
          * - The start address of the block written.
          * - The number of elements written.
          *
          * @return         The offset position in the reply packet directly after
          *                 the Modbus function code.
          * @param code     The Modbus function code expected in the reply.
          * @param reply    The reply packet to check.
          * @param start    The start address of the write request.
          * @param count    The number of data elements in the write request.
          * @exception      A ModbusException is thrown if any of the checks fail.
          */
        int performWriteReplyChecks( char functionCode, const std::string & reply,
                                     int start, int count );
        
        /**
          * appendByteCount
          *
          * Appends a byte count to the supplied request packet. The byte count
          * is calculated based on the number of data items and the size of
          * each item in bytes. Note that a zero byte count is appended if the
          * calculated count exceeds the capacity of a byte. This does not strictly
          * match the Modbus protocol but allows for communication to non-Modicon
          * devices that can handle large packet sizes.
          *
          * @param request      The request packet to append to.
          * @param dataCount    The number of data elements that will follow.
          * @param elementSize  The size in bytes of each data element.
          */
        virtual void appendByteCount( std::string & request, int dataCount, 
                              int elementSize = sizeof(char) );

        /**
          * MSB
          *
          * Most signficant byte method.
          *
          * @return         The most significant byte of the supplied integer.
          * @param data     The integer to be processed.
          */
        inline char MSB( int data ) const
		{
			return data >> 8;
		}

        /**
          * LSB
          *
          * Least signficant byte method.
          *
          * @return         The least significant byte of the supplied integer.
          * @param data     The integer to be processed.
          */
        inline char LSB( int data ) const
		{
			return data & 0xff;
		}

        /**
          * ToInt
          *
          * Converts a char into an int.
          *
          * @return         The integer.
          * @param data     The char to be converted as an unsigned char.
          */
        inline int ToInt( char data ) const
		{
			return 0xff & (unsigned) data;
		}

        /**
          * getWord
          *
          * Converts a high byte and a low byte into a WORD.
          * 
          * @return         An unsigned 16 bit word.
          * @param high     The high order byte.
          * @param low      The low order byte.
          */
        inline WORD getWord( char high, char low ) const
        {
            return ( (ToInt(high) << 8) | ToInt(low) );
        }

        /**
          * getWord
          *
          * Extracts a 16 bit unsigned word from the packet at the defined
          * offset in a big-endian format. The offset is incremented appropriately.
          *
          * @param packet   The string packet to read.
          * @param offset   The offset to start read from. This offset is
          *                 modified to point to the next word in the
          *                 packet.
          * @return         A 16 bit unsigned word read from the packet.
          */
        WORD getWord( const std::string & packet, int & offset );

        /**
          * appendWord
          * 
          * Appends a WORD to the supplied communication packet string.
          *
          * @param request  The string to append to.
          * @param word     The word value to append.
          */
        void appendWord( std::string & request, WORD word );

        /**
          * performAddressCheck
          *
          * Checks whether the address is valid and whether the modbus object
          * is enabled for communications.
          *
          * @param address  The address to be checked.
          * @exception      A ModbusException is thrown if the checks fail.
          */
        void performAddressCheck( int address );

		/**
		  * hexDumpDebug
          *
		  * Sends a hex dump of the specified buffer to the debug log output.
		  *
		  * @param prefix   The prefix to place in front of the log.
		  * @param buffer   The buffer to output in hex.
		  * @param length   The length of the buffer.
          * @param fullDump If true; dump full length of buffer at INFO Level
          *                 Else dump only 1st 128 byte at DEBUG Level
		  */

		void hexDump( const char* prefix, const char* buffer, int length, bool fullDump = false );

    private:
        ModbusConnection( const ModbusConnection& );
        ModbusConnection& operator=( const ModbusConnection& );

    public:
        static const char READ_COILS_CODE;
        static const char READ_DISCRETE_INPUTS_CODE;
        static const char READ_HOLDING_REGISTERS_CODE;
        static const char READ_INPUT_REGISTERS_CODE;
        static const char WRITE_SINGLE_REGISTER_CODE;
        static const char WRITE_MULTIPLE_COILS_CODE;
        static const char WRITE_MULTIPLE_REGISTERS_CODE;
        static const char MASK_WRITE_REGISTER_CODE;
        static const int BITS_PER_BYTE;
		static const int BITS_PER_WORD;
        static const int MAX_BYTE_UNSIGNED_INTEGER;

    private:
        int m_device;               // The address of the Modbus device.
        volatile bool m_enabled;    // The connection is enabled for communications.
    };
}

#endif // ModbusConnection_h

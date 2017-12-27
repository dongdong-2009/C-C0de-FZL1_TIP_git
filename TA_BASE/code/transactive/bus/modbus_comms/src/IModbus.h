/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/IModbus.h $
  * @author  A. Ivankovic
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  * Defines an interface that represents a connection to a device that implements
  * the Modbus protocol for communication. The interface device memory related
  * methods and does not expose the raw communications protocol.
  *
  * Note: This interface represents a portion of the Modbus protocol functionality.
  * Refer to the Modbus protocol documentation for details on how this interface
  * can be extended on as "as required" basis.
  *
  */

#ifndef IModbus_h
#define IModbus_h

#include    "DataBlock.h"

namespace TA_Base_Bus
{
    class IModbus
    {

    public:
        /**
          * ~IModbus
          *
          * Destructor.
          */
        virtual ~IModbus()
		{
		}

        /**
          * getDeviceAddress
          *
          * Device address accessor.
          *
          * @return        The address of the Modbus device.
          */
        virtual int getDeviceAddress() const = 0;

        /**
          * setDeviceAddress
          *
          * Device address mutator.
          *
          * @param address  The address of the Modbus device.
          *
          */
        virtual void setDeviceAddress( int address ) = 0;

        /**
          * enable
          *
          * Enables this object for Modbus device communications. This object
          * will subsequently initiate connections to the actual modbus device
          * as required to respond to communication method calls.
          */
        virtual void enable() = 0;

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
        virtual void disable() = 0;

        /**
          * isEnabled
          *
          * @return Whether this IModbus instance is currently enabled.
          */
        virtual bool isEnabled() const = 0;

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
        virtual void readCoils( DataBlock<bool>& coils ) = 0;

        /**
          * readDiscreteInputs
          *
          * Reads a discrete input data block from a Modbus device.
          *
          * @param inputs   The data block of discrete inputs to be read.    
          * @exception      A ModbusException is thrown if the Modbus protocol interaction
          *                 succeeded but an error code was returned in the protocol response.
          */
        virtual void readDiscreteInputs( DataBlock<bool>& inputs ) = 0;

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
        virtual void readHoldingRegisters( DataBlock<WORD>& registers ) = 0;

        /**
          * readInputRegisters
          *
          * Reads input data from the input register block in a Modbus device.
          *
          * @param inputs   The input register data block to be read.
          * @exception      A ModbusException is thrown if the Modbus protocol interaction
          *                 succeeded but an error code was returned in the protocol response..
          */
        virtual void readInputRegisters( DataBlock<WORD>& inputs ) = 0;

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
        virtual void writeMultipleCoils( const DataBlock<bool>& coils ) = 0;

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
        virtual void writeMultipleRegisters( const DataBlock<WORD>& registers ) = 0;

		/**
		* writeMultipleRegisters
		*
		* Writes a block of output data to the specified holding register block in a
		* Modbus device.
		*
		* @param registers	The holding register data block to be written to the device.
		* @param startAddress the startaddress
		* @exception			A ModbusException is thrown if the Modbus protocol
		* 					interaction succeeded but an error code was returned 
		* 					in the protocol response.
		*/
		virtual void writeMultipleRegisters( const DataBlock<WORD>& registers,WORD startAddress ) = 0;

		/**
		*writeMultipleRegisters
		*
		*Writes a block of output data to the specified holding register block in a
		*Modbus Device
		* registers includes start address, number of words written,byte count and Force Data
		*/
		virtual void writeMultipleRegisters( const DataBlock<BYTE>& registers ) = 0;

		/**
		*writeMultipleRegisters
		*
		*Writes a block of output data to the specified holding register block in a
		*Modbus Device
		* registers includes start address, number of words written,byte count and Force Data
		*/
		virtual void writeMultipleRegisters( const std::string& registers ) = 0;


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
        virtual void writeHoldingRegister( int address, WORD value ) = 0;

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
        virtual void writeHoldingRegister( int address, WORD andMask, WORD orMask ) = 0;

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
        virtual void writeHoldingRegisterBit( int address, int bit, bool value ) = 0;
    };
}

#endif // IModbus_h

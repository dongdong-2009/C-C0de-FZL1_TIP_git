#ifndef IMODBUSHELPER_H
#define IMODBUSHELPER_H

#include <string>

#include "bus/modbus_comms/src/DataBlock.h"


//using namespace std;
namespace TA_Base_Bus {
	class IModbusHelper{
	public:
		/**
		* send data to FEPs
		*
		* it will throw TA_Base_Bus::ModbusException when there is any Modbus exception occured.
		* block only includes the force data 
		*
		* @param block  force data, modbus connection will layout MSB and LSB accordingly. 
		* @param startaddress start address
		*/
		virtual int SendData(DataBlock<WORD>& block,WORD startAddress)=0;

		/**
		* send data to FEPs
		*
		* it will throw TA_Base_Bus::ModbusException when there is any Modbus exception occured.
		* block only includes the force data and start address
		*
		* @param block  force data, the block.start() will be the start address,modbus connection will layout MSB and LSB accordingly. 
		*/
		virtual int SendData(DataBlock<WORD>& block) = 0;

		/**
		* send data to FEPs, there will no re-layout bytes according modbus word format at backend. the layout should 
		* be the client's job because backend don't where is the alignment of words.
		* @param block  byte block, includes start address, number for words to write, byte count and force data
		*/

		virtual int SendData(DataBlock<BYTE>& block)=0;


		/**
		* send data to FEPs, there will no re-layout bytes according modbus word format at backend. the layout should 
		* be the client's job because backend don't where is the alignment of words.
		* @param block  string, includes start address, number for words to write, byte count and force data
		*                       ASCII hex strings. for example "ABCD1234" etc.
		*/
		virtual int SendData(std::string& block)=0;

		/**
		* Appends a WORD to the supplied communication data block, the size of datablock will be changed.
		* When sending, bytes of the word will be laid out according Modbus standard wordformat
		*
		* @param block  The datablock to append to.
		* @param word     The word value to append.
		*/
		virtual  void appendWord( DataBlock<WORD>&block, WORD word )=0;

		/**
		* If the size of datablock is less than offset + 2, it will be resized to offset + 2. 
		* If the size of datablock is larger than offset + 2, the size of datablock is not be changed. 
		* When sending , bytes of the word will be laid out according Modbus standard wordformat
		*
		* @param block  The datablock to append to.
		* @param offset the start location to fill the word
		* @param word   The word value to append.
		# 
		*/
		virtual void fillWord(DataBlock<WORD>& block, int offset, WORD word)= 0;

		virtual ~IModbusHelper(){};
	};
}
#endif

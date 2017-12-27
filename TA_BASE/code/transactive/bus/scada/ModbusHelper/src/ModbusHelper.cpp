#include <memory>
#include "core/utilities/src/DebugUtil.h"

#include "bus/modbus_comms/src/ModbusException.h"
#include "bus/modbus_comms/src/SteModbusTcpConnection.h"

#include "boost/format.hpp"
#include "boost/lexical_cast.hpp"
#include "ModbusHelper.h"

using namespace TA_Base_Core;
namespace TA_Base_Bus {
	void ModbusHelper::HexString(const std::string& data,std::string& output )
	{
		std::string::const_iterator di=data.begin();
		output.erase(output.begin(),output.end());
		int dd;
		for(;di!=data.end();di++)
		{
			dd=static_cast<int>(*di) &0xff;

			output+=dd /16 +((dd /16)<10? '0':'A'-10);
			output+=dd %16 +((dd %16)<10? '0':'A'-10);
			output+=' ';
		}

	}
	ModbusHelper::ModbusHelper(std::string primaryHost,std::string primaryPort, std::string secondaryHost,  std::string secondaryPort, std::string slaveAddress, int retries, unsigned long socketTimeout, bool steModbus)
	{  
		FUNCTION_ENTRY("ModbusHelper::ModbusHelper");

		boost::shared_ptr<FEPConnectionInfo> primaryFep(new FEPConnectionInfo(primaryHost,primaryPort,socketTimeout,steModbus));
		this->primaryFep_ = primaryFep;

		boost::shared_ptr<FEPConnectionInfo> standbyFep(new FEPConnectionInfo(secondaryHost,secondaryPort,socketTimeout,steModbus));
		this->standbyFep_ = standbyFep;

		this->slaveAddress_= slaveAddress;
		this->retries_ = retries;
		using boost::lexical_cast;
		using boost::bad_lexical_cast;
		try{
			this->deviceId_ = boost::lexical_cast<int>(slaveAddress_);
		}
		catch(bad_lexical_cast& ex)
		{
			this->deviceId_ = -1;
		}

		FUNCTION_EXIT;
	}
	ModbusHelper::ModbusHelper(std::string primaryHost,std::string primaryPort, std::string secondaryHost,  std::string secondaryPort, 
		int slaveAddress, int startAddress, int retries, unsigned long socketTimeout, bool steModbus)
	{  
		FUNCTION_ENTRY("ModbusHelper::ModbusHelper");

		boost::shared_ptr<FEPConnectionInfo> primaryFep(new FEPConnectionInfo(primaryHost,primaryPort,socketTimeout, steModbus));
		this->primaryFep_ = primaryFep;

		boost::shared_ptr<FEPConnectionInfo> standbyFep(new FEPConnectionInfo(secondaryHost,secondaryPort,socketTimeout, steModbus));
		this->standbyFep_ = standbyFep;


		this->slaveAddress_= slaveAddress;
		this->deviceId_ = slaveAddress;

		this->retries_ = retries;
		
		this->startAddress_ = startAddress;

		FUNCTION_EXIT;
	}
	void ModbusHelper::appendWord( DataBlock<WORD>& block, WORD word )
	{
		FUNCTION_ENTRY("ModbusHelper::appendWord");

		block.setRange(block.start(), block.end()+ 1);
		block.set(block.end() -1, word);

		FUNCTION_EXIT;
	}
	void ModbusHelper::fillWord( DataBlock<WORD>& block,int offset, WORD word )
	{
		FUNCTION_ENTRY("ModbusHelper::fillWord");

		if(block.end() < offset )
		{
			block.setRange(block.start(), offset+1 );
		}
		block.set(offset,word);

		FUNCTION_EXIT;
	}

	bool ModbusHelper::internalSendData(DataBlock<WORD>& block,WORD startAddress,boost::shared_ptr<FEPConnectionInfo> connInfo,TA_Base_Bus::ModbusException& exception)
	{
		bool result = true;
		try
		{
			boost::shared_ptr<IModbus> pModbus = connInfo->getConnection();
			pModbus->setDeviceAddress(this->deviceId_);
			if (connInfo->isSTEModbus())
			{
				pModbus->disable();
				pModbus->enable();
				pModbus->writeMultipleRegisters(block,startAddress);
				pModbus->disable();
				connInfo->resetConnection();
			}
			else
			{
				pModbus->writeMultipleRegisters(block,startAddress);
			}	
		}
		catch(TA_Base_Bus::ModbusException& ex)
		{
			exception = ex;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Primary Exception", ex.what());
			connInfo->resetConnection();
			result =false;
		}
		return result;
	}
	bool ModbusHelper::internalSendData(std::string& block,boost::shared_ptr<FEPConnectionInfo> connInfo,TA_Base_Bus::ModbusException& exception)
	{
		bool result = true;
		try
		{
			boost::shared_ptr<IModbus> pModbus = connInfo->getConnection();
			pModbus->setDeviceAddress(this->deviceId_);
			if (connInfo->isSTEModbus())
			{
				pModbus->disable();
				pModbus->enable();
				pModbus->writeMultipleRegisters(block);
				pModbus->disable();
				connInfo->resetConnection();
			}
			else
			{
				pModbus->writeMultipleRegisters(block);
			}
		}
		catch(TA_Base_Bus::ModbusException& ex)
		{
			exception = ex;
			connInfo->resetConnection();
			result =false;
		}
		return result;
	}
	bool ModbusHelper::internalSendData(DataBlock<WORD>& block,boost::shared_ptr<FEPConnectionInfo> connInfo,TA_Base_Bus::ModbusException& exception)
	{
		bool result = true;
		try
		{
			boost::shared_ptr<IModbus> pModbus = connInfo->getConnection();
			pModbus->setDeviceAddress(this->deviceId_);
			if (connInfo->isSTEModbus())
			{
				pModbus->disable();
				pModbus->enable();
				pModbus->writeMultipleRegisters(block);
				pModbus->disable();
				connInfo->resetConnection();
			}
			else
			{
				pModbus->writeMultipleRegisters(block);
			}
		}
		catch(TA_Base_Bus::ModbusException& ex)
		{
			exception = ex;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Primary Exception", ex.what());
			connInfo->resetConnection();
			result =false;
		}
		return result;
	}
	bool ModbusHelper::internalSendData(DataBlock<BYTE>& block,boost::shared_ptr<FEPConnectionInfo> connInfo,TA_Base_Bus::ModbusException& exception)
	{
		bool result = true;
		try
		{
			boost::shared_ptr<IModbus> pModbus = connInfo->getConnection();
			pModbus->setDeviceAddress(this->deviceId_);
			if (connInfo->isSTEModbus())
			{
				pModbus->disable();
				pModbus->enable();
				pModbus->writeMultipleRegisters(block);
				pModbus->disable();
				connInfo->resetConnection();
			}
			else
			{
				pModbus->writeMultipleRegisters(block);
			}
		}
		catch(TA_Base_Bus::ModbusException& ex)
		{
			exception = ex;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Primary Exception", ex.what());
			connInfo->resetConnection();
			result =false;
		}
		return result;
	}

	std::string  ModbusHelper::DataBlock2String(DataBlock<WORD>& block)
	{
		std::string result; 
		std::ostringstream  ss; 

		boost::format farmatter("%1$#06x");

		for(int i= block.start(); i <= block.end(); i ++)
		{
			ss<< farmatter % block[i];
			ss << "\t";		
		}
		result = ss.str();
		return result;
	}
	std::string  ModbusHelper::DataBlock2String(DataBlock<BYTE>& block)
	{
		std::string result; 
		std::ostringstream  ss; 
		boost::format farmatter("%1$#04x");
		for(int i= block.start(); i <= block.end(); i ++)
		{	
			ss<< farmatter %block[i];
			ss<< "\t";
		}
		result = ss.str();

		return result;
	}
	void ModbusHelper::String2DataBlock(std::string& data, DataBlock<BYTE> & block)
	{
		block.setStartAndLength(0,data.length());
		
		//start address
		block.set(0, data[1]);
		block.set(1, data[0]);

		//word length
		block.set(2, data[3]);
		block.set(3, data[2]);

		//byte count
		block.set(4, data[4]);
		
		//force data
		for(int i=5; i< data.length(); i += 2)
		{
			block.set(i, data[i+1]);
			block.set(i+1, data[i]);
		}

	}
	int ModbusHelper::SendData(std::string& block)
	{

		FUNCTION_ENTRY("ModbusHelper::SendData");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"SendData(std::string& block) Datablock: %s,len:%d", block.c_str(),block.length() );
		std::string hexstr;
		HexString(block,hexstr);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"SendData(std::string& block) Datablock: %s,len:%d", hexstr.c_str(),block.length() );

		DataBlock<BYTE> byteBlock;		

		this->String2DataBlock(block,byteBlock);
		std::string data  = DataBlock2String(byteBlock) ;
		LOG_GENERIC ( SourceInfo, DebugUtil::DebugInfo, "Datablock: start %d, length: %d, data: %s", byteBlock.start(), byteBlock.length(), data.c_str() );

		int result = 0;

		//send data to two connections, get the result and exceptions , raise exception accordingly
		TA_Base_Bus::ModbusException primaryException("unknow error",ModbusException::UNKNOWN_MODBUS_EXCEPTION);
		TA_Base_Bus::ModbusException secondaryException("unknow error",ModbusException::UNKNOWN_MODBUS_EXCEPTION); 

		for(int i = 1; i <= this->retries_; i ++)
		{
			bool primaryResult = this->internalSendData(byteBlock, this->primaryFep_,primaryException);

			if(primaryResult )
			{
				result =0;
				break;
			}

			bool secondaryResult = this->internalSendData(byteBlock, this->standbyFep_,secondaryException);

			if(secondaryResult)
			{
				result = 0;
				this->standbyFep_.swap(this->primaryFep_);
				break;
			}
		}
		if( -1 == result)
		{
			//if we can know which one is the primary FEP and which is the secondary one, raise the exception got from primary one
			//secondary one only get negative acknowledge
			//if( ModbusException::ACKNOWLEDGE_ERR == primaryException.getFailType()
			std::string msg= "Failed to send to primary and secondary FEPs, and respectively exceptions: " + this->combineExceptions(primaryException, secondaryException);
			TA_THROW( ModbusException( msg, ModbusException::FAILED_SEND_TWO_FEPS ) );
		}
		FUNCTION_EXIT;

		return result;
	}
	int ModbusHelper::SendData(DataBlock<WORD>& block)
	{
		FUNCTION_ENTRY("ModbusHelper::SendData");

		std::string data  = DataBlock2String(block) ;

		LOG_GENERIC ( SourceInfo, DebugUtil::DebugInfo, "ModbusHelper::SendData(DataBlock<WORD>& block) datablock: start %d, length: %d, data: %s", block.start(), block.length(), data.c_str());


		int result = 0;

		//send data to two connections, get the result and exceptions , raise exception accordingly
		TA_Base_Bus::ModbusException primaryException("unknow error",ModbusException::UNKNOWN_MODBUS_EXCEPTION);
		TA_Base_Bus::ModbusException secondaryException("unknow error",ModbusException::UNKNOWN_MODBUS_EXCEPTION); 
		for(int i = 1; i <= this->retries_; i ++)
		{
			bool primaryResult = this->internalSendData(block, this->primaryFep_,primaryException);
			if(primaryResult) 
			{
				result = 0;
				break;
			}
			
			bool secondaryResult = this->internalSendData(block, this->standbyFep_,secondaryException);

			if(secondaryResult)
			{
				result = 0;
				this->standbyFep_.swap(this->primaryFep_); //primary changes as standby one, standby changes as primary
				break;
			}
			
			result = -1;
		}
		
		if( -1 == result)
		{
			std::string msg= "Failed to send to primary and secondary FEPs, and respectively exceptions: " + this->combineExceptions(primaryException, secondaryException);
			
			TA_THROW( ModbusException( msg, ModbusException::FAILED_SEND_TWO_FEPS ) );
		}


		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Successfully done ModbusHelper::SendData(DataBlock<WORD>& block) datablock: start %d, length: %d, data: %s", block.start(), block.length(), data.c_str());
				
		FUNCTION_EXIT;

		return result;
	}

	int ModbusHelper::SendData(DataBlock<WORD>& block, WORD startAddress)
	{
		FUNCTION_ENTRY("ModbusHelper::SendData");
		std::string data  = DataBlock2String(block) ;
		LOG_GENERIC ( SourceInfo, DebugUtil::DebugInfo, "SendData(DataBlock<WORD>& block, WORD startAddress) Datablock: start address  %d, start %d, length: %d, data: %s ",startAddress, block.start(), block.length(), data.c_str());
		int result = 0;

		//send data to two connections, get the result and exceptions , raise exception accordingly
		TA_Base_Bus::ModbusException primaryException("unknow error",ModbusException::UNKNOWN_MODBUS_EXCEPTION);
		TA_Base_Bus::ModbusException secondaryException("unknow error",ModbusException::UNKNOWN_MODBUS_EXCEPTION); 
		for(int i = 1; i <= this->retries_; i ++)
		{
			bool primaryResult = this->internalSendData(block,startAddress, this->primaryFep_,primaryException);
			if(primaryResult)
			{
				result = 0;
				break;
			}
			bool secondaryResult = this->internalSendData(block,startAddress, this->standbyFep_,secondaryException);
			if(secondaryResult)
			{
				result = 0;
				this->standbyFep_.swap(this->primaryFep_);
				break;
			}

			result = -1;
		}
		if( -1 == result)
		{
			std::string msg= "Failed to send to primary and secondary FEPs, and respectively exceptions: " + this->combineExceptions(primaryException, secondaryException);
			TA_THROW( ModbusException( msg, ModbusException::FAILED_SEND_TWO_FEPS ) );
		}
		LOG_GENERIC ( SourceInfo, DebugUtil::DebugInfo, "Succesfully done SendData(DataBlock<WORD>& block, WORD startAddress) Datablock: start address  %d, start %d, length: %d, data: %s ",startAddress, block.start(), block.length(), data.c_str());

		FUNCTION_EXIT;

		return result;
	}


	int ModbusHelper::SendData(DataBlock<BYTE>& block)
	{

		FUNCTION_ENTRY("ModbusHelper::SendData");
		std::string data  = DataBlock2String(block) ;
		LOG_GENERIC ( SourceInfo, DebugUtil::DebugInfo, "SendData(DataBlock<BYTE>& block) Datablock: start %d, length: %d, data: %s", block.start(), block.length(), data.c_str() );
		int result = 0;

		//send data to two connections, get the result and exceptions , raise exception accordingly
		TA_Base_Bus::ModbusException primaryException("unknow error",ModbusException::UNKNOWN_MODBUS_EXCEPTION);
		TA_Base_Bus::ModbusException secondaryException("unknow error",ModbusException::UNKNOWN_MODBUS_EXCEPTION); 
		for(int i = 1; i <= this->retries_; i ++)
		{
			bool primaryResult = this->internalSendData(block, this->primaryFep_,primaryException);
		

			if(primaryResult )
			{
				//failed to send data to the two FEPs
				//combine the exceptions				
				result = 0;
				break;
			}

			bool secondaryResult = this->internalSendData(block, this->standbyFep_,secondaryException);
			if(secondaryResult)
			{
				result = 0;
				this->standbyFep_.swap(this->primaryFep_);
				break;
			}
		}
		if( -1 == result)
		{
			std::string msg= "Failed to send to primary and secondary FEPs, and respectively exceptions: " + this->combineExceptions(primaryException, secondaryException);
			TA_THROW( ModbusException( msg, ModbusException::FAILED_SEND_TWO_FEPS ) );
		}
		FUNCTION_EXIT;

		return result;
	}

	std::string   ModbusHelper::combineExceptions(ModbusException & primaryEx, ModbusException& secondaryEx)
	{
		std::stringstream ss ;
		ss << "primaryFEP exception type: " << primaryEx.getFailType() << "\t"
			<< "primaryFEP exception description: " << primaryEx.what() << "\t"
			<< "secondaryFEB exception type: " << secondaryEx.getFailType() << "\t"
			<< "secondaryFEB exception description: " << secondaryEx.what() ;
		return ss.str();

	}
	ModbusHelper::~ModbusHelper()
	{
	}
}

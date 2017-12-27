#ifndef MODBUS_POLLING_COMMAND_QUEUE_H
#define MODBUS_POLLING_COMMAND_QUEUE_H

#include "bus/modbus_comms/src/DataBlock.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "boost/shared_ptr.hpp"
#include <queue>

namespace TA_Base_Bus
{
	enum ECommandType
	{
		FAST_POLLING = 0,
		SLOW_POLLING,
		WRITE_REQUEST_REGISTER,
		WRITE_REQUEST_COILS
	};
    
	class ModbusCommand
	{
	public:
		ModbusCommand(ECommandType cmdType,int startAddress,int endAddress);
		ModbusCommand(DataBlock< WORD >* addressBlock);
		ModbusCommand(DataBlock< bool >* addressBlock);
        
		virtual ~ModbusCommand();
		bool waitResult();
		void postResult(bool bSuccess);
		int getStartAddress();
		int getEndAddress();
        DataBlock< WORD >* getDataBlockRegister();
        DataBlock< bool >* getDataBlockCoils();
		ECommandType getCommandType();
		std::string getCommandTypeString();

	private:
        ECommandType    		m_commandType;
		int						m_startAddress;
		int						m_endAddress;
        DataBlock< WORD >* 		m_addressBlockRegister;
        DataBlock< bool >* 		m_addressBlockCoils;
		bool					m_exeResult;
		TA_Base_Core::Semaphore m_semaphore;
	};

	typedef boost::shared_ptr<ModbusCommand> ModbusCommandPtr;

	class ModbusCommandQueue
	{
	public:
		ModbusCommandQueue();
		virtual ~ModbusCommandQueue();
		ModbusCommandPtr getNextCommand();
		void scheduleCommand(ModbusCommandPtr& cmd);
		void cleanCommand();
		void unBlock();
		bool isEmpty();
		bool isNotEmpty();
	private:
		std::queue< ModbusCommandPtr >					m_commands;
		TA_Base_Core::ReEntrantThreadLockable			m_commandLock;
		TA_Base_Core::Semaphore							m_semaphore;
	};
}

#endif

#include "CommandQueue.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	ModbusCommand::ModbusCommand( ECommandType cmdType,int startAddress,int endAddress ) 
		:
		m_commandType(cmdType),
		m_exeResult(false), 
		m_startAddress(startAddress),
		m_endAddress(endAddress),
		m_addressBlockRegister(NULL),
		m_addressBlockCoils(NULL),
		m_semaphore(0)
	{	
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "ModbusCommandType[%d|%s] startAddress[%d],endAddress[%d]",m_commandType,getCommandTypeString().c_str(), m_startAddress, m_endAddress);
	}

	ModbusCommand::ModbusCommand( DataBlock< WORD >* addressBlock ) 
		:
		m_commandType(WRITE_REQUEST_REGISTER),
		m_exeResult(false), 
		m_addressBlockRegister(addressBlock),
		m_addressBlockCoils(NULL),
		m_semaphore(0)
	{	
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "ModbusCommandType[%d|%s] startAddress[%d],endAddress[%d]",m_commandType,getCommandTypeString().c_str(), 
		m_addressBlockRegister->start(),  m_addressBlockRegister->end());
	}

	ModbusCommand::ModbusCommand( DataBlock< bool >* addressBlockCoils ) 
		:
		m_commandType(WRITE_REQUEST_COILS),
		m_exeResult(false), 
		m_addressBlockRegister(NULL),
		m_addressBlockCoils(addressBlockCoils),
		m_semaphore(0)
	{	
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "ModbusCommandType[%d|%s] startAddress[%d],endAddress[%d]",m_commandType,getCommandTypeString().c_str(), 
		m_addressBlockCoils->start(),  m_addressBlockCoils->end());
	}
        	
	
	ModbusCommand::~ModbusCommand()
	{
		m_semaphore.post();
	}

	void ModbusCommand::postResult( bool bSuccess )
	{
		m_exeResult = bSuccess;
		m_semaphore.post();
	}

	bool ModbusCommand::waitResult()
	{
		m_semaphore.wait();
		return m_exeResult;
	}

	int ModbusCommand::getStartAddress()
	{
		return m_startAddress;
	}

	int ModbusCommand::getEndAddress()
	{
		return m_endAddress;
	}
    
	ECommandType ModbusCommand::getCommandType()
	{
		return m_commandType;
	}

	std::string ModbusCommand::getCommandTypeString()
	{
		std::string commandTypeStr;
		switch (m_commandType)
		{
			case FAST_POLLING:
				commandTypeStr = "Fast Polling";
				break;
			case SLOW_POLLING:
				commandTypeStr = "Slow Polling";
				break;			
			case WRITE_REQUEST_REGISTER:
				commandTypeStr = "Register Write Request";
				break;
			case WRITE_REQUEST_COILS:
				commandTypeStr = "Coils Write Request";
				break;			
			default:
				break;
		}
		return commandTypeStr;
	}

    DataBlock< WORD >* ModbusCommand::getDataBlockRegister()
    {
        return m_addressBlockRegister;
    }
	
    DataBlock< bool >* ModbusCommand::getDataBlockCoils()
    {
        return m_addressBlockCoils;
    }

	ModbusCommandQueue::ModbusCommandQueue()
		:
	m_semaphore(0)
	{

	}

	ModbusCommandQueue::~ModbusCommandQueue()
	{
		cleanCommand();
	}

	void ModbusCommandQueue::scheduleCommand( ModbusCommandPtr& cmd)
	{
		{
			TA_Base_Core::ThreadGuard guard(m_commandLock);
			m_commands.push(cmd);	
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "ModbusCommandQueue.size() = %d", m_commands.size());
		}
		m_semaphore.post();
	}

	ModbusCommandPtr ModbusCommandQueue::getNextCommand()
	{
		m_semaphore.wait();
		ModbusCommandPtr nextCmd;
		{
			TA_Base_Core::ThreadGuard guard(m_commandLock);
			nextCmd = m_commands.front();
			m_commands.pop();
		}
		return nextCmd;
	}

	void ModbusCommandQueue::cleanCommand()
	{
		TA_Base_Core::ThreadGuard guard(m_commandLock);
		while(!m_commands.empty())
		{
			m_commands.pop();
		}
	}

	void ModbusCommandQueue::unBlock()
	{
		m_semaphore.post();
	}

	bool ModbusCommandQueue::isEmpty()
	{
		TA_Base_Core::ThreadGuard guard(m_commandLock);
		return m_commands.empty();
	}

    bool ModbusCommandQueue::isNotEmpty()
	{
		TA_Base_Core::ThreadGuard guard(m_commandLock);
        bool isNotEmpty = m_commands.empty();
		return !isNotEmpty;
	}
}

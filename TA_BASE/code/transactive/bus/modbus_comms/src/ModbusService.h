/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/bus/modbus_comms/src/ModbusService.h $
  * @author:  A. Ivankovic
  * @version: $Revision: #9 $
  *
  * Last modification: $DateTime: 2017/03/15 09:17:43 $
  * Last modified by:  $Author: yong.liu $
  * 
  * .
  */

#ifndef __MODBUS_SERVICE_H__
#define __MODBUS_SERVICE_H__

#include <string>
#include <map>
#include <vector>

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/Time_Value.h"
#include "ace/os_include/netinet/os_tcp.h"
#include "ModbusInClient.h"

#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
// start, length, check value.
#define VALUE_IN_RANGE(s, l, v) ((s <= v) && v < (s + l))

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
	class ModbusInClient;

	class PointAddress
	{
	public:
		void print();
	public:
		PointAddress::PointAddress()
			:m_wordAddress(0), m_bitAddress(0)
		{
		
		}

		PointAddress::PointAddress(unsigned long	word, unsigned long	bit)
			:m_wordAddress(word), m_bitAddress(bit)
		{
		
		}
		unsigned long	m_wordAddress;
		unsigned long	m_bitAddress;
	};

	class PointValueStruct
	{
	public:
		PointAddress	m_address;
		unsigned long	m_size;		//must be one of (1, 16, 32). other values will be invalid.
		unsigned int	m_value;
		double m_Dblvalue;
	};

	typedef std::vector<PointValueStruct> PointValueVector;

	class ModbusService : public TA_Base_Core::Thread
	{
	public:
		ModbusService(std::string& port, unsigned int startWord, unsigned int length, unsigned long socketTO = 2000);
		~ModbusService();

		virtual void	run(); 
		virtual void	terminate();

		bool	getData(char* buf, unsigned int startWord, unsigned int wordCount);
		bool	setData(unsigned int wordAddress, WORD value);
		bool	setData(PointValueVector& values);
		unsigned long	getPeerSocketTimeout(){	return 	m_peersocketTimeout;}
		void 	setDataInitialized() {m_InitialisedDatas = true;}

	private:
		void socketInit();
		bool socketSelect();
		bool socketAccept();
		void addNewClient(ModbusInClient* clientConnect);
	private:
		volatile bool			m_socketInited;
		ACE_SOCK_Acceptor		m_acceptor;
		ACE_Time_Value			m_socketAcceptTimeout;
		std::string				m_servicePort;
		volatile bool			m_toTerminate;
		unsigned int			m_startWord;
		unsigned int			m_length;
		WORD*					m_datas;
		volatile bool			m_InitialisedDatas;
		unsigned long			m_peersocketTimeout;
		std::map<void*, ModbusInClient*>	m_connects;

		TA_Base_Core::ReEntrantThreadLockable m_dataLock;
		TA_Base_Core::ReEntrantThreadLockable m_connectsLock;
	};
}
#endif	//__MODBUS_SERVICE_H__

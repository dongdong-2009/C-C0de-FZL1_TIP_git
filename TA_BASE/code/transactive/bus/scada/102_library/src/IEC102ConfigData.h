#ifndef IEC102CONFIGDATA_H
#define IEC102CONFIGDATA_H
  
#include <string>  

namespace TA_Base_Bus
{  
/* 
* brief Generic configuration struct 
* 
*/  
	struct IEC102ConfigData
	{

		IEC102ConfigData(	std::string address, 
							std::string 	port, 
							unsigned short	remoteAddress=0,
							unsigned long	messageResponseTime=100000, 
							int 			sendRetryTime=1)
				:	m_address(address),
					m_port(port),
					m_remoteAddress(remoteAddress),
					m_messageResponseTime(messageResponseTime),
					m_sendRetryTime(sendRetryTime)
		{
					
		}

		IEC102ConfigData(	unsigned short	remoteAddress=0,
							unsigned long	messageResponseTime=100000, 
							int 			sendRetryTime=1)
				:	m_remoteAddress(remoteAddress),
					m_messageResponseTime(messageResponseTime),
					m_sendRetryTime(sendRetryTime)
		{
					
		}
		
		std::string 		m_address;
		std::string 		m_port;
		unsigned short 		m_remoteAddress;
		unsigned long		m_messageResponseTime;
		int					m_sendRetryTime;
	};
	
}

#endif


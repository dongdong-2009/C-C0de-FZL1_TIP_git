//////////////////////////////////////////////////////////////////////
///		@file		Agent.h
///		@author		zhilin,ouyang
///		@date		2012-08-30 14:44:41
///
///		@brief	    the message agent who intest in specific message   
///                 it contains the name, address of the subscriber.
///                           
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  AGENT_H
#define  AGENT_H
#include "AQCommonDef.h"
#include <string>

NS_BEGIN(TA_AQ_Lib)

class CAgent
{
public:
	CAgent(void);
	~CAgent(void);
	CAgent(const std::string& strName, const std::string& strAddr = "");
	CAgent(const CAgent& rsh);
	CAgent& operator=(const CAgent& rhs);

public:
	void    setName(const std::string& strName);
	std::string getName() const;

	void    setAddress(const std::string strAddr);
	std::string getAddr() const;

public:
	friend bool operator==(const CAgent& rLeft, const CAgent& rRight);

private:
	std::string          m_strName;
	std::string          m_strAddr;
};

NS_END(TA_AQ_Lib)

#endif  // AGENT_H











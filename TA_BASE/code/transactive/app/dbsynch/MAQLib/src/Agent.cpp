//////////////////////////////////////////////////////////////////////
///		@file		Agent.cpp
///		@author		zhilin,ouyang
///		@date		2012-08-30 14:44:41
///
///		@brief	    the message agent who intest in specific message   
///                 it contains the name, address of the subscriber.
///                           
///                 
//////////////////////////////////////////////////////////////////////
#include "Agent.h"

NS_BEGIN(TA_AQ_Lib)

CAgent::CAgent(void)
{

}
CAgent::~CAgent(void)
{

}

CAgent::CAgent(const std::string& strName, const std::string& strAddr)
: m_strName(strName)
, m_strAddr(strAddr)
{
}

CAgent::CAgent(const CAgent& rhs)
{
	m_strName = rhs.m_strName;
	m_strAddr = rhs.m_strAddr;
}

CAgent& CAgent::operator=(const CAgent& rhs)
{
	m_strName = rhs.m_strName;
	m_strAddr = rhs.m_strAddr;

	return *this;
}

void  CAgent::setName(const std::string& strName)
{
	m_strName = strName;
}

std::string CAgent::getName() const
{
	return m_strName;
}

void  CAgent::setAddress(const std::string strAddr)
{
	m_strAddr = strAddr;
}

std::string CAgent::getAddr() const
{
	return m_strAddr;
}

bool operator==(const CAgent& rLeft, const CAgent& rRight)
{
	return (rLeft.m_strName == rRight.m_strName &&
		    rLeft.m_strAddr == rRight.m_strAddr);
}


NS_END(TA_AQ_Lib)
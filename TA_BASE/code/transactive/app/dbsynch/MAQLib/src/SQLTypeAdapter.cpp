//////////////////////////////////////////////////////////////////////
///		@file		SQLTypeAdapter.cpp
///		@author		zhilin,ouyang
///		@date		2010-12-8 14:44:41
///
///		@brief	    to converts many different data types to strings 
///                 suitable for use in SQL queries. This class provides 
///                 implicit conversion between many C++ types and
///                 SQL-formatted string representations of that data 
///                 without losing important type information.              
//////////////////////////////////////////////////////////////////////
#include "SQLTypeAdapter.h"
#include "UtilityFun.h"
#include "core/exceptions/src/SQLException.h"
#include "core/utilities/src/DebugUtil.h"

NS_BEGIN(TA_AQ_Lib)

SQLTypeAdapter::SQLTypeAdapter(const SQLTypeAdapter& other)
: m_str(other.m_str)
{
}

SQLTypeAdapter::SQLTypeAdapter(const std::string& str)
: m_str(str)
{
}

SQLTypeAdapter::SQLTypeAdapter(const char* str)
: m_str(str)
{
}

SQLTypeAdapter::SQLTypeAdapter(char c)
: m_str(stream2string(c))
{
}

SQLTypeAdapter::SQLTypeAdapter(short i)
: m_str(stream2string(i))
{
}

SQLTypeAdapter::SQLTypeAdapter(unsigned short i)
: m_str(stream2string(i))
{
}

SQLTypeAdapter::SQLTypeAdapter(int i)
: m_str(stream2string(i))
{
}

SQLTypeAdapter::SQLTypeAdapter(unsigned i)
: m_str(stream2string(i))
{
}

SQLTypeAdapter::SQLTypeAdapter(long i)
: m_str(stream2string(i))
{
}

SQLTypeAdapter::SQLTypeAdapter(unsigned long i)
: m_str(stream2string(i))
{
}

SQLTypeAdapter::SQLTypeAdapter(long long i)
: m_str(stream2string(i))
{
}

SQLTypeAdapter::SQLTypeAdapter(ulong64 i)
: m_str(stream2string(i))
{
}

SQLTypeAdapter::SQLTypeAdapter(float i)
: m_str(stream2string(i))
{
}

SQLTypeAdapter::SQLTypeAdapter(double i)
: m_str(stream2string(i))
{
}

SQLTypeAdapter& SQLTypeAdapter::operator =(const SQLTypeAdapter& rhs)
{
	return assign(rhs);
}

SQLTypeAdapter& SQLTypeAdapter::assign(const SQLTypeAdapter& sta)
{
	m_str = sta.m_str;
	return (*this);
}

SQLTypeAdapter& SQLTypeAdapter::assign(const char* pc)
{
	m_str = pc;
	return (*this);
}

char SQLTypeAdapter::at(size_t i) const
{
	if (i < length())	
		return m_str[i];
	else		
		TA_THROW(TA_Base_Core::BadIndex("Not enough chars in SQLTypeAdapter", int(i), int(length())));
}

int SQLTypeAdapter::compare(const SQLTypeAdapter& other) const
{
	return m_str.compare(other.m_str);
}

int SQLTypeAdapter::compare(const std::string& other) const
{
	return m_str.compare(other);
}

int SQLTypeAdapter::compare(size_t pos, size_t num, std::string& other) const
{
	return m_str.compare(pos, num, other);
}

int SQLTypeAdapter::compare(const char* other) const
{
	return m_str.compare(other);
}

int SQLTypeAdapter::compare(size_t pos, size_t num, const char* other) const
{
	return m_str.compare(pos, num, other);
}


NS_END(TA_AQ_Lib)













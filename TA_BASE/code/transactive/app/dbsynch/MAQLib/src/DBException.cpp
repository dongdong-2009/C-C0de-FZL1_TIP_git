//////////////////////////////////////////////////////////////////////
///		@file		DBException.cpp
///		@author		zhilin,ouyang
///		@date		2010-11-24 14:44:41
///
///		@brief	    Exceptions which will occur in database wrapper 
///					in the database synchronization module.  
///                 
//////////////////////////////////////////////////////////////////////
#include "DBException.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDBException::CDBException( const std::string status, const std::string reason, 
                          long nativeCode ) throw()
: m_status( status )
, m_reason( reason )
, m_nativeCode( nativeCode )
{
}

CDBException::~CDBException() throw()
{
    // Do nothing. 
}

std::string CDBException::getSummary() const
{

	std::ostringstream ostr;

	ostr << "Status: " << m_status.c_str() << " Reason: " << m_reason.c_str()
		 << " NativeCode: " << m_nativeCode;
	return ostr.str();
}

std::string CDBException::getStatus() const
{
    return m_status;
}

std::string CDBException::getReason() const
{
    return m_reason;
}

long CDBException::getNativeCode() const
{
    return m_nativeCode;
}


//////////////////////////////////////////////////////////////////////
///		@file		DBException.h
///		@author		zhilin,ouyang
///		@date		2010-11-24 14:44:41
///
///		@brief	    Exceptions which will occur in database wrapper 
///					in the database synchronization module.  
///                 
//////////////////////////////////////////////////////////////////////

#ifndef DB_EXCEPTION_H
#define DB_EXCEPTION_H

#include <string>
#include <exception>

class CDBException : public std::exception
{
public:
	CDBException( const std::string status, const std::string reason, 
                     long nativeCode ) throw();
	virtual ~CDBException() throw();

	std::string getSummary() const;
	std::string getStatus() const;
	std::string getReason() const;
	long getNativeCode() const;
private:
	std::string    m_status;
	std::string    m_reason;
	long	       m_nativeCode;
};

#endif // DB_EXCEPTION_H
























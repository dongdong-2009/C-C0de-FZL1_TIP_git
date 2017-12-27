//////////////////////////////////////////////////////////////////////
///		@file		SQLException.h
///		@author		zhilin,ouyang
///		@date		2010-12-8 14:44:41
///
///		@brief	    to declare some exceptions which caused in SQL
///                 statement construct
//////////////////////////////////////////////////////////////////////
#ifndef SQLEXCEPTION_H
#define SQLEXCEPTION_H

#include "TransactiveException.h"
#include <sstream>

namespace TA_Base_Core
{

class DbSyncException : public TransactiveException
{
public:
	DbSyncException(int nErrCode, const char* msg) : 
	  TransactiveException(msg),
	  m_nErrCode(nErrCode){}
	  
	  DbSyncException(const std::string& msg) :
	  TransactiveException(msg){}

	  int getErrorCode() { return m_nErrCode; }

protected:
	int   m_nErrCode;

};

class BadIndex : public DbSyncException
{
public:
	/// \brief Create exception object
	///
	/// \param what type of object bad index tried on
	/// \param bad_index index value the container didn't like
	/// \param max_index largest legal index value for container
	explicit BadIndex(int nErrCode, const char* what, int bad_index, int max_index) :
	DbSyncException(nErrCode, what)
	{
		std::ostringstream outs;
		outs << "Index " << bad_index << " on " << what <<
			" out of range, max legal index is " << max_index;
		m_msg = outs.str();
	}

	/// \brief Destroy exception
	~BadIndex() throw() { }
};

class BadParamCount : public DbSyncException
{
public:
	/// \brief Create exception object
	explicit BadParamCount(int nErrCode, const char* w = "") :
	DbSyncException(nErrCode, w)
	{
	}

	/// \brief Destroy exception
	~BadParamCount() throw() { }
};

class FileNotExist : public DbSyncException
{
public:
	/// \brief Create exception object
	explicit FileNotExist(int nErrCode, const char* w = "") :
	DbSyncException(nErrCode, w)
	{
	}

	/// \brief Destroy exception
	~FileNotExist() throw() { }
};

class InValidPtr : public DbSyncException
{
public:
	/// \brief Create exception object
	explicit InValidPtr(int nErrCode, const char* w = "") :
	DbSyncException(nErrCode, w)
	{
	}

	/// \brief Destroy exception
	~InValidPtr() throw() { }
};

class SrvInfoNotFound : public DbSyncException
{
public:
	/// \brief Create exception object
	explicit SrvInfoNotFound(int nErrCode, const char* w = "") :
	DbSyncException(nErrCode, w)
	{
	}

	/// \brief Destroy exception
	~SrvInfoNotFound() throw() { }
};

	
}  // end of namespace TA_Base_Core


#endif // SQLEXCEPTION_H

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

class SQLCodeException : public TransactiveException
{
public:
	SQLCodeException(const char* msg) : 
	  TransactiveException(msg){}
	  
	  SQLCodeException(const std::string& msg) :
	  TransactiveException(msg){}
};

class BadIndex : public SQLCodeException
{
public:
	/// \brief Create exception object
	///
	/// \param what type of object bad index tried on
	/// \param bad_index index value the container didn't like
	/// \param max_index largest legal index value for container
	explicit BadIndex(const char* what, int bad_index, int max_index) :
	SQLCodeException(what)
	{
		std::ostringstream outs;
		outs << "Index " << bad_index << " on " << what <<
			" out of range, max legal index is " << max_index;
		m_msg = outs.str();
	}

	/// \brief Destroy exception
	~BadIndex() throw() { }
};

class BadParamCount : public SQLCodeException
{
public:
	/// \brief Create exception object
	explicit BadParamCount(const char* w = "") :
	SQLCodeException(w)
	{
	}

	/// \brief Destroy exception
	~BadParamCount() throw() { }
};

class SQLFileNotExist : public SQLCodeException
{
public:
	/// \brief Create exception object
	explicit SQLFileNotExist(const char* w = "") :
	SQLCodeException(w)
	{
	}

	/// \brief Destroy exception
	~SQLFileNotExist() throw() { }
};

class DbTypeNotSupported : public SQLCodeException
{
public:
	/// \brief Create exception object
	explicit DbTypeNotSupported(const char* what, int bad_dbType) :
	SQLCodeException(what)
	{
		std::ostringstream outs;
		outs << what << " input dbtype is: " << bad_dbType;
		m_msg = outs.str();
	}

	/// \brief Destroy exception
	~DbTypeNotSupported() throw() { }
};

	
}  // end of namespace TA_Base_Core


#endif // SQLEXCEPTION_H

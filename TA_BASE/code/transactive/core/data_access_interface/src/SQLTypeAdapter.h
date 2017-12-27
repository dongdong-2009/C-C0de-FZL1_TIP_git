//////////////////////////////////////////////////////////////////////
///		@file		SQLTypeAdapter.h
///		@author		zhilin,ouyang
///		@date		2010-11-29 14:44:41
///
///		@brief	    to converts many different data types to strings 
///                 suitable for use in SQL queries. This class provides 
///                 implicit conversion between many C++ types and
///                 SQL-formatted string representations of that data 
///                 without losing important type information.              
//////////////////////////////////////////////////////////////////////
#ifndef SQLTYPEADAPTER_H
#define SQLTYPEADAPTER_H
#include "SQLCommonDef.h"
#include "core/database/src/CommonType.h"
#include <string>

NS_BEGIN(TA_Base_Core)

class SQLTypeAdapter
{
public:
	/// \brief Default constructor; empty string
	SQLTypeAdapter();

	/// \brief Copy ctor
	/// \param other the other SQLTypeAdapter object
	SQLTypeAdapter(const SQLTypeAdapter& other);

	/// \brief Create a copy of a C++ string
	SQLTypeAdapter(const std::string& str);

	/// \brief Create a copy of a null-terminated C string
	SQLTypeAdapter(const char* str);

	/// \brief Create a single-character string
	SQLTypeAdapter(char c);

	/// \brief Create a string representation of a short int value
	SQLTypeAdapter(short i);

	/// \brief Create a string representation of an unsigned
	/// \c short int value
	SQLTypeAdapter(unsigned short i);

	/// \brief Create a string representation of an int value
	SQLTypeAdapter(int i);

	/// \brief Create a string representation of an unsigned int
	/// value
	SQLTypeAdapter(unsigned i);

	/// \brief Create a string representation of a long int value
	SQLTypeAdapter(long i);

	/// \brief Create a string representation of an unsigned
	/// \c long int value
	SQLTypeAdapter(unsigned long i);

	/// \brief Create a string representation of a 64-bit value
	SQLTypeAdapter(long long i);	

	/// \brief Create a string representation of a float value
	SQLTypeAdapter(float i);

	/// \brief Create a string representation of a double value
	SQLTypeAdapter(double i);

	/// \brief Create a string representation of a SQLStatement struct
	///
	/// \see assign(const SQLStatement&) for details
	SQLTypeAdapter(const SQLStatement& rhs);

	/// \brief Standard assignment operator
	///
	/// \see assign(const SQLTypeAdapter&) for details
	SQLTypeAdapter& operator =(const SQLTypeAdapter& rhs);

	/// \brief Return pointer to raw data buffer
	const char* data(int nIndex) const { return (enumBeginDbType == m_nDataCategory ? m_strParams[enumBeginDbType].c_str() : m_strParams[nIndex].c_str()); }

	/// \brief Copies another SQLTypeAdapter's data buffer into this
	/// object.
	///
	/// \param sta Other object to copy
	///
	/// \retval *this
	SQLTypeAdapter& assign(const SQLTypeAdapter& sta);	

	/// \brief Returns the character at a given position within the
	/// string buffer.
	char at(size_t i, size_t uIndex) const;

	/// \brief Return number of bytes in data buffer
	size_t length(size_t nIndex) const { return (enumBeginDbType == m_nDataCategory ? m_strParams[enumBeginDbType].length() : m_strParams[nIndex].length()); }
	size_t size(size_t nIndex) const { return length(nIndex); } ///< alias for length()

	bool is_null(size_t nIndex) const { return m_strParams[nIndex].empty(); }
	bool empty() const { return m_bEmpty; }

	/// \brief Compare the internal buffer to the given string
	///
	/// Works just like string::compare(const std::string&).
	int compare(const SQLTypeAdapter& other) const;
	int getDataCategory() const { return m_nDataCategory; }

	/// \brief clear the internal buffer to the given string
	///
	/// Works just like string::clear(const std::string&).
	void clear();

	/// \brief series of add functions for add data for parts of 
	///  the SQL statement
	/// Works just like string::append 
	void add(int nDynamicKey);
	void add(const std::string& strVal);
	void add(const SQLTypeAdapter& other);
	void add(const std::string& strOracle, const std::string& strMysql);
	void add(const std::string& strColunm, const std::string& strVal, int nColType, int nOperKey, bool bIsAQ = false);
	void add(const std::string& strColunm, const std::string& strVal, int nColType, const char* strOperator = "", bool bIsAQ = false);

	void add(const Row& vecVal, int nDynKey, bool bIsLast);
	void add(int nFuncKey, const std::string& strColunm, int nOperKey);
	void add(const std::string& strColunm, const std::string& strVal, int nDynaKey, 
			 int nFuncKey, const char* strOperator, bool bIsAQ = false);
	void add(const std::string& strColunm, const std::string& strVal, int nDynaKey, 
			 int nFuncKey, int nOperKey, bool bIsAQ = false);

	void add(const std::string& strColunm, int nDynaKey, int nFuncKey, bool bIsAQ = false);

	// TO_DATE('" + toTimeString + "','Mon DD HH24:MI:SS YYYY')
	void add(int nDynaKey, int nFuncKey, const std::string& strVal, bool bIsAQ);
	
	// add for PA dynamic SQL
	void add(SQLStatement& rSql, const std::string& strWhere);

private:
	/// \brief copy the internal buffer to the given arrary
	///
	/// only used in the class.
	void _CopyData(const SQLTypeAdapter& other);
	void _AddData(const SQLTypeAdapter& other);

private:
	//std::string m_str;
	bool m_bEmpty;
	int  m_nDataCategory;
	std::string m_strParams[defSupportDBCount];      // contains 3 items including: common, oracle, mysql
};

NS_END(TA_Base_Core)

#endif /// SQLTYPEADAPTER_H

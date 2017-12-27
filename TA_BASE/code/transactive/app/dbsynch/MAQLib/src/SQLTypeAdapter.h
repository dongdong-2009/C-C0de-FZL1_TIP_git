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
#include "AQCommonDef.h"
#include <string>

NS_BEGIN(TA_AQ_Lib)

class SQLTypeAdapter
{
public:
	/// \brief Default constructor; empty string
	SQLTypeAdapter(){}

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

	/// \brief Create a string representation of a unsigned 64-bit value
	SQLTypeAdapter::SQLTypeAdapter(ulong64 i);

	/// \brief Create a string representation of a float value
	SQLTypeAdapter(float i);

	/// \brief Create a string representation of a double value
	SQLTypeAdapter(double i);

	/// \brief Standard assignment operator
	///
	/// \see assign(const SQLTypeAdapter&) for details
	SQLTypeAdapter& operator =(const SQLTypeAdapter& rhs);

	/// \brief Returns a const char pointer to the object's raw data
	operator const char*() const { return m_str.c_str(); }

	/// \brief Return pointer to raw data buffer
	const char* data() const { return m_str.c_str(); }

	/// \brief Copies another SQLTypeAdapter's data buffer into this
	/// object.
	///
	/// \param sta Other object to copy
	///
	/// \retval *this
	SQLTypeAdapter& assign(const SQLTypeAdapter& sta);

	/// \brief Copies a C string or a raw buffer into this object.
	///
	/// \param pc Pointer to char buffer to copy
	/// \param len Number of characters to copy; default tells function
	/// to use the return value of strlen() instead.
	///
	/// \retval *this
	SQLTypeAdapter& assign(const char* pc);

	/// \brief Returns the character at a given position within the
	/// string buffer.
	char at(size_t i) const;

	/// \brief Return number of bytes in data buffer
	size_t length() const { return m_str.length(); }
	size_t size() const { return length(); } ///< alias for length()

	bool is_null() const { return m_str.empty(); }

	/// \brief Compare the internal buffer to the given string
	///
	/// Works just like string::compare(const std::string&).
	int compare(const SQLTypeAdapter& other) const;

	/// \brief Compare the internal buffer to the given string
	///
	/// Works just like string::compare(const std::string&).
	int compare(const std::string& other) const;

	/// \brief Compare the internal buffer to the given string
	///
	/// Works just like string::compare(size_type, size_type, 
	/// std::string&).
	int compare(size_t pos, size_t num, std::string& other) const;

	/// \brief Compare the internal buffer to the given string
	///
	/// Works just like string::compare(const char*).
	int compare(const char* other) const;

	/// \brief Compare the internal buffer to the given string
	///
	/// Works just like string::compare(size_type, size_type, 
	/// const char*).
	int compare(size_t pos, size_t num, const char* other) const;

private:
	std::string m_str;
	
};

NS_END(TA_AQ_Lib)

#endif /// SQLTYPEADAPTER_H

//////////////////////////////////////////////////////////////////////
///		@file		SQLVarParms.h
///		@author		zhilin,ouyang
///		@date		2010-11-29 14:44:41
///
///		@brief	    to hold all of the variable parameters of a SQL
///                 statement into a vector.
//////////////////////////////////////////////////////////////////////
#ifndef SQLVARPARMS_H
#define SQLVARPARMS_H
#include <vector>
#include "SQLTypeAdapter.h"

NS_BEGIN(TA_Base_Core)

class SQLVarParms : public std::vector<SQLTypeAdapter>
{
public:
	/// \default constructor
	SQLVarParms(){}

	/// \brief Clears the list
	void clear() { erase(begin(), end()); }

	/// \brief Access element number n
	SQLTypeAdapter& operator [](size_type n)
	{
		if (n >= size()) {
			insert(end(), (n + 1) - size(), "");
		}
		return std::vector<SQLTypeAdapter>::operator [](n);
	}

	/// \brief Access element number n
	const SQLTypeAdapter& operator [](size_type n) const
	{ return std::vector<SQLTypeAdapter>::operator [](n); }

	/// \brief Adds an element to the list
	SQLVarParms& operator <<(const SQLTypeAdapter& str)
	{
		push_back(str);
		return *this;
	}

	/// \brief Adds an element to the list
	SQLVarParms& operator +=(const SQLTypeAdapter& str)
	{
		push_back(str);
		return *this;
	}

	/// \brief Build a composite of two parameter lists
	///
	/// If this list is (a, b) and \c other is (c, d, e, f, g), then
	/// the returned list will be (a, b, e, f, g).  That is, all of this
	/// list's parameters are in the returned list, plus any from the
	/// other list that are in positions beyond what exist in this list.
	///
	/// If the two lists are the same length or this list is longer than
	/// the \c other list, a copy of this list is returned.
	SQLVarParms operator +(
		const SQLVarParms& other) const;

};

NS_END(TA_Base_Core)

#endif  /// SQLVARPARMS_H

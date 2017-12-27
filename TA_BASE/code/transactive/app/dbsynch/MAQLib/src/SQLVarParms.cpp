//////////////////////////////////////////////////////////////////////
///		@file		SQLVarParms.cpp
///		@author		zhilin,ouyang
///		@date		2010-11-29 14:44:41
///
///		@brief	    to hold all of the variable parameters of a SQL
///                 statement into a vector.
//////////////////////////////////////////////////////////////////////
#include "SQLVarParms.h"

NS_BEGIN(TA_AQ_Lib)

SQLVarParms SQLVarParms::operator +(const SQLVarParms& other) const
{
	if (other.size() <= size()) {
		return *this;
	}
	SQLVarParms New = *this;

	for (size_t i = size(); i < other.size(); i++) {
		New.push_back(other[i]);
	}

	return New;
}

NS_END(TA_AQ_Lib)
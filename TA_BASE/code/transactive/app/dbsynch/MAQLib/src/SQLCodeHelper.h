//////////////////////////////////////////////////////////////////////
///		@file		SQLFileHelper.h
///		@author		zhilin,ouyang
///		@date		2010-12-9 14:44:41
///
///		@brief	    to load the sql.dat file and store the SQL statements
///                 into a hash-table for later retrieve.
//////////////////////////////////////////////////////////////////////
#ifndef SQLFILEHELPER_H
#define SQLFILEHELPER_H
#include "AQCommonDef.h"
#include <string>

NS_BEGIN(TA_AQ_Lib)

class CSQLCodeHelper
{

public:
	// SQLFileHelper Constructors and destructor
	CSQLCodeHelper();
	~CSQLCodeHelper();

	/**
	* GetSQLString
	*
	* @brief: to retrieve the SQL statement from the hash-table which
	*         store the SQL statements loaded from the sql.dat file.
	*         It's match the SQL statement based on the input SQL Key
	*
	* @param  pszSQLID  the SQL statement key which map into
	*                   the sql.dat file
	* Returns the SQL statement.
	*/
	void getSQLString(int nDbType, const std::string& strSQLKey, std::string& strSQL);

private:
	int  m_nArrSize;
	
};

NS_END(TA_AQ_Lib)

#endif  /// SQLFILEHELPER_H

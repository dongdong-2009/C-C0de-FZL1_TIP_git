//////////////////////////////////////////////////////////////////////
///		@file		SQLCode.h
///		@author		zhilin,ouyang
///		@date		2010-11-29 14:44:41
///
///		@brief	    to hold all of the SQL statements used in our 
///                 system in one location. The SQLCode class is based 
///                 on the Singleton design pattern.
//////////////////////////////////////////////////////////////////////
#ifndef SQLCODE_H
#define SQLCODE_H
#include "AQCommonDef.h"
#include "SQLVarParms.h"
#include "sqlmacrodef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

NS_BEGIN(TA_AQ_Lib)

using namespace TA_Base_Core;
class CSQLCodeHelper;

class CSQLCode 
{
public:

	// get the only instance of the class
	static CSQLCode& getInstance();	
	static void removeInstance();

	/**
	* buildSQLStatement
	*
	* @brief: to build the SQL statement.
	* @param: varParms contains the SQL ID, variables in sequence.
	*
	* Returns the SQL statement if successfully otherwise empty.
	*/
	void  buildSQLStatement(const SQLVarParms& varParms, std::string& strSQL);

private:		

	inline
	void    _GetDbTypeAndSQLKey(const SQLVarParms& varParms, int& nDbType, std::string& strSQLKey);
	size_t  _GetSQLSize(const SQLVarParms& varParms, const std::string& strFormat);
	size_t  _CheckStrCount(const char* pSrc, const char* pTarget);
	void    _BuildSQL(const SQLVarParms& varParms, const std::string& strFormat, std::string& strSQL);
	void    _BuildLargeSQL(const SQLVarParms& varParms, size_t uSQLSize, const std::string& strFormat, std::string& strSQL);

private:
	CSQLCode();
	~CSQLCode();
	DEF_DISABLE_COPY_AND_ASSIGNMENT(CSQLCode);

	static CSQLCode* m_pInstance;	            // the only one instance of the class
	CSQLCodeHelper*  m_pSqlCodeHelper;          // the SQL code helper

	static NonReEntrantThreadLockable m_instanceLock;
	//ReEntrantThreadLockable  m_threadLock;

};

NS_END(TA_AQ_Lib)

#endif   /// SQLCODE_H














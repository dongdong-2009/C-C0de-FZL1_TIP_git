//////////////////////////////////////////////////////////////////////
///		@file		SQLTypeAdapter.cpp
///		@author		zhilin,ouyang
///		@date		2010-12-8 14:44:41
///
///		@brief	    to converts many different data types to strings 
///                 suitable for use in SQL queries. This class provides 
///                 implicit conversion between many C++ types and
///                 SQL-formatted string representations of that data 
///                 without losing important type information.              
//////////////////////////////////////////////////////////////////////
#include "SQLTypeAdapter.h"
#include "SQLParamsHelper.h"
#include "core/exceptions/src/SQLException.h"
#include "core/utilities/src/DebugUtil.h"

NS_BEGIN(TA_Base_Core)

static const std::string s_strFuncLeftBegin("(");
static const std::string s_strFuncRightEnd(")");
static const std::string s_strAQTag("''");
static const std::string s_strNonAQTag("'");
static const std::string s_strDelimit1(",");

SQLTypeAdapter::SQLTypeAdapter()
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(true)
{
}

SQLTypeAdapter::SQLTypeAdapter(const SQLTypeAdapter& other)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	_CopyData(other);
}

SQLTypeAdapter::SQLTypeAdapter(const std::string& str)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = str;
}

SQLTypeAdapter::SQLTypeAdapter(const char* str)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = str;
}

SQLTypeAdapter::SQLTypeAdapter(char c)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = stream2string(c);
}

SQLTypeAdapter::SQLTypeAdapter(short i)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = stream2string(i);
}

SQLTypeAdapter::SQLTypeAdapter(unsigned short i)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = stream2string(i);
}

SQLTypeAdapter::SQLTypeAdapter(int i)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = stream2string(i);
}

SQLTypeAdapter::SQLTypeAdapter(unsigned i)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = stream2string(i);
}

SQLTypeAdapter::SQLTypeAdapter(long i)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = stream2string(i);
}

SQLTypeAdapter::SQLTypeAdapter(unsigned long i)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = stream2string(i);
}

SQLTypeAdapter::SQLTypeAdapter(long long i)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = stream2string(i);
}

SQLTypeAdapter::SQLTypeAdapter(float i)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = stream2string(i);
}

SQLTypeAdapter::SQLTypeAdapter(double i)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(false)
{
	m_strParams[0] = stream2string(i);
}

SQLTypeAdapter::SQLTypeAdapter(const SQLStatement& rhs)
: m_nDataCategory(enumBeginDbType)
, m_bEmpty(true)
{
	if (rhs.strCommon.empty() && rhs.strOracle.empty() && rhs.strMySQL.empty())
	{
		m_nDataCategory = enumBeginDbType;
		return;
	}
	m_bEmpty = false;
	if (!rhs.strCommon.empty())
	{
		m_nDataCategory = enumBeginDbType;
		m_strParams[enumBeginDbType] = rhs.strCommon;
	}
	else
	{
		m_nDataCategory = enumOracleDb;
		m_strParams[enumOracleDb] = rhs.strOracle;
		m_strParams[enumMysqlDb] = rhs.strMySQL;
	}
}

SQLTypeAdapter& SQLTypeAdapter::operator =(const SQLTypeAdapter& rhs)
{
	return assign(rhs);
}



SQLTypeAdapter& SQLTypeAdapter::assign(const SQLTypeAdapter& sta)
{
	_CopyData(sta);
	return (*this);
}

char SQLTypeAdapter::at(size_t i, size_t uIndex) const
{
	if (i < length(uIndex))	
		return m_strParams[uIndex].at(i);
	else		
		TA_THROW(BadIndex("Not enough chars in SQLTypeAdapter", int(i),
			int(length(uIndex))));
}

int SQLTypeAdapter::compare(const SQLTypeAdapter& other) const
{
	int nRet = 0;
	for (int i = 0; i < defSupportDBCount; i++)
	{
		nRet = m_strParams[i].compare(other.m_strParams[i]);
		if ( nRet ) return  nRet;
	}

	return  nRet;
}

void SQLTypeAdapter::clear()
{
	m_bEmpty = true;
	m_nDataCategory = enumBeginDbType;
	m_strParams[enumBeginDbType].clear();
	m_strParams[enumOracleDb].clear();
	m_strParams[enumMysqlDb].clear();	
}

void SQLTypeAdapter::add(int nDynamicKey)
{
	if (!m_strParams[enumBeginDbType].empty())
	{
		m_strParams[enumOracleDb].append(m_strParams[enumBeginDbType]);
		m_strParams[enumMysqlDb].append(m_strParams[enumBeginDbType]);

		m_strParams[enumBeginDbType].clear();		
	}
	m_nDataCategory = enumOracleDb;
	m_bEmpty = false;

	DynamicData DynFormatData;
	CSQLParamsHelper sqlParaHelper;
	sqlParaHelper.getDynamicSQLPara(nDynamicKey, DynFormatData);

	m_strParams[enumOracleDb].append(DynFormatData.strOracl);
	m_strParams[enumMysqlDb].append(DynFormatData.strMysql);
}

void SQLTypeAdapter::add(const std::string& strVal)
{
	if ( strVal.empty() ) return;
	m_bEmpty = false;
	if (enumBeginDbType != m_nDataCategory)
	{
		m_strParams[enumOracleDb].append(strVal);
		m_strParams[enumMysqlDb].append(strVal);
	}
	else
	{
		m_strParams[enumBeginDbType].append(strVal);
	}	
}

void SQLTypeAdapter::add(const SQLTypeAdapter& other)
{
	_AddData(other);
}

void SQLTypeAdapter::add(const std::string& strOracle, const std::string& strMysql)
{
	if (strOracle.empty() || strMysql.empty()) return;
	
	if (!m_strParams[enumBeginDbType].empty())
	{
		m_strParams[enumOracleDb].append(m_strParams[enumBeginDbType]);
		m_strParams[enumMysqlDb].append(m_strParams[enumBeginDbType]);

		m_strParams[enumBeginDbType].clear();		
	}
	m_nDataCategory = enumOracleDb;
	m_bEmpty = false;

	m_strParams[enumOracleDb].append(strOracle);
	m_strParams[enumMysqlDb].append(strMysql);
}

void SQLTypeAdapter::add(const std::string& strColunm, const std::string& strVal, int nColType, int nOperKey, bool bIsAQ)
{
	std::string strOper;
	CSQLParamsHelper sqlParaHelper;
	sqlParaHelper.getOperator(nOperKey, strOper);
	add(strColunm, strVal, nColType, strOper.c_str(), bIsAQ);
}

void SQLTypeAdapter::add(const std::string& strColunm, const std::string& strVal, int nColType, const char* strOperator, bool bIsAQ)
{
	m_bEmpty = false;
	if (enumBeginDbType != m_nDataCategory)
	{
		if (bIsAQ && enumValType_STR == nColType)
		{
			m_strParams[enumOracleDb].append(strColunm + strOperator);
			m_strParams[enumMysqlDb].append(strColunm + strOperator);
			m_strParams[enumOracleDb].append(s_strAQTag + strVal + s_strAQTag);
			m_strParams[enumMysqlDb].append(s_strAQTag + strVal + s_strAQTag);
		}
		else if (enumValType_STR == nColType)
		{
			m_strParams[enumOracleDb].append(strColunm + strOperator);
			m_strParams[enumMysqlDb].append(strColunm + strOperator);
			m_strParams[enumOracleDb].append(s_strNonAQTag + strVal + s_strNonAQTag);
			m_strParams[enumMysqlDb].append(s_strNonAQTag + strVal + s_strNonAQTag);
		}
		else
		{
			m_strParams[enumOracleDb].append(strColunm + strOperator + strVal);
			m_strParams[enumMysqlDb].append(strColunm + strOperator + strVal);
		}
	}
	else
	{
		if (bIsAQ && enumValType_STR == nColType)
		{
			m_strParams[enumBeginDbType].append(strColunm + strOperator);
			m_strParams[enumBeginDbType].append(s_strAQTag + strVal + s_strAQTag);
		}
		else if (enumValType_STR == nColType)
		{
			m_strParams[enumBeginDbType].append(strColunm + strOperator);
			m_strParams[enumBeginDbType].append(s_strNonAQTag + strVal + s_strNonAQTag);
		}
		else
		{
			m_strParams[enumBeginDbType].append(strColunm + strOperator + strVal);
		}
	}
}

// used in ProfileHelper.cpp
void SQLTypeAdapter::add(const Row& vecVal, int nDynKey, bool bIsLast)
{
	if (vecVal.empty()) return;

	if (!m_strParams[enumBeginDbType].empty())
	{
		m_strParams[enumOracleDb].append(m_strParams[enumBeginDbType]);
		m_strParams[enumMysqlDb].append(m_strParams[enumBeginDbType]);

		m_strParams[enumBeginDbType].clear();		
	}
	m_nDataCategory = enumOracleDb;
	m_bEmpty = false;
	
	CSQLParamsHelper sqlParaHelper;
	DynamicData DynFormatData;
	sqlParaHelper.getDynamicSQLPara(nDynKey, DynFormatData);
	m_strParams[enumOracleDb].append(DynFormatData.strOracl);
	m_strParams[enumMysqlDb].append(DynFormatData.strMysql);

	size_t uSize = vecVal.size();
	for (size_t i = 0; i < uSize; i++)
	{
		m_strParams[enumOracleDb].append(vecVal[i]);
		m_strParams[enumMysqlDb].append(vecVal[i]);
		if (i != uSize - 1)
		{
			m_strParams[enumOracleDb].append(s_strDelimit1);
			m_strParams[enumMysqlDb].append(s_strDelimit1);
		}
	}
	m_strParams[enumOracleDb].append(s_strFuncRightEnd);
	m_strParams[enumMysqlDb].append(s_strFuncRightEnd);
	
	if (!bIsLast) 
	{		
		m_strParams[enumMysqlDb].append(s_strDelimit1);
	}	
}

void SQLTypeAdapter::add(int nFuncKey, const std::string& strColunm, int nOperKey)
{
	if (!m_strParams[enumBeginDbType].empty())
	{
		m_strParams[enumOracleDb].append(m_strParams[enumBeginDbType]);
		m_strParams[enumMysqlDb].append(m_strParams[enumBeginDbType]);

		m_strParams[enumBeginDbType].clear();		
	}
	m_nDataCategory = enumOracleDb;
	m_bEmpty = false;

	std::string strOper;
	CSQLParamsHelper sqlParaHelper;
	sqlParaHelper.getOperator(nOperKey, strOper);
	DynamicData DynFunData;
	sqlParaHelper.getDynamicSQLPara(nFuncKey, DynFunData);

	m_strParams[enumOracleDb].append(strColunm + strOper + DynFunData.strOracl);
	m_strParams[enumMysqlDb].append(strColunm + strOper + DynFunData.strMysql);
}

void SQLTypeAdapter::add(const std::string& strColunm, const std::string& strVal, int nDynaKey, 
						 int nFuncKey, const char* strOperator, bool bIsAQ)
{
	if (!m_strParams[enumBeginDbType].empty())
	{
		m_strParams[enumOracleDb].append(m_strParams[enumBeginDbType]);
		m_strParams[enumMysqlDb].append(m_strParams[enumBeginDbType]);

		m_strParams[enumBeginDbType].clear();		
	}
	m_nDataCategory = enumOracleDb;
	m_bEmpty = false;

	std::string strOper(strOperator);
	CSQLParamsHelper sqlParaHelper;
	DynamicData DynFormatData, DynFunData;
	sqlParaHelper.getDynamicSQLPara(nDynaKey, DynFormatData);
	sqlParaHelper.getDynamicSQLPara(nFuncKey, DynFunData);
	
	m_strParams[enumOracleDb].append(strColunm + strOper);
	m_strParams[enumMysqlDb].append(strColunm + strOper);
	m_strParams[enumOracleDb].append(DynFunData.strOracl + s_strFuncLeftBegin);
	m_strParams[enumMysqlDb].append(DynFunData.strMysql + s_strFuncLeftBegin);
	if (bIsAQ)
	{
		m_strParams[enumOracleDb].append(s_strAQTag + strVal + s_strAQTag);
		m_strParams[enumMysqlDb].append(s_strAQTag + strVal + s_strAQTag);	
		m_strParams[enumOracleDb].append(s_strDelimit1);
		m_strParams[enumMysqlDb].append(s_strDelimit1);
		m_strParams[enumOracleDb].append(s_strAQTag + DynFormatData.strOracl + s_strAQTag);
		m_strParams[enumMysqlDb].append(s_strAQTag + DynFormatData.strMysql + s_strAQTag);
		m_strParams[enumOracleDb].append(s_strFuncRightEnd);
		m_strParams[enumMysqlDb].append(s_strFuncRightEnd);
	}
	else
	{
		m_strParams[enumOracleDb].append(s_strNonAQTag + strVal + s_strNonAQTag);
		m_strParams[enumMysqlDb].append(s_strNonAQTag + strVal + s_strNonAQTag);	
		m_strParams[enumOracleDb].append(s_strDelimit1);
		m_strParams[enumMysqlDb].append(s_strDelimit1);
		m_strParams[enumOracleDb].append(s_strNonAQTag + DynFormatData.strOracl + s_strNonAQTag);
		m_strParams[enumMysqlDb].append(s_strNonAQTag + DynFormatData.strMysql + s_strNonAQTag);
		m_strParams[enumOracleDb].append(s_strFuncRightEnd);
		m_strParams[enumMysqlDb].append(s_strFuncRightEnd);
	}	
}

void SQLTypeAdapter::add(const std::string& strColunm, const std::string& strVal, int nDynaKey, 
						 int nFuncKey, int nOperKey, bool bIsAQ)
{
	std::string strOper;
	CSQLParamsHelper sqlParaHelper;
	sqlParaHelper.getOperator(nOperKey, strOper);
	add(strColunm, strVal, nDynaKey, nFuncKey, strOper.c_str(), bIsAQ);	
}

void SQLTypeAdapter::add(const std::string& strColunm, int nDynaKey, int nFuncKey, bool bIsAQ)
{
	if (!m_strParams[enumBeginDbType].empty())
	{
		m_strParams[enumOracleDb].append(m_strParams[enumBeginDbType]);
		m_strParams[enumMysqlDb].append(m_strParams[enumBeginDbType]);

		m_strParams[enumBeginDbType].clear();		
	}
	m_nDataCategory = enumOracleDb;
	m_bEmpty = false;

	DynamicData DynFormatData, DynFunData;
	CSQLParamsHelper sqlParaHelper;
	sqlParaHelper.getDynamicSQLPara(nDynaKey, DynFormatData);
	sqlParaHelper.getDynamicSQLPara(nFuncKey, DynFunData);

	if (bIsAQ)
	{
		m_strParams[enumOracleDb].append(DynFunData.strOracl + s_strFuncLeftBegin);
		m_strParams[enumMysqlDb].append(DynFunData.strMysql + s_strFuncLeftBegin);
		m_strParams[enumOracleDb].append(strColunm + s_strDelimit1);
		m_strParams[enumMysqlDb].append(strColunm + s_strDelimit1);
		m_strParams[enumOracleDb].append(s_strAQTag + DynFormatData.strOracl + s_strAQTag);
		m_strParams[enumMysqlDb].append(s_strAQTag + DynFormatData.strMysql + s_strAQTag);
		m_strParams[enumOracleDb].append(s_strFuncRightEnd);
		m_strParams[enumMysqlDb].append(s_strFuncRightEnd);
	}
	else
	{
		m_strParams[enumOracleDb].append(DynFunData.strOracl + s_strFuncLeftBegin);
		m_strParams[enumMysqlDb].append(DynFunData.strMysql + s_strFuncLeftBegin);
		m_strParams[enumOracleDb].append(strColunm + s_strDelimit1);
		m_strParams[enumMysqlDb].append(strColunm + s_strDelimit1);
		m_strParams[enumOracleDb].append(s_strNonAQTag + DynFormatData.strOracl + s_strNonAQTag);
		m_strParams[enumMysqlDb].append(s_strNonAQTag + DynFormatData.strMysql + s_strNonAQTag);
		m_strParams[enumOracleDb].append(s_strFuncRightEnd);
		m_strParams[enumMysqlDb].append(s_strFuncRightEnd);
	}
}

void  SQLTypeAdapter::add(int nDynaKey, int nFuncKey, const std::string& strVal, bool bIsAQ)
{
	if (!m_strParams[enumBeginDbType].empty())
	{
		m_strParams[enumOracleDb].append(m_strParams[enumBeginDbType]);
		m_strParams[enumMysqlDb].append(m_strParams[enumBeginDbType]);

		m_strParams[enumBeginDbType].clear();		
	}
	m_nDataCategory = enumOracleDb;
	m_bEmpty = false;

	DynamicData DynFormatData, DynFunData;
	CSQLParamsHelper sqlParaHelper;
	sqlParaHelper.getDynamicSQLPara(nDynaKey, DynFormatData);
	sqlParaHelper.getDynamicSQLPara(nFuncKey, DynFunData);

	if (bIsAQ)
	{
		m_strParams[enumOracleDb].append(DynFunData.strOracl + s_strFuncLeftBegin);
		m_strParams[enumMysqlDb].append(DynFunData.strMysql + s_strFuncLeftBegin);
		m_strParams[enumOracleDb].append(s_strAQTag + strVal + s_strAQTag);
		m_strParams[enumMysqlDb].append(s_strAQTag + strVal + s_strAQTag);
		m_strParams[enumOracleDb].append(s_strDelimit1);
		m_strParams[enumMysqlDb].append(s_strDelimit1);
		m_strParams[enumOracleDb].append(s_strAQTag + DynFormatData.strOracl + s_strAQTag);
		m_strParams[enumMysqlDb].append(s_strAQTag + DynFormatData.strMysql + s_strAQTag);
		m_strParams[enumOracleDb].append(s_strFuncRightEnd);
		m_strParams[enumMysqlDb].append(s_strFuncRightEnd);
	}
	else
	{
		m_strParams[enumOracleDb].append(DynFunData.strOracl + s_strFuncLeftBegin);
		m_strParams[enumMysqlDb].append(DynFunData.strMysql + s_strFuncLeftBegin);
		m_strParams[enumOracleDb].append(s_strNonAQTag + strVal + s_strNonAQTag);
		m_strParams[enumMysqlDb].append(s_strNonAQTag + strVal + s_strNonAQTag);
		m_strParams[enumOracleDb].append(s_strDelimit1);
		m_strParams[enumMysqlDb].append(s_strDelimit1);
		m_strParams[enumOracleDb].append(s_strNonAQTag + DynFormatData.strOracl + s_strNonAQTag);
		m_strParams[enumMysqlDb].append(s_strNonAQTag + DynFormatData.strMysql + s_strNonAQTag);
		m_strParams[enumOracleDb].append(s_strFuncRightEnd);
		m_strParams[enumMysqlDb].append(s_strFuncRightEnd);
	}
}

void SQLTypeAdapter::add(SQLStatement& rSql, const std::string& strWhere)
{
	if (!rSql.strCommon.empty())
	{
		rSql.strCommon.append(strWhere);
	}
	else
	{
		rSql.strOracle.append(strWhere);
		rSql.strMySQL.append(strWhere);
	}
}

void  SQLTypeAdapter::_CopyData(const SQLTypeAdapter& other)
{
	m_bEmpty = other.m_bEmpty;
	m_nDataCategory = other.m_nDataCategory;
	for (int i = 0; i < defSupportDBCount; i++)
	{
		m_strParams[i] = other.m_strParams[i];
	}
}

void SQLTypeAdapter::_AddData(const SQLTypeAdapter& other)
{
	if (other.empty()) return;

	m_bEmpty = false;
	if (enumBeginDbType != other.getDataCategory())
	{
		if (enumBeginDbType == m_nDataCategory)
		{
			m_strParams[enumOracleDb].append(m_strParams[enumBeginDbType]);
			m_strParams[enumMysqlDb].append(m_strParams[enumBeginDbType]);

			m_strParams[enumBeginDbType].clear();
			m_nDataCategory = enumOracleDb;
		}		

		m_strParams[enumOracleDb].append(other.data(enumOracleDb));
		m_strParams[enumMysqlDb].append(other.data(enumMysqlDb));
	}
	else
	{
		if (enumBeginDbType != m_nDataCategory)
		{
			m_strParams[enumOracleDb].append(other.data(enumBeginDbType));
			m_strParams[enumMysqlDb].append(other.data(enumBeginDbType));
		}
		else
		{
			m_strParams[enumBeginDbType].append(other.data(enumBeginDbType));
		}		
	}
}


NS_END(TA_Base_Core)
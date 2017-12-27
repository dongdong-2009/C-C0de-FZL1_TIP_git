//////////////////////////////////////////////////////////////////////
///		@file		SQLParamsHelper.cpp
///		@author		zhilin,ouyang
///		@date		2012-12-11 14:44:41
///
///		@brief	    the implementation of class CSQLParamsHelper to help 
///                 class SQLTypeAdapter to accept dynamic  parameters  
///                 between different database such as datetime format 
///                 different between Oracle & MySQL.            
//////////////////////////////////////////////////////////////////////
#include "SQLParamsHelper.h"

NS_BEGIN(TA_Base_Core)

struct stOperator
{
	int           nOperKey;
	const char*   pczOperator;
};

struct stDynamicParaData 
{
	int              nKey;
	const char*      pczOracle;
	const char*      pczMysql;
};

static const stOperator s_stOperator[] =
{
	//operator Key                   //the operator
	{enumOperKey_EQUAL,               "="  },
	{enumOperKey_NOTEQU,              "!=" },
	{enumOperKey_ABOVE,               ">"  },
	{enumOperKey_BELOW,               "<"  },
	{enumOperKey_GETHAN,              ">=" },
	{enumOperKey_LETHAN,              "<=" }
};

static const stDynamicParaData s_stParaData[] =
{
	//EDynamicSQLKey                 //the format of Oracle                      //the format of MySQL
	{enumDateFormat_1,              "dd/mm/yyyy hh24:mi:ss.ff3",                 "%d/%m/%Y %H:%i:%S.%f"},
	{enumDateFormat_2,              "dd/mm/yyyy hh24:mi:ss",                     "%d/%m/%Y %H:%i:%S"},
	{enumDateFormat_3,              "DDMMYYYYHH24MISS",                          "%d%m%Y%H%i%S"	},
	{enumDateFormat_4,              "YYYYMMDDHH24MISS",                          "%Y%m%d%H%i%S" },
	{enumDateFormat_5,              "Mon DD HH24:MI:SS YYYY",                    "%b %d %H:%i:%S %Y" },
	{enumDateFormat_6,              "YYYYMMDD",                                  "%Y%m%d" },
	{enumDateFormat_7,              "hh:mi:ss dd/mm/yyyy",                       "%H:%i:%S %d/%m/%Y" },
	{enumDateFormat_8,              "DD/MM/YYYY",                                "%d/%m/%Y" },

	{enumFuncFormat_1,              "TO_TIMESTAMP",                              "STR_TO_DATE" },
	{enumFuncFormat_2,              "TO_DATE",                                   "STR_TO_DATE" },
	{enumFuncFormat_3,              "SYSDATE",                                   "SYSDATE()" },
	{enumFuncFormat_4,              "INTERVAL '1' DAY",                          "INTERVAL 1 DAY" },
	{enumFuncFormat_5,              "INTERVAL '1' Second",                       "INTERVAL 1 Second" },
	{enumFuncFormat_6,              "-",                                         "DATE_SUB" },
	{enumFuncFormat_7,              "+",                                         "DATE_ADD" },
	{enumFuncFormat_8,              "TO_TIMESTAMP",                              "DATE_FORMAT" },
	{enumFuncFormat_9,              "SYSDATE - INTERVAL '1' DAY",                "DATE_SUB( SYSDATE() , INTERVAL 1 DAY)" },
	{enumFuncFormat_10,             "TO_CHAR",                                   "DATE_FORMAT" },
	{enumFuncFormat_11,             "NVL",                                       "IF" },
	{enumFuncFormat_12,             "TO_CHAR(NVL",                               "DATE_FORMAT(IF" },

	{enumSyntax_Format_1,           " and rownum < ",                            " LIMIT " },
	{enumSyntax_Format_4,           " rownum < ",								 " LIMIT " },
	{enumSyntax_Format_2,			" into se_profile_location (SEPROF_ID,LOCATIONKEY) values (", "(" },
	{enumSyntax_Format_3,           "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS') as DATE_MODIFIED",
									"DATE_FORMAT(IF(DATE_MODIFIED IS NULL,STR_TO_DATE('12:00:00 01/01/1990','%H:%i:%S %d/%m/%Y'),DATE_MODIFIED),'%Y%m%d%H%i%S') as DATE_MODIFIED"},
	{enumSyntax_Format_5,           "CONDITION",                                  "`CONDITION`" }
};

void  CSQLParamsHelper::getOperator(int nKey, std::string& strVal)
{
	const stOperator* s_pOperator = s_stOperator;
	while (s_pOperator->nOperKey != enumOperKey_End)
	{
		if (s_pOperator->nOperKey == nKey)
		{
			strVal = s_pOperator->pczOperator;

			break;
		}
		s_pOperator++;
	}
}

void  CSQLParamsHelper::getDynamicSQLPara(int nKey, DynamicData& rData)
{
	const stDynamicParaData* s_pParaData = s_stParaData;
	while (s_pParaData->nKey != DynamicSQLKeyLast)
	{
		if (s_pParaData->nKey == nKey)
		{
			rData.strOracl = s_pParaData->pczOracle;
			rData.strMysql = s_pParaData->pczMysql;

			break;
		}
		s_pParaData++;
	}
}

NS_END(TA_Base_Core)

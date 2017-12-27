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


#include <iostream>
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <functional>


class CFileAllClass;
class ISystemList;
class ISystemListNode;
class CTableAllClass;
class CDbAllClass;




class sqlgen
{
public:
	// SQLFileHelper Constructors and destructor
	sqlgen();
	~sqlgen();	


private:	
	typedef std::map<std::string, std::string>				mapSqlIdLineConT;
	typedef std::map<std::string, std::string>::iterator    mapSqlIdLineConIterT;
	typedef std::map<std::string, std::string>::value_type	mapSqlIdLineConValueT;

	typedef std::vector<std::string>						VrtLinesConT;	
	typedef std::vector<std::string>::iterator				VrtLinesConIterT;
	
	

//=======================================================================
	
private:
	CFileAllClass				*m_pFileAllClass;
		
	VrtLinesConT				m_vrt_AllLinesInFile;
	VrtLinesConT				m_vrt_WriteTo_SQLH;
	VrtLinesConT				m_vrt_WriteTo_SQLMAPDEFH;

	
private:
	std::string                 m_strSrcFileName;
	std::ofstream				m_OfstreamErrorReport;   //      "./ErrorReport.log"	 /*OUT*/

private:
	std::ofstream				m_pri_Ofstream_SQLH;		//defSQLFileName_Out_db        "./sql.h"			 /*OUT*/
	std::ofstream				m_pri_Ofstream_SQLMAPDEFH;   // defSQLFileName_Out_Index     "./sqlmapdef.h"	 /*OUT*/
	int							m_pri_nDefineIDIndex;
private:
	mapSqlIdLineConT            m_mapAQArray;
private:
	int							m_nIndexTable;	

private:
	int							_ReadAllLineFromFile(const std::string& strFileName, VrtLinesConT& vrtAllLines);
	int							_CheckAllLineFromFileRuleSample(VrtLinesConT& vrtAllLines);
	int							_RemoveCommentInAllLine(VrtLinesConT& vrtAllLines);	 
	int							_TurnToOneLineInAllLine(VrtLinesConT& vrtAllLines);	
	int							_RemoveBlackLineInAllLine(VrtLinesConT& vrtAllLines);	
	
	int							_AnalyzeAllLineToClass(VrtLinesConT& vrtAllLines, CFileAllClass **ppFileAllClass);	
	int							_AnalyzeFileAllClassToVrt(CFileAllClass *pFileAllClass, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH);		
	int							_CheckAllClassRuleSameSize( CFileAllClass *pFileAllClass);
	
	
private:	
	int							_RemoveUnVisableCharExceptBlackChar(std::string& strSrc);
	int							_do_job();
	int							_LOG(const std::string& strLog);
	
	int							_ProcessOneTable(VrtLinesConT& VtrOneTable, ISystemList *pList);  
	int							_ProcessOneTableOneDb(VrtLinesConT& VtrOneDb, ISystemList *pList);
	int							_ProcessOneTableOneDbOneSql(VrtLinesConT& VtrOneSql, ISystemList *pList);
	
private:	
	int							_OpenAllOutPutFiles(); 	
	int							_WriteBeginToFiles();
	int							_WriteDataToFiles(VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH);
	
	int							_WriteAQArrayBeginToFile();			
	int							_WriteAQArrayDataToFile(mapSqlIdLineConT& mapAQArray);
	int							_WriteAQArrayEndToFile();
	
	int							_WriteEndToFiles();
	int							_ClossAllOutPutFiles();
private:	
	int							_AnalyzeTableCommonList ( const std::string& strTable, ISystemListNode *pListNodeComon, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH );
	int							_AnalyzeTableOracleMysqlList ( const std::string& strTable, ISystemListNode *pListNodeOracle, ISystemListNode *pListNodeMysql, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH );
	int							_AnalyzeTableAllClassToVrt_TypeCommon( CTableAllClass *pTableAllClass, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH );
	int							_AnalyzeTableAllClassToVrt_TypeCommonOracleMysql( CTableAllClass *pTableAllClass, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH );
	int							_AnalyzeTableAllClassToVrt_TypeOracleMysql( CTableAllClass *pTableAllClass, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH );
	int							_AnalyzeTableAllClassToVrt( CTableAllClass *pTableAllClass, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH );
	int							_GetMysqlSqlLineWithSameSqlID ( const std::string& strSqlIDOrale, ISystemListNode *pListNodeMysql, std::string& strMysqlSqlLine);
private:
	int							_CheckAndGetAQArray ( const std::string& strSQLID, const std::string& strSQLLine, mapSqlIdLineConT& mapAQArray );
	int							_SystemStricmp(const char* paramValue1, const char* paramValue2);
	int							_CheckCTableAllClassRuleSameSize( CTableAllClass *pTableAllClass);
	int							_WriteToLogErrorFile_CTableAllClass( CTableAllClass *pErrorTableAllClass);
	int							_WriteToLogErrorFile_CDbAllClass( CDbAllClass *pErrorDbAllClass);
	
private:
	std::string					_GetIndexTableName(const std::string& strTableName);
	int							_Check_TablesCount(CFileAllClass *m_pFileAllClass);

};

//NS_END(TA_Base_Core)

#endif  /// SQLFILEHELPER_H






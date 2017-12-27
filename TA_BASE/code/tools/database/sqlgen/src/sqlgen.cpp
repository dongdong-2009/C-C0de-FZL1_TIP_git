
#include "sqlgen.h"

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <functional>



#include "CommonDef.h"
#include "CommonData.h"
#include "UtilityFun.h"
#include "CAllDataClass.h"

#include "ISystemList.h"
#include "ISystemListImp.h"

#include "ISystemListNode.h"
#include "ISystemListNodeImp.h"

#include "ISystemListNodeData.h"
#include "ISystemLIstNodeDataImp.h"




sqlgen::sqlgen()
{


	m_nIndexTable = 0;
	m_strSrcFileName =	 defSQLFileName;
	m_pFileAllClass = NULL;
	m_vrt_AllLinesInFile.clear();
	m_pri_nDefineIDIndex = 0;

   _do_job();

}

sqlgen::~sqlgen()
{
	if (this->m_OfstreamErrorReport.is_open())
	{
		this->m_OfstreamErrorReport.close();
	}
	
	if (NULL != m_pFileAllClass)
	{
		delete m_pFileAllClass;
		m_pFileAllClass = NULL;
	}
	
	m_vrt_AllLinesInFile.clear();
	
	m_vrt_WriteTo_SQLH.clear();
	m_vrt_WriteTo_SQLMAPDEFH.clear();
	m_mapAQArray.clear();
	
}
int sqlgen::_do_job()
{
	int nFunRet = 0;
	std::string strLogError;
	
	remove(defSQLFileName_Out_db);
	remove(defSQLFileName_Out_Index);
	remove(defSQLFilename_ErrorReport);
	
	
	m_OfstreamErrorReport.open(defSQLFilename_ErrorReport);
	if (false == this->m_OfstreamErrorReport.is_open())
	{
		nFunRet = -1;
		return nFunRet;
	}
	
	nFunRet =  _ReadAllLineFromFile(m_strSrcFileName, m_vrt_AllLinesInFile);
	if (0 != nFunRet)
	{
		strLogError = "error! _ReadAllLineFromFile()";
		nFunRet = -1;
		return nFunRet;
	}

	nFunRet =  _CheckAllLineFromFileRuleSample(m_vrt_AllLinesInFile);
	if (0 != nFunRet)
	{
		strLogError = "error! _CheckAllLineFromFileRuleSample()";
		nFunRet = -1;
		return nFunRet;
	}
	nFunRet =  _RemoveCommentInAllLine(m_vrt_AllLinesInFile);
	if (0 != nFunRet)
	{
		strLogError = "error! _RemoveCommentInAllLine()";
		nFunRet = -1;
		return nFunRet;
	}
	nFunRet =  _TurnToOneLineInAllLine(m_vrt_AllLinesInFile);
	if (0 != nFunRet)
	{
		strLogError = "error! _TurnToOneLineInAllLine()";
		nFunRet = -1;
		return nFunRet;
	}
	nFunRet =  _RemoveBlackLineInAllLine(m_vrt_AllLinesInFile);
	if (0 != nFunRet)
	{
		strLogError = "error! _RemoveBlackLineInAllLine()";
		nFunRet = -1;
		return nFunRet;
	}


	nFunRet =  _AnalyzeAllLineToClass(m_vrt_AllLinesInFile, &m_pFileAllClass);
	if (0 != nFunRet)
	{
		strLogError = "error! _AnalyzeAllLineToClass()";
		nFunRet = -1;
		return nFunRet;
	}
	nFunRet = _CheckAllClassRuleSameSize(m_pFileAllClass);
	if (0 != nFunRet)
	{
		strLogError = "error! _CheckAllClassRuleSameSize()";
		nFunRet = -1;
		return nFunRet;
	}
	

	nFunRet = _AnalyzeFileAllClassToVrt(m_pFileAllClass, m_vrt_WriteTo_SQLH, m_vrt_WriteTo_SQLMAPDEFH);
	if (0 != nFunRet)
	{
		strLogError = "error! _AnalyzeFileAllClassToVrt()";
		nFunRet = -1;
		return nFunRet;
	}
	
	nFunRet = _Check_TablesCount(m_pFileAllClass);
	if (0 != nFunRet)
	{
		strLogError = "error! _Check_TablesCount()";
		nFunRet = -1;
		return nFunRet;
	}

	nFunRet = _OpenAllOutPutFiles();
	if (0 != nFunRet)
	{
		strLogError = "error! _OpenAllOutPutFiles()";
		nFunRet = -1;
		return nFunRet;
	}
	nFunRet = _WriteBeginToFiles();
	if (0 != nFunRet)
	{
		strLogError = "error! _WriteBeginToFiles()";
		nFunRet = -1;
		return nFunRet;
	}
	nFunRet = _WriteDataToFiles(m_vrt_WriteTo_SQLH, m_vrt_WriteTo_SQLMAPDEFH);
	if (0 != nFunRet)
	{
		strLogError = "error! _WriteDataToFiles()";
		nFunRet = -1;
		return nFunRet;
	}
#if 0	
	nFunRet	= _WriteAQArrayBeginToFile();	
	if (0 != nFunRet)
	{
		strLogError = "error! _WriteAQArrayBeginToFile()";
		nFunRet = -1;
		return nFunRet;
	}		
	nFunRet	= _WriteAQArrayDataToFile(m_mapAQArray);
	if (0 != nFunRet)
	{
		strLogError = "error! _WriteAQArrayDataToFile()";
		nFunRet = -1;
		return nFunRet;
	}
	nFunRet	= _WriteAQArrayEndToFile();
	if (0 != nFunRet)
	{
		strLogError = "error! _WriteAQArrayEndToFile()";
		nFunRet = -1;
		return nFunRet;
	}
#endif


	nFunRet = _WriteEndToFiles();
	if (0 != nFunRet)
	{
		strLogError = "error! _WriteEndToFiles()";
		nFunRet = -1;
		return nFunRet;
	}
	nFunRet = _ClossAllOutPutFiles();
	if (0 != nFunRet)
	{
		strLogError = "error! _ClossAllOutPutFiles()";
		nFunRet = -1;
		return nFunRet;
	}	
	
	
	
	


	if (NULL != m_pFileAllClass)
	{
		delete m_pFileAllClass;
		m_pFileAllClass = NULL;
	}
	m_vrt_AllLinesInFile.clear();
	m_vrt_WriteTo_SQLH.clear();
	m_vrt_WriteTo_SQLMAPDEFH.clear();
	m_mapAQArray.clear();

	return nFunRet;

}
int sqlgen::_ReadAllLineFromFile( const std::string& strFileName, VrtLinesConT& vrtAllLines )
{
	int nFunRet = 0;
	std::ifstream IfstreamSrcFile;

	char inBuf[MAX_SQLSTRING_LEN_NUM] = {0};
	int nFilelines = 0;


	if (strFileName.empty())
	{
		nFunRet = -1;
		return nFunRet;
	}
	
	IfstreamSrcFile.open(strFileName.c_str());
	if (IfstreamSrcFile.fail())
	{
		nFunRet = -1;
		return nFunRet;
	}
	
	
	// Read in all lines and add them to a vector
	while ( IfstreamSrcFile.getline ( inBuf, MAX_SQLSTRING_LEN_NUM ) )
	{
		nFilelines++;
		char * crPtr = strchr( inBuf, LINE_END );
		if( 0 != crPtr )
		{
			*crPtr = '\0';
		}
		if (LINE_COMMONT != inBuf[0])
		{
			std::string line = std::string(inBuf);
			this->_RemoveUnVisableCharExceptBlackChar(line);// remove /r/n
			if (line.size() > 0)
			{
				vrtAllLines.push_back( line );
			}	
		}
	}//while

	// close file
	if (IfstreamSrcFile.is_open())
	{
		IfstreamSrcFile.close();
	}

	nFunRet = 0;
	return nFunRet;

}

int sqlgen::_CheckAllLineFromFileRuleSample( VrtLinesConT& vrtAllLines )
{
	int nFunRet = 0;
	
	std::string::size_type		nFindTmp;
	VrtLinesConIterT			iterAllLine;

	std::string					strLog;
	int							nLineIndex = 1;
	char						szTmp[defBuffSize];
	std::string					strOneLineTmp;

	iterAllLine = vrtAllLines.begin();
	nLineIndex = 1;
	nFindTmp = std::string::npos;
	while ( iterAllLine != vrtAllLines.end() )
	{
		strOneLineTmp.clear();
		strOneLineTmp = *iterAllLine;
	
		trim(strOneLineTmp);

		if (!strOneLineTmp.empty())  
		{

			if (std::string::npos == strOneLineTmp.find(";")
				&& std::string::npos == strOneLineTmp.find("+")
				&& std::string::npos == strOneLineTmp.find("=")
				&& std::string::npos == strOneLineTmp.find("[")
				&& std::string::npos == strOneLineTmp.find("[[")
				&& std::string::npos == strOneLineTmp.find("AQ"))
			{
				strLog.clear();
				memset(szTmp, 0, defBuffSize);
				sprintf(szTmp, "%d", nLineIndex);

				strLog = "error! nLineIndex=";
				strLog += szTmp;
				strLog += "    LineStr=";
				strLog += (*iterAllLine);

				_LOG(strLog);
				strLog.clear();

				nFunRet = -1;
			}//if 

		}
		else
		{
			//""
			//(*iterAllLine) = "";

		}
		iterAllLine++;
		nLineIndex++;
	}//while



	return nFunRet;
}


int  sqlgen::_SystemStricmp(const char* paramValue1, const char* paramValue2)
{
	int iRet =-1;
	char ch1 = *paramValue1;
	char ch2 = *paramValue2;

	if (NULL == paramValue1 || NULL == paramValue2)
	{
		iRet = -1;
		return -1;
	}
	do 
	{
		ch1 = *paramValue1;
		ch2 = *paramValue2;
		if( ch1 <= 'Z' && ch1 >= 'A' ) 
		{
			ch1 += 32;
		}
		if( ch2 <= 'Z' && ch2 >= 'A' )
		{
			ch2 += 32;
		}

		iRet = ch1 - ch2;

		if( (0 == iRet) && (ch1 != 0) && (ch2 != 0) )
		{
			paramValue1 ++;
			paramValue2 ++;
		}
		else 
		{
			break;
		}
	} while ( !iRet );


	return iRet;
}




int sqlgen::_WriteToLogErrorFile_CDbAllClass( CDbAllClass *pErrorDbAllClass)
{
	int nFunRet = 0;

	ISystemList					*pSqlAllClassList = NULL;
	int							nSqlAllClassListCount = 0;
	ISystemListNode				*pSqlAllClassListNode = NULL;
	ISystemListNodeData			*pSqlAllClassListNodeData = NULL;
	enumDataType                SqlAllClassListNodeDataType = DataType_NULL; 
	CSqlAllClass                 *pSqlAllClass = NULL;

	if (NULL == pErrorDbAllClass || NULL == pErrorDbAllClass->m_pSqlAllClassList)
	{
		nFunRet = -1;
		return nFunRet;
	}
	_LOG(pErrorDbAllClass->m_strDbName);
	pSqlAllClassList = pErrorDbAllClass->m_pSqlAllClassList;
	nSqlAllClassListCount = pSqlAllClassList->getNodesCount();

	for (int nIndex = 0; nIndex < nSqlAllClassListCount; nIndex++)
	{
		pSqlAllClassListNode = pSqlAllClassList->getSystemListNodeByIndex(nIndex);
		pSqlAllClassListNodeData = pSqlAllClassListNode->getNodeData();
		SqlAllClassListNodeDataType = pSqlAllClassListNodeData->getDataType();
		if (DataType_CSqlAllClass == SqlAllClassListNodeDataType)
		{
			pSqlAllClass = (CSqlAllClass *)pSqlAllClassListNodeData->getData();	
			_LOG(pSqlAllClass->m_strSqlId);	
		}

	}//for

	return nFunRet;		
}





int sqlgen::_WriteToLogErrorFile_CTableAllClass( CTableAllClass *pErrorTableAllClass)
{
	int nFunRet = 0;

	ISystemList					*pDbAllClassList = NULL;
	int							nDbAllClassListCount = 0;
	ISystemListNode				*pDbAllClassListNode = NULL;
	ISystemListNodeData			*pDbAllClassListNodeData = NULL;
	enumDataType                DbAllClassListNodeDataType = DataType_NULL; 
	CDbAllClass                 *pDbAllClass = NULL;
	

	if (NULL == pErrorTableAllClass || NULL == pErrorTableAllClass->m_pDbAllClassList)
	{
		nFunRet = -1;
		return nFunRet;
	}
	_LOG(pErrorTableAllClass->m_strTableName);
	pDbAllClassList = pErrorTableAllClass->m_pDbAllClassList;
	nDbAllClassListCount = pDbAllClassList->getNodesCount();

	for (int nIndex = 0; nIndex < nDbAllClassListCount; nIndex++)
	{
		pDbAllClassListNode = pDbAllClassList->getSystemListNodeByIndex(nIndex);
		pDbAllClassListNodeData = pDbAllClassListNode->getNodeData();
		DbAllClassListNodeDataType = pDbAllClassListNodeData->getDataType();
		if (DataType_CDbAllClass == DbAllClassListNodeDataType)
		{
			pDbAllClass = (CDbAllClass *)pDbAllClassListNodeData->getData();		
			_WriteToLogErrorFile_CDbAllClass(pDbAllClass);
		}

	}//for

	


	return nFunRet;		
}


int sqlgen::_CheckCTableAllClassRuleSameSize( CTableAllClass *pTableAllClass)
{
	int nFunRet = 0;
	int						nOralceSqlCount = 0;
	int						nMysqlSqlCount = 0;
	std::string             strDbOracle;
	std::string             strDbMysql;
	
	ISystemList					*pDbAllClassList = NULL;
	int							nDbAllClassListCount = 0;
	ISystemListNode				*pDbAllClassListNode = NULL;
	ISystemListNodeData			*pDbAllClassListNodeData = NULL;
	enumDataType                DbAllClassListNodeDataType = DataType_NULL; 
	CDbAllClass                 *pDbAllClass = NULL;
	
	if (NULL == pTableAllClass || NULL == pTableAllClass->m_pDbAllClassList)
	{
		nFunRet = -1;
		return nFunRet;
	}
	
	pDbAllClassList = pTableAllClass->m_pDbAllClassList;
	nDbAllClassListCount = pDbAllClassList->getNodesCount();
	strDbOracle = DB_ORACLE;
	strDbMysql = DB_MYSQL;
	nOralceSqlCount = 0;
	nMysqlSqlCount = 0;
	for (int nIndex = 0; nIndex < nDbAllClassListCount; nIndex++)
	{
		pDbAllClassListNode = pDbAllClassList->getSystemListNodeByIndex(nIndex);
		pDbAllClassListNodeData = pDbAllClassListNode->getNodeData();
		DbAllClassListNodeDataType = pDbAllClassListNodeData->getDataType();
		if (DataType_CDbAllClass == DbAllClassListNodeDataType)
		{
			pDbAllClass = (CDbAllClass *)pDbAllClassListNodeData->getData();
			
			if (0 == _SystemStricmp(pDbAllClass->m_strDbName.c_str(), strDbOracle.c_str()))
			{
			   nOralceSqlCount = pDbAllClass->m_pSqlAllClassList->getNodesCount();
			}
			else if (0 == _SystemStricmp(pDbAllClass->m_strDbName.c_str(), strDbMysql.c_str()))
			{
				nMysqlSqlCount = pDbAllClass->m_pSqlAllClassList->getNodesCount();
			}
			
			
		}
	
	}//for
	
	if (nOralceSqlCount == nMysqlSqlCount)
	{
		nFunRet = 0;
	}
	else
	{
	    nFunRet = -1;
	    std::string strLogError;
	    strLogError = "error!" + pTableAllClass->m_strTableName;
	    _LOG(strLogError);
		strLogError = "error! nOralceSqlCount != nMysqlSqlCount";	    
		_LOG(strLogError);
	    
	}
	

	return nFunRet;
}
int sqlgen::_CheckAllClassRuleSameSize( CFileAllClass *pFileAllClass)
{
	int nFunRet = 0;
	ISystemList					*pTableAllClassList = NULL;
	int							nTableAllClassListCount = 0;
	ISystemListNode				*pTableAllClassListNode = NULL;
	ISystemListNodeData			*pTableAllClassListNodeData = NULL;
	enumDataType                TableAllClassListNodeDataType = DataType_NULL; 
	CTableAllClass                 *pTableAllClass = NULL;
	
	int							nDbAllClassListCount = 0;
	
	if (NULL == pFileAllClass || NULL == pFileAllClass->m_pTableAllClassList)
	{
		nFunRet = -1;
		return nFunRet;
	}
	
	pTableAllClassList = pFileAllClass->m_pTableAllClassList;
	nTableAllClassListCount = pTableAllClassList->getNodesCount();
	for (int nIndex = 0; nIndex < nTableAllClassListCount; nIndex++)
	{
		nDbAllClassListCount = 0;
		pTableAllClassListNode = pTableAllClassList->getSystemListNodeByIndex(nIndex);
		pTableAllClassListNodeData = pTableAllClassListNode->getNodeData();
		TableAllClassListNodeDataType = pTableAllClassListNodeData->getDataType();
		if (DataType_CTableAllClass == TableAllClassListNodeDataType)
		{
			pTableAllClass = (CTableAllClass *)pTableAllClassListNodeData->getData();
			nDbAllClassListCount = pTableAllClass->m_pDbAllClassList->getNodesCount();
		}
		
		if (1 < nDbAllClassListCount)
		{
			nFunRet = _CheckCTableAllClassRuleSameSize(pTableAllClass);
			if (0 != nFunRet)
			{
				std::string strLOGError = "error! _CheckCTableAllClassRuleSameSize()";
				_LOG(strLOGError);
				_WriteToLogErrorFile_CTableAllClass(pTableAllClass);
				nFunRet = -1;
				return nFunRet; 				
			}
		}
		

	}  //for

	
	
	
	
	
	
	
	
	return nFunRet;
}
int sqlgen::_RemoveCommentInAllLine( VrtLinesConT& vrtAllLines )
{
   int nFunRet = 0;
   std::string::size_type nFindTmp = std::string::npos;
	VrtLinesConIterT iterAllLine;

	iterAllLine = vrtAllLines.begin();

	while ( iterAllLine != vrtAllLines.end() )
	{			
		trim(*iterAllLine);

		if (!(*iterAllLine).empty())  
		{		
			if (';' == (*iterAllLine)[0])
			{
				(*iterAllLine) = "";
				iterAllLine++;
				continue;
			}	
		}
		else
		{
			//""
			//(*iterAllLine) = "";
			
		}
		iterAllLine++;
	}//while
	
	
	nFunRet = 0;
	return nFunRet;
}

int sqlgen::_TurnToOneLineInAllLine( VrtLinesConT& vrtAllLines )
{
	int nFunRet = 0;

	std::string strGetLine;
	std::string strTmp;

	VrtLinesConIterT iterAllLine = vrtAllLines.begin();
	VrtLinesConIterT iterTmp = vrtAllLines.begin();

	bool bFindJia = false;

	int nLineCont = vrtAllLines.size();

	strGetLine.clear();
	strTmp.clear();

	while ( iterAllLine != vrtAllLines.end() )
	{
		strGetLine = "";
		strGetLine = (*iterAllLine);

		if (!strGetLine.empty())
		{ 		
			bFindJia = false;
			if (LINE_ADD == strGetLine[0])
			{
				bFindJia = true;
			} 	

			if (bFindJia)
			{
				strTmp += "\\";
				strTmp += "\n";
				strTmp +=strGetLine.substr(1);   //'+' -- '\'  '\r\n'
				(*iterAllLine) = "";
			}
			else
			{
				if (!strTmp.empty())
				{
					//strTmp += "\r\n";
					(*iterTmp) = strTmp;
					strTmp.erase();
				}
				iterTmp = iterAllLine;
				strTmp +=strGetLine;
			}
		}//if 

		iterAllLine++;
	}//while

	//last one line
	if ((iterAllLine == vrtAllLines.end()) && (iterTmp != vrtAllLines.end()))
	{
		if (!strTmp.empty())
		{
			//strTmp += "\r\n";
			(*iterTmp) = strTmp;
			strTmp.erase();
			strGetLine.clear();	 
		}
	}


	nFunRet = 0;
	return nFunRet;
}

int sqlgen::_RemoveBlackLineInAllLine( VrtLinesConT& vrtAllLines )
{
	int nFunRet = 0;

	int nSizeCount = -1;

	VrtLinesConIterT vtrLineIter = vrtAllLines.begin();

	nSizeCount = vrtAllLines.size();
	vtrLineIter = vrtAllLines.begin();
	while (vtrLineIter != vrtAllLines.end())
	{
		if ( (*vtrLineIter).length() < 1)
		{
			vrtAllLines.erase(vtrLineIter);
			nSizeCount = vrtAllLines.size();
			vtrLineIter = vrtAllLines.begin();
		}
		else
		{
			vtrLineIter++;	
		}  			
	}//while	  


	return nFunRet;
}

int sqlgen::_AnalyzeAllLineToClass( VrtLinesConT& vrtAllLines, CFileAllClass **ppFileAllClass )
{
	int nFunRet = 0;  
		
	std::string::size_type nFindTmp = std::string::npos;

	int								nLinesCount = -1;		 
	VrtLinesConIterT				vtrLineIter;
	VrtLinesConT					VtrProcessOneTable;
	
	CFileAllClass					*pFileAllClass = NULL;
	
	if (NULL != ppFileAllClass && NULL != *ppFileAllClass)
	{
		delete *ppFileAllClass;
		*ppFileAllClass = NULL;
	}
	*ppFileAllClass = new CFileAllClass();
	pFileAllClass =	 *ppFileAllClass;
	pFileAllClass->m_strFileName = m_strSrcFileName;
	pFileAllClass->m_pTableAllClassList = new ISystemListImp();
	
	

	nLinesCount = vrtAllLines.size();
	vtrLineIter = vrtAllLines.begin();
	VtrProcessOneTable.clear();
	while ( vtrLineIter != vrtAllLines.end() )
	{
		std::string strOneLine(""); 
		strOneLine = (*vtrLineIter);


		if (strOneLine.empty())
		{
			vtrLineIter++;
			continue;
		}	

		if (std::string::npos != strOneLine.find("[") 
			&& std::string::npos == strOneLine.find("[["))
		{
			if (VtrProcessOneTable.size() > 0)
			{
				_ProcessOneTable(VtrProcessOneTable, pFileAllClass->m_pTableAllClassList);   //one table
				VtrProcessOneTable.clear();	
			}
		}

		VtrProcessOneTable.push_back(strOneLine);

		vtrLineIter++;
		
		if (vrtAllLines.end() == vtrLineIter)
		{
			_ProcessOneTable(VtrProcessOneTable, pFileAllClass->m_pTableAllClassList);
			VtrProcessOneTable.clear();
		}
		
		
	}//while
	
	
	
	pFileAllClass = NULL;
	
	
	
	return nFunRet;

}

//================================================
 
 

int sqlgen::_RemoveUnVisableCharExceptBlackChar(std::string& strSrc)
{
	int nstrSrcSize = -1;
	std::string strTmp("");

	if (strSrc.size() < 1)
		return 0;

	if ('\0' == strSrc[0])
		return 0;

	nstrSrcSize = strSrc.size();

	for (int nIndex = 0; nIndex < nstrSrcSize; nIndex++)
	{
		if (strSrc[nIndex] < 0x20 || strSrc[nIndex] > 0x7e ) //32  126 strSrc[nIndex] < 0x21 || strSrc[nIndex] > 0x7e
		{
			//strTmp += strSrc[nIndex];
		}
		else
		{
			strTmp += strSrc[nIndex];
		}


		if (0x09 == strSrc[nIndex])//
		{
			strTmp += strSrc[nIndex];
		}
	}

	strSrc =  strTmp;

	nstrSrcSize = strSrc.size(); 


	return nstrSrcSize;
}

int sqlgen::_LOG(const std::string& strLog)
{
	int nFunRet = 0;

	if (false == this->m_OfstreamErrorReport.is_open())
	{
		nFunRet = -1;
		return nFunRet;
	}
	this->m_OfstreamErrorReport << strLog << std::endl;


	return nFunRet;
} 
 
 

int sqlgen::_ProcessOneTable(VrtLinesConT& VtrOneTable, ISystemList *pList)
{
	int nFunRet = 0;
	
	std::string::size_type nFindTmp		= std::string::npos;

	VrtLinesConIterT					VtrIterTable;
	VrtLinesConT						VtrOneDb;
	std::string                         strTableName;
	
	
	if (VtrOneTable.size() < 1 || NULL == pList)
	{
		nFunRet = -1;
		return nFunRet;
	}
	CTableAllClass *pNewTableAllClass = new CTableAllClass();
	pNewTableAllClass->m_pDbAllClassList = new ISystemListImp();	
	
	pList->addSystemListNodeWithData(DataType_CTableAllClass, pNewTableAllClass);
	
	
	
	
	VtrIterTable = VtrOneTable.begin();
	while (VtrIterTable != VtrOneTable.end())
	{
	  std::string	strLineTmp = (*VtrIterTable);
	  if (std::string::npos != strLineTmp.find("[") && std::string::npos == strLineTmp.find("[["))
	  {//[]
		  strTableName =  strLineTmp;
		  pNewTableAllClass->m_strTableName = strTableName;	
		  VtrIterTable++;
		  continue;		  		
	  }
	  
	  
	  if (std::string::npos != strLineTmp.find("[") && std::string::npos != strLineTmp.find("[["))
	  {  //[[]]
		
		  if (VtrOneDb.size() > 0)
		  {
			_ProcessOneTableOneDb(VtrOneDb, pNewTableAllClass->m_pDbAllClassList);
			
			VtrOneDb.clear(); 
		  }		
	  }	  
  	  VtrOneDb.push_back(strLineTmp);	
	
	  VtrIterTable++;
	  
	  
	  if (VtrOneTable.end() == VtrIterTable)
	  {
		  _ProcessOneTableOneDb(VtrOneDb, pNewTableAllClass->m_pDbAllClassList);

		  VtrOneDb.clear(); 
	  }						 
								  
	}//while
	
	
	//===================================	
	pNewTableAllClass = NULL;

	return nFunRet;
} 
 

int sqlgen::_ProcessOneTableOneDb(VrtLinesConT& VtrOneDb, ISystemList *pList)
{
	int nFunRet = 0;

	std::string::size_type nFindTmp		= std::string::npos;

	VrtLinesConIterT					VtrIterDb;
	std::string                         strDbName;

	VrtLinesConT						VtrOneSql;
	

	if (VtrOneDb.size() < 1 || NULL == pList)
	{
		nFunRet = -1;
		return nFunRet;
	}
	CDbAllClass *pNewCDbAllClass = new CDbAllClass();
	pNewCDbAllClass->m_strDbName = strDbName;
	pNewCDbAllClass->m_pSqlAllClassList = new ISystemListImp();	

	pList->addSystemListNodeWithData(DataType_CDbAllClass, pNewCDbAllClass);




	VtrIterDb = VtrOneDb.begin();
	VtrOneSql.clear();
	while (VtrIterDb != VtrOneDb.end())
	{
		std::string	strLineTmp = (*VtrIterDb);
		if (std::string::npos != strLineTmp.find("[[") )
		{//[]
			strDbName =  strLineTmp;
			pNewCDbAllClass->m_strDbName = strDbName;
			VtrIterDb++;
			continue;		  		
		}

		VtrOneSql.push_back(strLineTmp);
		_ProcessOneTableOneDbOneSql(VtrOneSql, pNewCDbAllClass->m_pSqlAllClassList);
		VtrOneSql.clear();
		
		
		VtrIterDb++;		 

	}//while


	//===================================	
	pNewCDbAllClass = NULL;

	return nFunRet;
}
 

int sqlgen::_ProcessOneTableOneDbOneSql(VrtLinesConT& VtrOneSql, ISystemList *pList)
{
	int nFunRet = 0;

	std::string::size_type nFindTmp		= std::string::npos;

	std::string                         strSqlId;
	std::string                         strSqlLine;
	VrtLinesConIterT                    VrtIter;
	std::string							strLineTmp;
	

	if (VtrOneSql.size() < 1 || NULL == pList)
	{
		nFunRet = -1;
		return nFunRet;
	}
	
	
	VrtIter = VtrOneSql.begin();
    while (VrtIter != VtrOneSql.end())
    {
		strLineTmp = *VrtIter;
		
		nFindTmp = strLineTmp.find_first_of("=");
		strSqlId = strLineTmp.substr(0, nFindTmp);
		strSqlLine   = strLineTmp.substr(nFindTmp + 1);//AQ:
    

		CSqlAllClass *pNewSqlAllClass = new CSqlAllClass();
		pNewSqlAllClass->m_strSqlId = strSqlId;
		pNewSqlAllClass->m_strSqlLine = strSqlLine;

		pList->addSystemListNodeWithData(DataType_CSqlAllClass, pNewSqlAllClass);
		pNewSqlAllClass = NULL;
	
		_CheckAndGetAQArray(strSqlId, strSqlLine, m_mapAQArray);

        
		VrtIter++;
    }
	
	

	return nFunRet;
}

 
 
//======================================================================================= 
 
int sqlgen::_OpenAllOutPutFiles()
{
	int nFunRet = 0;
 
	m_pri_Ofstream_SQLH.open(defSQLFileName_Out_db);
	if (m_pri_Ofstream_SQLH.fail())
	{
		_ClossAllOutPutFiles();
		nFunRet = -1;
		return nFunRet;
	}
	
	m_pri_Ofstream_SQLMAPDEFH.open(defSQLFileName_Out_Index);
	if (m_pri_Ofstream_SQLMAPDEFH.fail())
	{
		_ClossAllOutPutFiles();
		nFunRet = -1;
		return nFunRet;
	}
   
	return nFunRet;
}

int sqlgen::_ClossAllOutPutFiles()
{
	int nFunRet = 0;

	if (m_pri_Ofstream_SQLH.is_open())
	{
		m_pri_Ofstream_SQLH.close();
	}
	if (m_pri_Ofstream_SQLMAPDEFH.is_open())
	{
		m_pri_Ofstream_SQLMAPDEFH.close();
	}

	return nFunRet;
}


int sqlgen::_CheckAndGetAQArray
(
 const std::string& strSQLID,
 const std::string& strSQLLine,
 mapSqlIdLineConT&  mapAQArray 
 )
{

	int nFunRet = 0;
	//def_AQQUEUE
	std::string strdefAQQueue;
	std::string strAQStr;
	std::string strCheck;
	int bIsAQLine;
	if (strSQLID.empty() || strSQLLine.empty())
	{
		nFunRet = -1;
		return nFunRet;
	}
	
	bIsAQLine = 0;
	strdefAQQueue = def_AQQUEUE;
	strCheck = strSQLLine;
	trim(strCheck);	
	strAQStr = strCheck.substr(0, strdefAQQueue.size());
	if (0 == strAQStr.compare(strdefAQQueue))
	{
		bIsAQLine = 1;	
	}
	
	if (1== bIsAQLine && mapAQArray.end() == mapAQArray.find(strSQLID))
	{
		std::string strSQLIDKey = strSQLID;
		std::string strSQLIDValue = strSQLID;
		mapAQArray.insert(mapSqlIdLineConValueT(strSQLIDKey, strSQLIDValue));
	}
	
	
	return nFunRet;
}


int sqlgen::_AnalyzeTableCommonList
(
	const std::string& strTable, 
	ISystemListNode *pListNodeComon, 
	VrtLinesConT& vrtSQLH, 
	VrtLinesConT& vrtSQLMAPDEFH
)
{

	int nFunRet = 0;

	std::string strTableName;
	ISystemListNodeData *pNodeDataCommon = NULL;	
	enumDataType nDataTypeCommon = DataType_NULL;	 
	CDbAllClass *pGetClassCommon = NULL;
	ISystemList *pGetListCommon = NULL;
	std::string strTableTypeCommon;

	int nIndex = 0;
	int nSqlCounts = 0;
	
	ISystemListNode  *pGetListNodeSql = NULL;
	ISystemListNodeData  *pGetListNodeDataSql = NULL;
	enumDataType nDataTypeSql = DataType_NULL;	 
	CSqlAllClass *pGetClassSql = NULL;
	
	
	std::string strSqlID;
	std::string strSqlLine;
	char chArryTmp[defBuffSize] = {0};
	std::string strArryTmp;
	
	
	strTableName = "//" + strTable + "\n";	 //[ALARMRULE]
	vrtSQLH.push_back(strTableName);	
	vrtSQLMAPDEFH.push_back(strTableName);

	
	pNodeDataCommon = pListNodeComon->getNodeData();
	nDataTypeCommon = pNodeDataCommon->getDataType();
	if (DataType_CDbAllClass == nDataTypeCommon)
	{
		pGetClassCommon = (CDbAllClass *)pNodeDataCommon->getData();
		pGetClassCommon->m_strDbName;
		pGetListCommon = pGetClassCommon->m_pSqlAllClassList;
	}
	
	

	nSqlCounts = pGetListCommon->getNodesCount();
	if (NULL != pGetListCommon && 0 < nSqlCounts)
	{
		strTableTypeCommon = "//[[common]]\n";	 //[[common]]
		vrtSQLMAPDEFH.push_back(strTableTypeCommon);

		for (nIndex = 0; nIndex < nSqlCounts; nIndex++)
		{
			strSqlID.clear();
			strSqlLine.clear();
			pGetListNodeSql = NULL;
			pGetListNodeDataSql = NULL;
			nDataTypeSql = DataType_NULL;
			pGetClassSql = NULL;
		
			pGetListNodeSql = pGetListCommon->getSystemListNodeByIndex(nIndex);
			pGetListNodeDataSql = pGetListNodeSql->getNodeData();
			nDataTypeSql = pGetListNodeDataSql->getDataType();
			if (DataType_CSqlAllClass != nDataTypeSql)
			{
				continue;
			}
			
			pGetClassSql = (CSqlAllClass *)pGetListNodeDataSql->getData();
			strSqlID = pGetClassSql->m_strSqlId	;
			strSqlLine = pGetClassSql->m_strSqlLine;

			
			/*sqlmadef.h*/
			sprintf(chArryTmp, "%d", this->m_pri_nDefineIDIndex);
			strArryTmp = chArryTmp; 				
			std::string strTmpOne = "#define \t" + strSqlID + " \t " + strArryTmp	+ "\n";	
			vrtSQLMAPDEFH.push_back(strTmpOne);


			/*sql.h*/
			sprintf(chArryTmp, "%d", this->m_pri_nDefineIDIndex);
			strArryTmp = chArryTmp; 				
			std::string strTmpTwo  = "/*" + strSqlID + "*//*" + strArryTmp + "*/(\"" + strSqlLine + "\"),\n";	 // /*3*/("selcet * from table"),
			vrtSQLH.push_back(strTmpTwo);


			sprintf(chArryTmp, "%d", this->m_pri_nDefineIDIndex + 1); 
			strArryTmp = chArryTmp;				
			std::string strTmpThree = "/*" + strArryTmp + "*/(\"" + "" + "\"),\n";	 // /*4*/(""),
			vrtSQLH.push_back(strTmpThree);

			sprintf(chArryTmp, "%d", this->m_pri_nDefineIDIndex + 1 + 1);
			strArryTmp = chArryTmp; 				
			std::string strTmpFour = "/*" + strArryTmp + "*/(\"" + "" + "\"),\n";	 // /*5*/(""),
			vrtSQLH.push_back(strTmpFour);


			this->m_pri_nDefineIDIndex += DB_NUM;
			
			
		}  //for
	}//if (NULL != pGetListCommon && 0 < nSqlCounts)
		
	return	nFunRet;

}


int sqlgen::_GetMysqlSqlLineWithSameSqlID
(
 const std::string& strSqlIDOrale, 
 ISystemListNode *pListNodeMysql, 
 std::string& strMysqlSqlLine
 )
 {
	 int nFunRet = 0;

	 std::string strTableName;
	 ISystemListNodeData *pNodeDataMysql = NULL;	
	 enumDataType nDataTypeMysql = DataType_NULL;	 
	 CDbAllClass *pGetClassMysql = NULL;
	 ISystemList *pGetListMysql = NULL;

	 int nIndex = 0;
	 int nSqlCounts = 0;

	 ISystemListNode  *pGetListNodeSql = NULL;
	 ISystemListNodeData  *pGetListNodeDataSql = NULL;
	 enumDataType nDataTypeSql = DataType_NULL;	 
	 CSqlAllClass *pGetClassSql = NULL;


	 std::string strSqlID;
	 std::string strSqlLine;
	 char chArryTmp[defBuffSize] = {0};
	 std::string strArryTmp;

	 strMysqlSqlLine.clear();


	 pNodeDataMysql = pListNodeMysql->getNodeData();
	 nDataTypeMysql = pNodeDataMysql->getDataType();
	 if (DataType_CDbAllClass == nDataTypeMysql)
	 {
		 pGetClassMysql = (CDbAllClass *)pNodeDataMysql->getData();
		 pGetClassMysql->m_strDbName;
		 pGetListMysql = pGetClassMysql->m_pSqlAllClassList;
	 }



	 nSqlCounts = pGetListMysql->getNodesCount();
	 if (NULL != pGetListMysql && 0 < nSqlCounts)
	 {
		nFunRet = -1;
		 for (nIndex = 0; nIndex < nSqlCounts; nIndex++)
		 {
			 strSqlID.clear();
			 strSqlLine.clear();
			 pGetListNodeSql = NULL;
			 pGetListNodeDataSql = NULL;
			 nDataTypeSql = DataType_NULL;
			 pGetClassSql = NULL;

			 pGetListNodeSql = pGetListMysql->getSystemListNodeByIndex(nIndex);
			 pGetListNodeDataSql = pGetListNodeSql->getNodeData();
			 nDataTypeSql = pGetListNodeDataSql->getDataType();
			 if (DataType_CSqlAllClass != nDataTypeSql)
			 {
				 continue;
			 }

			 pGetClassSql = (CSqlAllClass *)pGetListNodeDataSql->getData();
			 strSqlID = pGetClassSql->m_strSqlId;
			 
			 if (0 == strSqlIDOrale.compare(strSqlID))
			 {
				 strMysqlSqlLine = pGetClassSql->m_strSqlLine;
				 nFunRet = 0;
				 break;
			 }


		 }  //for
	 }//if (NULL != pGetListCommon && 0 < nSqlCounts)

	 return	nFunRet;
 }
int sqlgen::_AnalyzeTableOracleMysqlList
(
 const std::string& strTable, 
 ISystemListNode *pListNodeOracle, 
 ISystemListNode *pListNodeMysql,
 VrtLinesConT& vrtSQLH, 
 VrtLinesConT& vrtSQLMAPDEFH
 )
{

	int nFunRet = 0;

	std::string strTableName;
	ISystemListNodeData *pNodeDataOracle = NULL;	
	enumDataType nDataTypeOracle = DataType_NULL;	 
	CDbAllClass *pGetClassOracle = NULL;
	ISystemList *pGetListOracle = NULL;
	std::string strTableTypeOracle;

	int nIndex = 0;
	int nSqlCounts = 0;

	ISystemListNode  *pGetListNodeSqlOrale = NULL;
	ISystemListNodeData  *pGetListNodeDataSqlOracle = NULL;
	enumDataType nDataTypeSqlOrale = DataType_NULL;	 
	CSqlAllClass *pGetClassSqlOracle = NULL;


	std::string strSqlID;
	std::string strSqlLine;
	char chArryTmp[defBuffSize] = {0};
	std::string strArryTmp;


	strTableName = "//" + strTable + "\n";	 //[ALARMRULE]
	vrtSQLH.push_back(strTableName);	
	vrtSQLMAPDEFH.push_back(strTableName);


	pNodeDataOracle = pListNodeOracle->getNodeData();
	nDataTypeOracle = pNodeDataOracle->getDataType();
	if (DataType_CDbAllClass == nDataTypeOracle)
	{
		pGetClassOracle = (CDbAllClass *)pNodeDataOracle->getData();
		pGetClassOracle->m_strDbName;
		pGetListOracle = pGetClassOracle->m_pSqlAllClassList;
	}



	nSqlCounts = pGetListOracle->getNodesCount();
	if (NULL != pGetListOracle && 0 < nSqlCounts)
	{
		strTableTypeOracle = "//[[oracle]][[mysql]]\n";	 //[[common]]
		vrtSQLMAPDEFH.push_back(strTableTypeOracle);

		for (nIndex = 0; nIndex < nSqlCounts; nIndex++)
		{
			strSqlID.clear();
			strSqlLine.clear();
			pGetListNodeSqlOrale = NULL;
			pGetListNodeDataSqlOracle = NULL;
			nDataTypeSqlOrale = DataType_NULL;
			pGetClassSqlOracle = NULL;

			pGetListNodeSqlOrale = pGetListOracle->getSystemListNodeByIndex(nIndex);
			pGetListNodeDataSqlOracle = pGetListNodeSqlOrale->getNodeData();
			nDataTypeSqlOrale = pGetListNodeDataSqlOracle->getDataType();
			if (DataType_CSqlAllClass != nDataTypeSqlOrale)
			{
				continue;
			}

			pGetClassSqlOracle = (CSqlAllClass *)pGetListNodeDataSqlOracle->getData();
			strSqlID = pGetClassSqlOracle->m_strSqlId;
			strSqlLine = pGetClassSqlOracle->m_strSqlLine;


			/*sqlmadef.h*/
			sprintf(chArryTmp, "%d", this->m_pri_nDefineIDIndex);
			strArryTmp = chArryTmp; 				
			std::string strTmpOne = "#define \t" + strSqlID + " \t " + strArryTmp	+ "\n";	
			vrtSQLMAPDEFH.push_back(strTmpOne);


			/*sql.h*/
			sprintf(chArryTmp, "%d", this->m_pri_nDefineIDIndex);
			strArryTmp = chArryTmp; 				
			std::string strTmpTwo  = "/*" + strSqlID + "*//*" + strArryTmp + "*/(\"" + "" + "\"),\n";	 // /*3*/("selcet * from table"),
			vrtSQLH.push_back(strTmpTwo);


			sprintf(chArryTmp, "%d", this->m_pri_nDefineIDIndex + 1); 
			strArryTmp = chArryTmp;				
			std::string strTmpThree = "/*" + strArryTmp + "*/(\"" + strSqlLine + "\"),\n";	 // /*4*/(""),
			vrtSQLH.push_back(strTmpThree);

			sprintf(chArryTmp, "%d", this->m_pri_nDefineIDIndex + 1 + 1);
			strArryTmp = chArryTmp; 
			std::string strSqlLineMysql;
			int nFind = _GetMysqlSqlLineWithSameSqlID(strSqlID, pListNodeMysql, strSqlLineMysql);
			if (0 != nFind)
			{
				std::string strLog = "Mysql cann't find Oracle:strSqlID=" + strSqlID;
				_LOG(strLog);
			}
							
			std::string strTmpFour = "/*" + strArryTmp + "*/(\"" + strSqlLineMysql + "\"),\n";	 // /*5*/(""),
			vrtSQLH.push_back(strTmpFour);


			this->m_pri_nDefineIDIndex += DB_NUM;


		}  //for
	}//if (NULL != pGetListCommon && 0 < nSqlCounts)

	return	nFunRet;
}

int sqlgen::_AnalyzeTableAllClassToVrt_TypeCommon( CTableAllClass *pTableAllClass, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH )
{
	int nFunRet = 0;
	int nDbsCount = 0;
	std::string    strTableName;

	ISystemList *pGet = NULL;
	ISystemListNode *pGetNodeCommon = NULL;

	if (NULL == pTableAllClass)
	{
		nFunRet = -1;
		return nFunRet;
	}
	strTableName = _GetIndexTableName(pTableAllClass->m_strTableName);

	pGet =	pTableAllClass->m_pDbAllClassList;

	if (NULL == pGet)
	{
		nFunRet = -1;
		return nFunRet;
	}

	nDbsCount = pGet->getNodesCount();

	if (0 == nDbsCount)
	{
		nFunRet = -1;
		return nFunRet;
	}	


	if (1 == nDbsCount)
	{
		pGetNodeCommon = NULL;

		pGetNodeCommon = pGet->getSystemListNodeByIndex(0);
		
		_AnalyzeTableCommonList(strTableName, pGetNodeCommon, vrtSQLH, vrtSQLMAPDEFH);
	}

	


	pGet = NULL;
	pGetNodeCommon = NULL;

	return nFunRet;
}

int sqlgen::_AnalyzeTableAllClassToVrt_TypeCommonOracleMysql( CTableAllClass *pTableAllClass, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH )
{
	int nFunRet = 0;
	int nDbsCount = 0;
	std::string    strTableName;

	ISystemList *pGet = NULL;
	ISystemListNode *pGetNodeOracle = NULL;
	ISystemListNode *pGetNodeMysql = NULL;
	ISystemListNode *pGetNodeCommon = NULL;


	if (NULL == pTableAllClass)
	{
		nFunRet = -1;
		return nFunRet;
	}
	strTableName = _GetIndexTableName(pTableAllClass->m_strTableName);

	pGet =	pTableAllClass->m_pDbAllClassList;

	if (NULL == pGet)
	{
		nFunRet = -1;
		return nFunRet;
	}

	nDbsCount = pGet->getNodesCount();

	if (3 > nDbsCount)
	{
		nFunRet = -1;
		return nFunRet;
	}	


	if (3 == nDbsCount)
	{
		pGetNodeOracle = NULL;
		pGetNodeCommon = pGet->getSystemListNodeByIndex(0);	
		_AnalyzeTableCommonList(strTableName, pGetNodeCommon, vrtSQLH, vrtSQLMAPDEFH);


		pGetNodeOracle = pGet->getSystemListNodeByIndex(1);
		pGetNodeMysql = pGet->getSystemListNodeByIndex(2);

		_AnalyzeTableOracleMysqlList(strTableName, pGetNodeOracle, pGetNodeMysql, vrtSQLH, vrtSQLMAPDEFH);
	}




	pGet = NULL;
	pGetNodeOracle = NULL;
	pGetNodeMysql = NULL;

	return nFunRet;
}

int sqlgen::_AnalyzeTableAllClassToVrt_TypeOracleMysql( CTableAllClass *pTableAllClass, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH )
{
	int nFunRet = 0;
	int nDbsCount = 0;
	std::string    strTableName;

	ISystemList *pGet = NULL;
	ISystemListNode *pGetNodeOracle = NULL;
	ISystemListNode *pGetNodeMysql = NULL;


	if (NULL == pTableAllClass)
	{
		nFunRet = -1;
		return nFunRet;
	}
	strTableName = _GetIndexTableName(pTableAllClass->m_strTableName);
	pGet =	pTableAllClass->m_pDbAllClassList;

	if (NULL == pGet)
	{
		nFunRet = -1;
		return nFunRet;
	}

	nDbsCount = pGet->getNodesCount();

	if (2 > nDbsCount)
	{
		nFunRet = -1;
		return nFunRet;
	}	


	if (2 == nDbsCount)
	{
		pGetNodeOracle = pGet->getSystemListNodeByIndex(0);
		pGetNodeMysql = pGet->getSystemListNodeByIndex(1);

		_AnalyzeTableOracleMysqlList(strTableName, pGetNodeOracle, pGetNodeMysql, vrtSQLH, vrtSQLMAPDEFH);
	}




	pGet = NULL;
	pGetNodeOracle = NULL;
	pGetNodeMysql = NULL;

	return nFunRet;
}
int sqlgen::_AnalyzeTableAllClassToVrt( CTableAllClass *pTableAllClass, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH )
{
	int nFunRet = 0;
	int nDbsCount = 0;

	ISystemList *pGet = NULL;
	
	if (NULL == pTableAllClass)
	{
		nFunRet = -1;
		return nFunRet;
	}
	pGet =	pTableAllClass->m_pDbAllClassList;

	if (NULL == pGet)
	{
		nFunRet = -1;
		return nFunRet;
	}

	nDbsCount = pGet->getNodesCount();
	
	if (0 == nDbsCount)
	{
		nFunRet = -1;
		return nFunRet;
	}
	else if (1 == nDbsCount)
	{
		nFunRet = _AnalyzeTableAllClassToVrt_TypeCommon(pTableAllClass, vrtSQLH, vrtSQLMAPDEFH);	
	}
	else if (2 == nDbsCount)
	{
		nFunRet = _AnalyzeTableAllClassToVrt_TypeOracleMysql(pTableAllClass, vrtSQLH, vrtSQLMAPDEFH);
	} 
	else if (3 == nDbsCount)
	{
		nFunRet = _AnalyzeTableAllClassToVrt_TypeCommonOracleMysql(pTableAllClass, vrtSQLH, vrtSQLMAPDEFH);
	} 
	pGet = NULL;
	
	 
	return nFunRet;

}

int sqlgen::_AnalyzeFileAllClassToVrt( CFileAllClass *pFileAllClass, VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH )
{
	int nFunRet = 0;
	int nTablesCount = 0;
	int nIndex = 0;
	
	std::string    strFileName;
	ISystemList *pGet = NULL;
	ISystemListNode *pGetNode = NULL;
	ISystemListNodeData *pGetNodeData = NULL;
	enumDataType         nGetNodeDataType = DataType_NULL;
	CTableAllClass		 *pGetClass = NULL;

	if (NULL == pFileAllClass)
	{
		nFunRet = -1;
		return nFunRet;
	}
	strFileName = pFileAllClass->m_strFileName;
	pGet =	pFileAllClass->m_pTableAllClassList;
	
	if (NULL == pGet)
	{
		nFunRet = -1;
		return nFunRet;
	}
	vrtSQLH.clear();
	vrtSQLMAPDEFH.clear();
	
	nTablesCount = pGet->getNodesCount();
	
	for (nIndex = 0; nIndex < nTablesCount; nIndex++)
	{
		pGetNode = NULL;
		pGetNodeData = NULL;
		pGetClass = NULL;
	
		pGetNode = pGet->getSystemListNodeByIndex(nIndex);
		pGetNodeData = pGetNode->getNodeData();
		
		nGetNodeDataType = pGetNodeData->getDataType();
		pGetClass = NULL;
		if (DataType_CTableAllClass == nGetNodeDataType)
		{
			pGetClass = (CTableAllClass *)pGetNodeData->getData();
			_AnalyzeTableAllClassToVrt(pGetClass, vrtSQLH, vrtSQLMAPDEFH);
			
			
		}
	
	}
	
	
	pGet = NULL;
	pGetNode = NULL;
	pGetNodeData = NULL;
	pGetClass = NULL; 
	
	return nFunRet;

}

int sqlgen::_WriteBeginToFiles()
{
	int nFunRet = 0;

	if (m_pri_Ofstream_SQLH.is_open())
	{
		m_pri_Ofstream_SQLH << SQLH_SQLMACRODEFH_FILE_COMMENT_HEADER << std::endl;
		m_pri_Ofstream_SQLH << SQLH_FILE_BEGIN << std::endl;

		
	}
	if (m_pri_Ofstream_SQLMAPDEFH.is_open())
	{
		m_pri_Ofstream_SQLMAPDEFH << SQLH_SQLMACRODEFH_FILE_COMMENT_HEADER << std::endl;
		m_pri_Ofstream_SQLMAPDEFH << SQLMACRODEFH_FILE_BEGIN << std::endl; 
		
	}
	
	
	return nFunRet;


}

int sqlgen::_WriteDataToFiles( VrtLinesConT& vrtSQLH, VrtLinesConT& vrtSQLMAPDEFH )
{
	int nFunRet = 0;

	VrtLinesConIterT iterVrt;
	
	iterVrt = vrtSQLH.begin();
	while (iterVrt != vrtSQLH.end())
	{ 
		if (m_pri_Ofstream_SQLH.is_open())
		{
			m_pri_Ofstream_SQLH << *iterVrt ;//<< std::endl; 
		}
		iterVrt++;
	}
	
	iterVrt = vrtSQLMAPDEFH.begin();
	while (iterVrt != vrtSQLMAPDEFH.end())
	{ 
		if (m_pri_Ofstream_SQLMAPDEFH.is_open())
		{
			m_pri_Ofstream_SQLMAPDEFH << *iterVrt ;//<< std::endl;

		}
		iterVrt++;
	}
	
	return nFunRet;

}

int sqlgen::_WriteEndToFiles()
{
	int nFunRet = 0;

	if (m_pri_Ofstream_SQLH.is_open())
	{
		m_pri_Ofstream_SQLH << SQLH_FILE_END << std::endl; 
	}
	if (m_pri_Ofstream_SQLMAPDEFH.is_open())
	{
		m_pri_Ofstream_SQLMAPDEFH << SQLMACRODEFH_FILE_END << std::endl;

	}
	 
	return nFunRet;
 
}

int sqlgen::_WriteAQArrayBeginToFile()
{
	int nFunRet = 0;
	
	if (this->m_pri_Ofstream_SQLMAPDEFH.is_open())
	{
		this->m_pri_Ofstream_SQLMAPDEFH << SQLMACRODEFH_FILE_AQ_QUEUE_BEGIN;// <<std::endl;	
	}
	
	
	return nFunRet;
}

int sqlgen::_WriteAQArrayDataToFile(mapSqlIdLineConT& mapAQArray)
{
	int nFunRet = 0;
	mapSqlIdLineConIterT iterMap;
	iterMap = mapAQArray.begin();
	while (iterMap != mapAQArray.end())
	{
		
		if (this->m_pri_Ofstream_SQLMAPDEFH.is_open())
		{
			this->m_pri_Ofstream_SQLMAPDEFH << iterMap->second << "\n";// <<std::endl;	
		}
	
		iterMap++;
	}
	
	
	
	return nFunRet;
}

int sqlgen::_WriteAQArrayEndToFile()
{
	int nFunRet = 0;
	
	if (this->m_pri_Ofstream_SQLMAPDEFH.is_open())
	{
		this->m_pri_Ofstream_SQLMAPDEFH << SQLMACRODEFH_FILE_AQ_QUEUE_END <<std::endl;	
	}

	return nFunRet;
}

std::string sqlgen::_GetIndexTableName(const std::string& strTableName)
{
	std::string strNewTableName;
	char szNewTableName[1024] = {0};
	
	sprintf(szNewTableName, "[%d]%s", m_nIndexTable, strTableName.c_str());
	m_nIndexTable++;
	
	strNewTableName = szNewTableName;
	return strNewTableName;
}

 int	sqlgen::_Check_TablesCount(CFileAllClass *m_pFileAllClass)  
 { 	
	 int nFunRet = 0;
	 
	 int nFileTablesCount = m_nIndexTable ;// + 1
	 int nSrcNodesCount = 0;
	 std::string strLog = "";
	 
	 
	 if (NULL != m_pFileAllClass && NULL != m_pFileAllClass->m_pTableAllClassList)
	 {
		 nSrcNodesCount = m_pFileAllClass->m_pTableAllClassList->getNodesCount();
		 if (nFileTablesCount != nSrcNodesCount)
		 {
			strLog = "error! nFileTablesCount != nSrcNodesCount";
			 _LOG(strLog);
			 nFunRet = -1;
		 }		
	 }	 

	 return nFunRet;
 }
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
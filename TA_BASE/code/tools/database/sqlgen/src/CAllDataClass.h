///////////////////////////////////////////////////////////
//  CDataTmp.h
//  Implementation of the Class CDataTmp
//  Created on:      20-Feb-2012 09:31:53
//  Original author: Shenglong.LIn
///////////////////////////////////////////////////////////

#if !defined(EA_72040107_BCB6_4ec5_8433_AD7F89877207__INCLUDED_)
#define EA_72040107_BCB6_4ec5_8433_AD7F89877207__INCLUDED_
#include <iostream>

#include "ISystemList.h"


//===============================================================

class CFileAllClass
{

public:
	CFileAllClass()
	{
		m_strFileName.c_str();
		m_pTableAllClassList = NULL;
		return;
	}
	virtual ~CFileAllClass()
	{
		m_strFileName.c_str();
		if (NULL != m_pTableAllClassList)
		{
			delete	m_pTableAllClassList;		
			m_pTableAllClassList = NULL; 
		}								 
		return;
	}
public:
	std::string m_strFileName;
	ISystemList *m_pTableAllClassList;	 //CTableAllClass
};

//===============================================================


class CTableAllClass
{

public:
	CTableAllClass()
	{
		m_strTableName.c_str();
		m_pDbAllClassList = NULL;
		return;
	}
	virtual ~CTableAllClass()
	{
		m_strTableName.c_str();
		if (NULL != m_pDbAllClassList)
		{
			delete	m_pDbAllClassList;		
			m_pDbAllClassList = NULL; 
		}								 
		return;
	} 
public:
	std::string m_strTableName;
	ISystemList *m_pDbAllClassList;	//CDbAllClass
};

//===============================================================



class CDbAllClass
{

public:
	CDbAllClass()
	{
		m_strDbName.c_str();
		m_pSqlAllClassList = NULL;
		return;
	}
	virtual ~CDbAllClass()
	{
		m_strDbName.c_str();
		if (NULL != m_pSqlAllClassList)
		{
			delete	m_pSqlAllClassList;		
			m_pSqlAllClassList = NULL; 
		}								 
		return;
	} 
public:
	std::string m_strDbName;
	ISystemList *m_pSqlAllClassList;	 //CSqlAllClass
};


//===============================================================


//===============================================================



class CSqlAllClass
{

public:
	CSqlAllClass(){return;}
	virtual ~CSqlAllClass(){return;} 
public:
	std::string m_strSqlId;
	std::string m_strSqlLine;
};


//===============================================================

#endif // !defined(EA_72040107_BCB6_4ec5_8433_AD7F89877207__INCLUDED_)

//////////////////////////////////////////////////////////////////////
///		@file		DeqMsgJob.h
///		@author		zhilin,ouyang
///		@date		2012-02-19 14:44:41
///
///		@brief	    to declare the class for dequeue message for the 
///                 local database, it's same for Oracle dequeue job
///                 database synchronization routine.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  DEQMSGJOB_H
#define  DEQMSGJOB_H

#include "CommonDef.h"
#include "CommonData.h"
#include "core/threads/src/Thread.h"

#include "IDatabase.h"

NS_BEGIN(TA_DB_Sync)


class CDeqMsgJob : public TA_Base_Core::Thread
{
public:
	CDeqMsgJob(const char* pzLocation);
	CDeqMsgJob(const std::string& strLocation);
	~CDeqMsgJob();

public:
	virtual void  run();
	virtual void  terminate();

public:
	void     setQName(const std::string& strQName);

private:
	int      _DeqMsg();
	void     _InitEnv();
	void     _Terminate();

private:
	bool                m_bTerminate;
	std::string         m_strLocation;
	std::string         m_strDeqJobName;
	std::string         m_strDeqSQL;
	TA_AQ_Lib::IDatabase*   m_pDatabase;

};

NS_END(TA_DB_Sync)



#endif /// DEQMSGJOB_H

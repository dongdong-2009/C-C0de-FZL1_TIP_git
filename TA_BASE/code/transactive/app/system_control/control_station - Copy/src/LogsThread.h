#if !defined(AFX_ZIPANDCLEARLOGS_H__6FE842D6_6799_4B54_9CEA_F03424267ACD__INCLUDED_)
#define AFX_ZIPANDCLEARLOGS_H__6FE842D6_6799_4B54_9CEA_F03424267ACD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "StdAfx.h"
#include <string>
#include "core\threads\src\Thread.h"



class CLogsThread : public TA_Base_Core::Thread
{
public:
	CLogsThread(std::string dirStr,std::string timeStr,CTime time_to_clear_file);
	virtual ~CLogsThread();
	
	virtual void run();
	virtual void terminate();
	
	
private:
	std::string m_dirStr;
	std::string m_timeStr;
	CTime m_time_to_clear_file;
	
};

#endif // !defined(AFX_ZIPANDCLEARLOGS_H__6FE842D6_6799_4B54_9CEA_F03424267ACD__INCLUDED_)

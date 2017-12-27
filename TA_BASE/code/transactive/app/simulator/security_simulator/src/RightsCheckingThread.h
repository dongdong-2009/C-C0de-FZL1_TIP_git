// RightsCheckingThread.h: interface for the RightsCheckingThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIGHTSCHECKINGTHREAD_H__94EE5490_B17B_43CF_9B24_9091AF8EEB3B__INCLUDED_)
#define AFX_RIGHTSCHECKINGTHREAD_H__94EE5490_B17B_43CF_9B24_9091AF8EEB3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/Thread.h"

#include "RightsHelper.h"
#include "CommonInterface.h"

using namespace TA_Base_Core;

class RightsCheckingThread : public TA_Base_Core::Thread
{
public:
	RightsCheckingThread(IRightsChecker& rightsChecker, IResultObserver& resultObserver, 
						 std::string& sessionId, std::string location, unsigned long resourceKey, 
						 unsigned long actionKey, unsigned long profile);
	virtual ~RightsCheckingThread();

	virtual void run();
	virtual void terminate();

private:
	IRightsChecker&	  m_rightsChecker;
	IResultObserver&  m_resultObserver;

	bool m_isRunning;
	bool m_isInitSuccess;

	std::string m_sessionId;
	std::string m_location;
	unsigned long m_resourceKey;
	unsigned long m_actionKey;
	unsigned long m_locationKey;
	unsigned long m_profile;
	unsigned long m_subsystem;
	std::vector< unsigned long > m_actionGroups;
};

#endif // !defined(AFX_RIGHTSCHECKINGTHREAD_H__94EE5490_B17B_43CF_9B24_9091AF8EEB3B__INCLUDED_)

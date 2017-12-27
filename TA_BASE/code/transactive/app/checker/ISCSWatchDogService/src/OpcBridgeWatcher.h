// OpcBridgeWatcher.h: interface for the OpcBridgeWatcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPCBRIDGEWATCHER_H__0C629E4C_3BF4_4067_8937_70CA4494ADD8__INCLUDED_)
#define AFX_OPCBRIDGEWATCHER_H__0C629E4C_3BF4_4067_8937_70CA4494ADD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WatchedModule.h"

const int RESTARTLIMITNUMBER = 5;

class OpcBridgeWatcher : public WatchedModule  
{
public:
	OpcBridgeWatcher();
	virtual ~OpcBridgeWatcher();
	void createStatusChecker();
	void createLogStatusChecker();
	virtual std::string getServiceName() const
	{
		return std::string( "Opc Bridge" );
	}
private:
	void heartBeatCall( const StatusChecker* pChecker );
	void onStatusChanged( const ObserverData& currentStatus );
private:
	StatusChecker* m_pStatusChecker;
	time_t m_nHeartBeat;
	time_t m_nHeartBeatOld;
};

#endif // !defined(AFX_OPCBRIDGEWATCHER_H__0C629E4C_3BF4_4067_8937_70CA4494ADD8__INCLUDED_)

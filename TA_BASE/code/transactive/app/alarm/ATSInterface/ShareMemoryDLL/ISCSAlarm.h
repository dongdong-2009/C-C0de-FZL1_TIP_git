// ISCSAlarm.h: interface for the CISCSAlarm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISCSALARM_H__B6C8F3D1_FE4C_4397_98AA_C85FD55B60F8__INCLUDED_)
#define AFX_ISCSALARM_H__B6C8F3D1_FE4C_4397_98AA_C85FD55B60F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "IISCSAlarm.h"
#include "DataStructInfo.h"

class CISCSAlarm : public IISCSAlarm  
{
public:
	CISCSAlarm();
	virtual ~CISCSAlarm();

	unsigned long retrieveAlarm( std::list<ExAlarmQueueItem*>& details );
	void setProfileInfo(const ProfileInfo& profileInfo);
	void submitAlarmAckUpdate( const AlarmAckUpdate& update ); //May catch 
	void submitAlarmRequest( const AlarmRequest& update ); //May catch
	void submitMmsJobRequest( const MmsJobRequest& update );	
};

#endif // !defined(AFX_ISCSALARM_H__B6C8F3D1_FE4C_4397_98AA_C85FD55B60F8__INCLUDED_)

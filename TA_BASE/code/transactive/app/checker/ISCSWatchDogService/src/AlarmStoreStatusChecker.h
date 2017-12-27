// AlarmStoreStatusChecker.h: interface for the AlarmStoreStatusChecker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMSTORESTATUSCHECKER_H__AA993739_6A76_438F_BFCC_7D22ECBCF9DA__INCLUDED_)
#define AFX_ALARMSTORESTATUSCHECKER_H__AA993739_6A76_438F_BFCC_7D22ECBCF9DA__INCLUDED_

#include "StatusChecker.h"
#include "core/alarm/idl/src/AlarmStoreCorbaDef.h"

#include <string>
#include <sstream>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AlarmStoreStatusChecker : public StatusChecker
{
public:
	AlarmStoreStatusChecker();
	virtual ~AlarmStoreStatusChecker();

	// main method 
	virtual long checkMethod();

	virtual bool connect();

	virtual void disconnect()
	{
		m_bIsConnect = false;
	}

	virtual std::string getServiceName() const
	{
		return std::string("Alarm Store Service");
	}

	virtual std::string getName() const
	{
		return std::string( "AlarmStoreStatusChecker" );
	}

	virtual bool isConnect() const
	{
		return m_bIsConnect;
	}

	std::string getIOR() const
	{
		return m_IOR;
	}

	std::string getAddress() const
	{
		return m_strAddr;
	}

	void setAddress( const std::string& addr )
	{
		m_strAddr = addr;
	}

	unsigned long getPort() const
	{
		return m_port;
	}

	void setPort( unsigned long port )
	{
		m_port = port;
	}

private:
	bool resolveAlarmStore();
private:
	TA_Base_Core::IAlarmStoreCorbaDef_var	m_alarmStore;
	long					lCurrentTime;
	long					IPreviseTime;
	std::string				m_strAddr;
	unsigned long			m_port;
	std::string				m_strName;
	std::string				m_IOR;
	bool					m_bIsConnect;
};

#endif // !defined(AFX_ALARMSTORESTATUSCHECKER_H__AA993739_6A76_438F_BFCC_7D22ECBCF9DA__INCLUDED_)

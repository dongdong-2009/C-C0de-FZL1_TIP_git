// OpcBridgeStatusChecker.h: interface for the OpcBridgeStatusChecker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPCBRIDGESTATUSCHECKER_H__F02382EC_D083_44A7_9CCE_C2C953C9E492__INCLUDED_)
#define AFX_OPCBRIDGESTATUSCHECKER_H__F02382EC_D083_44A7_9CCE_C2C953C9E492__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StatusChecker.h"
#include "ItemDef.h"

//const CString	SERVERNAME					= "WtSvrTst2";
const int		OPCREADITEMFAILLIMITNUMBER	= 20;
const CString   SERVERNAME					= "TransActiveDataSource";

class OpcBridgeStatusChecker : public StatusChecker  
{
public:
	OpcBridgeStatusChecker();
	virtual ~OpcBridgeStatusChecker();
	bool connect();
	void disconnect();

	// OpcBridgeStatusChecker do not supprot pause, this will exit when receive pause call
	virtual void pause();
	virtual void continues();

	std::string getServiceName() const
	{
		return std::string( "Opc Bridge" );
	}

	std::string getName() const
	{
		return std::string( "OpcBridgeStatusChecker" );
	}

	bool isConnect() const
	{
		return false;
	}

private:
	void init();
	bool OpcConnectLocal();
	void InitWatchdog();
	void UnInitWatchdog();
	void AddNewItem ( CString accessPath, CString itemName );
	bool OpcReadItem();
	long checkMethod();
private:
	CItemDef		m_opcWatchdogItem;
	
	HANDLE			m_hConnection;
	HANDLE			m_hGroup;
	
	FILETIME		m_timeStampOld;

	bool			m_bIsConnect;
	bool			m_bCreateOpcWatchdogTag;
};

#endif // !defined(AFX_OPCBRIDGESTATUSCHECKER_H__F02382EC_D083_44A7_9CCE_C2C953C9E492__INCLUDED_)

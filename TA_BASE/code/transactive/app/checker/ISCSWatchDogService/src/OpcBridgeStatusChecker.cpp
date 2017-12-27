// OpcBridgeStatusChecker.cpp: implementation of the OpcBridgeStatusChecker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OpcBridgeStatusChecker.h"
#include "opcda.h"
#include "opc_ae.h"
#include "wtclientapi.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OpcBridgeStatusChecker::OpcBridgeStatusChecker() : 
m_bIsConnect(false),
m_bCreateOpcWatchdogTag(false)
{
	m_hConnection =	INVALID_HANDLE_VALUE;
	m_hGroup =		INVALID_HANDLE_VALUE;

	m_timeStampOld.dwHighDateTime = 0;
	m_timeStampOld.dwLowDateTime = 0;

	// move this action to init() function
//	InitWatchdog();
}

OpcBridgeStatusChecker::~OpcBridgeStatusChecker()
{
	// clean-up the opc connection
	// remove items, remove group, close connection.
	RemoveOPCItem ( m_hConnection, m_hGroup, m_opcWatchdogItem.m_hItem );
	
	if ( m_hGroup != INVALID_HANDLE_VALUE )
	{
		RemoveOPCGroup ( m_hConnection, m_hGroup );
	}
	
	if ( m_hConnection != INVALID_HANDLE_VALUE )
	{
		DisconnectOPC( m_hConnection );		
	}

	UnInitWatchdog();
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "OpcBridgeStatusChecker destruction!" );
}

void OpcBridgeStatusChecker::init()
{
	InitWatchdog();
}

void OpcBridgeStatusChecker::pause()
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"OpcBridgeStatusChecker pause() start" );
	StatusChecker::pause();
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"OpcBridgeStatusChecker pause() OK" );
}

void OpcBridgeStatusChecker::continues()
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"OpcBridgeStatusChecker continues() start" );
	StatusChecker::continues();
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"OpcBridgeStatusChecker continues() OK" );
}

void OpcBridgeStatusChecker::InitWatchdog()
{
	// To use APARTMENTHREADED model comment out call to WTclientCoInit
	// and enable following section of code
	// BE SURE TO INCLUDE _WIN32_DCOM IN THE
	// PROJECT SETTINGS PREPROCESSOR-DEFINITIONS
	HRESULT	r1;
	HRESULT	r2;
	
	r1 = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	
	r2 = CoInitializeSecurity(
		NULL,   //Points to security descriptor 
		-1,     //Count of entries in asAuthSvc 
		NULL,   //Array of names to register 
		NULL,   //Reserved for future use 
		RPC_C_AUTHN_LEVEL_NONE,     //The default authentication level for proxies
		RPC_C_IMP_LEVEL_IMPERSONATE,//The default impersonation level for proxies
		NULL,                       //Reserved; must be set to  NULL 
		EOAC_NONE,                  //Additional client or server-side capabilities
		NULL                        //Reserved for future use 
		);
}

void OpcBridgeStatusChecker::UnInitWatchdog()
{
	CoUninitialize();
}

bool OpcBridgeStatusChecker::OpcConnectLocal()
{
	m_hConnection =	INVALID_HANDLE_VALUE;
	m_hGroup =		INVALID_HANDLE_VALUE;

 	m_timeStampOld.dwHighDateTime = 0;
 	m_timeStampOld.dwLowDateTime = 0;

	InitWatchdog();
	
	DWORD	rate;
	float	deadband;
		
	m_hConnection = ConnectOPC( "", SERVERNAME, true );
		
	if ( m_hConnection!=INVALID_HANDLE_VALUE )
	{
		rate = 0;
		deadband = 0.0f;
		
		m_hGroup = AddOPCGroup( m_hConnection, "Group 1", &rate, &deadband );
		
		if ( m_hGroup!=INVALID_HANDLE_VALUE )
		{
			AddNewItem ( "", "OpcWatchdog" );
		}
		
		return true;
	} 
	else
	{
		return false;
	}
}

bool OpcBridgeStatusChecker::OpcReadItem()
{
	if( ReadOPCItemFromDevice(m_hConnection, m_hGroup, m_opcWatchdogItem.m_hItem, &(m_opcWatchdogItem.m_value), 
		&(m_opcWatchdogItem.m_timestamp), &(m_opcWatchdogItem.m_dwQuality)) )
	{
		CString strMsg;
		strMsg.Format("%s ---%s-%d  %s-%d  %s-%d-%d", 
			"OpcReadItem",
			"value", m_opcWatchdogItem.m_value.intVal,
			"quality", m_opcWatchdogItem.m_dwQuality, 
			"timestamp", m_opcWatchdogItem.m_timestamp.dwHighDateTime,
			m_opcWatchdogItem.m_timestamp.dwLowDateTime);

//		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, strMsg );

		return true;
	}
	else
	{
		static int number = 0;
		static int dayOld = 0;
		
		time_t currentTime;
		time( &currentTime );
		struct tm hb = *gmtime( &currentTime );
		
		if( (number++)<OPCREADITEMFAILLIMITNUMBER )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "OpcReadItem Failed" );
			
			disconnect();
		}
		
		if( number>=10000 )
		{
			number = OPCREADITEMFAILLIMITNUMBER;
		}
		
		if( dayOld!=hb.tm_mday )
		{
			number = 0;
		}
		
		dayOld = hb.tm_mday;

		return false;
	}
}

void OpcBridgeStatusChecker::AddNewItem( CString accessPath, CString itemName )
{
	m_opcWatchdogItem.m_csName = itemName;
	m_opcWatchdogItem.m_csPath = accessPath;
	m_opcWatchdogItem.m_hItem = AddOPCItem( m_hConnection, m_hGroup, itemName );

	if ( m_opcWatchdogItem.m_hItem!=INVALID_HANDLE_VALUE )
	{
		m_bCreateOpcWatchdogTag = true;
	}
	else
	{
		m_bCreateOpcWatchdogTag = false;
	}
}

long OpcBridgeStatusChecker::checkMethod()
{
	// get connect status each check
	if( connect() )
	{
		if ( m_bCreateOpcWatchdogTag )
		{
			if ( OpcReadItem() )
			{
				if( (CompareFileTime(&m_timeStampOld, &(m_opcWatchdogItem.m_timestamp))==0) 
					|| (OPC_QUALITY_GOOD!=m_opcWatchdogItem.m_dwQuality)
					) 
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "notify OpcBridge Observer" );
					notifyObservers();
				}
				else
				{
					cleanSuccessiveError();
				}
				
				m_timeStampOld.dwHighDateTime = m_opcWatchdogItem.m_timestamp.dwHighDateTime;
				m_timeStampOld.dwLowDateTime = m_opcWatchdogItem.m_timestamp.dwLowDateTime;
			}
		}
	}

	return 1;
}

bool OpcBridgeStatusChecker::connect()
{
	if ( !m_bIsConnect )
	{
		m_bIsConnect = OpcConnectLocal();

		if ( m_bIsConnect )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "connected Opc Bridge" );
		} 
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "connect Opc Bridge failed!" );
		}
	}

	return m_bIsConnect;
}

void OpcBridgeStatusChecker::disconnect()
{
	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "disconnect Opc Bridge start" );

	m_bIsConnect = false;
	m_bCreateOpcWatchdogTag = false;

	// clean-up the opc connection
	// remove items, remove group, close connection.
	if ( m_bCreateOpcWatchdogTag )
	{
		RemoveOPCItem ( m_hConnection, m_hGroup, m_opcWatchdogItem.m_hItem );
	}

	if ( m_hGroup != INVALID_HANDLE_VALUE )
	{
		RemoveOPCGroup ( m_hConnection, m_hGroup );
		m_hGroup = INVALID_HANDLE_VALUE;
	}
	
	if ( m_hConnection != INVALID_HANDLE_VALUE )
	{
		DisconnectOPC( m_hConnection );
		m_hConnection = INVALID_HANDLE_VALUE;
	}

	UnInitWatchdog();
	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "disconnect Opc Bridge end" );
}
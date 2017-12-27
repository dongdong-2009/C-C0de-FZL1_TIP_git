// LogStatusChecker.h: interface for the LogStatusChecker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGSTATUSCHECKER_H__F02382EC_D083_44A7_9CCE_C2C953C9E492__INCLUDED_)
#define AFX_LOGSTATUSCHECKER_H__F02382EC_D083_44A7_9CCE_C2C953C9E492__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StatusChecker.h"
#include "LogAnalyser.h"

// static const std::string OpcExeFileName("TA_OpcBridgeService.exe");
// static const std::string OpcLogFileName("c:\\transactive\\logs\\Log_OpcBridge");

class LogStatusChecker : public StatusChecker  
{
public:
	/**
	 * LogStatusChecker
	 *
	 * Construct with ExeFile name and logFile path name, search key limit map
	 * you should call start() method to start thread
	 */
	LogStatusChecker( const std::string& ExeFileName, 
					  const std::string& LogFilePathName,
					  const std::map<std::string, unsigned long>& KeyLimitMap,
					  unsigned long ulRefreshInterval = 3000 ): m_strName(""),  m_bIsConnect(false)
	{
		setErrorNotifyCountLimit(1);
		m_logAnalyser.setWatchedExeName(ExeFileName);
		m_logAnalyser.setLogFile(LogFilePathName);
		addKey( KeyLimitMap );
		m_logAnalyser.setRefreshInterval(3000); // 3s
		m_logAnalyser.start();
	}

	/**
	 * LogStatusChecker
	 *
	 * Construct with ExeFile name and logFile path name, search key limit map
	 */
	LogStatusChecker( const std::string& ExeFileName, 
		const std::string& LogFilePathName,
		unsigned long ulRefreshInterval = 3000 ): m_strName(""), m_bIsConnect(false)
	{
		setErrorNotifyCountLimit(1);
		m_logAnalyser.setWatchedExeName(ExeFileName);
		m_logAnalyser.setLogFile(LogFilePathName);
		m_logAnalyser.setRefreshInterval(3000); // 3s
		//		m_logAnalyser.start();
	}

	virtual void terminate()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"LogStatusChecker terminate called!" );
		m_logAnalyser.terminateAndWait();
		StatusChecker::terminate();
	}

	void setName( const std::string& strName )
	{
		m_strName = strName;
	}

	virtual ~LogStatusChecker()
	{
		m_logAnalyser.terminateAndWait();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "LogStatusChecker destruction!" );
	}

	void setFileRefreshInterval( unsigned long ulInterval )
	{
		m_logAnalyser.setRefreshInterval( ulInterval );
	}

	bool connect();
	void disconnect();

	bool isConnect() const
	{
		return m_bIsConnect;
	}

	std::string getName() const
	{
		return m_strName;
	}

	/**
	 * pause
	 *
	 * pause both log analyze thread and status checker thread
	 */
	void pause()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"++++++LogStatusChecker pause() called!!!!!!!" );
		m_logAnalyser.pause();
		StatusChecker::pause();
	}

	/**
	 * start
	 *
	 * start log analyze thread and status checker thread
	 */
	void start()
	{
		m_logAnalyser.start();
		StatusChecker::start();
	}


	/**
	 * continues
	 *
	 * continue both log analyze thread and status checker thread
	 */
	void continues()
	{
		m_logAnalyser.continues();
		StatusChecker::continues();
	}
	
	/**
	 * addKey
	 *
	 * add search key limit in map, this operation will restart log analyze 
	 */
	void addKey( const std::map<std::string, unsigned long>& KeyLimitMap )
	{
		std::pair< std::map<std::string, unsigned long>::iterator, bool> pairMapRet;
		{
			for ( std::map<std::string, unsigned long>::const_iterator itKeyMap = KeyLimitMap.begin();
			itKeyMap != KeyLimitMap.end(); ++itKeyMap )
			{
				m_KeyLimitMap[itKeyMap->first] = itKeyMap->second;
				addKey( itKeyMap->first, itKeyMap->second );
			}
		}
	}

	/**
	 * addKey
	 *
	 * add search key limit, this operation will restart log analyze 
	 */
	bool addKey( const std::string& strKey, unsigned long ulLimit, pFuncMathchLine pMethod = stdStringContain )
	{
//		std::pair< std::map<std::string, unsigned long>::iterator, bool> pairMapRet;
		bool bInsert = false;
		{
			TA_Base_Core::ThreadGuard guard( m_LockKeyMap );
			m_KeyLimitMap.insert( std::make_pair( strKey, ulLimit ) );
			std::map<std::string, unsigned long>::iterator itFind = m_KeyLimitMap.find(strKey);
			if ( itFind == m_KeyLimitMap.end() )
			{
				m_KeyLimitMap[strKey] = 0;
				bInsert = true;
			}
			else
			{
				bInsert = true;
			}
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Add %s, searchkey:[\"%s\"]",
				(bInsert?"OK":"Failed"), strKey.c_str() );
		}
		if ( bInsert )
		{
			m_logAnalyser.addSearchKey( strKey, pMethod );
		}
		return bInsert;
	}
	
	/**
	 * removeKey
	 *
	 * remove search key for log file analyze
	 */
	bool removeKey( const std::string& strKey, unsigned long ulLimit )
	{
		{
			TA_Base_Core::ThreadGuard guard( m_LockKeyMap );
			m_KeyLimitMap.erase( strKey );
		}
		{
			m_logAnalyser.removeSearchKey( strKey );
		}
		return true;
	}
	
protected:
	LogStatusChecker()
	{
	}
	
private:
	long checkMethod();
	virtual void init();
private:
	LogAnalyser									m_logAnalyser;
	std::map<std::string, unsigned long>		m_KeyLimitMap;
	TA_Base_Core::NonReEntrantThreadLockable	m_LockKeyMap;
	std::string									m_strName;		// name of log status checker

	bool										m_bIsConnect;
};

#endif // !defined(AFX_LOGSTATUSCHECKER_H__F02382EC_D083_44A7_9CCE_C2C953C9E492__INCLUDED_)

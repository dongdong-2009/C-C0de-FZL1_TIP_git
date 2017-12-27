/**
 * LogAnalyser.h
 *
 * this class wrap common method to get special statistics info from log files
 *
 * CreateData: 2009/03/13 13:35:08
 * Author:	LY
 *
 */
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctime>
#include <windows.h>
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DebugUtilMacros.h"
#include "core/synchronisation/src/Condition.h"

#pragma warning(disable: 4786)


// compare function type, use to match key in file
typedef bool (*pFuncMathchLine)( const std::string& strLine, const std::string& strKey );

// default compare function
static bool stdStringContain( const std::string& strLine, const std::string& strKey )
{
	return ( strLine.find( strKey ) != std::string::npos );
}

// unprintable ascii character find function
static bool findUnPrintableASCIICharacter( const std::string& strLine, const std::string& strKey )
{
	std::string::size_type pos = strLine.find( strKey );
	if ( pos != std::string::npos )
	{
		pos += strKey.length();
		std::string strSubstr = strLine.substr( pos, strLine.find( ")", pos ) - pos ); // end at ")"
		std::string::const_iterator itChar = strSubstr.begin();
		for( ; itChar != strSubstr.end(); ++itChar )
		{
			// unprintable ascii code 
			if ( *itChar < 0x20 || *itChar > 0x7e )
				return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
// class LogAnalyser
class LogAnalyser : public TA_Base_Core::Thread
{
public:
	LogAnalyser(): m_bTerminate(false), m_ulRefreshInterval(1000), 
		m_bPause(false), m_strProcessID(""), m_maxWriteTime(0)
	{

	}
	virtual ~LogAnalyser()
	{
		terminateAndWait();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "LogAnalyser destruction!" );
	}

public:

	void setLogFile( const std::string& fileName);
	void addSearchKey( const std::string& searchKey, pFuncMathchLine p = stdStringContain );
	void removeSearchKey( const std::string& searchKey );
	void setFileStartFlag(const std::string& startFlag);
	void getLogFileInfo();	// debug use, print analyze result
	void setWatchedExeName( const std::string& strExeName );

	// use to get current key count
	unsigned long getKeyCount( const std::string& strKey )
	{
		{
			// TODO: lock SearchKeyList for searching
			TA_Base_Core::ThreadGuard guard(m_LockSearchList);
			if ( m_SearchKeyList.find(strKey) == m_SearchKeyList.end() )
			{
				return 0;
			}
		}
		{
			// TODO: lock SearchResultMap for read
			TA_Base_Core::ThreadGuard guard(m_LockResult);
			return m_SearchResultMap[strKey];
		}
	}

	void reAnalyze();	// clean current result, clean file list, get process id then run again
	void resetProcessID();	// use m_strExeName get related process id, use CreateToolhelp32Snapshot...
	virtual void terminate();	// thread method virtual function, set m_bTerminate flag

	// main method in run loop
	void refresh();	// refresh file list
	void analyze(); // analyze file in file list
	void checkAlive();	// check whether log file max write time is raising
	 
	// set run loop interval
	void setRefreshInterval(unsigned long ulRefreshInterval);
	
	// pause refresh(), analyze() and checkAlive() method in run loop
	void pause()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"++++++Log Analyser pause() called!!!!!!!" );
		m_bPause = true;
		stopWait();
	}
	
	// continue
	void continues()
	{
		checkAlive();
		m_bPause = false;
		stopWait();
	}
	
	/////////////////////////////////////////////////////////////////
	// status check method
	bool isPaused() const
	{
		return m_bPause;
	}
	bool isTerminate() const
	{
		return m_bTerminate;
	}

private:
	virtual void run();	// virtual method from thread
	
	// SearchKeyInfo struct
	struct SearchKeyInfo
	{
		std::string				strSearchKey;	// key
		std::set<unsigned long>	setLineNum;		// apperance line sequence
		
		bool operator < ( const struct SearchKeyInfo& a ) const
		{
			return ( strSearchKey.compare(a.strSearchKey) < 0 );
		}
		
		bool operator == ( const struct SearchKeyInfo& a ) const
		{
			return ( strSearchKey.compare(a.strSearchKey) == 0 );
		}

		explicit SearchKeyInfo( const std::string& strKey )
			:strSearchKey(strKey)
		{
		}

		SearchKeyInfo():strSearchKey("")
		{
		}
	};

	// SearchKeyList use
	typedef std::map<std::string, pFuncMathchLine> SearchKeyMap;
	typedef std::set<SearchKeyInfo> SearchInfoSet;

	// log file info
	struct LogAnalyseInfo
	{
		std::string				strFileName;	// file name with path
		std::fstream::pos_type	currentPos;		// read pos ( current read line - 1 ) pos
		SearchInfoSet			searchInfo;		// searched key info
		time_t					lastWriteTime;	// file last modify time
		bool					finished;		// has read to eof at last time
		unsigned long			currentLineCnt;	// current read line number - 1
		
		// we sort file with name and file name length
		bool operator < ( const struct LogAnalyseInfo& a ) const
		{
			if ( strFileName.length() == a.strFileName.length() )
			{
				return ( strFileName.compare( a.strFileName ) < 0 );
			}
			else
			{
				return ( strFileName.length() < a.strFileName.length() );
			}
		}
		
		// not used at all
		bool operator == ( const struct LogAnalyseInfo& a ) const
		{
			return ( strFileName.compare(a.strFileName) == 0 );
		}
		
		// constructor
		explicit LogAnalyseInfo( const std::string& strFile, time_t fileTime = 0 )
			:strFileName(strFile), currentPos(std::fstream::beg), lastWriteTime(fileTime), 
			finished(false), currentLineCnt(0)
		{
		}

		// default constructor
		LogAnalyseInfo():strFileName(""), currentPos(std::fstream::beg), lastWriteTime(0), 
			finished(false), currentLineCnt(0)
		{
		}

		// remove a search key info
		void cleanSearchKey( const std::string strSearchKey )
		{
			std::set<SearchKeyInfo>::iterator itInfo = searchInfo.find(SearchKeyInfo(strSearchKey));
			if ( itInfo != searchInfo.end() )
			{
				searchInfo.erase(itInfo);
			}
		}
		// increase a search key
		void addLineCount( const std::string& strSearchKey, unsigned long lineNumber )
		{
			std::set<SearchKeyInfo>::iterator itInfo = searchInfo.find(SearchKeyInfo(strSearchKey));
			if ( itInfo != searchInfo.end() )
			{
				itInfo->setLineNum.insert(lineNumber);
			}
			else
			{
				SearchKeyInfo tmp;
				tmp.strSearchKey = strSearchKey;
				tmp.setLineNum.insert(lineNumber);
				searchInfo.insert(tmp);
			}
		}
		
		// get key count
		unsigned long getKeyLineCount( const std::string& strSearchKey ) const
		{
			std::set<SearchKeyInfo>::const_iterator itInfo = searchInfo.find(SearchKeyInfo(strSearchKey));
			if ( itInfo != searchInfo.end() )
			{
				return itInfo->setLineNum.size();
			}
			return 0;
		}
	};

	typedef std::set<LogAnalyseInfo> LogInfoSet;

private:
	unsigned long createFileList(); // analyze m_strLogFileName, get pathname and file head key
	
	// method to refresh file in path
	unsigned long searchFile( const std::string& strFilePath, const std::string& strFileName );
	// method to analyze file context
	bool analyzeFile( LogAnalyseInfo& logFileInfo );

	// run sleep method
	inline void conditionWait(unsigned long milliseconds)
	{
		m_condTerminate.timedWait(milliseconds);
	}
	inline virtual void stopWait()
	{
		m_condTerminate.signal();
	}

	bool refreshFileInfo( LogAnalyseInfo& logFileInfo );
private:
	LogInfoSet									m_fileList;	// all log file list
	TA_Base_Core::NonReEntrantThreadLockable	m_LockAll;
	SearchKeyMap								m_SearchKeyList;// analyze key
	TA_Base_Core::NonReEntrantThreadLockable	m_LockSearchList;
	std::map<std::string, unsigned long>		m_SearchResultMap;	// search key result
	TA_Base_Core::NonReEntrantThreadLockable	m_LockResult;
	
	std::string									m_logFileName;	// log file name key

	// log file start key, not use as we have process id
	std::string									m_strStartFlag;

	std::string									m_strExeName;
	std::string									m_strProcessID;
	
	// createFileList() set, analyze() use, come from m_logFileName
	std::string									m_strFilePath;	// log file path name
	std::string									m_strFileName;	// log file names

	// log analyze info
	// current valid log file name which contain last start flag, not use
	std::string									m_strStartFileName;
	// last analyze file, not use
	std::string									m_strCurrentReadFile;

	time_t										m_maxWriteTime;	// checkAlive use
	
	// thread use
	unsigned long								m_ulRefreshInterval;	// anaylze file interval
	bool										m_bPause;		//
	bool										m_bTerminate;	//
	TA_Base_Core::Condition						m_condTerminate;	//

};
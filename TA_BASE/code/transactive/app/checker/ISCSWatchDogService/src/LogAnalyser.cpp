#include "LogAnalyser.h"

#include "boost/lexical_cast.hpp"
#include <cerrno>

#include <TLHELP32.H>
//#pragma comment( lib, "toolhelp.lib" )

using TA_Base_Core::DebugUtil;

DWORD getProcessID( const std::string& strExeName )
{
	DWORD nRet = 0xFFFFFFFF;
	HANDLE hProcessSnapshot = ::CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS, 0 );
	if ( hProcessSnapshot != INVALID_HANDLE_VALUE )
	{
		PROCESSENTRY32 processEntry;
		if ( Process32First( hProcessSnapshot, &processEntry ) == TRUE )
		{
			do
			{
				// for Win2000, szExeFile name max length is 18
				// we have to match file head
				if( strExeName.find(processEntry.szExeFile) == 0 )
				{
					nRet = processEntry.th32ProcessID;
					break;
				}
			}while ( Process32Next( hProcessSnapshot, &processEntry ) == TRUE );
		}
		// new SDK support, this one should be used to instead of ::CloseHandle(XXX) if you have it
		//CloseToolhelp32Snapshot( hProcessSnapshot );
		::CloseHandle( hProcessSnapshot );
	}
	return nRet;
}


// Microsoft provide
void TimetToFileTime( time_t t, LPFILETIME pft )
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = (DWORD)(ll >> 32);
}

// FILETIME process unit: 100 nanosecond ( 1/10000000 second ) since January 1, 1601.
// 1601 to 1970 nanosecond: 116444736000000000
void FileTimeToTimet( time_t& t, const FILETIME& fTime )
{
	ULONGLONG ll = (ULONGLONG)fTime.dwHighDateTime << 32 | fTime.dwLowDateTime;
	t = ((ll - 116444736000000000) / 10000000);
}

//////////////////////////////////////////////////////////////////////////
// LogAnalyser method
//////////////////////////////////////////////////////////////////////////
void LogAnalyser::setLogFile( const std::string& fileName )
{
	m_logFileName = fileName;
	createFileList();
}

void LogAnalyser::addSearchKey( const std::string& searchKey, pFuncMathchLine pMethod )
{
	LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Add Search Key:[%s]", searchKey.c_str() );
	std::pair< SearchKeyMap::iterator , bool > setInsertRet;
	{
		// TODO: lock search key list
		TA_Base_Core::ThreadGuard guard(m_LockSearchList);
		setInsertRet = m_SearchKeyList.insert( std::make_pair( searchKey, pMethod ) );
	}
	if ( setInsertRet.second )
	{
		// TODO: Lock result map
		TA_Base_Core::ThreadGuard guard(m_LockResult);
		m_SearchResultMap[searchKey] = 0;
	}
	else
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Add Search falied: Key:[%s] already exist!", searchKey.c_str() );
		return;
	}
	reAnalyze();
}

void LogAnalyser::removeSearchKey( const std::string& searchKey )
{
	LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Remove Search Key:[%s]", searchKey.c_str() );
	// TODO: lock search key list
	TA_Base_Core::ThreadGuard guard(m_LockSearchList);
	m_SearchKeyList.erase(searchKey);
	// m_SearchResultMap do not remove this key
	//	erase from m_SearchKeyList, analyzeFile will not find this key again
}


void LogAnalyser::setFileStartFlag( const std::string& startFlag )
{
	if ( m_strStartFlag.compare(startFlag) != 0 )
	{
		removeSearchKey( m_strStartFlag );
		m_strStartFlag = startFlag;
		addSearchKey( m_strStartFlag );
	}
}

void LogAnalyser::setRefreshInterval( unsigned long ulRefreshInterval )
{
	m_ulRefreshInterval = ulRefreshInterval;
}

void LogAnalyser::setWatchedExeName( const std::string& strExeName )
{
	{
		TA_Base_Core::ThreadGuard guard(m_LockAll);
		m_strExeName = strExeName;
	}
	resetProcessID();
}


/**
*	getLogFileInfo
*
* Debug use function, output log files info
*/
void LogAnalyser::getLogFileInfo() 
{
	int nCount = 0;
	LogInfoSet tmp;
	{
		// TODO: lock here
		TA_Base_Core::ThreadGuard guard( m_LockAll );
		tmp = m_fileList;
	}
	LogInfoSet::const_iterator itLogInfo = tmp.begin();
	for ( ; itLogInfo != tmp.end(); ++itLogInfo, ++nCount )
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "%03d: %s\t ReadPos:%lu, SearchKey count:%d, lastWriteTime:%ld", nCount, 
			itLogInfo->strFileName.c_str(), boost::lexical_cast<unsigned long>( itLogInfo->currentPos ), 
			itLogInfo->searchInfo.size(), itLogInfo->lastWriteTime );
		for( SearchKeyMap::const_iterator itKey = m_SearchKeyList.begin(); itKey != m_SearchKeyList.end(); ++itKey )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "\t\tKey:%s, count:%lu", 
				itKey->first.c_str(), itLogInfo->getKeyLineCount(itKey->first) );
		}
// 		SearchInfoSet::const_iterator itKey = itLogInfo->searchInfo.begin();
// 		for( ; itKey != itLogInfo->searchInfo.end(); ++itKey )
// 		{
// 			std::ostringstream strLineNumber;
// 			std::copy(itKey->setLineNum.begin(), itKey->setLineNum.end(), 
// 				std::ostream_iterator<unsigned long>( strLineNumber, " ") );
// 			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "\t\tKey:%s, line:%s", itKey->strSearchKey.c_str(), strLineNumber.str().c_str() );
// 		}
	}
	
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "\n\n" );
		TA_Base_Core::ThreadGuard guard( m_LockResult );
		for( std::map<std::string, unsigned long>::const_iterator it = m_SearchResultMap.begin();
			it != m_SearchResultMap.end(); ++it )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Search Key:%s, count:%lu", it->first.c_str(), it->second );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Thread method
//////////////////////////////////////////////////////////////////////////
void LogAnalyser::run()
{
	try
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, " LogAnalyser start main loop, Terminate flag:%s",
			(m_bTerminate?"TRUE":"FALSE") );
		int i = 0;
		while( !m_bTerminate )
		{
			if ( !m_bPause )
			{
				refresh();
				analyze();
				if ( ++i == 10 )
				{
					checkAlive();
					i = 0;
				}
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, 
					" LogAnalyser paused...." );
				conditionWait(3600000);	// wait long long time until stopWait() called
				continue;
			}
			conditionWait( m_ulRefreshInterval );
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "LogAnalyser quit run loop!" );
	}
	catch(...)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "LogAnalyser run raise exception!!!" );
	}
}

void LogAnalyser::terminate()
{
	m_bTerminate = true;
	stopWait();
}

//////////////////////////////////////////////////////////////////////////
// File path parse
//////////////////////////////////////////////////////////////////////////

/**
  *	createFileList
  *
  * we need directory access and file search, using boost::filesystem library
  */
// request Lock m_LockAll
unsigned long LogAnalyser::createFileList()
{
	if ( m_logFileName.empty() )
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "log file name is empty!!!");
		return 0;
	}

	std::string strFilePath;
	std::string strFileName;

	size_t pos = m_logFileName.find_last_of("\\");
	if ( std::string::npos == pos )
	{
		pos = m_logFileName.find_last_of("/");
	}
	if ( std::string::npos == pos )
	{
		strFilePath = "";
		strFileName = m_logFileName;
	}
	else
	{
		strFilePath = m_logFileName.substr( 0, pos + 1 );
		strFileName = m_logFileName.substr( pos + 1);
	}
	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "File path is:[%s], File Name is:[%s]", 
		strFilePath.c_str(), strFileName.c_str() );
	if ( strFilePath.empty() )
	{
		return 0;
	}

	unsigned long ulLogFileCount = 0;

	// strFilePath with "\" at the end
	{
		// TODO: lock file path
		TA_Base_Core::ThreadGuard guard(m_LockAll);
		m_strFilePath = strFilePath;
		m_strFileName = strFileName;
	}
	// main loop will call this method
//	ulLogFileCount = searchFile( strFilePath, strFileName );

	return ulLogFileCount;
}

/**
  *	analyze
  *
  * analyze log files, main method for this class
  * using std fstream
  */
// request Lock m_LockAll
void LogAnalyser::analyze()
{
	LogInfoSet tmp;
	{
		// TODO: lock fileList
		TA_Base_Core::ThreadGuard guard(m_LockAll);
		tmp = m_fileList;
	}
	LogInfoSet::iterator itLogInfo = tmp.begin();
	for ( ; itLogInfo != tmp.end(); ++itLogInfo )
	{
		if ( analyzeFile( *itLogInfo ) )
		{
			// TODO: lock fileList
			TA_Base_Core::ThreadGuard guard(m_LockAll);
			LogInfoSet::iterator itUpdate = m_fileList.find( *itLogInfo );
			if ( itUpdate != m_fileList.end() )
			{
					*itUpdate = *itLogInfo;
			}
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Analyze file %s failed!!!", itLogInfo->strFileName.c_str() );
		}
	}
}

// request Lock m_LockSearchList and m_LockAll
bool LogAnalyser::analyzeFile( LogAnalyseInfo& logFileInfo )
{
	// open log file for read
	bool bRet = false;
	if ( logFileInfo.finished )
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "File [%s] has finished and no change, lastModifyTime:%lu, readPos:%lu, lineCnt:%lu", 
			logFileInfo.strFileName.c_str(), logFileInfo.lastWriteTime, 
			boost::lexical_cast<unsigned long>( logFileInfo.currentPos ),
			logFileInfo.currentLineCnt );
		return true;
	}

	std::ifstream logFile( logFileInfo.strFileName.c_str() );
	if ( !logFile.is_open() )
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "open file [%s] failed:%s", logFileInfo.strFileName.c_str(), strerror(errno) );
	}
	else
	{
		// copy SearchKey list here
		SearchKeyMap TmpKeyList;
		{
			// TODO: Lock searchKeyList
			TA_Base_Core::ThreadGuard guard( m_LockSearchList );
			TmpKeyList = m_SearchKeyList;
		}
		logFile.seekg( logFileInfo.currentPos );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "\t Read file [%s], start pos:%lu", 
			logFileInfo.strFileName.c_str(),
			boost::lexical_cast<unsigned long>(logFileInfo.currentPos) );
		std::string strLine;
		unsigned long lineCnt = logFileInfo.currentLineCnt;

		while( !logFile.eof() )
		{
			logFileInfo.currentPos = logFile.tellg();
			logFileInfo.currentLineCnt = lineCnt;

			std::getline( logFile, strLine );
			++lineCnt;

			for( SearchKeyMap::const_iterator itKey = TmpKeyList.begin();
				itKey != TmpKeyList.end(); ++itKey )
			{
// 				if ( strLine.find(*itKey) != std::string::npos )
				if ( itKey->second( strLine, itKey->first ) )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Find Key[%s] in file[%s] at line[%lu]",
						itKey->first.c_str(), logFileInfo.strFileName.c_str(), lineCnt );
					logFileInfo.addLineCount( itKey->first, lineCnt );
					{
						// TODO: lock SearchResultMap for write
						TA_Base_Core::ThreadGuard guard(m_LockResult);
						m_SearchResultMap[itKey->first] += 1;
					}
				}
			}// end of search key for loop

			LOG_GENERIC( SourceInfo, DebugUtil::DebugTrace, "\tRead pos:%lu, Line:%lu", 
				boost::lexical_cast<unsigned long>(logFileInfo.currentPos), 
				logFileInfo.currentLineCnt );

		}// end of read file line while loop
		logFileInfo.finished = true;	// file read has finished
		{
			// TODO: Lock all to update m_maxWriteTime
			TA_Base_Core::ThreadGuard guard( m_LockAll );
			if ( m_maxWriteTime < logFileInfo.lastWriteTime )
			{
				m_maxWriteTime = logFileInfo.lastWriteTime;
			}
		}
		bRet = true;
	}// end of else

	return bRet;
}

// request Lock m_LockAll
void LogAnalyser::refresh()
{
	std::string strTmpPath, strTmpName;
	{
		// TODO: Lock all to read m_strFilePath and m_strFileName
		TA_Base_Core::ThreadGuard guard(m_LockAll);
		strTmpPath = m_strFilePath;
		strTmpName = m_strFileName;
	}
	searchFile( strTmpPath, strTmpName );
}

// request Lock m_LockAll
unsigned long LogAnalyser::searchFile( const std::string& strFilePath, const std::string& strFileName )
{
	unsigned long ulLogFileCount = 0;
	time_t		  lastWriteTime = 0;

	// use windows API get files
	WIN32_FIND_DATA			fileFindInfo;
	HANDLE					hFile;
	HANDLE					hOpenFile;
	FILETIME				ftLastWrite;
	DWORD					dwSizeLow;

	std::string strTmpFileName = strFilePath + "*.*";
	std::string strProcessID = ".NOTEXIST.";
	{
		// TODO: Lock all to check m_strProcessID
		TA_Base_Core::ThreadGuard guard(m_LockAll);
		if ( m_strProcessID.empty() )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, " ProcessID for [%s] do not set, wait for another time!!!", m_strExeName.c_str() );
			return 0;
		}
		strProcessID = "." + m_strProcessID + ".";
	}

	hFile = ::FindFirstFile( strTmpFileName.c_str(), &fileFindInfo );
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( fileFindInfo.cFileName[0] != '.' && 
				!(fileFindInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )	// not dot and directory
			{
				strTmpFileName = strFilePath + fileFindInfo.cFileName;
				std::string::size_type pos = strTmpFileName.find( strFileName );
				// match file name and process id
				if ( pos != std::string::npos && 
					strTmpFileName.substr( pos ).find( strProcessID ) != std::string::npos )
				{
					hOpenFile = ::CreateFile( strTmpFileName.c_str(),
						GENERIC_READ,	// we read file
						FILE_SHARE_WRITE | FILE_SHARE_READ, // other can read/write file
						NULL,	// no security attribute
						OPEN_EXISTING,	// open exists
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED // 
						, NULL );	// 

					if ( hOpenFile !=  INVALID_HANDLE_VALUE )
					{
						::GetFileTime( hOpenFile, NULL, NULL, &ftLastWrite );
						dwSizeLow = ::GetFileSize( hOpenFile, NULL );
						FileTimeToTimet( lastWriteTime, ftLastWrite );
						LogInfoSet::iterator itFile;
						{
							// TODO: lock fileList and m_maxWriteTime
							TA_Base_Core::ThreadGuard guard(m_LockAll);
							if ( lastWriteTime > m_maxWriteTime ) // file write time is newer than maxWriteTime
							{								
								itFile = m_fileList.find( LogAnalyseInfo(strTmpFileName) );
								if ( itFile == m_fileList.end() )
								{
									m_fileList.insert( LogAnalyseInfo( strTmpFileName, lastWriteTime ) );
									LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Add new log file [%s] to anaylse, size:%lu", 
										strTmpFileName.c_str(), dwSizeLow );
									++ulLogFileCount;
								}
								else if ( lastWriteTime > itFile->lastWriteTime )
								{
									LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, " Log file [%s] updated, lastTime:%lu, currentTime:%lu, size:%lu", 
										strTmpFileName.c_str(), itFile->lastWriteTime, lastWriteTime, dwSizeLow );
									itFile->lastWriteTime = lastWriteTime;
									itFile->finished = false;
									++ulLogFileCount;
								}
								else
								{
// 									LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "File [%s] no change", itFile->strFileName.c_str() );
								}
							}
							else
							{
// 								LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "File [%s] too old, ignore, current LastModifyTime:%lu", 
// 									strTmpFileName.c_str(), m_maxWriteTime );
							}
						}// release Lock fileList
						::CloseHandle( hOpenFile );
					}
					else
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, " CreateFile call for [%s] failed:%lu",
							strTmpFileName.c_str(), ::GetLastError() );
					}
				}// end if of match file name
			}
		}while( ::FindNextFile( hFile, &fileFindInfo ) == TRUE );
	}
	::FindClose( hFile );
	
	return ulLogFileCount;
}

// request Lock m_LockAll
void LogAnalyser::resetProcessID()
{
	// TODO: Lock all to read m_strExeName and m_strProcessID
	TA_Base_Core::ThreadGuard guard(m_LockAll);
	m_strProcessID = "";
	unsigned long ulProcessID = getProcessID( m_strExeName );
	if ( ulProcessID == 0xFFFFFFFF )
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "%s is not running, log analyser exit!", m_strExeName.c_str() );
//		terminate();
		ulProcessID = 0;
	}
	else
	{
		m_strProcessID = boost::lexical_cast<std::string>( ulProcessID );
	}
}

// request lock m_LockAll
void LogAnalyser::checkAlive()
{
	static unsigned long ulCount = 0;
	static time_t maxWriteTime = 0;
	static time_t stopTime = 0;
	static int iunChangeCnt = 0;

	DWORD dwProcessID;
	bool bReset = false;
	{
		// TODO: Lock all to read m_strExeName and m_strProcessID
		TA_Base_Core::ThreadGuard guard( m_LockAll );
		dwProcessID = getProcessID( m_strExeName );
		if ( dwProcessID != 0xFFFFFFFF )	// process with name still running
		{
			if ( m_strProcessID.compare( boost::lexical_cast<std::string>( dwProcessID ) ) != 0 )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "%s ProcessID has change, our:[%s], running:[%lu]!", 
					m_strExeName.c_str(), m_strProcessID.c_str(), dwProcessID );
				bReset = true;	// need reAnalyze
			}
			else if ( maxWriteTime == m_maxWriteTime && iunChangeCnt > 0 )	// log file has stop update for a while
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "%s ProcessID[%lu] log file no update for [%d] times, stop detected at:%lu, current time:%lu!!!", 
					m_strExeName.c_str(), dwProcessID, iunChangeCnt, stopTime, time(NULL) );
				++iunChangeCnt;
			}
			else if ( maxWriteTime == m_maxWriteTime && iunChangeCnt == 0 )	// detect log update stop
			{
				stopTime = time(NULL);
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "%s ProcessID[%lu] log file stop update, detected at:%lu!!!", 
					m_strExeName.c_str(), dwProcessID, stopTime );
				++iunChangeCnt;
			}
			else	// everything is all right
			{
				iunChangeCnt = 0;
				maxWriteTime = m_maxWriteTime;
			}
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "%s is not running!!!", m_strExeName.c_str() );
			bReset = true;	// need reAnalyze
// 			terminate();
		}
	}// release m_LockAll
	if ( bReset )
	{
		resetProcessID();
		reAnalyze();
	}

}


// request lock m_LockAll and m_LockResult
void LogAnalyser::reAnalyze()
{
	{// clean file info
		TA_Base_Core::ThreadGuard guard( m_LockAll );
		m_fileList.clear();
	}
	{// clean current result
		TA_Base_Core::ThreadGuard guard( m_LockResult );
		std::map<std::string, unsigned long>::iterator itKeyResult = m_SearchResultMap.begin();
		for ( ; itKeyResult != m_SearchResultMap.end(); ++itKeyResult )
		{
			itKeyResult->second = 0;
		}
	}
}
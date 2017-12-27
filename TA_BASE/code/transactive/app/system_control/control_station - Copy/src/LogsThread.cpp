// ApplyRights.cpp: implementation of the CApplyRights class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "app\system_control\control_station\src\LogsThread.h"
#include "ZipArchive.h"
#include "core\utilities\src\FunctionEntry.h"

#include "core\utilities\src\DebugUtil.h"
//using namespace std;

using TA_Base_Core::DebugUtil;

CLogsThread::CLogsThread(std::string dirStr,std::string timeStr,CTime time_to_clear_file) : 
								m_dirStr(dirStr),
								m_timeStr(timeStr),
								m_time_to_clear_file(time_to_clear_file)
{
}

CLogsThread::~CLogsThread()
{
	
}

void CLogsThread::run()
{
	FUNCTION_ENTRY("LogsThread::run");
	LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "CLogsThread::run()" );
	CZipArchive zipArchive;

	zipArchive.Open( (m_dirStr + "ArchivedLogs" + m_timeStr + ".zip").c_str(), CZipArchive::zipCreate);

	CFileFind finder;
	CFileStatus filestatus;
	BOOL bWorking = finder.FindFile((m_dirStr+"ArchivedLogs" + m_timeStr+"\\*.*").c_str());

	while (bWorking)
			{
				bWorking = finder.FindNextFile();
				
				if(!finder.IsDots() && !finder.IsDirectory() && finder.GetFilePath().Find("zip") == -1 )
				{
				CFile::GetStatus(finder.GetFilePath(), filestatus);
				if( filestatus.m_mtime < m_time_to_clear_file )
				{
				CString filePath = finder.GetFilePath();
				zipArchive.AddNewFile(filePath);

				}
				}
			 }
	
	finder.Close();
	zipArchive.Close();


	{
		std::string deleteFolder = "cmd.exe /c rmdir /s /q "+m_dirStr + "ArchivedLogs" + m_timeStr;
		char *deleteName = (char*)deleteFolder.c_str();
		PROCESS_INFORMATION ProcessInfo2;
		STARTUPINFOA StartupInfo2; //This is an [in] parameter
		ZeroMemory(&StartupInfo2, sizeof(StartupInfo2));
		StartupInfo2.wShowWindow=SW_HIDE;
		if(CreateProcessA(NULL,deleteName ,NULL,NULL,false,CREATE_NO_WINDOW ,NULL,NULL,&StartupInfo2,&ProcessInfo2))
		{
			WaitForSingleObject(ProcessInfo2.hProcess,INFINITE);
			CloseHandle(ProcessInfo2.hThread);
			CloseHandle(ProcessInfo2.hProcess);
		}
		else
		{
			//MessageBoxA("The process could not be started...");
		}
		
		
	}
	FUNCTION_EXIT;
}

void CLogsThread::terminate()
{
}

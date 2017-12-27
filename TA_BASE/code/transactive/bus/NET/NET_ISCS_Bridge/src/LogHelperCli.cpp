/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: $
  * @author 
  * @version $Revision:  $
  * Last modification: $DateTime: $
  * Last modified by:  $Author:  $
  * 
  * This class is C++/CLI Wrapper for Log Utiltiy in ISCS.
  */

#include "stdafx.h"
#include "LogHelperCli.h"

using namespace System::Runtime::InteropServices;

namespace NET_ISCS_Bridge 
{

	LogHelperCli^ LogHelperCli::GetInstance()
	{
		if(m_instance == nullptr)
		{
			m_instance = gcnew LogHelperCli();
		}
		return m_instance;  
	}

	void LogHelperCli::ReleaseInstance()
	{
		//TODO check
		TA_Base_Core::DebugUtil::getInstance().removeInstance();
	}

	LogHelperCli::LogHelperCli()
	{

	}

	void LogHelperCli::Log(const System::String ^ file, Int32 line, NET_ISCS_Bridge::EDebugTypeManaged type, ... array<Object^>^ params  )
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "LOG Function entered");
		TA_Base_Core::DebugUtil::EDebugType level = static_cast<TA_Base_Core::DebugUtil::EDebugType>(type);

		char* chars = (char*)Marshal::StringToHGlobalAnsi((String ^)file).ToPointer();	
		std::string fileName = chars;

		int i =0;
		int managedLevel;
		bool bFoundGerenicLog = false;;
		if(level == TA_Base_Core::DebugUtil::GenericLog )
		{
			bFoundGerenicLog = true;
			managedLevel = (int)(params[i]);
			i++;
		}

		String^ formatString = (String ^) params[i++];
		int arraySize = params->Length -i;
		array<Object ^>^ args = gcnew array< Object^ >(arraySize);
		int j =0;
		for(; i< params->Length && j<arraySize ;i++, j++)
		{
			args[j] = params[i];
		}

		String ^ formatedString = FormatAString(formatString, args); 
		char* arg1 = (char*)Marshal::StringToHGlobalAnsi(formatedString).ToPointer();	

		if(bFoundGerenicLog)
			TA_Base_Core::DebugUtil::getInstance().log(fileName,line,level,managedLevel,arg1);	
		else
			TA_Base_Core::DebugUtil::getInstance().log(fileName,line,level,arg1);	

		Marshal::FreeHGlobal(IntPtr((void*)chars));
		Marshal::FreeHGlobal(IntPtr((void*)arg1));

	}

	void LogHelperCli::Log_Generic(String^ file, Int32 line, ... array<Object^>^ params)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "LOG_Generic Function entered");
		char* chars = (char*)Marshal::StringToHGlobalAnsi((String ^)file).ToPointer();	
		std::string fileName = chars;

		int i =0;
		int managedLevel;
		managedLevel = (int)(params[i++]);
		String^ formatString = (String ^) params[i++];
		int arraySize = params->Length -i;
		array<Object ^>^ args = gcnew array< Object^ >(arraySize);
		int j =0;
		for(; i< params->Length && j<arraySize ;i++, j++)
		{
			args[j] = params[i];
		}

		String ^ formatedString = FormatAString(formatString, args); 
		char* arg1 = (char*)Marshal::StringToHGlobalAnsi(formatedString).ToPointer();	
		TA_Base_Core::DebugUtil::getInstance().logGeneric(fileName,line,managedLevel,arg1);	

		Marshal::FreeHGlobal(IntPtr((void*)chars));
		Marshal::FreeHGlobal(IntPtr((void*)arg1));

	}

	void LogHelperCli::LogBeforeCOTS(String^ cotsname)
	{

	}

	void LogHelperCli::LogAfterCOTS(String^ cotsname)
	{

	}

	String ^ LogHelperCli::FormatAString(String ^format, ...array<Object^> ^args)
	{
		return String::Format(format, args);
	}

	void LogHelperCli::SetFile(String^ name)
	{
		const char* fileName = (const char*)Marshal::StringToHGlobalAnsi((String ^)name).ToPointer();	
		TA_Base_Core::DebugUtil::getInstance().setFile(fileName);
		Marshal::FreeHGlobal(IntPtr((void*)fileName));
	}
	void LogHelperCli::SetMaxSize(Int32 maxSizeInBytes)
	{
		int sizeInBytes = static_cast<int>(maxSizeInBytes);
		TA_Base_Core::DebugUtil::getInstance().setMaxSize(sizeInBytes);
	}

	EDebugLevelManaged  LogHelperCli::GetDebugLevelFromString(String^ levelStr)
	{
		const char* level = (const char*)Marshal::StringToHGlobalAnsi((String ^)levelStr).ToPointer();	
		TA_Base_Core::DebugUtil::EDebugLevel enuLevel =  TA_Base_Core::DebugUtil::getInstance().getDebugLevelFromString(level);
		EDebugLevelManaged managedLevel = static_cast<EDebugLevelManaged>(enuLevel);
		Marshal::FreeHGlobal(IntPtr((void*)level));
		return managedLevel;
	}

	String^ LogHelperCli::GetStringDebugLevel(EDebugLevelManaged level)
	{
		TA_Base_Core::DebugUtil::EDebugLevel unManagedLevel = static_cast<TA_Base_Core::DebugUtil::EDebugLevel>(level);
		char * levelStr = (char *)TA_Base_Core::DebugUtil::getInstance().getStringDebugLevel(unManagedLevel);
		String^ managedLevelStr = gcnew String(levelStr);
		return managedLevelStr;
	}


	void LogHelperCli::SetLevel(EDebugLevelManaged level)
	{
		TA_Base_Core::DebugUtil::EDebugLevel unManagedLevel = static_cast<TA_Base_Core::DebugUtil::EDebugLevel>(level);
		TA_Base_Core::DebugUtil::getInstance().setLevel(unManagedLevel);
	}

	String^ LogHelperCli::GetFileName()
	{
		//LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "GetFileName entered");
		//StackFrame ^ CallStack = gcnew StackFrame(1,true);
		//return CallStack->GetFileName();
		return "";
	}
	
	Int32 LogHelperCli::GetLineNumber()
	{
		//LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "GetLineNumber entered");
		//StackFrame^ CallStack = gcnew StackFrame(1,true);
		//return CallStack->GetFileLineNumber();
		return 1;
	}

}
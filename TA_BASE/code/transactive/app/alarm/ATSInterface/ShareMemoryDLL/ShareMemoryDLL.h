#ifndef SHARE_MEMORY_INCLUDE_
#define SHARE_MEMORY_INCLUDE_
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SHAREMEMORYDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses thisDLL. This way any other project whose source files include this file see 
// SHAREMEMORYDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef SHAREMEMORYDLL_EXPORTS
#define SHAREMEMORYDLL_API __declspec(dllexport)
#else
#define SHAREMEMORYDLL_API __declspec(dllimport)
#endif

#include "DataStructInfo.h"
#include "AlarmProvider.h"

extern ISCS_ALARM_QUEUE* g_pIscsAlarmQueue;
extern MMS_QUEUE* g_pAtsMmsQueue;
extern ACK_QUEUE* g_pAtsAckQueue;


extern IAlarmProvider *g_pIATSAlarm;

class CShareMemoryDLL
{
public:
	CShareMemoryDLL(void);
	~CShareMemoryDLL();


	// initialize or uninitialize interface.
	static void init();
	static void fini();
// 	static void setAppName(std::string appName);
// 	static std::string getAppName();
	//static void initNew();
    
private:
	static std::string m_appName;
// 	static ISCS_ALARM_QUEUE* m_pIscsAlarmQueue;
// 	static ATS_MMS_QUEUE* m_pAtsMmsQueue;
// 	static ATS_ACK_QUEUE* m_pAtsAckQueue;
};
#endif;

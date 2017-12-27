//////////////////////////////////////////////////////////////////////
///		@file		ConfigFileHelper.h
///		@author		zhilin,ouyang
///		@date		2012-01-06 16:44:41
///
///		@brief	    to implement the config file I/O for the system. 
///                 for example read the config file at start.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  CONFIGFILEHELPER_H
#define  CONFIGFILEHELPER_H
#include "CommonData.h"
#include <map>
#include <vector>

NS_BEGIN(TA_AQ_Lib)
class IDatabase;
NS_END(TA_AQ_Lib)

NS_BEGIN(TA_DB_Sync)

class CConfigInfoHelper
{
	typedef struct st_aqm_info_tag
	{
		uint32  uiSubCnt;
		int     nPayloadType;
	}AQMINFO, *PAQMINFO;

private: 
	typedef  std::vector<std::string>                             vecLineT;
	typedef  std::vector<std::string>::const_iterator             vecLinecIterT;

	typedef  std::map<std::string, AQMINFO>                       mapQSubCntT;
	typedef  std::map<std::string, AQMINFO>::iterator             mapQSubCntIterT;
	typedef  std::map<std::string, AQMINFO>::value_type           mapQSubCntValueT;

	typedef  std::map<uint32, PRO_THD*>                           mapQThreadPoolT;
	typedef  std::map<uint32, PRO_THD*>::iterator                 mapQThreadPoolIterT;
	typedef  std::map<uint32, PRO_THD*>::value_type               mapQThreadPoolValueT;
	typedef  std::map<std::string, mapQThreadPoolT*>              mapAllQThreadPoolT;
	typedef  std::map<std::string, mapQThreadPoolT*>::iterator    mapAllQThreadPoolIterT;	 
	typedef  std::map<std::string, mapQThreadPoolT*>::value_type  mapAllQThreadPoolValueT;

	typedef  std::map<std::string, std::string>                   mapConfigDataPairT;
	typedef  std::map<std::string, std::string>::iterator         mapConfigDataPairIterT;
	typedef  std::map<std::string, std::string>::value_type       mapConfigDataPairValueT;


public:
	CConfigInfoHelper(void);
	CConfigInfoHelper(const char* pczFileName);
	CConfigInfoHelper(const std::string& strFileName);
	~CConfigInfoHelper();

public:
	int  initConfigParameter(pg_config_parameter pConfigPara);	
	void setDBConStr(const std::string& strCon);
	int  monitorDBConfig(mapAllQThreadPoolT& rWorkThreadPool, mapAllQThreadPoolT& rNewThreadPool);

private:
	int  _InitDataFromFile(pg_config_parameter pConfigPara);
	int  _MonitorDBConfig(mapAllQThreadPoolT& rWorkThreadPool, mapAllQThreadPoolT& rNewThreadPool);

private:
	int  _LoadConfigFile(vecLineT& vecLines);
	int  _FillCfgData(pg_config_parameter pConfigPara);
	int  _FillMapData(const vecLineT& vecLines);
	int  _ParseLogLevel(pg_config_parameter pConfigPara, const std::string& strValue);
	void _InitLogVariables(pg_config_parameter pConfigPara);	

private:	
	int  _GetHostName(std::string& strHostName);
	int  _GetServerPortFromDB(const std::string& strHostName, pg_config_parameter pConfigPara);
	int  _GetAgentNameFromDB(pg_config_parameter pConfigPara);
	int  _GetAQSysQueueManagerInfoFromDB();
private:
	int   _GetQSubCntFromDB(mapQSubCntT& rMapSubCnt);
	void  _GetExistSubIDs(mapQThreadPoolT* pWorkMap, std::string& strExistSubID);
	int   _GetNewThreadInfoFromDB(const std::string& strQName,  mapQSubCntT& rQsubMap, mapQThreadPoolT*& rPMapPool);
	int   _GetNewThreadInfoFromDB(const std::string& strQName,  std::string& strExistSubID, mapQSubCntT& rQsubMap, mapQThreadPoolT*& rPMapPool);

private:
	void  _InitDB();
	void  _ParseRcvAddr(PRO_THD*& pThd, const std::string& strSubName, const std::string& strAddr);

	DEF_DISABLE_COPY_AND_ASSIGNMENT(CConfigInfoHelper);

private:
	std::string              m_strFileName;  
	mapConfigDataPairT       m_mapValuePair;
	IDatabase*               m_pDatabase;

};

NS_END(TA_DB_Sync)

#endif  // CONFIGFILEHELPER_H





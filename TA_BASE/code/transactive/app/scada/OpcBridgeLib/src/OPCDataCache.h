/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/SCADA_development/TA_BASE/transactive/app/scada/OpcBridgeLib/src/OPCDataCache.h $
  * @author  DarrenS
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2013/05/09 17:18:15 $
  * Last modified by : $Author: builder $
  *
  * OPCDataCache .
  *
  */

#ifndef OPCDATACACHE_H
#define OPCDATACACHE_H

#pragma warning ( disable:4786 )

#include <map>
#include <vector>
#include <string>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"
#include "core/threads/src/Thread.h"
#include "core/data_access_interface/src/OPCEntityDataAccessFactory.h"

namespace TA_Base_App
{
	class OPCDataCacheFile;

	enum EOPCDATAATTRIBUTETYPE
	{
		E_OPC_DATA_ATTR_NONE			= 0,
		E_OPC_DATA_ATTR_DN_LABEL		= 0x1,	// 0000 0000 0000 0001
		E_OPC_DATA_ATTR_DN_SHORTLABEL	= 0x2,	// 0000 0000 0000 0010
		E_OPC_DATA_ATTR_DN_ASSETNAME	= 0x4,	// 0000 0000 0000 0100
		E_OPC_DATA_ATTR_DN_ONTIP		= 0x8,	// 0000 0000 0000 1000
		E_OPC_DATA_ATTR_DN_ACKSTATE		= 0x10,	// 0000 0000 0001 0000
		E_OPC_DATA_ATTR_DN_ALARMSTATE	= 0x20,	// 0000 0000 0010 0000
		E_OPC_DATA_ATTR_DN_QUALITY		= 0x40,	// 0000 0000 0100 0000
		E_OPC_DATA_ATTR_DP_ACKSTATE		= 0x80,	// 0000 0000 1000 0000
		E_OPC_DATA_ATTR_DP_ALARMSTATE	= 0x100,	// 0000 0001 0000 0000
		E_OPC_DATA_ATTR_DP_QUALITY		= 0x200,	// 0000 0010 0000 0000
		E_OPC_DATA_ATTR_DP_VALUE		= 0x400, 	// 0000 0100 0000 0000
		E_OPC_DATA_ATTR_ALL				= 0xffff 	// 1111 1111 1111 1111
	};
    
	struct DataNodeCacheData
	{
		unsigned long entityKey;
		unsigned long agentKey;
		unsigned long locationKey;
		std::string   entityName;
		std::string	  shortLabel;
		std::string   label;
		std::string   assetName;
		unsigned long preCreateAttrs;

		DataNodeCacheData(){};
		DataNodeCacheData(unsigned long paraEntityKey, unsigned long paraAgentKey, 
						unsigned long paraLocationKey, std::string& paraName, std::string& paraShortLabel,
						std::string& paraLabel, std::string& paraAssetName, unsigned long attrs = 0)
		:
		entityKey(paraEntityKey),
		agentKey(paraAgentKey),
		locationKey(paraLocationKey),
		entityName(paraName),
		shortLabel(paraShortLabel),
		label(paraLabel),
		assetName(paraAssetName),
		preCreateAttrs(attrs)
		{
		}
	};

	struct DataPointCacheData
	{
		unsigned long entityKey;
		unsigned long agentKey;
		unsigned long locationKey;
		std::string   entityName;
		unsigned long preCreateAttrs;

		DataPointCacheData(){};
		DataPointCacheData(unsigned long paraEntityKey, unsigned long paraAgentKey, 
						unsigned long paraLocationKey, std::string& paraName, unsigned long attrs = 0)
		:
		entityKey(paraEntityKey),
		agentKey(paraAgentKey),
		locationKey(paraLocationKey),
		entityName(paraName),
		preCreateAttrs(attrs)
		{
		}
	};

	enum OPCCacheDataType
	{
		E_CACHEDATA_UNKNOWN = 0,
		E_CACHEDATA_DN,
		E_CACHEDATA_DP
	};

	struct OPCCacheData
	{
		OPCCacheDataType	type;
		DataNodeCacheData	dnData;
		DataPointCacheData	dpData;
		
		OPCCacheData()
		:
		type(E_CACHEDATA_UNKNOWN)
		{
		}
	};

	struct OPCDynPersistData
	{
		std::string				name;
		EOPCDATAATTRIBUTETYPE	flag;
		OPCDynPersistData(std::string& objName, EOPCDATAATTRIBUTETYPE attrFlag)
		:name(objName), flag(attrFlag)
		{
		}
	};

	typedef std::vector<OPCDynPersistData> OPCDynPersistDataList;

	typedef std::vector<DataNodeCacheData>  DataNodeCacheDataList;
	typedef std::vector<DataPointCacheData> DataPointCacheDataList;

	typedef std::map<std::string, DataNodeCacheData*>	DataNodeCacheDataMap;
	typedef std::map<std::string, DataPointCacheData*>	DataPointCacheDataMap;

	class OPCDataCache : public TA_Base_Core::Thread
	{
	public:

		OPCDataCache();

		virtual ~OPCDataCache();

		void getOPCDnDataAtAllLocations(DataNodeCacheDataList& dnList);
		void getOPCDnDataForDynamicCreate(DataNodeCacheDataList& dnList);
		void getOPCDnDataAtLocations(std::vector<unsigned long>& locationKeys, DataNodeCacheDataList& dnList);
		
		// include is Dynamic in all locations and is not dynamic in parameter locations.
		void getOPCDnDataForDynamicAndLocs(std::vector<unsigned long>& locationKeys, DataNodeCacheDataList& dnList);

		void getOPCDpDataAtAllLocations(DataPointCacheDataList& dpList);
		void getOPCDpDataForDynamicCreate(DataPointCacheDataList& dpList);
		void getOPCDpDataAtLocations(std::vector<unsigned long>& locationKeys, DataPointCacheDataList& dpList);
		
		// include is Dynamic in all locations and is not dynamic in parameter locations.
		void getOPCDpDataForDynamicAndLocs(std::vector<unsigned long>& locationKeys, DataPointCacheDataList& dpList);

		OPCCacheData getOPCCacheData(std::string& objectName);
		void persistDynamicObject(std::string& objectName, const EOPCDATAATTRIBUTETYPE attr);

		void run();
		void terminate();

		//load all locations when vector is empty.
		void loadCache(const std::vector<unsigned long>& locationKeys);
		void deleteCache();

	protected:

		void InitDataFromDB();
		void updateDataNameIndex();
		void mergeDynamicData();
		void CheckDBUpdate();
		void updateDataCacheFile();
		void updateDynamicDataFile();
		bool isLocationsChanged(const std::vector<unsigned long>& locationKeys);	//sorted keys.
		void saveNewDbData(TA_Base_Core::OPCDataNodeDataList* dnList, TA_Base_Core::OPCDataPointDataList* dpList);


		// withing exception throw.
		void loadDbData(TA_Base_Core::OPCDataNodeDataList*& dnList, TA_Base_Core::OPCDataPointDataList*& dpList);
		
    private:

        TA_Base_Core::ReEntrantThreadLockable	m_lock;
		TA_Base_Core::Condition					m_condition;
		std::string		m_cacheFilePath;
		timeb			m_createTime;
		timeb			m_lastCheckDbTime;
		bool			m_bUsingCacheFile;
		volatile bool	m_beRunning;
		volatile bool	m_bDataInited;
		volatile bool	m_cacheUpdated;
		volatile bool	m_dynamicUpdated;


		OPCDataCacheFile*			m_CacheFileHelper;

		std::vector<unsigned long>	m_loadLocations;

		DataNodeCacheDataList	m_dnList;
		DataPointCacheDataList	m_dpList;

		DataNodeCacheDataMap	m_dnMap;
		DataPointCacheDataMap	m_dpMap;
		
		TA_Base_Core::ReEntrantThreadLockable	m_persistlock;
		OPCDynPersistDataList   m_persistList;

		static const std::string OPC_CACHE_FILE_PATH_PARAM;
		static const std::string OPC_PRE_CREATE_FILE;
    };
}

#endif // #ifndef OPCDataCache_H

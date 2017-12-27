/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/SCADA_development/TA_BASE/transactive/app/scada/OpcBridgeLib/src/OPCDataCache.cpp $
  * @author  DarrenS
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2013/05/09 17:18:15 $
  * Last modified by : $Author: builder $
  *
  * OPCDataCache .
  *
  */

#pragma warning ( disable:4786 )

#include <algorithm>
#include <sstream>
#include "ace/OS.h"

#include "OPCDataCache.h"
#include "OPCDataCacheFile.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/data_access_interface/opctag_alias/src/OpcTagAliasAccessFactory.h"
#include "core/data_access_interface/opctag_alias/src/IOpcTagAlias.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;

namespace TA_Base_App
{
	const std::string OPCDataCache::OPC_CACHE_FILE_PATH_PARAM = "OPCCacheFilePath";
	const std::string OPCDataCache::OPC_PRE_CREATE_FILE = "OPCDynamicSave.bin";

	OPCDataCache::OPCDataCache()
		:
		m_CacheFileHelper(0),
		m_bUsingCacheFile(false),
		m_beRunning(false),
		m_bDataInited(false),
		m_cacheUpdated(false),
		m_dynamicUpdated(false)
	{
        FUNCTION_ENTRY("OPCDataCache");

		ftime ( &m_createTime );
		m_lastCheckDbTime.time = 0;

		m_dnList.clear();
		m_dpList.clear();
		m_loadLocations.clear();

		m_CacheFileHelper = new OPCDataCacheFile();

		if (TA_Base_Core::RunParams::getInstance().isSet(OPC_CACHE_FILE_PATH_PARAM.c_str()))
		{
			m_bUsingCacheFile = true;
			m_cacheFilePath = TA_Base_Core::RunParams::getInstance().get(OPC_CACHE_FILE_PATH_PARAM.c_str());
		}

		start();

		FUNCTION_EXIT;
	}

	OPCDataCache::~OPCDataCache()
	{
		FUNCTION_ENTRY("~OPCDataCache()");

		terminateAndWait();
		
        TA_THREADGUARD(m_lock);
		if (m_CacheFileHelper)
		{
			delete m_CacheFileHelper;
			m_CacheFileHelper = 0;
		}

		m_dnList.clear();
		m_dpList.clear();

		FUNCTION_EXIT;
	}

	void OPCDataCache::loadCache(const std::vector<unsigned long>& locationKeys)
	{
		FUNCTION_ENTRY("loadCache");

		TA_THREADGUARD(m_lock);
		std::vector<unsigned long> newLoc;
		newLoc = locationKeys;
		std::sort(newLoc.begin(), newLoc.end());

		bool bLocsChanged = isLocationsChanged(newLoc);
		bool bCacheLoad = false;
		bool bDBLoad = false;
		if (m_bUsingCacheFile && !m_bDataInited && !bLocsChanged)
		{
			std::vector<unsigned long> tLocs;
			if (1 == m_CacheFileHelper->getSerializedData(m_cacheFilePath, tLocs, m_dnList, m_dpList))
			{
				bCacheLoad = true;
			}
		}
		m_loadLocations = newLoc;
		if (!bCacheLoad && bLocsChanged)
		{
			InitDataFromDB();
			bDBLoad = true;
		}
		
		updateDataNameIndex();
		mergeDynamicData();

		m_bDataInited = true;

		m_condition.signal();

		FUNCTION_EXIT;
	}

	void OPCDataCache::deleteCache()
	{
		FUNCTION_ENTRY("deleteCache");

		TA_THREADGUARD(m_lock);
		m_CacheFileHelper->deleteFile(m_cacheFilePath);
		FUNCTION_EXIT;
	}

	void OPCDataCache::terminate()
	{
		m_beRunning = false;
		m_condition.signal();
	}

	void OPCDataCache::getOPCDnDataAtLocations(
		std::vector<unsigned long>& locationKeys,
		DataNodeCacheDataList& dnList)
	{
		FUNCTION_ENTRY("getOPCDnDataAtLocations");
		TA_THREADGUARD(m_lock);

		int sz = m_dnList.size();
		int i = 0;
		for (i = 0; i < sz; ++i)
		{
			bool inLocations = false;
			unsigned long location = m_dnList[i].locationKey;
			for (int j = locationKeys.size() - 1; j >= 0 && !inLocations; --j)
			{
				if (location == locationKeys[j])
				{
					inLocations = true;
				}
			}
			if (inLocations)
			{
				dnList.push_back(m_dnList[i]);
				dnList[dnList.size() - 1].preCreateAttrs = E_OPC_DATA_ATTR_ALL;
			}
		}

		FUNCTION_EXIT;
	}

	void OPCDataCache::getOPCDnDataForDynamicAndLocs(
		std::vector<unsigned long>& locationKeys,
		DataNodeCacheDataList& dnList)
	{
		FUNCTION_ENTRY("getOPCDnDataAtLocations");
		TA_THREADGUARD(m_lock);

		int sz = m_dnList.size();
		bool inLocations = false;
		int i = 0;
		for (i = 0; i < sz; ++i)
		{
			inLocations = false;
			unsigned long location = m_dnList[i].locationKey;
			for (int j = locationKeys.size() - 1; j >= 0 && !inLocations; --j)
			{
				if (location == locationKeys[j])
				{
					inLocations = true;
				}
			}
			if (inLocations)
			{
				dnList.push_back(m_dnList[i]);
				dnList[dnList.size() - 1].preCreateAttrs = E_OPC_DATA_ATTR_ALL;
			}
			else if(E_OPC_DATA_ATTR_NONE != m_dnList[i].preCreateAttrs)
			{
				dnList.push_back(m_dnList[i]);
			}
		}

		FUNCTION_EXIT;
	}

	void OPCDataCache::getOPCDnDataAtAllLocations(DataNodeCacheDataList& dnList)
	{
		FUNCTION_ENTRY("getOPCDnDataAtAllLocations");
		TA_THREADGUARD(m_lock);

		int sz = m_dnList.size();
		dnList.clear();
		dnList.reserve(sz);
		int i = 0;
		for (i = 0; i < sz; ++i)
		{
			dnList.push_back(m_dnList[i]);
			dnList[i].preCreateAttrs = E_OPC_DATA_ATTR_ALL;
		}

		FUNCTION_EXIT;
	}

	void OPCDataCache::getOPCDnDataForDynamicCreate(DataNodeCacheDataList& dnList)
	{
		FUNCTION_ENTRY("getOPCDnDataForDynamicCreate");
		TA_THREADGUARD(m_lock);

		int sz = m_dnList.size();
		int i = 0;
		for (i = 0; i < sz; ++i)
		{
			if (E_OPC_DATA_ATTR_NONE != m_dnList[i].preCreateAttrs)
			{
				dnList.push_back(m_dnList[i]);
			}
		}

		FUNCTION_EXIT;
	}

	void OPCDataCache::getOPCDpDataAtLocations(
		std::vector<unsigned long>& locationKeys,
		DataPointCacheDataList& dpList)
	{
		FUNCTION_ENTRY("getOPCDpDataAtLocations");
		TA_THREADGUARD(m_lock);

		int sz = m_dpList.size();

		int i = 0;
		for (i = 0; i < sz; ++i)
		{
			bool inLocations = false;
			unsigned long location = m_dpList[i].locationKey;
			for (int j = locationKeys.size() - 1; j >= 0 && !inLocations; --j)
			{
				if (location == locationKeys[j])
				{
					inLocations = true;
				}
			}
			if (inLocations)
			{
				dpList.push_back(m_dpList[i]);
				dpList[dpList.size() - 1].preCreateAttrs = E_OPC_DATA_ATTR_ALL;
			}
		}

		FUNCTION_EXIT;
	}

	void OPCDataCache::getOPCDpDataForDynamicAndLocs(
		std::vector<unsigned long>& locationKeys,
		DataPointCacheDataList& dpList)
	{
		FUNCTION_ENTRY("getOPCDpDataAtLocations");
		TA_THREADGUARD(m_lock);

		int sz = m_dpList.size();
		bool inLocations = false;
		int i = 0;
		for (i = 0; i < sz; ++i)
		{
			inLocations = false;
			unsigned long location = m_dpList[i].locationKey;
			for (int j = locationKeys.size() - 1; j >= 0 && !inLocations; --j)
			{
				if (location == locationKeys[j])
				{
					inLocations = true;
				}
			}
			if (inLocations)
			{
				dpList.push_back(m_dpList[i]);
				dpList[dpList.size() - 1].preCreateAttrs = E_OPC_DATA_ATTR_ALL;
			}
			else if(E_OPC_DATA_ATTR_NONE != m_dpList[i].preCreateAttrs)
			{
				dpList.push_back(m_dpList[i]);
			}
		}
		FUNCTION_EXIT;
	}

	void OPCDataCache::getOPCDpDataAtAllLocations(DataPointCacheDataList& dpList)
	{
		FUNCTION_ENTRY("getOPCDpDataAtAllLocations");
		TA_THREADGUARD(m_lock);

		int sz = m_dpList.size();

		dpList.clear();
		dpList.reserve(sz);

		int i = 0;
		for (i = 0; i < sz; ++i)
		{
			dpList.push_back(m_dpList[i]);
			dpList[i].preCreateAttrs = E_OPC_DATA_ATTR_ALL;
		}

		FUNCTION_EXIT;
	}

	void OPCDataCache::getOPCDpDataForDynamicCreate(DataPointCacheDataList& dpList)
	{
		FUNCTION_ENTRY("getOPCDpDataForDynamicCreate");
		TA_THREADGUARD(m_lock);

		int sz = m_dpList.size();
		dpList.clear();
		dpList.reserve(sz);

		int i = 0;
		for (i = 0; i < sz; ++i)
		{
			if (E_OPC_DATA_ATTR_NONE != m_dpList[i].preCreateAttrs)
			{
				dpList.push_back(m_dpList[i]);
			}
		}

		FUNCTION_EXIT;
	}

	OPCCacheData OPCDataCache::getOPCCacheData(std::string& objectName)
	{
		FUNCTION_ENTRY("getOPCEntityData");
		TA_THREADGUARD(m_lock);

		OPCCacheData data;

		DataNodeCacheDataMap::const_iterator it = m_dnMap.find(objectName);
		if (m_dnMap.end() != it)
		{
			data.type = E_CACHEDATA_DN;
			data.dnData = *(it->second);
		}
		else
		{
			DataPointCacheDataMap::const_iterator it = m_dpMap.find(objectName);
			if (m_dpMap.end() != it)
			{
				data.type = E_CACHEDATA_DP;
				data.dpData = *(it->second);
			}
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "find data name[%s] type[%d]", objectName.c_str(), data.type);

		FUNCTION_EXIT;
		return data;
	}

	void OPCDataCache::persistDynamicObject(std::string& objectName, const EOPCDATAATTRIBUTETYPE attr)
	{
		FUNCTION_ENTRY("persistDynamicObject");
		
		if ( m_CacheFileHelper )
		{
			TA_THREADGUARD(m_persistlock);
			m_persistList.push_back(OPCDynPersistData(objectName, attr));
		}

		FUNCTION_EXIT;
	}

	void OPCDataCache::InitDataFromDB()
	{
		FUNCTION_ENTRY("InitDataFromDB");

        TA_THREADGUARD(m_lock);

		TA_Base_Core::OPCDataNodeDataList*  dnList = 0;
		TA_Base_Core::OPCDataPointDataList* dpList = 0;

		loadDbData(dnList, dpList);

		saveNewDbData(dnList, dpList);

		delete dnList;
		delete dpList;

		FUNCTION_EXIT;
	}

	void OPCDataCache::saveNewDbData(TA_Base_Core::OPCDataNodeDataList* dnList, TA_Base_Core::OPCDataPointDataList* dpList)
	{
		if (0 == dnList || 0 == dpList)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "in saveNewDbData, pointer is null.");
			return;
		}
		int i = 0;
		int sz = dnList->size();
		m_dnList.clear();
		m_dnList.reserve(sz);
		for (i = 0; i < sz; ++i)
		{
			TA_Base_Core::OPCDataNodeData& data = dnList->at(i);
			m_dnList.push_back(DataNodeCacheData(
			data.entityKey,
			data.agentKey,
			data.locationKey,
			data.entityName,
			data.shortLabel,
			data.label,
			data.assetName
			));
		}

		sz = dpList->size();
		m_dpList.clear();
		m_dpList.reserve(sz);
		for (i = 0; i < sz; ++i)
		{
			TA_Base_Core::OPCDataPointData& data = dpList->at(i);
			m_dpList.push_back(DataPointCacheData(
			data.entityKey,
			data.agentKey,
			data.locationKey,
			data.entityName
			));
		}

		m_cacheUpdated = true;
	}

	bool OPCDataCache::isLocationsChanged(const std::vector<unsigned long>& locationKeys)
	{
		bool bChanged = false;
		const std::vector<unsigned long>& newLoc = locationKeys;
		std::vector<unsigned long> tLocs;
		if (m_bDataInited)
		{
			tLocs = m_loadLocations;
		}
		else if (m_bUsingCacheFile)
		{
			if (0 == m_CacheFileHelper->getSerializedLocs(m_cacheFilePath, tLocs))
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
					"failed to get locations from cacheFile[%s], so return locations changed.", m_cacheFilePath.c_str());
				bChanged = true;
			}
		}
		else
		{
			bChanged = true;
		}

		if (!bChanged && tLocs.size() != newLoc.size())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "locations diff, new count[%d], cache count[%d].",
						newLoc.size(), tLocs.size());
			bChanged = true;
		}
		else if (!bChanged)
		{
			for (int i = newLoc.size() - 1; i >= 0 && !bChanged; --i)
			{
				if (newLoc[i] != tLocs[i])
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "location diff, new loc[%d], cache loc[%d].",
						newLoc[i], tLocs[i]);
					bChanged = true;
				}
			}
		}
		return bChanged;
	}

	void OPCDataCache::loadDbData(TA_Base_Core::OPCDataNodeDataList*& dnList, TA_Base_Core::OPCDataPointDataList*& dpList)
	{
		FUNCTION_ENTRY("loadDbData");

		TA_Base_Core::OPCEntityDataAccessFactory & dbCon = TA_Base_Core::OPCEntityDataAccessFactory::getInstance();

		try 
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "loadDbData start.");

			if (0 == m_loadLocations.size())
			{
				dnList = dbCon.getOPCDnDataAtAllLocations();
				dpList = dbCon.getOPCDpDataAtAllLocations();
			}
			else
			{
				dnList = dbCon.getOPCDnDataAtLocations(m_loadLocations);
				dpList = dbCon.getOPCDpDataAtLocations(m_loadLocations);
			}
			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "loadDbData finished. DataNode:%lu, DataPoint:%lu", dnList->size(), dpList->size());
		}
		catch ( const TA_Base_Core::DatabaseException & dbe )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", dbe.what() );

			std::ostringstream desc;
			desc << "Caught DatabaseException: Failed to load DataPoint and DataNode Entity List.";
			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
		}
		catch ( const TA_Base_Core::DataException & de )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", de.what() );

			std::ostringstream desc;
			desc << "Caught DataException: Failed to load DataPoint and DataNode Entity List.";
			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
		}
		catch (...)
		{
			std::ostringstream desc;
			desc << "Caught unknown exception: Failed to load DataNode Entity List.";
			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
		}

		FUNCTION_EXIT;
	}

	void OPCDataCache::CheckDBUpdate()
	{
	}

	void OPCDataCache::updateDataNameIndex()
	{
		FUNCTION_ENTRY("updateDataNameIndex");
        TA_THREADGUARD(m_lock);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "start to update data name index.");

		int i = 0;
		int sz = 0;

		m_dnMap.clear();
		sz = m_dnList.size();
		for(i = 0; i < sz; ++i)
		{
			m_dnMap[m_dnList[i].entityName] = &m_dnList[i];
		}

		m_dpMap.clear();
		sz = m_dpList.size();
		for(i = 0; i < sz; ++i)
		{
			m_dpMap[m_dpList[i].entityName] = &m_dpList[i];
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "data name index updated. DataNode size:%lu, DataPoint size:%lu", m_dnMap.size(), m_dpMap.size());

		FUNCTION_EXIT;
	}

	void OPCDataCache::mergeDynamicData()
	{
		FUNCTION_ENTRY("mergeDynamicData");
        TA_THREADGUARD(m_lock);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "start to merge dynamic data.");

		int i = 0;
		int sz = 0;

		PreCreateNodeList dnList, dpList;
		m_CacheFileHelper->getPreCreateNode(OPC_PRE_CREATE_FILE, dnList, dpList);

		sz = dnList.size();
		for(i = 0; i < sz; ++i)
		{
			DataNodeCacheDataMap::const_iterator it = m_dnMap.find(std::string(dnList[i].name));
			if (m_dnMap.end() != it)
			{
				it->second->preCreateAttrs = dnList[i].preCreateAttrs;
			}
		}

		sz = dpList.size();
		for(i = 0; i < sz; ++i)
		{
			DataPointCacheDataMap::const_iterator it = m_dpMap.find(std::string(dpList[i].name));
			if (m_dpMap.end() != it)
			{
				it->second->preCreateAttrs = dpList[i].preCreateAttrs;
			}
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "merge dynamic data finished. DataNode size:%lu, DataPoint size:%lu",
			dnList.size(), dpList.size());

		FUNCTION_EXIT;
	}	

	void OPCDataCache::updateDataCacheFile()
	{
		FUNCTION_ENTRY("updateDataCacheFile");

		if (m_bUsingCacheFile && m_cacheUpdated && m_CacheFileHelper)
		{
			TA_THREADGUARD(m_lock);
			m_cacheUpdated = false;
			m_CacheFileHelper->setSerializedData(m_cacheFilePath, m_loadLocations, m_dnList, m_dpList);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "data cache file [%s] updated.", m_cacheFilePath.c_str());
		}

		FUNCTION_EXIT;
	}

	void OPCDataCache::updateDynamicDataFile()
	{
		FUNCTION_ENTRY("updateDynamicDataFile");
		
		unsigned long persistSize = m_persistList.size();
		
		if (persistSize > 0 && m_CacheFileHelper)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PersistList size:%lu", persistSize);

			OPCDynPersistDataList tmpList;
			{
				TA_THREADGUARD(m_persistlock);
				m_persistList.swap(tmpList);
			}

			TA_THREADGUARD(m_lock);

			bool dynamicUpdated = false;
			for (OPCDynPersistDataList::iterator itPersist = tmpList.begin(); itPersist != tmpList.end(); ++itPersist)
			{
				std::string& objectName = itPersist->name;
				EOPCDATAATTRIBUTETYPE attr = itPersist->flag;
				DataNodeCacheDataMap::const_iterator it = m_dnMap.find(objectName);
				if (m_dnMap.end() != it)
				{
					if (!(attr & it->second->preCreateAttrs))
					{
						it->second->preCreateAttrs = attr | it->second->preCreateAttrs;
						dynamicUpdated = true;
					}
				}
				else
				{
					DataPointCacheDataMap::const_iterator it = m_dpMap.find(objectName);
					if (m_dpMap.end() != it)
					{
						if (!(attr & it->second->preCreateAttrs))
						{
							it->second->preCreateAttrs = attr | it->second->preCreateAttrs;
							dynamicUpdated = true;
						}
					}
				}
			}

			if ( dynamicUpdated )
			{
				m_CacheFileHelper->setPreCreateNode(OPC_PRE_CREATE_FILE, m_dnList, m_dpList);
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "dynamic data file [%s] updated.", OPC_PRE_CREATE_FILE.c_str());
			}
		}

		FUNCTION_EXIT;
	}

	void OPCDataCache::run()
	{
		FUNCTION_ENTRY("run");
		m_beRunning = true;
		while (m_beRunning)
		{
			updateDataCacheFile();
			updateDynamicDataFile();
			CheckDBUpdate();
			if (m_beRunning)
			{
				m_condition.timedWait(1 * 60 * 1000);
			}
		}
		FUNCTION_EXIT;
	}

}


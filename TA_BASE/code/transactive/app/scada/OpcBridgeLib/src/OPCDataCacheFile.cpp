/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/SCADA_development/TA_BASE/transactive/app/scada/OpcBridgeLib/src/OPCDataCacheFile.cpp $
  * @author  DarrenS
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2013/05/09 17:18:15 $
  * Last modified by : $Author: builder $
  *
  * OPCDataCacheFile .
  *
  */

#pragma warning ( disable:4786 )

#include <fstream>
#include "ace/OS.h"
#include <sys/timeb.h>
#include "time.h"

#include "OPCDataCacheFile.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	const std::string OPCDataCacheFile::OPC_DATA_CACHE_BEGIN = "OPCDataCacheBegin";
	const std::string OPCDataCacheFile::OPC_DATA_CACHE_END	 = "OPCDataCacheEnd";
	const std::string OPCDataCacheFile::OPC_DATA_CACHE_VERSION	 = "1.0.0.3";

	void DataNodeSaveData::convertToMe(const DataNodeCacheData& obj)
	{
		entityKey = obj.entityKey;
		agentKey  = obj.agentKey;
		locationKey = obj.locationKey;
		strncpy(entityName, obj.entityName.c_str(), obj.entityName.size());
		strncpy(shortLabel, obj.shortLabel.c_str(), obj.shortLabel.size());
		strncpy(label, obj.label.c_str(), obj.label.size());
		strncpy(assetName, obj.assetName.c_str(), obj.assetName.size());
		preCreateAttrs = obj.preCreateAttrs;
	}

	void DataPointSaveData::convertToMe(const DataPointCacheData& obj)
	{
		entityKey = obj.entityKey;
		agentKey  = obj.agentKey;
		locationKey = obj.locationKey;
		strncpy(entityName, obj.entityName.c_str(), obj.entityName.size());
		preCreateAttrs = obj.preCreateAttrs;
	}

	int OPCDataCacheFile::deleteFile(const std::string& filePath)
	{
		int ret = 0;
		TA_Base_Core::ThreadGuard guard( m_lock );
		if( remove(filePath.c_str()) != 0 )
		{
			ret = 0;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to delete file [%s].", filePath.c_str());
		}
		else
		{
			ret = 1;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, " file [%s] successfully deleted.", filePath.c_str());
		}
		return ret;
	}

	int OPCDataCacheFile::getSerializedData(
		const std::string& filePath,
		std::vector<unsigned long>& locations,
		DataNodeCacheDataList& dnList,
		DataPointCacheDataList& dpList)
	{
		TA_Base_Core::ThreadGuard guard( m_lock );
		int ret = 0;
		std::ifstream myFile(filePath.c_str(), ios::in | ios::binary);
		if (myFile.is_open())
		{
			try
			{
				if(CheckFileContent(myFile))
				{
					myFile.seekg(OPC_DATA_CACHE_BEGIN.size() + OPC_DATA_CACHE_VERSION.size() + 4);
					readLocations(myFile, locations);

					int dnPreCount = 0;
					myFile.read((char*)&dnPreCount, 4);		
					myFile.seekg(sizeof(PreCreateNode) * dnPreCount, ios::cur);

					int dpPreCount = 0;
					myFile.read((char*)&dpPreCount, 4);
					myFile.seekg(sizeof(PreCreateNode) * dpPreCount, ios::cur);

					readDataNode(myFile, dnList);
					readDataPoint(myFile, dpList);
					ret = 1;
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "finished to read data cache file [%s], data node size[%d], datapoint size[%d]",
						filePath.c_str(), dnList.size(), dpList.size());
				}
			}
			catch(std::exception ex)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to read data cache file [%s], exception[%s].", filePath.c_str(), ex.what());
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to read data cache file [%s], unknow exception.", filePath.c_str());
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to open file [%s] to read.", filePath.c_str());
		}
		myFile.close();
		return ret;
	};

	int OPCDataCacheFile::setSerializedData(
		const std::string& filePath,
		const std::vector<unsigned long>& locations,
		const DataNodeCacheDataList& dnList,
		const DataPointCacheDataList& dpList)
	{
		TA_Base_Core::ThreadGuard guard( m_lock );
		m_isPreCreateNode = false;

		return writeAll(filePath, locations, dnList, dpList);
	};

	int OPCDataCacheFile::getSerializedLocs(
		const std::string& filePath,
		std::vector<unsigned long>& locations)
	{
		TA_Base_Core::ThreadGuard guard( m_lock );
		int ret = 0;
		std::ifstream myFile(filePath.c_str(), ios::in | ios::binary);
		if (myFile.is_open())
		{
			try
			{
				if(CheckFileContent(myFile))
				{
					myFile.seekg(OPC_DATA_CACHE_BEGIN.size() + OPC_DATA_CACHE_VERSION.size() + 4);
					readLocations(myFile, locations);
					ret = 1;
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "finished to read data cache file [%s], got [%d] locations",
						filePath.c_str(), locations.size());
				}
			}
			catch(std::exception ex)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to read data cache file [%s], exception[%s].", filePath.c_str(), ex.what());
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to read data cache file [%s], unknow exception.", filePath.c_str());
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to open file [%s] to read.", filePath.c_str());
		}
		myFile.close();
		return ret;
	};

	int OPCDataCacheFile::setPreCreateNode(
		const std::string& filePath,
		const DataNodeCacheDataList& dnList,
		const DataPointCacheDataList& dpList)
	{
		TA_Base_Core::ThreadGuard guard( m_lock );
		m_isPreCreateNode = true;

		std::vector<unsigned long> locations;
		locations.clear();
		
		return writeAll(filePath, locations, dnList, dpList);
	};

	int OPCDataCacheFile::getPreCreateNode(const std::string& filePath,
			PreCreateNodeList& dnList, PreCreateNodeList& dpList)
	{
		TA_Base_Core::ThreadGuard guard( m_lock );
		int ret = 0;
		std::ifstream myFile(filePath.c_str(), ios::in | ios::binary);
		if (myFile.is_open())
		{
			try
			{
				if(CheckFileContent(myFile))
				{
					myFile.seekg(OPC_DATA_CACHE_BEGIN.size() + OPC_DATA_CACHE_VERSION.size() + 4);
					int locCount = 0;
					myFile.read((char*)&locCount, 4);		
					myFile.seekg(sizeof(unsigned long) * locCount, ios::cur);
					readPreCreateNode(myFile, dnList);
					readPreCreateNode(myFile, dpList);
					ret = 1;
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
						"finished to read data cache file [%s], got [%d] datanode preCreate, [%d] datapoints preCreate",
						filePath.c_str(), dnList.size(), dpList.size());
				}
			}
			catch(std::exception ex)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to read data cache file [%s], exception[%s].", filePath.c_str(), ex.what());
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to read data cache file [%s], unknow exception.", filePath.c_str());
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to open file [%s] to read.", filePath.c_str());
		}
		myFile.close();
		return ret;
	};

	int OPCDataCacheFile::writeAll(
		const std::string& filePath,
		const std::vector<unsigned long>& locations,
		const DataNodeCacheDataList& dnList,
		const DataPointCacheDataList& dpList)
	{
		int ret = 0;
		std::ofstream myFile(filePath.c_str(), ios::out | ios::binary);
		if (myFile.is_open())
		{
			try
			{
				writeHead(myFile);
				writeLocations(myFile, locations);
				writePreCreateNode(myFile, dnList);
				writePreCreateNode(myFile, dpList);
				writeDataNode(myFile, dnList);
				writeDataPoint(myFile, dpList);
				writeTail(myFile);
				ret = 1;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "finished to write data cache file [%s]",
					filePath.c_str());
			}
			catch(std::exception ex)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to write data cache file [%s], exception[%s].", filePath.c_str(), ex.what());
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to write data cache file [%s], unknow exception.", filePath.c_str());
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to open file [%s] to write.", filePath.c_str());
		}
		myFile.close();
		return ret;
	};

	bool OPCDataCacheFile::CheckFileContent(std::ifstream& fs)
	{
		bool fileOk = true;
		char buf[1024] = {0};
		fs.read(buf, OPC_DATA_CACHE_BEGIN.size());
		if (OPC_DATA_CACHE_BEGIN != buf)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to check data cache file, cache begin error.");
			return false;
		}
		memset(buf, 0, 1024);
		fs.read(buf, OPC_DATA_CACHE_VERSION.size());
		if (OPC_DATA_CACHE_VERSION != buf)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to check data cache file, cache version error. old[%s], new[%s]",
				buf, OPC_DATA_CACHE_VERSION.c_str());
			return false;
		}

		timeb currentSystemTime;
		ftime ( &currentSystemTime );

		timeb oldTime;
		fs.read((char*)&oldTime.time, 4);
		
		int locCount = 0;
		fs.read((char*)&locCount, 4);		
		fs.seekg(sizeof(unsigned long) * locCount, ios::cur);

		int dnPreCount = 0;
		fs.read((char*)&dnPreCount, 4);		
		fs.seekg(sizeof(PreCreateNode) * dnPreCount, ios::cur);

		int dpPreCount = 0;
		fs.read((char*)&dpPreCount, 4);
		fs.seekg(sizeof(PreCreateNode) * dpPreCount, ios::cur);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
			"in check data cache file, old time[%d], data node preCreate size[%d], datapoint preCreate size[%d].",
			oldTime.time, dnPreCount, dpPreCount);

		int dnCount = 0;
		fs.read((char*)&dnCount, 4);
		fs.seekg(sizeof(DataNodeSaveData) * dnCount, ios::cur);		

		int dpCount = 0;
		fs.read((char*)&dpCount, 4);
		fs.seekg(sizeof(DataPointSaveData) * dpCount, ios::cur);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
			"in check data cache file, old time[%d], data node size[%d], datapoint size[%d].",
			oldTime.time, dnCount, dpCount);
		
		memset(buf, 0, 1024);
		fs.read(buf, OPC_DATA_CACHE_END.size());
		if (OPC_DATA_CACHE_END != buf)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to check data cache file, cache end error.");
			return false;
		}
		return fileOk;
	}

	bool OPCDataCacheFile::writeHead(std::ofstream& out)
	{
		bool ret = true;
		out.write(OPC_DATA_CACHE_BEGIN.c_str(), OPC_DATA_CACHE_BEGIN.size());
		out.write(OPC_DATA_CACHE_VERSION.c_str(), OPC_DATA_CACHE_VERSION.size());
		timeb currentSystemTime;
		ftime ( &currentSystemTime );
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "writeHead current time[%d].", currentSystemTime.time);
		out.write((char*)&currentSystemTime.time, 4);
		return ret;
	}

	bool OPCDataCacheFile::writeTail(std::ofstream& out)
	{
		bool ret = true;
		out.write(OPC_DATA_CACHE_END.c_str(), OPC_DATA_CACHE_END.size());
		return ret;
	}

	bool OPCDataCacheFile::writeLocations(std::ofstream& out, const std::vector<unsigned long>& vect)
	{
		bool ret = true;

		int listSize = m_isPreCreateNode? 0 : vect.size();
		out.write((char *)&listSize, 4);
		if (0 == listSize)
		{
			return ret;
		}
		int bufSize = sizeof(unsigned long) * 1000;
		char* buf = new char[bufSize];

		int i = 0;
		while (i < listSize)
		{
			memset(buf, 0, bufSize);
			unsigned long* pData = (unsigned long*)buf;

			int j = 0;
			while(j < 1000 && i + j < listSize)
			{
				pData[j] = vect[i + j];
				j = j + 1;
			}
			out.write(buf, j * sizeof(unsigned long));
			i = i + 1000;
		}
		delete buf;
		return ret;
	}

	bool OPCDataCacheFile::writeDataNode(std::ofstream& out, const DataNodeCacheDataList& dnList)
	{
		bool ret = true;

		int listSize = m_isPreCreateNode? 0 : dnList.size();
		out.write((char *)&listSize, 4);
		if (0 == listSize)
		{
			return ret;
		}
		int bufSize = sizeof(DataNodeSaveData) * 1000;
		char* buf = new char[bufSize];

		int i = 0;
		while (i < listSize)
		{
			memset(buf, 0, bufSize);
			DataNodeSaveData* pDn = (DataNodeSaveData*)buf;

			int j = 0;
			while(j < 1000 && i + j < listSize)
			{
				pDn[j].convertToMe(dnList[i + j]);
				j = j + 1;
			}
			out.write(buf, j * sizeof(DataNodeSaveData));
			i = i + 1000;
		}
		delete buf;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "finished to write data node size[%d]", dnList.size());
		return ret;
	}

	bool OPCDataCacheFile::writeDataPoint(std::ofstream& out, const DataPointCacheDataList& dpList)
	{
		bool ret = true;

		int listSize = m_isPreCreateNode? 0 : dpList.size();
		out.write((char *)&listSize, 4);
		if (0 == listSize)
		{
			return ret;
		}
		int bufSize = sizeof(DataPointSaveData) * 1000;
		char* buf = new char[bufSize];

		int i = 0;
		while (i < listSize)
		{
			memset(buf, 0, bufSize);
			DataPointSaveData* pData = (DataPointSaveData*)buf;

			int j = 0;
			while(j < 1000 && i + j < listSize)
			{
				pData[j].convertToMe(dpList[i + j]);
				j = j + 1;
			}
			out.write(buf, j * sizeof(DataPointSaveData));
			i = i + 1000;
		}
		delete buf;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "finished to write datapoint size[%d]", dpList.size());
		return ret;
	}

	template< typename t_NodeList>
	bool OPCDataCacheFile::writePreCreateNode(std::ofstream& out, const t_NodeList& nodeList)
	{
		bool ret = true;

		int listSize = 0;
		if (!m_isPreCreateNode)
		{
			out.write((char *)&listSize, 4);
			return ret;
		}
		listSize = nodeList.size();
		int writeCount = 0, writeTotal = 0;
		int i = 0;
		while (i < listSize)
		{
			if (nodeList[i].preCreateAttrs)
			{
				++writeCount;
			}
			++i;
		}
		out.write((char *)&writeCount, 4);
		if (0 == writeCount)
		{
			return ret;
		}
		writeTotal = writeCount;

		int bufSize = sizeof(PreCreateNode) * 1000;
		char* buf = new char[bufSize];

		writeCount = 0;
		memset(buf, 0, bufSize);
		PreCreateNode* pData = (PreCreateNode*)buf;
		i = 0;
		while (i < listSize)
		{
			if (nodeList[i].preCreateAttrs)
			{
				strncpy(pData[writeCount].name, nodeList[i].entityName.c_str(),  nodeList[i].entityName.size());
				pData[writeCount].preCreateAttrs = nodeList[i].preCreateAttrs;
				++writeCount;
				if (1000 == writeCount)
				{
					out.write(buf, bufSize);
					writeCount = 0;
					memset(buf, 0, bufSize);
				}
			}
			++i;
		}
		if (0 < writeCount)
		{
			out.write(buf, writeCount * sizeof(PreCreateNode));
			writeCount = 0;
		}
		delete buf;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "finished to PreCreateNode size[%d]", writeTotal);
		return ret;
	}

	bool OPCDataCacheFile::readLocations(std::ifstream& fs, std::vector<unsigned long>& locations)
	{
		bool ret = true;

		int listSize = 0;
		fs.read((char *)&listSize, 4);
		if (0 == listSize)
		{
			return ret;
		}

		int bufSize = sizeof(unsigned long) * 1000;
		char* buf = new char[bufSize];
		locations.clear();
		locations.reserve(listSize);

		int i = 0;
		while (i < listSize)
		{
			memset(buf, 0, bufSize);
			int readNum = 1000;
			if (i + 1000 > listSize)
			{
				readNum = listSize - i;
			}
			fs.read(buf, readNum * sizeof(unsigned long));

			unsigned long* pObj = (unsigned long*)buf;

			int j = 0;
			while(j < readNum)
			{
				locations.push_back(pObj[j]);
				j = j + 1;
			}

			i = i + 1000;
		}
		delete buf;
		return ret;
	}

	bool OPCDataCacheFile::readDataNode(std::ifstream& fs, DataNodeCacheDataList& dnList)
	{
		bool ret = true;

		int listSize = 0;
		fs.read((char *)&listSize, 4);
		if (0 == listSize)
		{
			return ret;
		}

		int bufSize = sizeof(DataNodeSaveData) * 1000;
		char* buf = new char[bufSize];
		dnList.clear();
		dnList.reserve(listSize);

		int i = 0;
		while (i < listSize)
		{
			memset(buf, 0, bufSize);
			int readNum = 1000;
			if (i + 1000 > listSize)
			{
				readNum = listSize - i;
			}
			fs.read(buf, readNum * sizeof(DataNodeSaveData));

			DataNodeSaveData* pDn = (DataNodeSaveData*)buf;

			int j = 0;
			while(j < readNum)
			{
				dnList.push_back(DataNodeCacheData(
					pDn[j].entityKey,
					pDn[j].agentKey,
					pDn[j].locationKey,
					std::string(pDn[j].entityName),
					std::string(pDn[j].shortLabel),
					std::string(pDn[j].label),
					std::string(pDn[j].assetName),
					pDn[j].preCreateAttrs
					));
				j = j + 1;
			}

			i = i + 1000;
		}
		delete buf;
		return ret;
	}

	bool OPCDataCacheFile::readDataPoint(std::ifstream& fs, DataPointCacheDataList& dpList)
	{
		bool ret = true;

		int listSize = 0;
		fs.read((char *)&listSize, 4);
		if (0 == listSize)
		{
			return ret;
		}

		int bufSize = sizeof(DataPointSaveData) * 1000;
		char* buf = new char[bufSize];

		dpList.clear();
		dpList.reserve(listSize);

		int i = 0;
		while (i < listSize)
		{
			memset(buf, 0, bufSize);
			int readNum = 1000;
			if (i + 1000 > listSize)
			{
				readNum = listSize - i;
			}
			fs.read(buf, readNum * sizeof(DataPointSaveData));

			DataPointSaveData* pObj = (DataPointSaveData*)buf;

			int j = 0;
			while(j < readNum)
			{
				dpList.push_back(DataPointCacheData(
					pObj[j].entityKey,
					pObj[j].agentKey,
					pObj[j].locationKey,
					std::string(pObj[j].entityName),
					pObj[j].preCreateAttrs
					));
				j = j + 1;
			}

			i = i + 1000;
		}
		delete buf;
		return ret;
	}

	template< typename t_NodeList>
	bool OPCDataCacheFile::readPreCreateNode(std::ifstream& fs, t_NodeList& nodeList)
	{
		bool ret = true;

		int listSize = 0;
		fs.read((char *)&listSize, 4);
		if (0 == listSize)
		{
			return ret;
		}

		int bufSize = sizeof(PreCreateNode) * 1000;
		char* buf = new char[bufSize];

		nodeList.clear();
		nodeList.reserve(listSize);

		int i = 0;
		while (i < listSize)
		{
			memset(buf, 0, bufSize);
			int readNum = 1000;
			if (i + 1000 > listSize)
			{
				readNum = listSize - i;
			}
			fs.read(buf, readNum * sizeof(PreCreateNode));

			PreCreateNode* pObj = (PreCreateNode*)buf;

			int j = 0;
			while(j < readNum)
			{
				nodeList.push_back(pObj[j]);
				j = j + 1;
			}

			i = i + 1000;
		}
		delete buf;
		return ret;
	}
}


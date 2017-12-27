/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/SCADA_development/TA_BASE/transactive/app/scada/OpcBridgeLib/src/OPCDataCacheFile.h $
  * @author  DarrenS
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2013/05/09 17:18:15 $
  * Last modified by : $Author: builder $
  *
  * OPCDataCacheFile .
  *
  */

/*
	store data structure:

startstring: 50  byte. eg. OPCDataCacheBegin
version	   : 10  byte. eg. 1.0.0.0
update time: 4   byte. eg. 1454545
locations  : 4   byte. eg. 2
loc1 : sizeof(unsigned long)
loc2 : sizeof(unsigned long)
...
dn preCreate counts: 4 byte.	eg. 2
dn1: sizeof(PreCreateNode).
dn2: sizeof(PreCreateNode).
...
dp preCreate counts: 4 byte.	eg. 2
dp1: sizeof(PreCreateNode).
dp2: sizeof(PreCreateNode).
...
dn counts  : 4   byte. eg. 5555
dn1 : sizeof(DataNodeSaveData).
dn2 : sizeof(DataNodeSaveData).
...
dp counts  : 4   byte. eg. 6666
dp1 : sizeof(DataPointSaveData).
dp2 : sizeof(DataPointSaveData).
...
end string : 50  byte. eg. OPCDataCacheEnd

*/

#ifndef OPCDATACACHEFILE_H
#define OPCDATACACHEFILE_H

#pragma warning ( disable:4786 )

#include <string>
#include <vector>
#include <fstream>
#include "OPCDataCache.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_App
{
	struct DataNodeSaveData
	{
		unsigned long	entityKey;
		unsigned long	agentKey;
		unsigned long	locationKey;
		char	entityName[200];
		char	shortLabel[100];
		char	label[200];
		char	assetName[200];
		unsigned long	preCreateAttrs;

		void convertToMe(const DataNodeCacheData& obj);
	};

	struct DataPointSaveData
	{
		unsigned long	entityKey;
		unsigned long	agentKey;
		unsigned long	locationKey;
		char	entityName[200];
		unsigned long	preCreateAttrs;

		void convertToMe(const DataPointCacheData& obj);
	};

	struct PreCreateNode
	{
		char			name[200];
		unsigned long	preCreateAttrs;
	};
	typedef std::vector<PreCreateNode> PreCreateNodeList;

	class OPCDataCacheFile
	{
	public:

		int getSerializedData(const std::string& filePath,
			std::vector<unsigned long>& locations,
			DataNodeCacheDataList& dnList, DataPointCacheDataList& dpList);

		int setSerializedData(const std::string& filePath,
			const std::vector<unsigned long>& locations,
			const DataNodeCacheDataList& dnList, const DataPointCacheDataList& dpList);
		
		int getSerializedLocs(const std::string& filePath,
			std::vector<unsigned long>& locations);

		int setPreCreateNode(const std::string& filePath,
			const DataNodeCacheDataList& dnList, const DataPointCacheDataList& dpList);
		int getPreCreateNode(const std::string& filePath,
			PreCreateNodeList& dnList, PreCreateNodeList& dpList);

		int deleteFile(const std::string& filePath);

	protected:
		
    private:
		int writeAll(const std::string& filePath,
			const std::vector<unsigned long>& locations,
			const DataNodeCacheDataList& dnList, const DataPointCacheDataList& dpList);

		bool CheckFileContent(std::ifstream& in);
		bool writeHead(std::ofstream& out);
		bool writeTail(std::ofstream& out);
		
		bool writeLocations(std::ofstream& out, const std::vector<unsigned long>& locations);
		bool writeDataNode(std::ofstream& out, const DataNodeCacheDataList& dnList);
		bool writeDataPoint(std::ofstream& out, const DataPointCacheDataList& dpList);
		
		template< typename t_NodeList>
		bool writePreCreateNode(std::ofstream& fs, const t_NodeList& nodeList);

		bool readLocations(std::ifstream& fs, std::vector<unsigned long>& locations);
		bool readDataNode(std::ifstream& fs, DataNodeCacheDataList& dnList);
		bool readDataPoint(std::ifstream& fs, DataPointCacheDataList& dpList);
		
		template< typename t_NodeList>
		bool readPreCreateNode(std::ifstream& fs, t_NodeList& nodeList);


		TA_Base_Core::ReEntrantThreadLockable m_lock;

		bool	m_isPreCreateNode;

		static const std::string OPC_DATA_CACHE_BEGIN;
		static const std::string OPC_DATA_CACHE_END;
		static const std::string OPC_DATA_CACHE_VERSION;
    };
}

#endif // #ifndef 
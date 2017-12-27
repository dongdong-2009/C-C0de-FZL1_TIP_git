#ifndef OPC_ENTITY_DATA_ACCESS_FACTORY_H
#define OPC_ENTITY_DATA_ACCESS_FACTORY_H

#include <string>
#include <vector>
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
	struct OPCDataPointData
	{
		unsigned long entityKey;
		unsigned long agentKey;
		unsigned long locationKey;
		std::string   entityName;
		OPCDataPointData(){}
		OPCDataPointData(unsigned long paraEntityKey, unsigned long paraAgentKey, 
						unsigned long paraLocationKey, std::string& paraName)
		:
		entityKey(paraEntityKey),
		agentKey(paraAgentKey),
		locationKey(paraLocationKey),
		entityName(paraName)
		{
		}
	};

	struct OPCDataNodeData
	{
		unsigned long entityKey;
		unsigned long agentKey;
		unsigned long locationKey;
		std::string   entityName;
		std::string	  shortLabel;
		std::string   label;
		std::string   assetName;
		
		OPCDataNodeData(){}

		OPCDataNodeData(unsigned long paraEntityKey, unsigned long paraAgentKey, 
						unsigned long paraLocationKey, std::string& paraName, std::string& paraShortLabel,
						std::string& paraLabel, std::string& paraAssetName)
		:
		entityKey(paraEntityKey),
		agentKey(paraAgentKey),
		locationKey(paraLocationKey),
		entityName(paraName),
		shortLabel(paraShortLabel),
		label(paraLabel),
		assetName(paraAssetName)
		{
		}
	};

	typedef std::vector<OPCDataPointData> OPCDataPointDataList;
	typedef std::vector<OPCDataNodeData>  OPCDataNodeDataList;	

	class OPCEntityDataAccessFactory
	{
		public:
			OPCEntityDataAccessFactory();
			virtual ~OPCEntityDataAccessFactory();
			static OPCEntityDataAccessFactory& getInstance();
			OPCDataPointDataList* getOPCDpDataAtLocations(std::vector<unsigned long>& locationKeys);
			OPCDataPointDataList* getOPCDpDataAtAllLocations();
			OPCDataNodeDataList* getOPCDnDataAtLocations(std::vector<unsigned long>& locationKeys);
			OPCDataNodeDataList* getOPCDnDataAtAllLocations();
		private:
			struct EntityParameterData
			{
				unsigned long entityKey;
				std::string   parameterName;
				std::string	  parameterValue;
			};
			typedef std::vector<EntityParameterData> EntityParameterDataList;

			void getOPCDpData( IDatabase* pDatabase, const SQLStatement& sql, OPCDataPointDataList& dataList );
			void getOPCDnData( IDatabase* pDatabase, const SQLStatement& sql, const SQLStatement& sqlParameter, 
				OPCDataNodeDataList& dataList, EntityParameterDataList& dnParameterList);
			void assembleDnOPCData(OPCDataNodeDataList& dataList, EntityParameterDataList& dnParameterList);
			static OPCEntityDataAccessFactory* s_instance;
	};
}

#endif
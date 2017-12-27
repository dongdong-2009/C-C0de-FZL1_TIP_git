/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  DarrenS
  * @version $Revision$
  *
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  * AttributeMapManager represents a single map of DATAOBJECT objects created
  * by the application.  Since the list can be accessed by multiple threads, the
  * class provides accessing methods that are thread safe.
  *
  */

#pragma warning ( disable:4786 )

#include <algorithm>
#include <sstream>
#include "ace/OS.h"

#include "app/scada/OpcBridgeLib/src/AttributeMapManager.h"
#include "app/scada/OpcBridgeLib/src/DataNodeAttributes.h"
#include "app/scada/OpcBridgeLib/src/DataPointAttributes.h"
#include "app/scada/OpcBridgeLib/src/OPCBridge.h"
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
	// custom SQL token to retrive all entities corresponding to alias opc tag.
	const std::string AttributeMapManager::SQL_CUSTOM_TOKEN = " and e.pkey IN (SELECT ENTITY_ID_DP FROM SC_OPC_TAG_ALIAS)";

	AttributeMapManager::AttributeMapManager( OpcBridge& opcBridge, OpcTagManager& opcTagManager)
	:
	m_bAlias(false),
	m_opcBridge( opcBridge ),
	m_opcTagManager( opcTagManager ),
	m_preCreateTagEntityCnt(0)
	{
        FUNCTION_ENTRY("AttributeMapManager()");

		Attribute::initDefaultValue();
		
		std::string precreateString = TA_Base_Core::RunParams::getInstance().get("PrecreateOpctag");
		if (0 == precreateString.compare("ALIAS"))
		{
			m_bAlias = true;
		}

		m_proxyManager = DisplayProxyManager::getInstance();
		
		populateDataObjects();

		if ( m_bAlias )
		{
			//need do this call after populateDataObjects
			loadOpcTagAliases();
		}

        FUNCTION_EXIT;
	}


	AttributeMapManager::~AttributeMapManager()
	{
		FUNCTION_ENTRY("~AttributeMapManager()");
		
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);

		m_entityNameMap.clear();
		m_entityKeyAliasMap.clear();

        DataObjectAttributesMap::iterator itr;
		for ( itr = m_dataObjects.begin(); itr != m_dataObjects.end(); ++itr )
		{
			delete itr->second;
			itr->second = NULL;
		}
		m_dataObjects.clear();
		
        for ( AliasMap::iterator aliasItr = m_aliasMap.begin(); aliasItr != m_aliasMap.end(); ++aliasItr )
		{
			delete aliasItr->second;
			aliasItr->second = NULL;
		}
		m_aliasMap.clear();

		FUNCTION_EXIT;
	}


	void AttributeMapManager::dynamicCreateTag( unsigned long entityKey, std::string& tagName, EOPCATTRIBUTETYPE attrType, OPCCacheData& opcEntityData)
	{
		//FUNCTION_ENTRY("dynamicCreateTag");
		if (0 == entityKey) return;

		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "MarkA--dynamicCreateTag, Tag name:%s", tagName.c_str());

		AbstractDataObjectAttributes* pAttribute = NULL;
		{
			ACE_Read_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);
			DataObjectAttributesMap::iterator itObject = m_dataObjects.find(entityKey);
			if ( itObject == m_dataObjects.end() )
			{
				pAttribute = dynamicCreateObjAttribute(opcEntityData);
			}
			else
			{
				pAttribute = itObject->second;
			}
		}

		if (NULL == pAttribute) return;
		
		try
		{	
			Attribute* pObjAttr = pAttribute->insertAttribute(tagName, attrType);
			if (pObjAttr != NULL)
			{
				EOPCDATAATTRIBUTETYPE flag = Attribute::convertAttrToCacheFlag(pObjAttr->getAttributeType()); 
				m_opcDataCache.persistDynamicObject(pAttribute->getObjectName(), flag);
			}
		}
		catch ( ... )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "MarkB--dynamicCreateTag() - Failed to create attribute for tag:%s", tagName.c_str() );
		}

		//FUNCTION_EXIT;
	}


    void AttributeMapManager::remove( unsigned long entityKey, EOPCATTRIBUTETYPE attrType, bool bAlias)
	{
		FUNCTION_ENTRY("remove");

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);

        DataObjectAttributesMap::iterator itr = m_dataObjects.find(entityKey);
       
		if (itr != m_dataObjects.end())
		{
			itr->second->eraseAttribute(attrType, bAlias);
		}
        
		FUNCTION_EXIT;
	}

	void AttributeMapManager::preCreateOpcTags(bool saveTagName)
	{
		unsigned long timeStart = GetTickCount();
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);

		if (saveTagName)
		{
			for (DataObjectAttributesMap::iterator iter = m_dataObjects.begin(); iter != m_dataObjects.end(); ++iter)
			{	
				iter->second->preCreateOpcTags(m_preCreatedOpcTagNames);
				m_preCreateTagEntityCnt++;
			}
		}
		else
		{
			for (DataObjectAttributesMap::iterator iter = m_dataObjects.begin(); iter != m_dataObjects.end(); ++iter)
			{	
				iter->second->preCreateOpcTags();
				m_preCreateTagEntityCnt++;
			}
		}

		unsigned long timeMid = GetTickCount();
		m_preCreatedOpcTagNames.sort();
		unsigned long timeEnd = GetTickCount();

		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "MarkB--Create tag cost time: %lu, sort cost time:%lu", (timeMid-timeStart), (timeEnd-timeMid));
	}

	void AttributeMapManager::preCreateAliasOpcTags()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Begin to PreLoad alias OpcTags");

        ACE_Read_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);

        unsigned long oriPreCreatedOpcTags = m_preCreatedOpcTagNames.size();

        for (EntityKeyAliasMap::iterator itAliasVector = m_entityKeyAliasMap.begin();
				itAliasVector != m_entityKeyAliasMap.end(); ++itAliasVector)
        {
			DataObjectAttributesMap::iterator itr = m_dataObjects.find(itAliasVector->first);
            std::vector<TA_Base_Core::IOpcTagAlias*>& itAlias = itAliasVector->second;

			if (itr != m_dataObjects.end())
			{
				itr->second->preCreateOpcTags(m_preCreatedOpcTagNames);

				for (unsigned long i = 0; i < itAlias.size(); ++ i)
				{
					std::string attrName	   = itAlias[i]->getAttributeName();
					EOPCATTRIBUTETYPE attrType = itr->second->getAttributeType(attrName);
					itr->second->insertAttribute(itAlias[i]->getOpcTagAlias(), attrType, true);
					m_preCreatedOpcTagNames.push_back(itAlias[i]->getOpcTagAlias());
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
						"preCreateAliasOpcTags() - alias tag [%s] for entity %lu was loaded", itAlias[i]->getOpcTagAlias().c_str(), itAliasVector->first );
				}
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, 
							"preCreateAliasOpcTags() - can't find corresponding alias entity %lu in m_dataObjects", itAliasVector->first );
			}
        }

        m_preCreatedOpcTagNames.sort();

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Finished to preload %u OpcTags for alias OpcTags", 
            m_preCreatedOpcTagNames.size() - oriPreCreatedOpcTags);
	}

    bool AttributeMapManager::isPreCreatedOpcTag(const std::string& tagName)
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);
        return m_preCreatedOpcTagNames.end() != std::find(m_preCreatedOpcTagNames.begin(),
                                                          m_preCreatedOpcTagNames.end(), 
                                                          tagName);
    }


    bool AttributeMapManager::isValidTagName(const std::string& tagName, unsigned long& entityKey, 
		EOPCATTRIBUTETYPE& attrType, OPCCacheData& opcEntityData)
    {
		bool isValid = false;

        ACE_Read_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);
      
		std::string dataObjectName = m_opcTagManager.getTagObjectName( tagName );
		opcEntityData = m_opcDataCache.getOPCCacheData(dataObjectName);
		if (E_CACHEDATA_DN == opcEntityData.type)
		{
			std::string attributeName = m_opcTagManager.getTagAttribute( tagName );
			isValid = DataNodeAttributes::isAttributeNameValid(attributeName, attrType);
			entityKey = opcEntityData.dnData.entityKey;
		}
		else if (E_CACHEDATA_DP == opcEntityData.type)
		{
			std::string attributeName = m_opcTagManager.getTagAttribute( tagName );
			isValid = DataPointAttributes::isAttributeNameValid(attributeName, attrType);
			entityKey = opcEntityData.dpData.entityKey;
		}

        return isValid;
    }


	void AttributeMapManager::populateDataObjects()
	{
		FUNCTION_ENTRY("populateDataObjects()");

		bool	bDynamicCreateTag   = false;
		bool	bPreCreateTag		= false;
		bool	bPreCreateTagAlias  = false;

		if (0 == TA_Base_Core::RunParams::getInstance().get("DynamicCreateTag").compare("YES"))
		{
			bDynamicCreateTag = true;
		}

		std::string precreateString = TA_Base_Core::RunParams::getInstance().get("PrecreateOpctag");
		if ( !precreateString.empty() )
		{
			if (0 == precreateString.compare("ALIAS"))
			{
				bPreCreateTagAlias = true;
			}
			else
			{
				bPreCreateTag = true;
			}
		}
	
		try 
		{
			if ( bPreCreateTagAlias )
			{
				unsigned long timeBegin = GetTickCount();
				TA_Base_Core::IEntityDataList dnEntities;
				TA_Base_Core::IEntityDataList dpEntities;
				TA_Base_Core::EntityAccessFactory & entityAccessFactory = TA_Base_Core::EntityAccessFactory::getInstance();
				dnEntities = entityAccessFactory.getEntitiesOfTypeWithCustomToken(TA_Base_Core::DataNodeEntityData::getStaticType(), SQL_CUSTOM_TOKEN, true);
				dpEntities = entityAccessFactory.getEntitiesOfTypeWithCustomToken(TA_Base_Core::DataPointEntityData::getStaticType(), SQL_CUSTOM_TOKEN, false);
				unsigned long timeEnd = GetTickCount();
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--TagAlias,Load DataNode:%lu, DataPoint:%lu, time cost:%lu", dnEntities.size(), dpEntities.size(), timeEnd-timeBegin);
				precreateDataNodeAttribute(dnEntities);
				precreateDataPointAttribute(dpEntities);
			}
			else
			{
				// we need consider both bPreCreateTag and bDynamicCreateTag are true
				DataPointCacheDataList opcDPList;
				DataNodeCacheDataList opcDNList;
				std::vector<unsigned long> locationKeys;
				bool loadAllLocations = false;
				if( bPreCreateTag )
				{
					std::vector<unsigned long> locationKeys;
					loadAllLocations = m_opcBridge.parseLocationString(precreateString, locationKeys);
					if (loadAllLocations)
					{
						std::vector<unsigned long> emptyLocsCache;
						m_opcDataCache.loadCache(emptyLocsCache);
						m_opcDataCache.getOPCDnDataAtAllLocations(opcDNList);
						m_opcDataCache.getOPCDpDataAtAllLocations(opcDPList);
					}
					else if (bDynamicCreateTag)
					{
						std::vector<unsigned long> emptyLocsCache; 
						m_opcDataCache.loadCache(emptyLocsCache);
						m_opcDataCache.getOPCDnDataForDynamicAndLocs(locationKeys, opcDNList);
						m_opcDataCache.getOPCDpDataForDynamicAndLocs(locationKeys, opcDPList);
					}
					else
					{
						m_opcDataCache.loadCache(locationKeys);
						m_opcDataCache.getOPCDnDataAtLocations(locationKeys, opcDNList);
						m_opcDataCache.getOPCDpDataAtLocations(locationKeys, opcDPList);
					}
				}
				else
				{
					std::vector<unsigned long> emptyLocsCache;
					m_opcDataCache.loadCache(emptyLocsCache);
					m_opcDataCache.getOPCDnDataForDynamicCreate(opcDNList);
					m_opcDataCache.getOPCDpDataForDynamicCreate(opcDPList);
				}
				
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Load OPCDataNodeData:%lu, OPCDataPointData:%lu, bDynamic:%d, bPrecreate:%d", 
					opcDNList.size(), opcDPList.size(), bDynamicCreateTag, bPreCreateTag);

				precreateDataNodeAttribute(opcDNList);
				precreateDataPointAttribute(opcDPList);
			}
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

	void AttributeMapManager::precreateDataNodeAttribute(TA_Base_Core::IEntityDataList& dnEntities)
	{
		unsigned long timeBegin = GetTickCount();

		unsigned long preDnAgentKey = 0;
		std::vector<DataNodeAttributes*>* pDnAgentAttr = NULL;
		std::map<unsigned long, std::vector<DataNodeAttributes*> > tmpDnAgentMap;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);

		int sizeBegin = m_dataObjects.size();
		for (TA_Base_Core::IEntityDataList::iterator itr = dnEntities.begin(); itr != dnEntities.end(); ++itr )
		{
			if (*itr == NULL) continue;
			
			unsigned long entityKey = (*itr)->getKey();
			unsigned long locationKey = (*itr)->getLocation();
			unsigned long agentKey	  = (*itr)->getAgent();
		
			if ( locationKey != 0 && agentKey != 0 )
			{
				DataNodeAttributes* dnAttributes = NULL;
				dnAttributes = new DataNodeAttributes(*itr);
				if (dnAttributes != NULL)
				{
					m_dataObjects.insert(std::make_pair(entityKey, dnAttributes));
					if (preDnAgentKey != agentKey)
					{
						preDnAgentKey = agentKey;
						pDnAgentAttr  = &(tmpDnAgentMap[agentKey]);
					}
					pDnAgentAttr->push_back(dnAttributes);
					if ( m_bAlias )
					{
						m_entityNameMap[(*itr)->getName()] = dnAttributes;
					}
				}			
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MarkB--Invalid location 0 or agent 0, has configured DataNode %s", (*itr)->getName().c_str());
			}

			delete (*itr);
		}
		int sizeEnd = m_dataObjects.size();
		m_proxyManager->preSetDnAttributesForAgents(tmpDnAgentMap);

		unsigned long timeEnd = GetTickCount();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--finish create %d DataNodeAttributes, cost time:%u", (sizeEnd - sizeBegin), (timeEnd - timeBegin));
	}

	void AttributeMapManager::precreateDataNodeAttribute(DataNodeCacheDataList& dnDataList)
	{
		unsigned long timeBegin = GetTickCount();
		unsigned long dnLocationKey;
		unsigned long dnAgentKey;
		unsigned long dnEntityKey;
		unsigned long preDnAgentKey = 0;
		std::vector<DataNodeAttributes*>* pDnAgentAttr = NULL;
		std::map<unsigned long, std::vector<DataNodeAttributes*> > tmpDnAgentMap;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);

		int sizeBegin = m_dataObjects.size();
		for (DataNodeCacheDataList::iterator itDnData = dnDataList.begin(); itDnData != dnDataList.end(); ++itDnData )
		{
			dnLocationKey	=	itDnData->locationKey;
			dnAgentKey		=	itDnData->agentKey;
			dnEntityKey		=	itDnData->entityKey;
	
			if ( dnLocationKey != 0 && dnAgentKey != 0 )
			{
				DataNodeAttributes* dnAttributes = NULL;
				dnAttributes = new DataNodeAttributes(*itDnData);
				if (dnAttributes != NULL)
				{
					m_dataObjects.insert(std::make_pair(dnEntityKey, dnAttributes));

					if (preDnAgentKey != dnAgentKey)
					{
						preDnAgentKey = dnAgentKey;
						pDnAgentAttr  = &(tmpDnAgentMap[dnAgentKey]);
					}
					pDnAgentAttr->push_back(dnAttributes);

					if ( m_bAlias )
					{
						m_entityNameMap[itDnData->entityName] = dnAttributes;
					}
				}		
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MarkB--Invalid location 0 or agent 0, has configured DataNode %s", itDnData->entityName.c_str());
			}
		}
		int sizeEnd = m_dataObjects.size();
		unsigned long timeMid = GetTickCount();
		m_proxyManager->preSetDnAttributesForAgents(tmpDnAgentMap);
		unsigned long timeEnd = GetTickCount();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--finish create %d DataNodeAttributes, cost time:%u,%u", (sizeEnd - sizeBegin), (timeMid - timeBegin), (timeEnd - timeMid));
	}

	void AttributeMapManager::precreateDataPointAttribute(TA_Base_Core::IEntityDataList& dpEntities)
	{
		unsigned long timeBegin = GetTickCount();

		unsigned long preDpAgentKey = 0;
		std::vector<DataPointAttributes*>* dpAgentAttr = NULL;
		std::map<unsigned long, std::vector<DataPointAttributes*> > tmpDpAgentMap;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);

		int sizeBegin = m_dataObjects.size();
		for (TA_Base_Core::IEntityDataList::iterator itr = dpEntities.begin(); itr != dpEntities.end(); ++itr )
		{
			if (*itr == NULL) continue;

			unsigned long entityKey = (*itr)->getKey();
			unsigned long agentKey  = (*itr)->getAgent();
			unsigned long locationKey = (*itr)->getLocation();

			if ( agentKey != 0 && locationKey != 0 )
			{
				DataPointAttributes* dpAttributes = NULL;
				dpAttributes = new DataPointAttributes(*itr);
				if (dpAttributes != NULL)
				{
					m_dataObjects.insert(std::make_pair(entityKey, dpAttributes));
					if (preDpAgentKey != agentKey)
					{
						preDpAgentKey = agentKey;
						dpAgentAttr   = &(tmpDpAgentMap[agentKey]);
					}
					dpAgentAttr->push_back(dpAttributes);
					if ( m_bAlias )
					{
						m_entityNameMap[(*itr)->getName()] = dpAttributes;
					}
				}		
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MarkB--Invalid location 0 or agent 0, has configured DataPoint %s", (*itr)->getName().c_str());
			}

			delete (*itr);
		}
		int sizeEnd = m_dataObjects.size();
		m_proxyManager->preSetDpAttributesForAgents(tmpDpAgentMap);

		unsigned long timeEnd = GetTickCount();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--finish create %d DataPointAttributes, cost time:%u", (sizeEnd - sizeBegin), (timeEnd - timeBegin));
	}

	void AttributeMapManager::precreateDataPointAttribute(DataPointCacheDataList& dpDataList)
	{
		unsigned long timeBegin = GetTickCount();

		unsigned long dpLocationKey;
		unsigned long dpAgentKey;
		unsigned long dpEntityKey;
		unsigned long preDpAgentKey = 0;
		std::vector<DataPointAttributes*>* dpAgentAttr = NULL;
		std::map<unsigned long, std::vector<DataPointAttributes*> > tmpDpAgentMap;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);

		int sizeBegin = m_dataObjects.size();
		for (DataPointCacheDataList::iterator itDpData = dpDataList.begin(); itDpData != dpDataList.end(); ++itDpData )
		{
			dpLocationKey = itDpData->locationKey;
			dpAgentKey	  = itDpData->agentKey;
			dpEntityKey	  = itDpData->entityKey;

			if (dpLocationKey != 0 && dpAgentKey != 0 )
			{
				DataPointAttributes* dpAttributes = NULL;
				dpAttributes = new DataPointAttributes(*itDpData);
				if (dpAttributes != NULL)
				{
					m_dataObjects.insert(std::make_pair(dpEntityKey, dpAttributes));

					if (preDpAgentKey != dpAgentKey)
					{
						preDpAgentKey = dpAgentKey;
						dpAgentAttr   = &(tmpDpAgentMap[dpAgentKey]);
					}
					dpAgentAttr->push_back(dpAttributes);

					if ( m_bAlias )
					{
						m_entityNameMap[itDpData->entityName] = dpAttributes;
					}
				}		
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MarkB--Invalid location 0 or agent 0, has configured DataPoint %s", itDpData->entityName.c_str());
			}
		}
		
		unsigned long timeMid = GetTickCount();

		m_proxyManager->preSetDpAttributesForAgents(tmpDpAgentMap);
		int sizeEnd = m_dataObjects.size();

		unsigned long timeEnd = GetTickCount();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--finish create %d DataPointAttributes, cost time:%u,%u", (sizeEnd - sizeBegin), (timeMid - timeBegin), (timeEnd - timeMid));
	}

	AbstractDataObjectAttributes* AttributeMapManager::dynamicCreateObjAttribute(OPCCacheData& opcData)
	{
		// we protect multi-thread accessing on top level
		AbstractDataObjectAttributes* pAttribute = NULL;
		if (E_CACHEDATA_DN == opcData.type)
		{
			DataNodeAttributes* pDnAttr = new DataNodeAttributes(opcData.dnData);
			m_dataObjects.insert(std::make_pair(opcData.dnData.entityKey, pDnAttr));
			m_proxyManager->dynamicSetDnAttribute(pDnAttr);
			pAttribute = pDnAttr;
		}
		else if (E_CACHEDATA_DP == opcData.type)
		{
			DataPointAttributes* pDpAttr = new DataPointAttributes(opcData.dpData);
			m_dataObjects.insert(std::make_pair(opcData.dpData.entityKey, pDpAttr));
			m_proxyManager->dynamicSetDpAttribute(pDpAttr);
			pAttribute = pDpAttr;
		}
		return pAttribute;
	}

    void AttributeMapManager::loadOpcTagAliases(void)
    {
        unsigned long entityKey;
        std::string opcTagAlias;
        std::string attributeName;
        std::vector<TA_Base_Core::IOpcTagAlias*> opcTagAliases;
        
        try
        {
            opcTagAliases = TA_Base_Core::OpcTagAliasAccessFactory::getInstance().getAllOpcTagAliases();
        }
        catch( const TA_Base_Core::DataException& de )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", de.what() );
        }
        catch (const TA_Base_Core::DbConnectionFailed& dbe )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DbConnectionFailed", dbe.what() );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Loading OpcTag Aliases" );
        }

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);
        for ( std::vector<TA_Base_Core::IOpcTagAlias*>::const_iterator itr = opcTagAliases.begin(); itr != opcTagAliases.end(); ++itr )
        {
            std::ostringstream desc;
            
            entityKey = (*itr)->getEntityKey();
            opcTagAlias = (*itr)->getOpcTagAlias();
            attributeName = (*itr)->getAttributeName();
            
            // Make sure that alias point to a valid datapoint/datanode attribute
            DataObjectAttributesMap::const_iterator doItr = m_dataObjects.find(entityKey);
            if ( doItr == m_dataObjects.end() )
            {
                desc << "Alias " << opcTagAlias << " points to invalid entity " << entityKey;
                TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
            }

            // Make sure that alias point to a valid attribute name      
			OPC_ENTITY_TYPE type = doItr->second->getObjectTypeNo();
			EOPCATTRIBUTETYPE attrType;
			if ( (E_OPC_DN == type && !DataNodeAttributes::isAttributeNameValid(attributeName, attrType)) ||
				 (E_OPC_DP == type && !DataPointAttributes::isAttributeNameValid(attributeName, attrType)) )
			{
				desc << "Alias " << opcTagAlias << " points to invalid attribute name " << attributeName;
				TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
			}

            // Check that we don't have duplicate aliases with the same name
            if ( m_aliasMap.find(opcTagAlias) != m_aliasMap.end() )
            {
                desc << "Duplicate OpcTagAlias with same name " << opcTagAlias;
                TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
            }

            // Check that we don't have alias matching datapoint/datanode name
            if ( m_entityNameMap.find(opcTagAlias) != m_entityNameMap.end() )
            {
                desc << "Illegal OpcTagAlias " << opcTagAlias << "; A datapoint/datapoint with the same name exists";

                TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
            }

            m_aliasMap[opcTagAlias] = (*itr);

            //also add alias into a entitykey map for easy search
            if (m_entityKeyAliasMap.find(entityKey) != m_entityKeyAliasMap.end())
            {
                m_entityKeyAliasMap[entityKey].push_back((*itr));
            }
            else
            {
                std::vector<TA_Base_Core::IOpcTagAlias*> alias;
                alias.push_back((*itr));
                m_entityKeyAliasMap[entityKey] = alias;
            }

			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Loaded Alias tags %s -> key:%u-attribute:%s",
                opcTagAlias.c_str(), doItr->second->getEntityKey(), attributeName.c_str() );
        }
    }

    
    void AttributeMapManager::refreshAllTags()
    {
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_dataObjectsMapLock);
        for(DataObjectAttributesMap::iterator iter = m_dataObjects.begin(); iter != m_dataObjects.end(); ++iter)
        {
			iter->second->processProxyUpdate();
        }
    }

	unsigned long AttributeMapManager::getPrecreateTagEntityCnt()
	{
		return m_preCreateTagEntityCnt;
	}

	void AttributeMapManager::deleteOPCCacheFile()
	{
		m_opcDataCache.deleteCache();
	}
}

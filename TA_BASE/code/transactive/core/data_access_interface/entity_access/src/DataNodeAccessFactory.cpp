/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File:$
  * @author:  HoaVu
  * @version: $Revision:$
  *
  * Last modification: $DateTime:$
  * Last modified by:  $Author:$
  *
  * DataNodeAccessFactory is a singleton that is used to retrieve
  * DataNode objects defined in database.
  *
  */

#if defined(_MSC_VER)
#pragma warning ( disable:4786 4503 )
#endif // defined _MSC_VER

#include <sstream>

#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/TransactiveException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/src/IDatabase.h"

#include "core/synchronisation/src/ThreadGuard.h"

#include "core/threads/src/Thread.h"

namespace TA_Base_Core
{
	//
	// Initialisation of static members
	//

    TA_Base_Core::NonReEntrantThreadLockable DataNodeAccessFactory::s_singletonLock;
	DataNodeAccessFactory *	DataNodeAccessFactory::s_instance = NULL;

	//
	// End of static members
	//


	DataNodeAccessFactory::DataNodeAccessFactory()
      : m_agentKey(0),
        m_dnHierarchyMap(NULL)
	{		
	}


	DataNodeAccessFactory::~DataNodeAccessFactory()
	{
        ThreadGuard guard( m_dnHierarchyMapLock );

        delete m_dnHierarchyMap;
		m_dnHierarchyMap = NULL;
        m_entityKeyMap.clear();
        m_entityNameMap.clear();
        m_agentKey = 0;
	}



	DataNodeAccessFactory * DataNodeAccessFactory::getInstance()
	{
		FUNCTION_ENTRY("DataNodeAccessFactory::getInstance ");

        ThreadGuard guard( s_singletonLock );
		
        // if found nothing
		if ( NULL == s_instance )
		{
			// create a new access factory for the specified location
			s_instance = new DataNodeAccessFactory ();
		}

		FUNCTION_EXIT;
		return s_instance;
	}


	void DataNodeAccessFactory::removeInstance()
	{
		FUNCTION_ENTRY("DataNodeAccessFactory::removeInstance ");

        ThreadGuard guard( s_singletonLock );
		
        delete s_instance;
        s_instance = NULL;

		FUNCTION_EXIT;
	}


    void DataNodeAccessFactory::getAllDataNodeEntitiesWithNameLikeToken ( const std::string& token,
                                                                          std::vector< DataNodeEntityData *> & dataNodetEntitiesList )
    {
        std::vector< IEntityData *> theDbEntities;
		theDbEntities.clear();

		// get the list of entities from the database
		try
		{
			theDbEntities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken ( DataNodeEntityData::getStaticType(), token );

			// for each of the db entities returned
			std::vector< IEntityData *>::iterator itr;
			for ( itr = theDbEntities.begin();
				  itr != theDbEntities.end();
				  itr++ )
			{
				// convert the db entity to data node entity
				DataNodeEntityData * dn = 0;
				dn = dynamic_cast < DataNodeEntityData * > ( *itr );

				// push it onto the returned list
				dataNodetEntitiesList.push_back ( dn );
			}
		}
		catch ( ... )
		{
			// do nothing
		}

		theDbEntities.clear();

    }


	void DataNodeAccessFactory::getAllDataNodeEntitiesWithAddressLikeToken ( const std::string& token,
                                                                             std::vector< DataNodeEntityData *> & dataNodeEntitiesList )
	{
        std::vector< IEntityData *> theDbEntities;
		theDbEntities.clear();

		// get the list of entities from the database
		try
		{
			theDbEntities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithAddressLikeToken ( DataNodeEntityData::getStaticType(), token );

			// for each of the db entities returned
			std::vector< IEntityData *>::iterator itr;
			for ( itr = theDbEntities.begin();
				  itr != theDbEntities.end();
				  itr++ )
			{
				// convert the db entity to data point entity
				DataNodeEntityData * dp = 0;
				dp = dynamic_cast < DataNodeEntityData * > ( *itr );

					// push it onto the returned list
					dataNodeEntitiesList.push_back ( dp );
				}
		}
		catch ( ... )
		{
			// do nothing
		}

		theDbEntities.clear();
	}


	void DataNodeAccessFactory::createDataNodeHierarchyMap(unsigned long agentKey, const SharedIEntityDataList& agentEntityList)
	{
		FUNCTION_ENTRY("DataNodeAccessFactory::createDataNodeHierarchyMap");

        //
        // IMPORTANT
        //
        // Please make sure you understand the goal of this before you make any changes
        //
        // Changing how this map is created has the potential (and the very likely possibility)
        // to severely degrade the startup performance and memory usage of any agent hosting datanodes.
        //

        ThreadGuard guard( m_dnHierarchyMapLock );

        // for a different agent?
		if (m_agentKey != agentKey)
		{
			destroyDataNodeHierarchyMap();
			m_agentKey = agentKey;
		}

        // if the same agent, is it created already?
		if ( NULL != m_dnHierarchyMap )
		{
			return;
		}

		// initialise m_dnHierarchyMap
		m_dnHierarchyMap = new DnHierarchyMapType;
		DataNodeHierarchyInformation temp;



        //------------------------------------------------------
        // Populate the map with entities owned by this agent
        //------------------------------------------------------

        // put each child datanode entity in the map
        // also index datapoint and datanode entity data into local maps by entity key
        
        SharedIEntityDataMap agentDataNodes;
        SharedIEntityDataMap agentDataPoints;
        EntityMapByName entityMapByName;

        {
            SharedIEntityDataList::const_iterator entityIter;

            for ( entityIter = agentEntityList.begin(); entityIter != agentEntityList.end(); ++entityIter)
            {
                // check the entity type
                if ((*entityIter)->getType() == DataNodeEntityData::getStaticType() )
                {
                    DataNodeEntityDataPtr dn = boost::dynamic_pointer_cast<DataNodeEntityData>(*entityIter);
                    TA_ASSERT(dn.get() != NULL, "Cant cast to correct type");

                    // insert a copy of temp into the map (after setting hierarchy type)
                    temp.name = dn->getName();
                    temp.synthesisType = dn->getSynthesisTypeEnum();
                    m_dnHierarchyMap->insert( DnHierarchyMapType::value_type( (*entityIter)->getKey(), temp) );

                    // index locally by pkey
                    agentDataNodes.insert( SharedIEntityDataMap::value_type((*entityIter)->getKey(), *entityIter) );
                    // and by name
                    entityMapByName.insert( EntityMapByName::value_type( (*entityIter)->getName(), *entityIter ) );
                }
                else if ((*entityIter)->getType() == DataPointEntityData::getStaticType() )
                {
                    // index locally by pkey
                    agentDataPoints.insert( SharedIEntityDataMap::value_type((*entityIter)->getKey(), *entityIter) );
                    // and by name
                    entityMapByName.insert( EntityMapByName::value_type( (*entityIter)->getName(), *entityIter ) );
                }

                m_entityKeyMap.insert( EntityMapByKey::value_type( (*entityIter)->getKey(), *entityIter ) );
                m_entityNameMap.insert( EntityMapByName::value_type( (*entityIter)->getName(), *entityIter) );

                // otherwise we dont care for this entity type
            }


            // now go through the map again
            // and those with parent key set to an entity in the hierarchy map
            // need to be added as children
            for ( entityIter = agentEntityList.begin(); entityIter != agentEntityList.end(); ++entityIter)
            {
                addChildEntityToHierarchyMap(*entityIter, (*entityIter)->getParent(), DataNodeEntityData::Hierarchy);
            }
        }

		//Load all of the datanode was not in this agent.
		{
       		std::vector< std::string > notFoundParentEntityNameList;
			EntityKeyNameMap entityKeyNameMap;
	        for (EntityMapByKey::iterator iterMapKey = m_entityKeyMap.begin();
	        iterMapKey != m_entityKeyMap.end(); ++iterMapKey)
	        {			
	        	unsigned long  parentKey = iterMapKey->second->getParent();
				
		        EntityMapByKey::iterator itFindParentKey = m_entityKeyMap.find(parentKey);
		        
		        if (itFindParentKey == m_entityKeyMap.end())
		        {
					entityKeyNameMap.insert( EntityKeyNameMap::value_type(parentKey, iterMapKey->second->getParentName()) );
		        }
  			}

	        for (EntityKeyNameMap::iterator iterKeyName = entityKeyNameMap.begin();
	        iterKeyName != entityKeyNameMap.end(); ++iterKeyName)
	        {			
				notFoundParentEntityNameList.push_back(iterKeyName->second);
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "DataNode %s  need to load for entityData.",iterKeyName->second.c_str());
	        }

			// load the entities
			SharedIEntityDataList listEntities = 
				EntityAccessFactory::getInstance().getEntitiesByName(notFoundParentEntityNameList);
			
			// add them to the map
			SharedIEntityDataList::iterator listIter;
			for ( listIter = listEntities.begin(); listIter != listEntities.end(); ++listIter )
			{
				m_entityKeyMap.insert( EntityMapByKey::value_type( (*listIter)->getKey(), *listIter ) );
				m_entityNameMap.insert( EntityMapByName::value_type( (*listIter)->getName(), *listIter ) );
			}			
		}

        {
            //------------------------------------------------------
            // Populate the map with entities not owned by this agent
            // but are children of entities owned by this agent
            // by hierarchy (parent key)
            //------------------------------------------------------

            // load all children of this agent's datanodes
            // But, there is no need to load anything that is already loaded
            // so note the true flag - or you will get duplicates
            SharedIEntityDataList childrenNotFromThisAgent =
                EntityAccessFactory::getInstance().getAllChildEntitiesOfAgentDataNode(m_agentKey, true);

            // add these to the hierarchy map
            // check the synthesis type before adding (hierarchy or both)
            SharedIEntityDataList::iterator listIter;
            for ( listIter = childrenNotFromThisAgent.begin(); listIter != childrenNotFromThisAgent.end(); ++listIter)
            {
                m_entityKeyMap.insert( EntityMapByKey::value_type( (*listIter)->getKey(), *listIter ) );
                m_entityNameMap.insert( EntityMapByName::value_type((*listIter)->getName(), *listIter ) );
                entityMapByName.insert( EntityMapByName::value_type( (*listIter)->getName(), *listIter ) );
                addChildEntityToHierarchyMap(*listIter, (*listIter)->getParent(), DataNodeEntityData::Hierarchy);
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "insert entity[%lu-%s] to m_entityKeyMap",(*listIter)->getKey(),(*listIter)->getName().c_str());
				
            }
        }


        //------------------------------------------------------
        // Populate the map based on list synthesis
        //------------------------------------------------------

        //
        // Now for those that are list or both synthesis type
        // get the names of the children
        //

        // maps entity names to parent datanode(s)
        typedef std::multimap< std::string, unsigned long > ChildEntityNameMap;
        ChildEntityNameMap childEntityNameMap;
        std::vector< std::string > childEntityNameList;

        {
            SharedIEntityDataMap::iterator mapIter;
            for ( mapIter = agentDataNodes.begin(); mapIter != agentDataNodes.end(); ++mapIter)
            {
                DataNodeEntityDataPtr dn = boost::dynamic_pointer_cast<DataNodeEntityData>(mapIter->second);
                TA_ASSERT(dn.get() != NULL, "Cant cast to correct type");
                DataNodeEntityData::ESynthesisType synthesisType = dn->getSynthesisTypeEnum();
        
                if ( (synthesisType == DataNodeEntityData::List) ||
                     (synthesisType == DataNodeEntityData::Both) )
                {
                    std::vector<std::string> childEntityNames = dn->getParsedListOfChildEntities();
                    unsigned long key = dn->getKey();
            
                    for (std::vector<std::string>::iterator nameIter = childEntityNames.begin();
                    nameIter != childEntityNames.end(); ++nameIter)
                    {
                        // before we load, lets see if its already loaded
                        EntityMapByName::iterator findIter = entityMapByName.find(*nameIter);
                        if (findIter != entityMapByName.end())
                        {
                            // already loaded
                            // add directly to the map
                            addChildEntityToHierarchyMap(findIter->second, key, DataNodeEntityData::List);
                        }
                        else
                        {
                            // needs loading
                            childEntityNameMap.insert( std::pair<std::string, unsigned long>(*nameIter, key) );
                            childEntityNameList.push_back(*nameIter);
                        }
                    }
                }
            }

        }


        {
            // load the entities
            SharedIEntityDataList listEntities = 
                EntityAccessFactory::getInstance().getEntitiesByName(childEntityNameList);

            // add them to the map
            SharedIEntityDataList::iterator listIter;
            for ( listIter = listEntities.begin(); listIter != listEntities.end(); ++listIter )
            {
                m_entityKeyMap.insert( EntityMapByKey::value_type( (*listIter)->getKey(), *listIter ) );
                m_entityNameMap.insert( EntityMapByName::value_type( (*listIter)->getName(), *listIter ) );
                // find the entity keys to add to
                std::pair<ChildEntityNameMap::iterator, ChildEntityNameMap::iterator> range = 
                    childEntityNameMap.equal_range( (*listIter)->getName() );

                for (ChildEntityNameMap::iterator rangeIter = range.first; rangeIter != range.second; ++rangeIter)
                {
                    addChildEntityToHierarchyMap(*listIter, rangeIter->second, DataNodeEntityData::List);
                }
            }
        }

        // trust me, you dont wanna enable this unless you are debugging map creation
        //logHierarchyMap();

		FUNCTION_EXIT;
	}


    void DataNodeAccessFactory::addChildEntityToHierarchyMap( IEntityDataPtr entity,
                                                              unsigned long parentKey,
                                                              DataNodeEntityData::ESynthesisType synthesisMatchType )
    {
        // find the entry in the hierarchy map
        DnHierarchyMapType::iterator findIter = m_dnHierarchyMap->find( parentKey );
        if (findIter != m_dnHierarchyMap->end())
        {
            // check the type of synthesis matches
            DataNodeEntityData::ESynthesisType synthesisType = findIter->second.synthesisType;
            if ( (synthesisType == synthesisMatchType) ||
                 (synthesisType == DataNodeEntityData::Both) )
            {
                // add to the right list
                if (entity->getType() == DataNodeEntityData::getStaticType() )
                {
                    findIter->second.childDataNodes.push_back(entity);
                }
                else if (entity->getType() == DataPointEntityData::getStaticType() )
                {
                    findIter->second.childDataPoints.push_back(entity);
                }
            }
        }
    }


    void DataNodeAccessFactory::logHierarchyMap()
    {
        if (m_dnHierarchyMap == NULL)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
            "Data Node Hierarchy Map is NULL" );
			return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
            "Data Node Hierarchy Map Loaded..." );

        for (DnHierarchyMapType::iterator iter = m_dnHierarchyMap->begin();
        iter != m_dnHierarchyMap->end(); ++iter)
        {
            std::string type;
            switch (iter->second.synthesisType)
            {
            case DataNodeEntityData::Hierarchy:
                type = "Hierarchy";
                break;
            case DataNodeEntityData::List:
                type = "List";
                break;
            case DataNodeEntityData::Both:
                type = "Both";
                break;
            case DataNodeEntityData::None:
                type = "None";
                break;
            default:
                type = "Unknown";
                break;
            }

            // print the name, the synthesis type, and the child entities
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                "Data Node %s, synthesis type %s, children:",
                iter->second.name.c_str(), type.c_str());

            // child DN
            SharedIEntityDataList::iterator childIter;
            for (childIter = iter->second.childDataNodes.begin();
            childIter != iter->second.childDataNodes.end(); ++childIter)
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                    "    %s", (*childIter)->getName().c_str());
            }

            // child DP
            for (childIter = iter->second.childDataPoints.begin();
            childIter != iter->second.childDataPoints.end(); ++childIter)
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                    "    %s", (*childIter)->getName().c_str());
            }
        }
    }
	

	void DataNodeAccessFactory::destroyDataNodeHierarchyMap()
	{
		FUNCTION_ENTRY("destroyDataNodeHierarchyMap");
		delete m_dnHierarchyMap;
        m_dnHierarchyMap = NULL;
        
		FUNCTION_EXIT;
	}

	bool DataNodeAccessFactory::isDataNodeHierarchyMapInitForAgent(unsigned long agentKey)
	{
		FUNCTION_ENTRY("isDataNodeHierarchyMapInit");
		FUNCTION_EXIT;
		return ( (m_dnHierarchyMap != NULL) && (m_agentKey == agentKey) );
	}

	SharedIEntityDataList DataNodeAccessFactory::getChildDataPoints(unsigned long dnKey)
	{
		FUNCTION_ENTRY("getChildDataPoint");
		
		if (m_dnHierarchyMap)
		{
			DnHierarchyMapType::iterator dnCacheIter;
			dnCacheIter = m_dnHierarchyMap->find(dnKey);
			if (dnCacheIter != m_dnHierarchyMap->end()) 
			{
				FUNCTION_EXIT;
				return (dnCacheIter->second).childDataPoints;
			}
		}
		
		SharedIEntityDataList ret;
		ret.clear();

		FUNCTION_EXIT;
		return ret;
	}

	SharedIEntityDataList DataNodeAccessFactory::getChildDataNodes(unsigned long dnKey)
	{
		FUNCTION_ENTRY("getChildDataNode");
		
		if (m_dnHierarchyMap)
		{
			DnHierarchyMapType::iterator dnCacheIter;
			dnCacheIter = m_dnHierarchyMap->find(dnKey);
			if (dnCacheIter != m_dnHierarchyMap->end()) 
			{
				FUNCTION_EXIT;
				return (dnCacheIter->second).childDataNodes;
			}
		}
		
		SharedIEntityDataList ret;
		ret.clear();
		
		FUNCTION_EXIT;
		return ret;
	}

    TA_Base_Core::IEntityDataPtr DataNodeAccessFactory::getEntityData(unsigned long entityKey)
    {
        ThreadGuard guard( m_dnHierarchyMapLock );
        EntityMapByKey::iterator it = m_entityKeyMap.find(entityKey);
        
        if (it != m_entityKeyMap.end())
        {
            return it->second;
        }

        TA_Base_Core::IEntityDataPtr entityData;
        return entityData;
    }

    TA_Base_Core::IEntityDataPtr DataNodeAccessFactory::getEntityData(const std::string& entityName)
    {
        ThreadGuard guard( m_dnHierarchyMapLock );
        EntityMapByName::iterator it = m_entityNameMap.find(entityName);
        
        if (it != m_entityNameMap.end())
        {
            return it->second;
        }

        TA_Base_Core::IEntityDataPtr entityData;
        return entityData;
    }

} // closes TA_Base_Core

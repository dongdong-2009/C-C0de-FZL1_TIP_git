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

#if !defined(DataNodeAccessFactory_774454B7_9ED6_4e4d_BB19_917B0DC71137__INCLUDED_)
#define DataNodeAccessFactory_774454B7_9ED6_4e4d_BB19_917B0DC71137__INCLUDED_

#include <string>
#include <map>
#include <vector>

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class IData;

    class DataNodeAccessFactory
	{
	public:

		static DataNodeAccessFactory * getInstance ( );
		static void removeInstance ();
		
		void getAllDataNodeEntitiesWithNameLikeToken ( const std::string& token, 
                                                       std::vector< DataNodeEntityData *> & dataNodetEntitiesList );

		void getAllDataNodeEntitiesWithAddressLikeToken ( const std::string& token,
                                                          std::vector< DataNodeEntityData *> & dataNodeEntitiesList );

		/**
		 * createDataNodeHierarchyMap
		 *
         * Creates a map of all datapoints and datanodes by name and by key
         * Its used to extract hierarchy and corba name information
         *
         * Remember to call destroyDataNodeHierarchyMap() when use is complete
         * Shared pointers are used, so it is safe to destroy the map if entity
         * data is given to other objects.
         *
         * @param agentKey the agent to create the map for
         * @param agentEntityList the list of entities already loaded for this agent by generic agent
         *        prevents double loading of entities.
         */
        void createDataNodeHierarchyMap(unsigned long agentKey,
                                        const SharedIEntityDataList& agentEntityList);
		
        /**
         * destroyDataNodeHierarchyMap
         *
         * Deletes the map created in createDataNodeHierarchyMap
         */
        void destroyDataNodeHierarchyMap();


        /** 
          * isDataNodeHierarchyMapInitForAgent
          *
          * has createDataNodeHierarchyMap been called?
          * does the map still exist?
          *
          * @param agentKey this agent key
          *
          * @return true if the map exists
          */
        bool isDataNodeHierarchyMapInitForAgent(unsigned long agentKey);


        /** 
          * getChildDataPoints
          *
          * gets the child data point entities for a datanode
          *
          * @param dnKey the datanode key
          *
          * @return the child entity data
          */
		SharedIEntityDataList  getChildDataPoints(unsigned long dnKey);


        /** 
          * getChildDataPoints
          *
          * gets the child data node entities for a datanode
          *
          * @param dnKey the datanode key
          *
          * @return the child entity data
          */
		SharedIEntityDataList  getChildDataNodes(unsigned long dnKey);

        /**
          * getEntityData
          *
          * gets the entity data any entity that had been loaded after createDataNodeHierarchyMap for the agent
          *
          * @param entityKey
          *
          * @return the entity data
          */
        TA_Base_Core::IEntityDataPtr getEntityData(unsigned long entityKey);

        /**
          * getEntityData
          *
          * gets the entity data any entity that had been loaded after createDataNodeHierarchyMap for the agent
          *
          * @param entityName
          *
          * @return the entity data
          */
        TA_Base_Core::IEntityDataPtr getEntityData(const std::string& entityName);

	private:

        struct DataNodeHierarchyInformation
        {
            // the name
            std::string name;
            // the synthesis type
            DataNodeEntityData::ESynthesisType synthesisType;

			SharedIEntityDataList childDataPoints;
			SharedIEntityDataList childDataNodes;

			DataNodeHierarchyInformation()
			{
				childDataPoints.clear();
				childDataNodes.clear();
			};
		};


        // helper for createDataNodeHierarchyMap
        void addChildEntityToHierarchyMap( IEntityDataPtr entity,
                                           unsigned long parentKey,
                                           DataNodeEntityData::ESynthesisType synthesisMatchType );

        // for debugging
        void logHierarchyMap();

        static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;
        static DataNodeAccessFactory * s_instance;
		
        TA_Base_Core::NonReEntrantThreadLockable m_dnHierarchyMapLock;
		unsigned long m_agentKey;

        typedef std::map< unsigned long, DataNodeHierarchyInformation > DnHierarchyMapType;
		DnHierarchyMapType * m_dnHierarchyMap;

        typedef std::map< std::string, IEntityDataPtr > EntityMapByName;
        typedef std::map< unsigned long, IEntityDataPtr > EntityMapByKey;
        EntityMapByKey m_entityKeyMap;
        EntityMapByName m_entityNameMap;

		typedef std::map< unsigned long,std::string> EntityKeyNameMap;

		DataNodeAccessFactory();
		DataNodeAccessFactory(const DataNodeAccessFactory & theDataNodeAccessFactory);
		DataNodeAccessFactory & operator=(const DataNodeAccessFactory & rhs);
		virtual ~DataNodeAccessFactory();
	};

} // closes TA_Base_Core

#endif // !defined(DataNodeAccessFactory_774454B7_9ED6_4e4d_BB19_917B0DC71137__INCLUDED_)

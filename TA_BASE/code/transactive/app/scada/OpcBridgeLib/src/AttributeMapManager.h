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

#ifndef ATTRIBUTEMAPMANAGER_H
#define ATTRIBUTEMAPMANAGER_H

#pragma warning ( disable:4786 )

#include <list>
#include <string>

#include "app/scada/OpcBridgeLib/src/AbstractDataObjectAttributes.h"
#include "app/scada/OpcBridgeLib/src/DisplayProxyManager.h"
#include "app/scada/OpcBridgeLib/src/OPCDataCache.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/threads/src/Thread.h"
#include "ace/RW_Mutex.h"
#include "ace/Guard_T.h"

namespace TA_Base_Core
{
    class IOpcTagAlias;
}

namespace TA_Base_App
{
	class OpcBridge;
	class OpcTagManager;

    typedef std::map<std::string, TA_Base_Core::IOpcTagAlias*>  AliasMap;
    typedef std::map<unsigned long, std::vector<TA_Base_Core::IOpcTagAlias*> > EntityKeyAliasMap;
    typedef std::map<std::string, AbstractDataObjectAttributes*>               EntityNameMap;

	class AttributeMapManager
	{
	public:

		AttributeMapManager( OpcBridge& opcBridge, OpcTagManager& opcTagManager);

		virtual ~AttributeMapManager();

		/**
		 * dynamicCreateTag
		 *
		 * Create an attribute on the list (if it does not already exist)
		 *
		 * @param 	entityKey		The entity key of the OPC tag to be created
		 * @param 	tagName		    The tag name of the OPC tag to be created
		 * @param 	attrType		The attribute type of the OPC tag relate
		 * @param 	opcEntityData	The OPCCacheData of the OPC tag belong to
		 *
		 */
        void dynamicCreateTag( unsigned long entityKey, std::string& tagName, EOPCATTRIBUTETYPE attrType, OPCCacheData& opcEntityData );

		/**
		 * remove
		 *
		 * Remove the specified attribute from the internal list
		 *
		 * @param entityKey	The entity key of the OPC tag to be removed
		 * @param attrType	The attrType of the OPC tag to be removed
		 * @param bAlias	is this OPC tag is a alias
		 *
		 */
		void remove( unsigned long entityKey, EOPCATTRIBUTETYPE attrType, bool bAlias = false );

        /**
         * preCreateOpcTags
         *
		 * Pre-create all tags which need pre-created
		 *
         * @param saveTagName whether save tag names
         * 
         */
		void preCreateOpcTags(bool saveTagName=false);

        /**
         * preCreateAliasOpcTags
         *
         * Pre-Create all alias OpcTags and corresponding entity tags
		 *	
         */
        void preCreateAliasOpcTags();

        /**
         * isPreCreatedOpcTag
         *
         * Check if the given tagName had been pre-created
         *
         * @return  true if the given tagName had been pre-created; else false
         */
        bool isPreCreatedOpcTag(const std::string& tagName);

		/**
         * isValidTagName
         *
         * Check if the given tagName is valid
         *
		 * @param tagName tag name which need to valid
		 * @param entityKey output entity key of the entity which this tag belongs to, if it is valid
		 * @param attrType output attribute type of this tag if it is valid
		 * @param opcEntityData output entity cache data if the tag name is valid
		 *
		 * @return true if this tag name is valid
         */
        bool isValidTagName(const std::string& tagName, unsigned long & entityKey, EOPCATTRIBUTETYPE& attrType, OPCCacheData& opcEntityData);
       
		// 
        // to refresh all tags with the original value and quality
        // so the timestamp of tag will be updated
        //
        void refreshAllTags();

		unsigned long getPrecreateTagEntityCnt();
		
		void deleteOPCCacheFile();
	protected:

		/**
		 * populateDataObjects
		 *
		 * Used to load OPC Entity Data from the database and create
		 * DataObjectAttributes for DataNodes and DataPoints
		 *
		 */
		void populateDataObjects();
		
		AbstractDataObjectAttributes* dynamicCreateObjAttribute(OPCCacheData& opcData);

		void precreateDataNodeAttribute(TA_Base_Core::IEntityDataList& dnEntities);

		void precreateDataNodeAttribute(DataNodeCacheDataList& dnDataList);

		void precreateDataPointAttribute(TA_Base_Core::IEntityDataList& dpEntities);

		void precreateDataPointAttribute(DataPointCacheDataList& dpDataList);
		/**
		 * loadOpcTagAliases
		 *
		 * Used to load OpcTag Aliases from the database 
		 *
		 */
		void loadOpcTagAliases(void);
	
    private:
		bool					m_bAlias;
        DataObjectAttributesMap m_dataObjects;
		ACE_RW_Mutex 			m_dataObjectsMapLock;
        OPCDataCache			m_opcDataCache;

		OpcBridge&              m_opcBridge;
		OpcTagManager&          m_opcTagManager;
        AliasMap                m_aliasMap;
        EntityKeyAliasMap       m_entityKeyAliasMap;
        EntityNameMap           m_entityNameMap;
        std::list<std::string>  m_preCreatedOpcTagNames;
		unsigned long			m_preCreateTagEntityCnt;
		DisplayProxyManager*    m_proxyManager;
		static const std::string SQL_CUSTOM_TOKEN;
    };
}

#endif // #ifndef ATTRIBUTEMAPMANAGER_H

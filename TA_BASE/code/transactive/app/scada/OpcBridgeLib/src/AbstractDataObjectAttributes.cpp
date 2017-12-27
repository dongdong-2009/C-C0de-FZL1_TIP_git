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
  * The AbstractDataObjectAttributes class maintains a list of attributes currently requested by the OPC Client
  * for a particular DataObject.
  *
  */
#include "app/scada/OpcBridgeLib/src/AbstractDataObjectAttributes.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/scada/OpcBridgeLib/attributes/DpAttributeNames.h"
#include "app/scada/OpcBridgeLib/attributes/DnAttributeNames.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	AbstractDataObjectAttributes::AbstractDataObjectAttributes(TA_Base_Core::IEntityData* entityData) 
		: m_next(NULL), 
		  m_attrSize(0),
		  m_hasAlias(false),
		  m_entityKey(entityData->getKey()), 
		  m_agentKey(entityData->getAgent()), 
		  m_locationKey(entityData->getLocation()),
		  m_objectName(entityData->getName()),
		  m_preCreateAttrFlags(E_OPC_DATA_ATTR_ALL)
	{
		initAttributeMap();
	}

	AbstractDataObjectAttributes::AbstractDataObjectAttributes(unsigned long entityKey, unsigned long agentKey, 
		unsigned long locationKey, unsigned long attrFlags, std::string& objectName)
	:
	m_next(NULL), 
	m_attrSize(0),
	m_entityKey(entityKey), 
	m_agentKey(agentKey), 
	m_locationKey(locationKey),
	m_objectName(objectName),
	m_preCreateAttrFlags(attrFlags)
	{
		initAttributeMap();
	}

    AbstractDataObjectAttributes::~AbstractDataObjectAttributes()
	{
		TA_Base_Core::ThreadGuard lock(m_attributeMapLock);
		for (int i = 0; i < ATTRIBUTE_MAX; ++i)
		{
			Attribute* pAttribute = m_attributeMap[i];
			if (pAttribute != NULL)
			{
				delete pAttribute;
				m_attributeMap[i] = NULL;
			}
		}
	}

    Attribute* AbstractDataObjectAttributes::insertAttribute(std::string& tagName, EOPCATTRIBUTETYPE attrType, bool bAlias /*= false*/)
	{
		bool step0 = false;
		bool step1 = false;
		Attribute* attribute = NULL;
		{
			TA_Base_Core::ThreadGuard lock(m_attributeMapLock);
			attribute = getAttribute(attrType, bAlias);
			if (NULL == attribute)
			{
				step0 = true;
				attribute = newAttribute(tagName, attrType);
				if (attribute != NULL)
				{
					++m_attrSize;
					attribute->m_bAlias = bAlias;
					addAttribute(attribute);
				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Tag %s Attribute %d already exists", tagName.c_str(), attrType);
			}
		}

		if (attribute != NULL && isConfigAvailable())
		{
			step1 = true;
			attribute->processProxyUpdate();
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "MarkA--insertAttribute-type:%d, tagName:%s, alias:%d, step0:%d, step1:%d",
			attrType, tagName.c_str(), bAlias, step0, step1);
		
		return attribute;
	}


	void AbstractDataObjectAttributes::eraseAttribute(EOPCATTRIBUTETYPE attrType, bool bAlias)
	{
		TA_Base_Core::ThreadGuard lock(m_attributeMapLock);
		Attribute* pAttribute = getAttribute(attrType, bAlias);
		if (pAttribute != NULL)
		{
			--m_attrSize;
			EOPCATTRIBUTETYPE type = pAttribute->getAttributeType();
			if (bAlias)
			{
				m_attributeMap[type + E_OPC_ATTR_MAX] = NULL;
			}
			else
			{
				m_attributeMap[type] = NULL;
			}
			delete pAttribute;
		}
	}

	void AbstractDataObjectAttributes::preCreateOpcTags(std::list<std::string>& opcTagNames)
	{
		std::vector<AttrData> attrs;
		getPrecreateAttributes(attrs);
		std::string tagNamePrefix = m_objectName + ".";
		std::string tagName;

		TA_Base_Core::ThreadGuard lock(m_attributeMapLock);

		for (std::vector<AttrData>::iterator itAttr = attrs.begin(); itAttr != attrs.end(); ++itAttr)
		{
			tagName = tagNamePrefix + itAttr->attrString;
			Attribute* attribute = newAttribute(tagName, itAttr->attrType);
			if (NULL != attribute)
			{
				++m_attrSize;
				addAttribute(attribute);
				opcTagNames.push_back(tagName);
			}
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "preCreateOpcTags tagName:(%s), flags:(%u)", tagName.c_str(), m_preCreateAttrFlags);
	}

	void AbstractDataObjectAttributes::preCreateOpcTags()
	{
		std::vector<AttrData> attrs;
		getPrecreateAttributes(attrs);
		std::string tagNamePrefix = m_objectName + ".";
		std::string tagName;
	
		TA_Base_Core::ThreadGuard lock(m_attributeMapLock);

		for (std::vector<AttrData>::iterator itAttr = attrs.begin(); itAttr != attrs.end(); ++itAttr)
		{
			tagName = tagNamePrefix + itAttr->attrString;
			Attribute* attribute = newAttribute(tagName, itAttr->attrType);
			if (NULL != attribute)
			{
				++m_attrSize;
				addAttribute(attribute);
			}
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "preCreateOpcTags tagName:(%s), flags:(%u)", tagName.c_str(), m_preCreateAttrFlags);
	}

	void AbstractDataObjectAttributes::processProxyUpdate()
	{	
		TA_Base_Core::ThreadGuard lock(m_attributeMapLock);
		int total = 0;
		int maxSize = m_hasAlias ? E_OPC_ATTR_MAX : ATTRIBUTE_MAX;
		for (int i = 0; i < maxSize; ++i)
		{
			Attribute* pAttribute = m_attributeMap[i];
			if (pAttribute != NULL)
			{
				pAttribute->processProxyUpdate();
				++total;
			}
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "MarkA--processProxyUpdate EntityKey:%u, Attributes number:%d",	m_entityKey, total );
	}

	void AbstractDataObjectAttributes::processProxyUpdateAlias()
	{
		TA_Base_Core::ThreadGuard lock(m_attributeMapLock);
		int total = 0;
		for (int i = E_OPC_ATTR_MAX; i < ATTRIBUTE_MAX; ++i)
		{
			Attribute* pAttribute = m_attributeMap[i];
			if (pAttribute != NULL && pAttribute->m_bAlias)
			{
				pAttribute->processProxyUpdate();
				++total;
			}
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "processProxyUpdateAlias entity %u, number of attributes %d", m_entityKey, total );
	}

    Attribute* AbstractDataObjectAttributes::getAttribute( EOPCATTRIBUTETYPE attrType, bool isAlias)
	{
		//we protect multi-threads accessing on top level
		Attribute* pAttribute = NULL;
		if (attrType >=  E_OPC_ATTR_UNKNOWN && attrType < E_OPC_ATTR_MAX)
		{
			if (isAlias)
			{
				pAttribute = m_attributeMap[attrType + E_OPC_ATTR_MAX];
			}
			else
			{
				pAttribute = m_attributeMap[attrType];
			}
		}
		return pAttribute;
	}

	void AbstractDataObjectAttributes::addAttribute( Attribute* attribute )
	{
		//we protect multi-threads accessing on top level
		EOPCATTRIBUTETYPE attrType = attribute->getAttributeType();
		bool			  isAlias = attribute->m_bAlias;
		if ( isAlias )
		{
			m_hasAlias	= true;
			m_attributeMap[attrType + E_OPC_ATTR_MAX] = attribute;
		}
		else
		{
			m_attributeMap[attrType] = attribute;
		}
	}

	void AbstractDataObjectAttributes::initAttributeMap()
	{
		for (int i = 0; i < ATTRIBUTE_MAX; ++i)
		{
			m_attributeMap[i] = NULL;
		}
	}
}

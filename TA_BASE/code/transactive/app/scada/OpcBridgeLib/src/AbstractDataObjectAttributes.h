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

#ifndef DATAOBJECTATTRIBUTES_H
#define DATAOBJECTATTRIBUTES_H

#pragma warning ( disable:4786 )

#include <list>
#include <map>
#include <string>
#include "app/scada/OpcBridgeLib/src/OPCDataCache.h"
#include "app/scada/OpcBridgeLib/src/Attribute.h"
#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_App
{
	enum OPC_ENTITY_TYPE
	{
		E_OPC_DN = 0,
		E_OPC_DP,
		E_OPC_OTHER
	};

	#define ATTRIBUTE_MAX (2*E_OPC_ATTR_MAX)

	struct AttrData
	{
		EOPCATTRIBUTETYPE attrType;
		std::string       attrString;

		AttrData(EOPCATTRIBUTETYPE type, const std::string& attrstr)
			:attrType(type),attrString(attrstr){}
	};

    class AbstractDataObjectAttributes
    {

    public:
        /**
         * AbstractDataObjectAttributes
         *
         * Constructor for the AbstractDataObjectAttributes class
         *
         * @param   entityData      The EntityData of the DataObject
         *
         */
		AbstractDataObjectAttributes(TA_Base_Core::IEntityData* entityData);

        /**
         * AbstractDataObjectAttributes
         *
         * Constructor for the AbstractDataObjectAttributes class
         * @param   entityKey      The entitykey of the DataObject
		 * @param   agentKey      The agentKey of the DataObject
		 * @param   locationKey      The locationKey of the DataObject
		 * @param   attrFlags      The Pre-create Attributes' flags of the DataObject
		 * @param   objectName      The objectName of the DataObject
         *
         */
		AbstractDataObjectAttributes(unsigned long entityKey, unsigned long agentKey, 
			unsigned long locationKey, unsigned long attrFlags, std::string& objectName);
        
        /**
         * ~AbstractDataObjectAttributes
         *
         * Destructor for the AbstractDataObjectAttributes class
         *
         */
        virtual ~AbstractDataObjectAttributes();

        /**
         * insertAttribute
         *
         * Creates a new attribute for this DataObject based on the tagName
         * and inserts it into the map
         *
         * @param   tagName         The full tag name of the attribute to be created
         * @param   attrType		The attribute type of the attribute to be created
         * @param   bAlias			whether this tagName is alias of attributeName
		 *
         */
        Attribute* insertAttribute(std::string& tagName, EOPCATTRIBUTETYPE attrType, bool bAlias = false);

        /**
         * eraseAttribute
         *
         * Finds the attribute from the attrName and deletes it
         *
         * @param   attrType    The attrType of the attribute to be erased
         * @param   bAlias		whether this is alias attribute
		 *
         */
		void eraseAttribute(EOPCATTRIBUTETYPE attrType, bool bAlias = false);

		/**
         * preCreateOpcTags
         *
         * Previous create OPC tags for attributes of this DataObject
         *
         * @param   opcTagNames   output all the tag's names which had been created
		 *
         */
		void preCreateOpcTags(std::list<std::string>& opcTagNames);

		/**
         * preCreateOpcTags
         *
         * Previous create OPC tags for attributes of this DataObject
         *
		 *
         */
		void preCreateOpcTags();

		/**
         * processProxyUpdate
         *
         * Update all the attributes' tag when their data had changed
         *
         */
        virtual void processProxyUpdate();

		/**
         * processProxyUpdateAlias
         *
         * Update all alias attributes' tag when their data had changed
         *
         */
        virtual void processProxyUpdateAlias();

		/**
         * getEntityKey
         *
         * get entity key of this data object
		 *
         * @return entity key of this data object
		 *
         */
		unsigned long getEntityKey() { return m_entityKey; }

		/**
         * getAgentKey
         *
         * get agent key of this data object
		 *
         * @return agent key of this data object
		 *
         */
		unsigned long getAgentKey() { return m_agentKey; }

		/**
         * getLocationKey
         *
         * get location key of this data object
		 *
         * @return location key of this data object
		 *
         */
		unsigned long getLocationKey() { return m_locationKey; }

		/**
         * getObjectName
         *
         * get name this data object
		 *
         * @return name of this data object
		 *
         */
		std::string getObjectName() { return m_objectName; }

		/**
         * getObjectTypeNo
         *
         * get object Type No of this data object
		 *
         * @return object Type enum No
		 *
         */
		virtual OPC_ENTITY_TYPE getObjectTypeNo() = 0;

		/**
         * getAttributeType
         *
         * get Attribute Type No of this attribute
		 *
		 * @param   attrName    The attribute name of the attribute
         * 
		 * @return object Type enum No
		 *
         */
		virtual EOPCATTRIBUTETYPE getAttributeType( std::string& attrName ) = 0;

		// current created attributes count
		int								m_attrSize;

		AbstractDataObjectAttributes*	m_next;

    protected:

		/**
         * getPrecreateAttributes
         *
         * get pre-create attributes of this DataObject
         *
         * @param   attrs output pre-create attributes's data
		 *
         */
		virtual void getPrecreateAttributes(std::vector<AttrData>& attrs) = 0;

        /**
         * newAttribute
         *
         * Creates a new attribute for this DataObject based on the tagName
         *
         * @param   tagName			The tag name of the attribute to be created
		 * @param   attrType        The attribute type of the attribute to be created
         * @return  A new attribute based on the attribute name
         *
         */
		virtual Attribute* newAttribute(const std::string& tagName, EOPCATTRIBUTETYPE attrType) = 0;
		/**
         * getAttribute
         *
         * get attribute of (attrType, isAlias)
         *
         * @param   attrType  The attribute type of this Attribute
		 * @param   isAlias   Whether this attribute is alias
         * @return  Attribute pointer
         *
         */
		Attribute* getAttribute( EOPCATTRIBUTETYPE attrType, bool isAlias = false);

		/**
         * addAttribute
         *
         * add attribute into attribute map
         *
         * @param   attribute  The attribute to be added
         *
         */
		void addAttribute(Attribute* attribute );

		/**
         * isConfigAvailable
         *
         * whether this data object's data had been updated
         *
         */
        virtual bool isConfigAvailable() = 0;
		
		/**
         * initAttributeMap
         *
         * init attribute map when construct data object
         *
         */
		void initAttributeMap();

    protected:
		bool				m_hasAlias;
		unsigned long		m_entityKey;
		unsigned long		m_agentKey;
		std::string			m_objectName;
		unsigned long		m_locationKey;
		unsigned long		m_preCreateAttrFlags;
		//if 0 <= index < E_OPC_ATTR_MAX store normal attribute pointer
		//if E_OPC_ATTR_MAX <= index < ATTRIBUTE_MAX store alias attribute pointer
		//Attribute A's alias index = Attribute A's normal index + E_OPC_ATTR_MAX
		Attribute*			m_attributeMap[ATTRIBUTE_MAX];
		TA_Base_Core::ReEntrantThreadLockable m_attributeMapLock;
    };

	typedef std::map <unsigned long, AbstractDataObjectAttributes*> DataObjectAttributesMap;
}

#endif // #ifndef DATAOBJECTATTRIBUTES_H

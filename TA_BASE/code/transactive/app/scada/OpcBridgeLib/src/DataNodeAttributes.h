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
  * The DataNodeAttributes class maintains a list of attributes currently requested by the OPC Client
  * for a particular DataNode
  *
  */

#ifndef DATANODEATTRIBUTES_H
#define DATANODEATTRIBUTES_H

#pragma warning ( disable:4786 )

#include "app/scada/OpcBridgeLib/src/AbstractDataObjectAttributes.h"

namespace TA_Base_App
{
    class DisplayDataNodeProxy;
    
	class DataNodeAttributes : public AbstractDataObjectAttributes
	{

	public:

		DataNodeAttributes(TA_Base_Core::IEntityData* entityData);

		DataNodeAttributes(DataNodeCacheData& opcDnData);
	
		virtual ~DataNodeAttributes();

        static bool isAttributeNameValid(const std::string & attributeName, EOPCATTRIBUTETYPE& attrType);

		virtual OPC_ENTITY_TYPE getObjectTypeNo();

		void	setStatus(unsigned long newStatus);

		bool	updateState(TA_Base_Bus::DataNodeDisplayState& newState);

		virtual EOPCATTRIBUTETYPE getAttributeType( std::string& attrName );

		std::string				getONITP();
		_variant_t				getVariantONITP();
		TA_Base_Bus::BooleanTag getAlarmSummary();
		TA_Base_Bus::EnumTag	getAlarmAckStateSummary();
		TA_Base_Bus::EnumTag	getQualitySummary();
		unsigned long			getQualityValue();

	protected:

		virtual void getPrecreateAttributes(std::vector<AttrData>& attrs);
		
		virtual Attribute* newAttribute(const std::string& tagName, EOPCATTRIBUTETYPE attrType);

		virtual bool isConfigAvailable();

		void initDisplayState();
		
	protected:
		bool			m_configAvaliable;
        std::string		m_label;
        std::string		m_shortLabel;
        std::string		m_assetName;
		TA_Base_Core::ReEntrantThreadLockable	m_stateLock;
		TA_Base_Bus::DataNodeDisplayState		m_state;
	};
}

#endif // #ifndef DATANODEATTRIBUTES_H

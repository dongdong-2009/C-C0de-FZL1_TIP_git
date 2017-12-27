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
  * Base class representing an attribute of a DataNode
  *
  */

#include <sstream>
#include "ace/OS.h"
#include "app/scada/OpcBridgeLib/src/Attribute.h"
#include "app/scada/OpcBridgeLib/src/TransActiveDataSource.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	bool	Attribute::m_updateTagPeriodically	= false;
	long	Attribute::m_total					= 0;
	TA_Base_Core::ReEntrantThreadLockable Attribute::m_totalLock;
	WORD    Attribute::m_defaultQuality = TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR;;
	VARIANT Attribute::m_defaultValue;

	Attribute::Attribute( const std::string & tagName, _variant_t initValue, WORD initQuality)
	:
	m_tagHandle(INVALID_HANDLE_VALUE),
	m_tagName(""),
	m_bAlias(false)
	{
		m_tagHandle = OpcTag::createTag(tagName.c_str(), initValue, initQuality, true);

		if ( INVALID_HANDLE_VALUE == m_tagHandle )
		{
			m_tagName = tagName;
			if ( FALSE == OpcTag::updateTagByName( tagName.c_str(),	initValue,	initQuality ) )
			{
				std::string errorMsg("Failed to create tag [");
				errorMsg += m_tagName;
				errorMsg += "]";
				LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", errorMsg.c_str() );
			}
		}

		m_quality = initQuality;
		m_value   = initValue;

		Attribute::incrementTotal();
	}


	Attribute::~Attribute()
	{
        Attribute::decrementTotal();
		update(m_defaultValue, m_defaultQuality);
		OpcTag::removeTag(m_tagHandle);
	}

	bool Attribute::isHandle ( HANDLE Handle )
	{
		return (Handle == m_tagHandle);
	}


	void Attribute::update(_variant_t  value, WORD quality)
	{
		try
		{
			bool isUpdate = false;
			if ( (m_value != value) || (m_quality != quality) || (m_bAlias && m_updateTagPeriodically) )
			{
				isUpdate	=	true;
				m_value		=	value;
				m_quality	=	quality;

				if ( m_tagHandle != INVALID_HANDLE_VALUE )
				{
					OpcTag::updateTag(m_tagHandle, value, quality);
				}
				else
				{
					OpcTag::updateTagByName( m_tagName.c_str(), value, quality );
				}
			}
			//LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MarkA--Update Tag:%s, isUpdate:%d", m_tagName.c_str(), isUpdate);	
		}
		catch(...)
		{
			//LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MarkB--update tag:%s unknown exception", m_tagName.c_str());	
		}
	}


    void Attribute::incrementTotal()
    {
        // Use ThreadGuard directly instead of TA_THREADGUARD to reduce overhead
        TA_Base_Core::ThreadGuard lock(m_totalLock);
        ++m_total;
    }


    void Attribute::decrementTotal()
    {
        // Use ThreadGuard directly instead of TA_THREADGUARD to reduce overhead
        TA_Base_Core::ThreadGuard lock(m_totalLock);
        --m_total;
    }

    
    long Attribute::getTotal()
    {
        // Use ThreadGuard directly instead of TA_THREADGUARD to reduce overhead
        TA_Base_Core::ThreadGuard lock(m_totalLock);
        return m_total;
    }
	
    bool Attribute::readTag( VARIANT& value, WORD& quality )
    {
        VARIANT		localValue;
        WORD		localQuality;
        FILETIME	timestamp;

        // Raymond Pau++
        // TD8160   FT-HI: OPCbridge leaks memory at 50k/sec
        VariantInit(&localValue);
        // TD8160   FT-HI: OPCbridge leaks memory at 50k/sec
        // ++Raymond Pau

        if ( TRUE == OpcTag::readTagWithTimestamp(m_tagHandle, &localValue, &localQuality, &timestamp) )
        {
            // Raymond Pau++
            // TD8160   FT-HI: OPCbridge leaks memory at 50k/sec
            VariantCopy(&value, &localValue);
            VariantClear(&localValue);
            // TD8160   FT-HI: OPCbridge leaks memory at 50k/sec
            // ++Raymond Pau
            quality = localQuality;

            return true;
        }

        // Raymond Pau++
        // TD8160   FT-HI: OPCbridge leaks memory at 50k/sec
        VariantClear(&localValue);
        // TD8160   FT-HI: OPCbridge leaks memory at 50k/sec
        // ++Raymond Pau
        return false;
    }

	void Attribute::initDefaultValue()
	{
		VariantInit(&(Attribute::m_defaultValue));
		Attribute::m_defaultValue.vt = VT_R8;
		Attribute::m_defaultValue.dblVal = 0.0;
	}

	EOPCDATAATTRIBUTETYPE Attribute::convertAttrToCacheFlag(EOPCATTRIBUTETYPE attrType)
	{
		EOPCDATAATTRIBUTETYPE cacheFlag = E_OPC_DATA_ATTR_NONE;

		switch(attrType)
		{
		case E_OPC_ATTR_UNKNOWN:
			cacheFlag = E_OPC_DATA_ATTR_NONE;
			break;
		case E_OPC_DN_LABEL:
			cacheFlag = E_OPC_DATA_ATTR_DN_LABEL;
			break;
		case E_OPC_DN_SHORTLABEL:
			cacheFlag = E_OPC_DATA_ATTR_DN_SHORTLABEL;
			break;
		case E_OPC_DN_ASSETNAME:
			cacheFlag = E_OPC_DATA_ATTR_DN_ASSETNAME;
			break;
		case E_OPC_DN_ONTIP:
			cacheFlag = E_OPC_DATA_ATTR_DN_ONTIP;
			break;
		case E_OPC_DN_ACKSTATE:
			cacheFlag = E_OPC_DATA_ATTR_DN_ACKSTATE;
			break;
		case E_OPC_DN_ALARMSTATE:
			cacheFlag = E_OPC_DATA_ATTR_DN_ALARMSTATE;
			break;
		case E_OPC_DN_QUALITY:
			cacheFlag = E_OPC_DATA_ATTR_DN_QUALITY;
			break;
		case E_OPC_DP_ACKSTATE:
			cacheFlag = E_OPC_DATA_ATTR_DP_ACKSTATE;
			break;
		case E_OPC_DP_ALARMSTATE:
			cacheFlag = E_OPC_DATA_ATTR_DP_ALARMSTATE;
			break;
		case E_OPC_DP_QUALITY:
			cacheFlag = E_OPC_DATA_ATTR_DP_QUALITY;
			break;
		case E_OPC_DP_VALUE:
			cacheFlag = E_OPC_DATA_ATTR_DP_VALUE;
			break;
		default:
			break;
		}

		return cacheFlag;
	}
}

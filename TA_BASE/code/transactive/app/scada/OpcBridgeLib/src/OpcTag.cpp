/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  HoaVu
  * @version $Revision$
  *
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  * OpcTag represents a tag that has been created in OPC Server.
  *
  */
#include "cots/WinTech/OPCServerToolKit/opc_ae.h"
#include "cots/WinTech/OPCServerToolKit/opcda.h"
#include "cots/WinTech/OPCServerToolKit/WTOPCsvrAPI.h"

#include "app/scada/OpcBridgeLib/src/OpcTag.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_App
{
	WORD    OpcTag::m_defaultQuality = TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR;;
	VARIANT OpcTag::m_defaultValue;

	OpcTag::OpcTag( const std::string & tagName, _variant_t & initValue, WORD & initQuality)
		: m_tagName(tagName),
		  m_tagHandle(INVALID_HANDLE_VALUE)
	{
		m_tagHandle = createTag(tagName.c_str(), initValue, initQuality, true);

		// if failed to create a new tag
        if ( INVALID_HANDLE_VALUE == m_tagHandle )
        {
            if ( FALSE == updateTagByName( tagName.c_str(),
                                           m_defaultValue,
                                           m_defaultQuality ) )
            {
                std::ostringstream desc;
                desc << "Failed to create tag [" << tagName << "]";
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, desc.str().c_str() );
            }
        }
	}
	
	OpcTag::OpcTag( const std::string & tagName) 
		: m_tagName(tagName),
		  m_tagHandle(INVALID_HANDLE_VALUE)
	{
		m_tagHandle = createTag(tagName.c_str(), m_defaultValue, m_defaultQuality, true);
		
		// if failed to create a new tag
        if ( INVALID_HANDLE_VALUE == m_tagHandle )
        {
            if ( FALSE == updateTagByName( tagName.c_str(),
				m_defaultValue,
				m_defaultQuality ) )
            {
                std::ostringstream desc;
                desc << "Failed to create tag [" << tagName << "]";
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, desc.str().c_str() );
            }
        }
	}


    OpcTag::~OpcTag()
    {
        // Raymond Pau++
        // TD8160   FT-HI: OPCbridge leaks memory at 50k/sec
        VARIANT value;
        VariantInit(&value);
        value.vt = VT_R8;
        value.dblVal = 0.0;
        // TD8160   FT-HI: OPCbridge leaks memory at 50k/sec
        // ++Raymond Pau
        WORD quality = TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR;

        updateTag(value, quality);
        removeTag(m_tagHandle);

        // Raymond Pau++
        // TD8160   FT-HI: OPCbridge leaks memory at 50k/sec
        VariantClear(&value);
        // TD8160   FT-HI: OPCbridge leaks memory at 50k/sec
        // ++Raymond Pau
    }

    const std::string & OpcTag::getTagName() const
    {
        return m_tagName;
    }


    HANDLE OpcTag::getHandle() const
    {
        return m_tagHandle;
    }


    void OpcTag::updateTag( VARIANT value, WORD quality )
    {
        if ( INVALID_HANDLE_VALUE == m_tagHandle )
        {
            if ( FALSE == updateTagByName( m_tagName.c_str(),
                                           value,
                                           quality ) )
            {
                // log error message
            }
        }
        else
        {
            ::UpdateTag(m_tagHandle, value, quality);
        }
    }

    bool OpcTag::readTag( VARIANT& value, WORD& quality )
    {
        VARIANT localValue;
        WORD localQuality;
        FILETIME timestamp;

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
    

    /*
    ** Win-tech OPC Server Library wrappers
    **
    ** Use these in preference to directly calling the functions.
    ** These wrappers ensure calls to the API are not overlapped.
    */

    //
    // createTag
    //
    HANDLE OpcTag::createTag(LPCSTR Name, VARIANT Value, WORD InitialQuality, BOOL IsWritable)
    {
        return ::CreateTag(Name, Value, InitialQuality, IsWritable);
    }
        
    //
    // updateTag
    //
    BOOL OpcTag::updateTag(HANDLE TagHandle, VARIANT Value, WORD Quality)
    {        
        return ::UpdateTag(TagHandle, Value, Quality);
    }

    //
    // updateTagByName
    //
    BOOL OpcTag::updateTagByName(LPCSTR Name, VARIANT Value, WORD Quality)
    {        
        return ::UpdateTagByName(Name, Value, Quality);
    }

    //
    // readTagWithTimestamp
    //
    BOOL OpcTag::readTagWithTimestamp(HANDLE TagHandle, VARIANT* Value, WORD* Quality, FILETIME* timestamp)
    {        
        return ::ReadTagWithTimeStamp(TagHandle, Value, Quality, timestamp);
    }

    //
    // removeTag
    //
    BOOL OpcTag::removeTag(HANDLE TagHandle)
    {        
        return ::RemoveTag(TagHandle);
    }
	
	void OpcTag::initDefaultValue()
	{
		VariantInit(&(OpcTag::m_defaultValue));
		OpcTag::m_defaultValue.vt = VT_R8;
		OpcTag::m_defaultValue.dblVal = 0.0;
	}
}

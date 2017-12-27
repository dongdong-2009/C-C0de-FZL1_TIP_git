/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/Banner.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/Banner.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    const std::string Banner::ENTITY_TYPE = "Banner";

    const std::string Banner::START_TAB = "StartTab";
    const std::string Banner::TOP_POSITION = "TopPosition";
    const std::string Banner::LEFT_POSITION = "LeftPosition";
    const std::string Banner::HEIGHT = "Height";
    const std::string Banner::WIDTH = "Width";
    const std::string Banner::CALL_TYPE_COLOUR_PREFIX = "CallTypeColour";


    Banner::Banner() 
    {
    }


    Banner::Banner( unsigned long key )
        : EntityData(key,getStaticType()), m_isValidData(false)
    {
    }

 
    Banner::~Banner()
    {
    }


    std::string Banner::getType()
    {
        return getStaticType();
    }


    std::string Banner::getStaticType()
    {
        return ENTITY_TYPE;
    }


        TA_Base_Core::IEntityData* Banner::clone(unsigned long key)
    {
        return( new Banner(key));
    }


    void Banner::invalidate()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");

        getHelper()->invalidate();

        m_isValidData = false;
    }


    std::string Banner::getStartTab()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");

        return getHelper()->getParameter(START_TAB);
    }

    
    int Banner::getTopPosition()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		std::string value = getHelper()->getParameter(TOP_POSITION);
		m_topPosition = EntityHelper::getIntegerData(value);

        return m_topPosition;
    }


    int Banner::getLeftPosition()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		std::string value = getHelper()->getParameter(LEFT_POSITION);
		m_leftPosition = EntityHelper::getIntegerData(value);

        return m_leftPosition;
    }


    int Banner::getHeight()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		std::string value = getHelper()->getParameter(HEIGHT);
		m_height = EntityHelper::getIntegerData(value);

        return m_height;
    }
    
    
    int Banner::getWidth()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		std::string value = getHelper()->getParameter(WIDTH);
		m_width = EntityHelper::getIntegerData(value);

        return m_width;
    }


    unsigned long Banner::getCallTypeColour(const std::string& callType)
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");

        EntityHelper::EntityParameterNameValueMap& allParameterValues = getHelper()->getAllParameters();
		EntityHelper::EntityParameterNameValueMapIter it = allParameterValues.begin();

        for (; it != allParameterValues.end(); ++it)
        {
            if ( startsWith(it->first, CALL_TYPE_COLOUR_PREFIX) )
            {
                // Get the call type (ie the part of the parameter name after
                // the call type prefix)
                std::string::size_type startPos = CALL_TYPE_COLOUR_PREFIX.size();
                std::string::size_type callTypeLength = (it->first).length() - startPos;
                std::string initCallType = (it->first).substr(startPos, callTypeLength);
                
                // Get the value
                unsigned long value = EntityHelper::getIntegerData(it->second);

                // Add the name and value to the map
                m_callTypeColourMap[initCallType] = value;
            }
		}
        
        if( m_callTypeColourMap.find(callType) == m_callTypeColourMap.end() )
        {
            std::ostringstream message;
            message << "Unrecognised call type: '" << callType << "'";
            const char* msg = message.str().c_str();
            TA_THROW( DataException(msg, DataException::NO_VALUE, "Call Type") );
        }

        return m_callTypeColourMap[callType];
    }



    bool Banner::startsWith(
        const std::string& containingString,
        const std::string& prefix)
    {
        return( containingString.substr(0, prefix.size()) == prefix );
    }

}

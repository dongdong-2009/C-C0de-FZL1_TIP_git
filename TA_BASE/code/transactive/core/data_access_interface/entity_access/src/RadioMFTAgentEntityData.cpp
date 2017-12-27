/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioMFTAgentEntityData.cpp $
 * @author:  Alexis Laredo
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * 
 * RadioMFTAgentEntityData is an implementation of IEntityData. It proves read-only access to RadioMFTAgent data,
 * and a specific framework for RadioMFTAgent parameters.
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include <iostream>

#include "core/data_access_interface/entity_access/src/RadioMFTAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string RadioMFTAgentEntityData::ENTITY_TYPE      = "RadioMFTAgent";
	const std::string RadioMFTAgentEntityData::BIM_RESOURCE_ID = "BIMResourceID";
	const std::string RadioMFTAgentEntityData::SPI_RESOURCE_ID = "SPIResourceID";
	const std::string RadioMFTAgentEntityData::PASSNUMBER = "SDSPassNumber";
	const std::string RadioMFTAgentEntityData::CLIENT_SSI = "SDSClientSSI";

    RadioMFTAgentEntityData::RadioMFTAgentEntityData(unsigned long key)
		: EntityData(key, getType())
    {
    }

    RadioMFTAgentEntityData::~RadioMFTAgentEntityData()
    {
    }

	unsigned int RadioMFTAgentEntityData::getBIMResource()
	{
		std::string value = getHelper()->getParameter(BIM_RESOURCE_ID);
		unsigned short bimRid = 0;
		if (!value.empty())
		{
			bimRid = static_cast < unsigned short> (EntityHelper::getIntegerData(value));
		}

        return bimRid;	
	}
    
	unsigned int RadioMFTAgentEntityData::getSPIResource()
	{
		std::string value("");
		value = getHelper()->getParameter(SPI_RESOURCE_ID);
		//unsigned short spiRid = 0;
		unsigned int spiRid = 0;
		if (!value.empty())
		{
			//spiRid = static_cast < unsigned short>(EntityHelper::getUnsignedLongData(value));
            spiRid = static_cast < unsigned int>(EntityHelper::getUnsignedLongData(value));			
		}
        return spiRid;
	}

	unsigned int RadioMFTAgentEntityData::getPassNumber()
	{
		std::string value("");
		value = getHelper()->getParameter(PASSNUMBER);
		unsigned int passNumber = 0;
		if (!value.empty())
		{
            passNumber = static_cast < unsigned long>(EntityHelper::getUnsignedLongData(value));
		}

        return passNumber;
	}

	unsigned int RadioMFTAgentEntityData::getClientSSI()
	{
		std::string value("");
		value = getHelper()->getParameter(CLIENT_SSI);
		unsigned int clientSSI = 0;
		if (!value.empty())
		{
            clientSSI = static_cast < unsigned long>(EntityHelper::getUnsignedLongData(value));
		}

        return clientSSI;
	}

    std::string RadioMFTAgentEntityData::getType()
    {
        return getStaticType();
    }


	std::string RadioMFTAgentEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}

    
    IEntityData* RadioMFTAgentEntityData::clone(unsigned long key)
    {
        return new RadioMFTAgentEntityData(key);        
    }


    void RadioMFTAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core


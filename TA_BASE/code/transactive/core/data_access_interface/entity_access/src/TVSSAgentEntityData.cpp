/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/TVSSAgentEntityData.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by:  $Author: $
  * 
  * 
  * TVSSAgentEntityData is a concrete implementation of IRadioEntityData, which is in turn an implementation of
  * IEntityData. It proves read-only access to Radio data, and a specific framework
  * for TVSS parameters.
  *
  */

//#ifdef __WIN32__
//#pragma warning(disable:4786) // disable the "identifier truncated" warning
//#endif

#include "core/data_access_interface/entity_access/src/tvssagententitydata.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string TVSSAgentEntityData::ENTITY_TYPE          = "TVSSAgent";
    const std::string TVSSAgentEntityData::PRIMARY_SERVERS      = "PrimaryServers";
    const std::string TVSSAgentEntityData::SECONDARY_SERVERS    = "SecondaryServers";
    const std::string TVSSAgentEntityData::XON_XOFF_TIMEOUT     = "XONXOFFTimeout";
	const std::string TVSSAgentEntityData::ASSET_NAME           = "AssetName";

    TVSSAgentEntityData::TVSSAgentEntityData( unsigned long key ) :
		EntityData( key, getType() )
    {
		LOG( SourceInfo, DebugUtil::FunctionEntry, "TVSSAgentEntityData (constructor)");
        LOG( SourceInfo, DebugUtil::FunctionExit, "TVSSAgentEntityData (constructor)");
    }

	TVSSAgentEntityData::~TVSSAgentEntityData()
    {
    }

	std::string TVSSAgentEntityData::getPrimaryTCPServers()
	{
		return getHelper()->getParameter(PRIMARY_SERVERS);
	}

	std::string TVSSAgentEntityData::getSecondaryTCPServers()
	{
		return getHelper()->getParameter(SECONDARY_SERVERS);
	}

	unsigned long TVSSAgentEntityData::getXONXOFFTimeout()
	{
		std::string value = getHelper()->getParameter(XON_XOFF_TIMEOUT);
		unsigned long XONXOFFTimeout = 0;
		if (!value.empty())
		{
			XONXOFFTimeout = EntityHelper::getUnsignedLongData(value);
		}

        return XONXOFFTimeout;
	}
	
	std::string TVSSAgentEntityData::getAssetName()
    {
		return getHelper()->getParameter(ASSET_NAME);
    }

	std::string TVSSAgentEntityData::getType()
    {
        return getStaticType();
    }

	std::string TVSSAgentEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}

    IEntityData* TVSSAgentEntityData::clone(unsigned long key)
    {
        return( new TVSSAgentEntityData(key) );
    }

	void TVSSAgentEntityData::invalidate()
	{
		getHelper()->invalidate();
	}

} //close namespace TA_Base_Core


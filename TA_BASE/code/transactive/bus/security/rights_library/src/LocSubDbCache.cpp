/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_library/src/LocSubDbCache.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/21 10:20:13 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  Caches rights.
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "RightsLibraryInternal.h"

namespace TA_Base_Bus
{
	// TODO: this should probably be configurable...
	const unsigned long LocSubDbCache::UNALLOCATED_SUBSYSTEM=0;

	LocSubDbCache::LocSubDbCache() :
		m_dbLookup(m_dbMap,TA_Base_Core::EntityAccessFactory::getInstance(),&TA_Base_Core::EntityAccessFactory::getEntity)
	{
	}

	LocSubDbCache::~LocSubDbCache()
	{
		DbMap::iterator it;

		for( it = m_dbMap.begin(); it != m_dbMap.end(); it++ )
		{
			delete it->second;
			it->second = NULL;
		}
	}

	unsigned long LocSubDbCache::getLocation( unsigned long resourceKey )
	{
		FUNCTION_ENTRY("getLocation");
		TA_Base_Core::IEntityData* entityData =  m_dbLookup.lookup( resourceKey );

		if( 0 != entityData )
		{
			FUNCTION_EXIT;
			return entityData->getLocation();
		}
		FUNCTION_EXIT;
		return(0);
	}

	unsigned long LocSubDbCache::getSubsystem( TA_Base_Core::IEntityData* entityData )
	{
		FUNCTION_ENTRY("getSubsystem");

		if( 0 != entityData ){

			unsigned long rval = entityData->getPhysicalSubsystem();

			if( UNALLOCATED_SUBSYSTEM == rval )
			{
				rval = entityData->getSubsystem();
			}
			FUNCTION_EXIT;
			return( rval );
		}

		FUNCTION_EXIT;
		return(0);
	}
	unsigned long LocSubDbCache::getSubsystem( unsigned long resourceKey )
	{
		FUNCTION_ENTRY("getSubsystem");
		TA_Base_Core::IEntityData* entityData = m_dbLookup.lookup( resourceKey );

		unsigned long rval = getSubsystem(entityData);

		FUNCTION_EXIT;
		return(rval);
	}

	bool LocSubDbCache::getLocSub(unsigned long  recoureKey, unsigned long  & location, unsigned long  & subsystem)
	{
		FUNCTION_ENTRY("getLocSub");

		TA_Base_Core::IEntityData* entityData = m_dbLookup.lookup( recoureKey );
		if(0 == entityData)
		{
			FUNCTION_EXIT;
			return false;
		}		
		
		subsystem = getSubsystem(entityData);
		location = entityData->getLocation();

		FUNCTION_EXIT;
		return true;		
	}
}

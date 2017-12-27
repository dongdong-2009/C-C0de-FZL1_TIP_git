#ifndef LOCSUBDBCACHE_H_INCLUDED
#define LOCSUBDBCACHE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_library/src/LocSubDbCache.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Caches locations and subsystems associated with resources from the database.
  */

namespace TA_Base_Bus
{
	class LocSubDbCache
	{
	public:
		LocSubDbCache();
		~LocSubDbCache();

		unsigned long getLocation ( unsigned long resourceKey );
		unsigned long getSubsystem( unsigned long resourceKey );
		unsigned long getSubsystem( TA_Base_Core::IEntityData* pEnity );
		bool getLocSub(unsigned long  recoureKey, unsigned long & location, unsigned long & subsystem);

	private:

		LocSubDbCache( const LocSubDbCache& );
		LocSubDbCache& operator=( const LocSubDbCache& );
		typedef std::map<unsigned long, TA_Base_Core::IEntityData*> DbMap;
		DbMap m_dbMap;
		
		static const unsigned long UNALLOCATED_SUBSYSTEM;

		DbCacheLookup< unsigned long, TA_Base_Core::IEntityData, TA_Base_Core::EntityAccessFactory> m_dbLookup;
	};
}

#endif // LOCSUBDBCACHE_H_INCLUDED

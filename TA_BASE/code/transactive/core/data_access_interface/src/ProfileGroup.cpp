/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ProfileGroup.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ProfileGroup is an implementation of IProfileGroup. It holds the data specific to an ProfileGroup entry
  * in the database, and allows read-only access to that data.
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/data_access_interface/src/ProfileGroup.h"

namespace TA_Base_Core
{
	const std::string ProfileGroup::PROFILE_GROUP = "Profile";

    ProfileGroup::ProfileGroup( const unsigned long key )
		: m_key( key),
        m_name(""),
		m_type(ProfileGroup::PROFILE_GROUP)
    {
		m_profileGroupHelper = new ProfileGroupHelper(m_type, m_key);
    }


    ProfileGroup::ProfileGroup(const unsigned long key, const std::string& name )
        : m_key( key ),
          m_name( name ),
		  m_type(ProfileGroup::PROFILE_GROUP)
    {
		m_profileGroupHelper = new ProfileGroupHelper(m_type, m_key);
    }

	ProfileGroup::ProfileGroup(const unsigned long row, TA_Base_Core::IData& data)
		: m_profileGroupHelper( new ProfileGroupHelper(ProfileGroup::PROFILE_GROUP, row, data) )
	{
		//TD17807 - marc_bugfix_start
		//Initialize m_key to an extremely high value
		//We will use this value later to check
		//if this has been initialized in the first place by the other constructors
		m_key = 0xFFFFFFFF;
		//TD17807 - marc_bugfix_end
	}

	ProfileGroup::~ProfileGroup()
	{
		if ( m_profileGroupHelper != NULL )
		{
			delete m_profileGroupHelper;
			m_profileGroupHelper = NULL;
		}
	}

	unsigned long ProfileGroup::getKey()
    {
		//TD17807 - marc_bugfix_start
		if (m_key == 0xFFFFFFFF)
		{
			m_key = m_profileGroupHelper->getKey(); 
		}
		//TD17807 - marc_bugfix_end
		return m_key;
    }


    std::string ProfileGroup::getName()
    {
        if (m_name.empty()) m_name = m_profileGroupHelper->getName(); 
		return m_name;
    }

	std::string ProfileGroup::getType()
	{
		return m_type;
	}

    void ProfileGroup::invalidate()
    {
		m_profileGroupHelper->invalidate();
		m_name = "";
    }


    bool ProfileGroup::isGroupUsedInProfile(unsigned long key)
    {
		return m_profileGroupHelper->isGroupUsedInProfile(key);
    }

} // closes TA_Base_Core

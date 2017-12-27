/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Profile.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/01/28 18:24:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * Profile is an implementation of IProfile. It holds the data specific to an profile entry
  * in the database, and allows read-only access to that data.
  *
  */


#ifdef __WIN32__
#pragma warning(disable: 4786)
#endif
 
#include "core/data_access_interface/src/Profile.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    Profile::Profile( const unsigned long key )
		: m_profileHelper( new ProfileHelper( key ) )
    {
    }


	Profile::Profile(const unsigned long row, TA_Base_Core::IData& data)
		: m_profileHelper( new ProfileHelper(row, data) )
	{
	}


    Profile::~Profile()
    {
		if ( m_profileHelper != NULL )
		{
			delete m_profileHelper;
			m_profileHelper = NULL;
		}
    }


    unsigned long Profile::getKey()
    {
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getKey();
    }


    std::string Profile::getName()
    {
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getName();
    }

	std::string Profile::getDisplayName()
    {
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getDisplayName();
    }


    bool Profile::isSystemProfile()
    {
        TA_ASSERT(m_profileHelper != NULL, "getName() has been called after profile was deleted");

        return m_profileHelper->isSystemProfile();
    }

    
    std::string Profile::getDefaultDisplay(unsigned long locationKey, int displayNumber)
    {
		// Check to see if the Profile Type is LOCAL
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getDefaultDisplay( locationKey, displayNumber );
    }


    std::string Profile::getParameterValue(unsigned long locationKey, const std::string& parameterName)
    {
        // Check to see if the Profile Type is LOCAL
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getParameterValue( locationKey, parameterName );
    }


    std::string Profile::getParameterActualValue(unsigned long locationKey, const std::string& parameterName)
    {
        // Check to see if the Profile Type is LOCAL
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getParameterActualValue( locationKey, parameterName );
    }


    bool Profile::requiresLocationToBeSelected()
    {
		// Check to see if the Profile Type is LOCAL
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->isLocalProfile();
    }

    void Profile::invalidate()
    {
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		m_profileHelper->invalidate();
    }

    std::vector<ILocation*> Profile::getAssociatedLocations() 
    { 
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getAssociatedLocations();
    }

    std::vector<unsigned long> Profile::getAssociatedLocationKeys() 
    { 
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getAssociatedLocationKeys();
    }

	std::string Profile::getActionGroupAsStringForResource( unsigned long resourceKey )
	{
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getActionGroupAsStringForResource( resourceKey );
	}

	std::string Profile::getActionGroupAsStringForSubsystem( unsigned long subsystemKey,
		   unsigned long subsystemStateKey	)
	{
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getActionGroupAsStringForSubsystem( subsystemKey, subsystemStateKey );
	}

	bool Profile::isExclusive()
	{
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return( !(m_profileHelper->isLocalProfile()) );
	}

	unsigned long Profile::getType()
	{
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getType( );
	}

	std::string Profile::getTypeAsString()
	{
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getTypeAsString( );
	}

	bool Profile::getActionGroup( unsigned long subsystem, unsigned long subsystemState,
		unsigned long& actionGroup, bool& isControl )
	{
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return( m_profileHelper->getActionGroup( subsystem, subsystemState, actionGroup, isControl ) );
	}

	unsigned long Profile::getAccessControlGroup()
	{
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return( m_profileHelper->getAccessControlGroup() );
	}

	std::string Profile::getItsiGroupName()
	{
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return( m_profileHelper->getItsiGroupName() );
    }

    std::string Profile::getAccessControlGroupAsString()
    {
        TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return( m_profileHelper->getAccessControlGroupAsString() );
    }

    std::vector<unsigned long> Profile::getAccessibleSubsystemKeys()
    {
        TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return( m_profileHelper->getAccessibleSubsystemKeys() );
    }

    bool Profile::isLocationAssociated(unsigned long locationId)
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper is NULL");

        return m_profileHelper->isLocationAssociated(locationId);
    }

    void Profile::setAccessControlGroup(unsigned long accessControlGroupKey)
    {
        FUNCTION_ENTRY("setAccessControlGroup");

        TA_ASSERT(m_profileHelper != NULL, "applyChanges() has been called after profile was deleted");

        m_profileHelper->setAccessControlGroup(accessControlGroupKey);
        
        FUNCTION_EXIT;
    }


    void Profile::setItsiGroup(const std::string& itsiGroupName)
    {
        FUNCTION_ENTRY("setItsiGroup");

        TA_ASSERT(m_profileHelper != NULL, "applyChanges() has been called after profile was deleted");

        m_profileHelper->setItsiGroup(itsiGroupName);

        FUNCTION_EXIT;
    }

} // closes TA_Base_Core

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/BVSStage.cpp $
  * @author Robert Young
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <sstream>

#include "core/data_access_interface/entity_access/src/BVSStage.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

namespace TA_Base_Core
{
    // 
    // The entity type is fixed for all instances of this class.
    // 

    const std::string BVSStage::ENTITY_TYPE = "BVSStage";

    // 
    // Entity parameters
    //

	const std::string BVSStage::START_LOCATION            = "StartLocation";
    const std::string BVSStage::END_LOCATION				= "EndLocation";
    
    //
    // BVSStage
    //
    BVSStage::BVSStage()
        : VideoInput( ENTITY_TYPE )
    {
    }


    //
    // BVSStage
    //
    BVSStage::BVSStage( unsigned long key ) 
        : VideoInput( key, ENTITY_TYPE )
    {
    }


    ///////////////////////////////////////////////////////////////////
    // 
    // Supported Features
    //
    ///////////////////////////////////////////////////////////////////

	unsigned long BVSStage::getEndLocation()
	{
        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
		std::string value = getHelper()->getParameter(END_LOCATION);
		m_endLocation = EntityHelper::getUnsignedLongData(value);
		
		return m_endLocation;
	}

    unsigned long BVSStage::getStartLocation()
	{
        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
		std::string value = getHelper()->getParameter(START_LOCATION);
		m_startLocation = EntityHelper::getUnsignedLongData(value);
		
		return m_startLocation;
	}

    std::vector<unsigned long> BVSStage::getLocationsForStage()
	{
		m_locations.clear();

		getStartLocation();
		getEndLocation();

		std::vector<ILocation*> locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
		std::vector<ILocation*>::iterator it;

		for(it = locations.begin(); it!= locations.end(); it++)
		{
			if(m_startLocation<=(*it)->getKey() && m_endLocation>=(*it)->getKey())
			{
				m_locations.push_back((*it)->getKey());
			}
		}
		return m_locations;
	}

	bool BVSStage::isLocationPartOfStage(unsigned long locationKey)
	{
		//TD18658
		m_locations.clear();
		
		getStartLocation();
		getEndLocation();
		
		std::vector<ILocation*> locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
		std::vector<ILocation*>::iterator it;
		
		for(it = locations.begin(); it!= locations.end(); it++)
		{
			if(m_startLocation<=(*it)->getKey() && m_endLocation>=(*it)->getKey())
			{
				m_locations.push_back((*it)->getKey());
			}
		}
		//TD18658

		std::vector<unsigned long>::iterator itLocation;
		for(itLocation = m_locations.begin(); itLocation!=m_locations.end(); itLocation++)
		{
			if(locationKey == (*itLocation))
			{
				return true;
			}
		}
		return false;
	}

    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string BVSStage::getType()
    {
        return getStaticType();
    }


    //
    // getStaticType
    //
    std::string BVSStage::getStaticType()
    {
        return ENTITY_TYPE;
    }


    //
    // clone
    //
    IEntityData* BVSStage::clone( unsigned long key )
    {
        return new BVSStage( key );        
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Protected methods
    //
    ///////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////////////////////////////
    //
    // Private methods
    //
    ///////////////////////////////////////////////////////////////////////


} // TA_Base_Core

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ProfileHelper.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #10 $
  *
  * Last modification: $DateTime: 2017/06/16 10:39:06 $
  * Last modified by:  $Author: limin.zhu $
  *
  * This is the base Profile class that supports all get and set operaion on profiles
  *
  * NOTE: This will also directly read the SE_PROFILE_TYPE table to decide if the profile requires a display
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#pragma warning(disable:4284)
#endif

#include <vector>
#include <map>
#include <set>
#include <ctime>
#include <algorithm>

#include "core/data_access_interface/src/ProfileHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/data_access_interface/src/IScadaDisplay.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string ProfileHelper::DELETED_FLAG("#");
	const unsigned long ProfileHelper::NORMAL_SUBSYSTEM_STATE(0);
    const unsigned long ProfileHelper::DEFAULT_NAME_MAX_LENGTH = 85;
    const unsigned long ProfileHelper::INVALID_ACCESS_CONTROL_GROUP_KEY = 0xFFFFFFFF;
	const unsigned long ProfileHelper::CENTRAL_PROFILE_TYPE = 1;
	const unsigned long ProfileHelper::LOCAL_PROFILE_TYPE = 0;
    volatile unsigned long ProfileHelper::m_nameMaxLength = 0;
    TA_Base_Core::NonReEntrantThreadLockable ProfileHelper::m_nameMaxLengthLock;

    ProfileHelper::ProfileHelper(const unsigned long key) 
		: m_isValidData( false ),
          m_isValidDisplayData( false ),
          m_isValidAccessibleSubsystemData( false ),
          m_isValidAssociatedLocationData( false ),
          m_isNew(false),
          m_key( key),
          m_name( "" ),
          m_type(CENTRAL_PROFILE_TYPE ),
          m_accessControlGroup(INVALID_ACCESS_CONTROL_GROUP_KEY), // TD 10958 Zero is a valid access control group key
          m_itsiGroup( "" ),
          m_isSystemProfile(false),
          m_localProfile( false ),
          m_dateCreated( 0 ),
          m_dateModified( 0 ),
		  m_displayName("")
	{
	}


    ProfileHelper::ProfileHelper(const unsigned long row, TA_Base_Core::IData& data)
		:   
		m_isValidData( false ),
		m_isValidDisplayData( false ),
		m_isValidAccessibleSubsystemData( false ),
		m_isValidAssociatedLocationData( false ),
		m_isNew(false),
		m_name( "" ),
		m_type( CENTRAL_PROFILE_TYPE ), // Default the type to central to match m_localProfile = false.
		m_accessControlGroup(INVALID_ACCESS_CONTROL_GROUP_KEY), 
        m_itsiGroup( "" ),
		m_isSystemProfile(false),
		m_localProfile( false ),
		m_dateCreated( 0 ),
		m_dateModified( 0 ),
		m_displayName("")
    {
        FUNCTION_ENTRY("ProfileHelper(const unsigned long, TA_Base_Core::IData&)");        

        reloadUsing(row, data);
        FUNCTION_EXIT;
    }
	

	ProfileHelper::ProfileHelper()
		:
		m_isValidData( false ),
		m_isValidDisplayData( false ),
		m_isValidAccessibleSubsystemData( false ),
		m_isValidAssociatedLocationData( false ),
		m_isNew(true),
		m_key( 0 ),
		m_name( "" ),
		m_type( CENTRAL_PROFILE_TYPE ), // Default the type to central to match m_localProfile = false.
		m_accessControlGroup(INVALID_ACCESS_CONTROL_GROUP_KEY), // TD 10958
        m_itsiGroup( "" ),
		m_isSystemProfile(false),
		m_localProfile( false ),
		m_dateCreated( 0 ),
		m_dateModified( 0 ),
		m_displayName("")
	{
		resetLocationDisplays();
        initialiseMaxFieldLengths();
	}


	ProfileHelper::ProfileHelper( const ProfileHelper& theHelper)
        : m_isValidData(false),
          m_isValidDisplayData( false ),
          m_isValidAccessibleSubsystemData( false ),
          m_isValidAssociatedLocationData( false ),
          m_isNew(true),
		  m_key(0),
		  m_name(theHelper.m_name),
          m_type(theHelper.m_type),
		  m_accessControlGroup(theHelper.m_accessControlGroup),
		  m_itsiGroup(theHelper.m_itsiGroup),
          m_isSystemProfile(theHelper.m_isSystemProfile),
          m_localProfile( theHelper.m_localProfile ),
          m_dateCreated(0),
          m_dateModified(0),
		  m_defaultProfileParams(theHelper.m_defaultProfileParams),
		  m_actionGroupNameMap(theHelper.m_actionGroupNameMap),
		  m_actionGroupMap(theHelper.m_actionGroupMap),
		  m_accessibleSubsystems(theHelper.m_accessibleSubsystems),
          m_associatedLocationKeys(theHelper.m_associatedLocationKeys),
		  m_displayName(theHelper.m_displayName)
    {
    }


	ProfileHelper::~ProfileHelper()
	{
	}


    unsigned long ProfileHelper::getKey()
    {
        // This method cannot be called until the key has been set
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_key;
    }


    std::string ProfileHelper::getName()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_name;
    }


	void ProfileHelper::setName( const std::string& name )
	{
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "ProfileHelper::setName" );
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        m_name = name;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "ProfileHelper::setName" );
	}
    
	  std::string ProfileHelper::getDisplayName()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_displayName;
    }
	void ProfileHelper::setDisplayName( const std::string& displayName )
	{
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "ProfileHelper::setDisplayName" );
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }
        m_displayName = displayName;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "ProfileHelper::setDisplayName" );
	}

    unsigned long ProfileHelper::getType()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_type;
    }

	std::string ProfileHelper::getTypeAsString()
	{
		return( getTypeName(getType()) );
	}
    
    std::string ProfileHelper::getTypeName(unsigned long typeKey)
    {
        FUNCTION_ENTRY("getTypeName");

        if (m_profileTypes.empty())
        {
            // We must load the profile types from the database
            TA_Base_Core::IDatabase* pDb = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(pDb->GetDbServerType(), SE_PROFILE_TYPE_STD_SELECT_21501);
//			std::string strSql  = pDb->prepareSQLStatement(SE_PROFILE_TYPE_SELECT_21501);
		SQLStatement strSql;
		pDb->prepareSQLStatement(strSql, SE_PROFILE_TYPE_SELECT_21501);
//			TA_ASSERT(strSql.size() > 0, "ProfileHelper::getTypeName(): Prepare SQL Statement error");

            /*std::string sql("select SEPTYP_ID, NAME from SE_PROFILE_TYPE");*/

		    std::vector<std::string> cols;
		    cols.push_back( "key" );
            cols.push_back( "name" );

		    IData* data = pDb->executeQuery( strSql, cols );

            try
            {
                do
                {
                    for (unsigned int i = 0; i < data->getNumRows(); ++i)
                    {
                        unsigned long key = data->getUnsignedLongData( i, "key" );
                        std::string name = data->getStringData(i, "name" );
                        m_profileTypes.insert(std::map<unsigned long, std::string>::value_type( key, name ) );
                    }
                    delete data;
                    data = NULL;

                }
				while( pDb->moreData( data) );
			}
			//exception
            catch ( ... )
            {
                delete data;
                data =  NULL;
            }
        }

        std::map<unsigned long, std::string>::iterator matchingType = m_profileTypes.find(typeKey);
        if (matchingType != m_profileTypes.end() )
        {
            FUNCTION_EXIT;
            return matchingType->second;
        }

        FUNCTION_EXIT;
        return "";
    }

        
        
    void ProfileHelper::setType( const unsigned long type )
	{
        FUNCTION_ENTRY("setType");

        if ( !m_isValidData && !m_isNew )
		{
            reload();
		}
        m_type = type;

		m_localProfile = false;
        if (m_type == LOCAL_PROFILE_TYPE)
        {
			m_localProfile = true;
		}

		// TD16290++
        if ( !m_isValidData && !m_isNew )
		{
			//TD15951++
			reloadDisplayData();
			//resetLocationDisplays(); //This is already called inside reloadDisplayData()
			//++TD15951
		}
		// ++TD16290

        FUNCTION_EXIT;
	}

    
    bool ProfileHelper::isSystemProfile()
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        return m_isSystemProfile;
	}


	bool ProfileHelper::isLocalProfile( )
	{

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        return m_localProfile;
	}


	std::string ProfileHelper::getDefaultDisplay( unsigned long locationKey, int displayNumber )
	{
        // This method cannot be called until the name has been set
        std::ostringstream displayName;
        displayName << DISPLAY << " " << displayNumber;

        return getParameterValue(locationKey, displayName.str());
	}

    
    std::string ProfileHelper::getParameterValue(unsigned long locationKey, const std::string& parameterName)
	{
        if ( !m_isValidDisplayData && !m_isNew )
		{
            reloadDisplayData();
        }

        if (!m_localProfile)
        {
            // For a central profile the locationKey is always 0
            locationKey = 0;
        }

        // find the location and return the new display
		DefaultProfileParamMap::iterator it = m_defaultProfileParams.find( locationKey );

		if ( it != m_defaultProfileParams.end() )
		{
            // HACK until this is properly reworked
            // this whole getParameterValue function should be replaced with
            // seperarate functions

            // Hmm is this even called any more?
            // it looks like it would fail badly for those parameters that are not displays
            // I think i'll assert on non display requests and see what breaks...
            if ( parameterName.compare(DISPLAY_1) == 0 )
            {
                unsigned long displayKey = it->second.display1;
                return ScadaDisplayAccessFactory::getInstance().getScadaDisplay(displayKey)->getPath();
            }
            if ( parameterName.compare(DISPLAY_2) == 0 )
            {
                unsigned long displayKey = it->second.display2;
                return ScadaDisplayAccessFactory::getInstance().getScadaDisplay(displayKey)->getPath();
            }
            if ( parameterName.compare(DISPLAY_3) == 0 )
            {
                unsigned long displayKey = it->second.display3;
                return ScadaDisplayAccessFactory::getInstance().getScadaDisplay(displayKey)->getPath();
            }
            TA_ASSERT(false, "Called getParameterValue for a non display parameter!"); 
		} 

		// If there is no location, then return nothing
		return "";
	}

    
    std::string ProfileHelper::getParameterActualValue(unsigned long locationKey, const std::string& parameterName)
	{
        if ( !m_isValidDisplayData && !m_isNew )
		{
            reloadDisplayData();
        }

        if (!m_localProfile)
        {
            // For a central profile the locationKey is always 0
            locationKey = 0;
        }

        // find the location and return the new display
		DefaultProfileParamMap::iterator it = m_defaultProfileParams.find( locationKey );

        // HACK until this is properly reworked
        // this whole getParameterActualValue function should be replaced with
        // seperarate functions

        // Yes the literal strings should be static const
        // but they are alread static const's in two different classes (bannerDlg.cpp, DisplyListCtrl.cpp)
        // thus making a mockery of the define only in one place concept
		if ( it != m_defaultProfileParams.end() )
		{
            if ( parameterName.compare(DISPLAY_1) == 0 )
            {
                std::ostringstream display;
                display << it->second.display1;
                return display.str();
            }
            if ( parameterName.compare(DISPLAY_2) == 0 )
            {
                std::ostringstream display;
                display << it->second.display2;
                return display.str();
            }
            if ( parameterName.compare(DISPLAY_3) == 0 )
            {
                std::ostringstream display;
                display << it->second.display3;
                return display.str();
            }
            if ( parameterName.compare(LeftBannerDefaultPage) == 0 )
            {
                return it->second.leftBanner ;
            }
            if ( parameterName.compare(CentreBannerDefaultPage) == 0 )
            {
                return it->second.centreBanner ;
            }
            if ( parameterName.compare(RightBannerDefaultPage) == 0 )
            {
                return it->second.rightBanner ;
            }
            TA_ASSERT(false,"Called getParameterValue for unknown parameter!"); 
		} 

		// If there is no location, then return nothing
		return "";
	}

    
    unsigned long ProfileHelper::getDisplayKey(unsigned long locationKey, int displayNumber)
    {
        // This method cannot be called until the name has been set
        std::ostringstream displayName;
        //displayName << "DISPLAY" << displayNumber << "_KEY";
        displayName << DISPLAY << " " << displayNumber;

        return getDisplayKey(locationKey, displayName.str());
    }


    unsigned long ProfileHelper::getDisplayKey(unsigned long locationKey, const std::string& parameterName)
	{
        if ( !m_isValidDisplayData && !m_isNew )
		{
            reloadDisplayData();
        }

        if (!m_localProfile)
        {
            // For a central profile the locationKey is always 0
            locationKey = 0;
        }

        // find the location and return the new display
		DefaultProfileParamMap::iterator it = m_defaultProfileParams.find( locationKey );

		if ( it != m_defaultProfileParams.end() )
		{
            // Ok we've found the right location so now look for the display number
            if ( parameterName.compare(DISPLAY_1) == 0 )
            {
                return it->second.display1;
            }
            if ( parameterName.compare(DISPLAY_2) == 0 )
            {
                return it->second.display2;
            }
            if ( parameterName.compare(DISPLAY_3) == 0 )
            {
                return it->second.display3;
            }
		} 

		// If there is no location, then throw an exception
        std::ostringstream reasonMsg;
        reasonMsg << "There was no display for display " << parameterName << " at location " << locationKey;
        TA_THROW( DataException(reasonMsg.str().c_str(), DataException::NO_VALUE, "") );
	}


    void ProfileHelper::setDisplay(unsigned long location, unsigned long screenNumber, long display)
    {
        FUNCTION_ENTRY("setDisplay");

        // This method cannot be called until the name has been set
        std::ostringstream displayName;
        displayName << DISPLAY << " " << screenNumber;

        std::ostringstream displayNumber;
        displayNumber << display;

        setParameter(location, displayName.str(), displayNumber.str());

        FUNCTION_EXIT;
    }


    void ProfileHelper::setParameter(unsigned long location, const std::string& parameterName, const std::string& parameterValue)
	{
        FUNCTION_ENTRY("setParameter");

        if ( !m_isValidDisplayData && !m_isNew )
		{
            reloadDisplayData();
        }

        // Update (or add) the display in the map

        // HACK this function should be replaced with seperate functions

        DefaultProfileParamMap::iterator matchingLocation = m_defaultProfileParams.find( location );
        if ( matchingLocation == m_defaultProfileParams.end() )
        {
            ProfileParameters tempParameters;
            tempParameters.display1 = 0;
            tempParameters.display2 = 0;
            tempParameters.display3 = 0;
            tempParameters.leftBanner = "";
            tempParameters.centreBanner = "";
            tempParameters.rightBanner = "";

            // Yes the literal strings should be static const
            // but they are alread static const's in two different classes (bannerDlg.cpp, DisplyListCtrl.cpp)
            // thus making a mockery of the define only in one place concept
            if ( parameterName.compare(DISPLAY_1) == 0 )
            {
                tempParameters.display1 = atol(parameterValue.c_str());
            }
            else if ( parameterName.compare(DISPLAY_2) == 0 )
            {
                tempParameters.display2 = atol(parameterValue.c_str());
            }
            else if ( parameterName.compare(DISPLAY_3) == 0 )
            {
                tempParameters.display3 = atol(parameterValue.c_str());
            }
            else if ( parameterName.compare(LeftBannerDefaultPage) == 0 )
            {
                tempParameters.leftBanner = parameterValue;
            }
            else if ( parameterName.compare(CentreBannerDefaultPage) == 0 )
            {
                tempParameters.centreBanner = parameterValue;
            }
            else if ( parameterName.compare(RightBannerDefaultPage) == 0 )
            {
                tempParameters.rightBanner = parameterValue;
            }
            else
            {
                TA_Assert("Called setParameter for unknown parameter!"); 
            }

            m_defaultProfileParams.insert( DefaultProfileParamMap::value_type( location, tempParameters ) );
            
            FUNCTION_EXIT;
            return;
		}

        //Otherwise it exists and we just have to update it

        // Yes the literal strings should be static const
        // but they are alread static const's in two different classes (bannerDlg.cpp, DisplyListCtrl.cpp)
        // thus making a mockery of the define only in one place concept
        if ( parameterName.compare(DISPLAY_1) == 0 )
        {
            matchingLocation->second.display1 = atol(parameterValue.c_str());
        }
        else if ( parameterName.compare(DISPLAY_2) == 0 )
        {
            matchingLocation->second.display2 = atol(parameterValue.c_str());
        }
        else if ( parameterName.compare(DISPLAY_3) == 0 )
        {
            matchingLocation->second.display3 = atol(parameterValue.c_str());
        }
        else if ( parameterName.compare(LeftBannerDefaultPage) == 0 )
        {
            matchingLocation->second.leftBanner = parameterValue;
        }
        else if ( parameterName.compare(CentreBannerDefaultPage) == 0 )
        {
            matchingLocation->second.centreBanner = parameterValue;
        }
        else if ( parameterName.compare(RightBannerDefaultPage) == 0 )
        {
            matchingLocation->second.rightBanner = parameterValue;
        }
        else
        {
            TA_Assert("Called setParameter for unknown parameter!"); 
        }

        FUNCTION_EXIT;
	}


    void ProfileHelper::removeDisplay(unsigned long location, unsigned long screenNumber)
    {
        FUNCTION_ENTRY("removeDisplay");

        // This method cannot be called until the name has been set
        std::ostringstream displayName;
        displayName << DISPLAY << " " << screenNumber;

        removeParameter(location, displayName.str());
        
        FUNCTION_EXIT;
    }


    void ProfileHelper::removeParameter(unsigned long location, const std::string& parameterName)
	{
        FUNCTION_ENTRY("removeDisplay");

        if ( !m_isValidDisplayData && !m_isNew )
		{
            reloadDisplayData();
		}

		// Remove the display from the map

        DefaultProfileParamMap::iterator matchingLocation = m_defaultProfileParams.find( location );
      
		if ( matchingLocation == m_defaultProfileParams.end() )
		{
            // This doesn't exist in the map so we have nothing to worry about
            FUNCTION_EXIT;
            return;
		}

        // Now attempt to erase the screen number if it exists
        //matchingLocation->second.erase( screenNumber );
        // matchingLocation->second.erase(parameterName);
        // should really delete this function
        TA_Assert("Called old function");
        FUNCTION_EXIT;
	}

    
    time_t ProfileHelper::getDateCreated()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t ProfileHelper::getDateModified()
	{
        // This method cannot be called until the name has been set
        if (!m_isValidData && !m_isNew)
		{
            reload();
		}
        return m_dateModified;
	}
        
        
    std::vector<ILocation*> ProfileHelper::getAssociatedLocations()
	{
        FUNCTION_ENTRY("getAssociatedLocations");

        // Cannot retrieve associated locations for a new profile (they haven't been set yet...)
		TA_ASSERT(!m_isNew,"m_key has not been set");

        // Use the location access factory to retrieve the necessary details
        FUNCTION_EXIT;
		return LocationAccessFactory::getInstance().getLocationsByProfile(m_key);
	}

    std::vector<unsigned long> ProfileHelper::getAssociatedLocationKeys()
    {
        FUNCTION_ENTRY("getAssociatedLocationKeys");
        if (m_associatedLocationKeys.empty() && !m_isNew)
        {
            m_associatedLocationKeys = getLocationKeys();
        }

        FUNCTION_EXIT;
        return m_associatedLocationKeys;
    }


	std::string ProfileHelper::getActionGroupAsStringForResource( unsigned long resourceKey )
	{
		TA_ASSERT(!m_isNew,"m_key has not been set");

		SubsystemMap::const_iterator subIt = m_subsystemMap.find( resourceKey );

		if( subIt != m_subsystemMap.end() )
		{
			ActionGroupKey agKey;
			agKey.push_back(subIt->second);
			agKey.push_back(NORMAL_SUBSYSTEM_STATE);

			ActionGroupNameMap::const_iterator dutIt = m_actionGroupNameMap.find( agKey );

			if( dutIt != m_actionGroupNameMap.end() )
			{
				return( dutIt->second );
			}
		}

		TA_Base_Core::IDatabase* pDb = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(pDb->GetDbServerType(), se_action_group_STD_SELECT_11507, resourceKey, NORMAL_SUBSYSTEM_STATE, m_key);
//		std::string strSql  = pDb->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11507, resourceKey, NORMAL_SUBSYSTEM_STATE, m_key);
		SQLStatement strSql;
		pDb->prepareSQLStatement(strSql, SE_ACTION_GROUP_SELECT_11507, resourceKey, NORMAL_SUBSYSTEM_STATE, m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::getActionGroupAsStringForResource(): Prepare SQL Statement error");

		/*std::ostringstream sql;
		sql << "select SE_ACTION_GROUP.NAME, SE_PROFILE_ACCESS.SUBSYSTEMKEY from SE_ACTION_GROUP, SE_PROFILE_ACCESS "
			<< "where SE_PROFILE_ACCESS.SUBSYSTEMKEY = "
			<< "(select SE_RESOURCE_V.SUBSYSTEMKEY from SE_RESOURCE_V where SE_RESOURCE_V.PKEY = " 
			<< resourceKey << ") "
			<< "and SE_PROFILE_ACCESS.SESSTA_ID = " << NORMAL_SUBSYSTEM_STATE << " "
			<< "and SE_PROFILE_ACCESS.SEPROF_ID = " << m_key << " "
			<< "and SE_PROFILE_ACCESS.SEAGRO_ID = SE_ACTION_GROUP.SEAGRO_ID";*/

		std::vector<std::string> cols;
		cols.push_back( "action_group" );
		cols.push_back( "subsystem_key" );

		std::auto_ptr<TA_Base_Core::IData> data( pDb->executeQuery( strSql, cols ) );

		// not finding an entry is NOT an error
		if( (NULL == data.get() ) || (0 == data->getNumRows()) ) 
        {
			return( "" );
        }

		// finding more than 1 entry IS and error!
		if( 1 != data->getNumRows() )
		{
			std::ostringstream reason;

			reason << "More than one ActionGroup found for profile ("
				   << m_key << ") "
				   << "resource (" << resourceKey << ")";

            TA_THROW( DataException(reason.str().c_str(), DataException::NOT_UNIQUE, "") );
		}

		std::string rval = data->getStringData( 0, cols[0] );
		unsigned long subsystemKey = data->getUnsignedLongData( 0, cols[1] );

		ActionGroupKey agKey;
		agKey.push_back(subsystemKey);
		agKey.push_back(NORMAL_SUBSYSTEM_STATE);
		if( rval.size() > 0 )
		{
			m_subsystemMap.insert( SubsystemMap::value_type(resourceKey , subsystemKey) );
			m_actionGroupNameMap.insert ( ActionGroupNameMap::value_type (agKey, rval) ); 
		}

		return( rval );
	}

	std::string ProfileHelper::getActionGroupAsStringForSubsystem( unsigned long subsystemKey,
		   unsigned long subsystemStateKey	)
	{
		TA_ASSERT(!m_isNew,"m_key has not been set");

		ActionGroupKey agKey;
		agKey.push_back(subsystemKey);
		agKey.push_back(subsystemStateKey);

		ActionGroupNameMap::iterator it = m_actionGroupNameMap.find( agKey );

		if( m_actionGroupNameMap.end() != it )
		{
			return( it->second );
		}

		TA_Base_Core::IDatabase* pDb = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(pDb->GetDbServerType(), se_action_group_STD_SELECT_11508, subsystemKey, subsystemStateKey, m_key);
//		std::string strSql  = pDb->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11508, subsystemKey, subsystemStateKey, m_key);
		SQLStatement strSql;
		pDb->prepareSQLStatement(strSql, SE_ACTION_GROUP_SELECT_11508, subsystemKey, subsystemStateKey, m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::getActionGroupAsStringForSubsystem(): Prepare SQL Statement error");

		/*std::ostringstream sql;
		sql << " select a.NAME from SE_ACTION_GROUP a, SE_PROFILE_ACCESS b"
			<< " where b.SUBSYSTEMKEY = "<< subsystemKey
			<< " and b.SESSTA_ID = " << subsystemStateKey
			<< " and b.SEPROF_ID = " << m_key
			<< " and b.SEAGRO_ID = a.SEAGRO_ID";*/

		std::vector<std::string> cols;
		cols.push_back( "action_group" );

		std::auto_ptr<TA_Base_Core::IData> data( pDb->executeQuery( strSql, cols ) );

		// not finding an entry is NOT an error
		if( (NULL == data.get() ) || (0 == data->getNumRows()) ) 
        {
			return( "" );
        }

		// finding more than 1 entry IS and error!
		if( 1 != data->getNumRows() )
		{
			std::ostringstream reason;

			reason << "More than one ActionGroup found for profile ("
				   << m_key << ") "
				   << "subsystem (" << subsystemKey << ")";

            TA_THROW( DataException(reason.str().c_str(), DataException::NOT_UNIQUE, "") );
		}

		std::string rval = data->getStringData( 0, cols[0] );

		if( rval.size() > 0 )
		{
			m_actionGroupNameMap.insert( ActionGroupNameMap::value_type(agKey, rval) );
		}

		return( rval );
	}

	bool ProfileHelper::getActionGroup( unsigned long subsystem, unsigned long subsystemState,
		unsigned long& actionGroup, bool& isControl )
	{
		TA_ASSERT(!m_isNew,"m_key has not been set");

		ActionGroupKey   agKey;
		agKey.push_back(subsystem);
		agKey.push_back(subsystemState);

		ActionGroupMap::const_iterator it;

		it = m_actionGroupMap.find( agKey );

		if( it != m_actionGroupMap.end() )
		{
			actionGroup = it->second.m_actionGroupKey;
			isControl   = it->second.m_isControl;
			return( true );
		}

		TA_Base_Core::IDatabase* pDb = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(pDb->GetDbServerType(), se_action_group_STD_SELECT_11509, subsystem, subsystemState, m_key);
//		std::string strSql  = pDb->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11509, subsystem, subsystemState, m_key);
		SQLStatement strSql;
		pDb->prepareSQLStatement(strSql, SE_ACTION_GROUP_SELECT_11509, subsystem, subsystemState, m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::getActionGroup(): Prepare SQL Statement error");

		/*std::ostringstream sql;
		sql << " select a.SEAGRO_ID, a.IS_CONTROLTYPE"
			<< " from SE_ACTION_GROUP a, SE_PROFILE_ACCESS b"
			<< " where b.SUBSYSTEMKEY = " << subsystem
			<< " and b.SESSTA_ID = " << subsystemState
			<< " and b.SEPROF_ID = " << m_key
			<< " and b.SEAGRO_ID = a.SEAGRO_ID";*/

		std::vector<std::string> cols;
		cols.push_back( "action_group"   );
		cols.push_back( "is_control"  );

		std::auto_ptr<TA_Base_Core::IData> data( pDb->executeQuery( strSql, cols ) );

		// not finding an entry is NOT an error
		if( (NULL == data.get() ) || (0 == data->getNumRows()) ) 
        {
			return( false );
        }

		// finding more than 1 entry IS and error!
		if( 1 != data->getNumRows() )
		{
			std::ostringstream reason;

			reason << "More than one ActionGroup found for profile ("
				   << m_key << ") "
				   << "subsystem (" << subsystem << ")";

            TA_THROW( DataException(reason.str().c_str(), DataException::NOT_UNIQUE, "") );
		}

		actionGroup  =  data->getUnsignedLongData( 0, cols[0] );
		isControl    = (data->getUnsignedLongData( 0, cols[1] ) == 1);

		ActionGroupEntry newEntry( actionGroup, isControl );

		return( m_actionGroupMap.insert( ActionGroupMap::value_type(agKey, newEntry) ).second );

		//return( true );
	}

	unsigned long ProfileHelper::getAccessControlGroup()
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		return m_accessControlGroup;
	}

    
    std::string ProfileHelper::getAccessControlGroupAsString()
	{
		return( getAccessControlGroupName(getAccessControlGroup()) );
	}

	
    std::string ProfileHelper::getAccessControlGroupName(unsigned long accessControlGroupKey)
    {
        FUNCTION_ENTRY("getAccessControlGroupName");

        std::map<unsigned long, std::string> groupNames;

        // We must load the profile types from the database
        TA_Base_Core::IDatabase* pDb = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(pDb->GetDbServerType(), SE_PROFILE_GROUP_STD_SELECT_19501);
//		std::string strSql  = pDb->prepareSQLStatement(SE_PROFILE_GROUP_SELECT_19501);
		SQLStatement strSql;
		pDb->prepareSQLStatement(strSql, SE_PROFILE_GROUP_SELECT_19501);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::getAccessControlGroupName(): Prepare SQL Statement error");

        /*std::string sql("select SEPGRO_ID, NAME from SE_PROFILE_GROUP");*/

		std::vector<std::string> cols;
		cols.push_back( "key" );
        cols.push_back( "name" );

		IData* data = pDb->executeQuery( strSql, cols );

        try
        {

            do
            {

                for (unsigned int i = 0; i < data->getNumRows(); ++i)
                {
                    unsigned long key = data->getUnsignedLongData( i, "key" );
                    std::string name = data->getStringData(i, "name" );
                    groupNames.insert(std::map<unsigned long, std::string>::value_type( key, name ) );
                }
                delete data;
                data = NULL;

            }
			while( pDb->moreData( data) );
		}
		//exception
        catch ( ... )
        {
            delete data;
            data =  NULL;
        }
        
        std::map<unsigned long, std::string>::iterator matchingType = groupNames.find(accessControlGroupKey);
        if (matchingType != groupNames.end() )
        {
            FUNCTION_EXIT;
            return matchingType->second;
        }

        FUNCTION_EXIT;
        return "";
    }

    std::string ProfileHelper::getItsiGroupName()
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		return m_itsiGroup;
    }

    std::vector<unsigned long> ProfileHelper::getAccessibleSubsystemKeys()
    {
        if (!m_isValidAccessibleSubsystemData && !m_isNew)
        {
            reloadAccessibleSubsystemData();
        }

        return m_accessibleSubsystems;
    }

    bool ProfileHelper::isLocationAssociated(unsigned long locationId)
    {
        FUNCTION_ENTRY("isLocationAssociated");
        if (!m_isValidAssociatedLocationData && !m_isNew)
        {
            reloadAssociatedLocationData();
        }

        std::vector<unsigned long>::iterator result = std::find(m_associatedLocationKeys.begin(),
            m_associatedLocationKeys.end(),locationId);

        FUNCTION_EXIT;
        return (result != m_associatedLocationKeys.end());
    }

    void ProfileHelper::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "ProfileHelper::invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new Profile");

        m_isValidData = false;
        m_isValidDisplayData = false;
        m_isValidAccessibleSubsystemData = false;
        m_isValidAssociatedLocationData = false;

        m_defaultProfileParams.clear();
		m_actionGroupNameMap.clear();
		m_actionGroupMap.clear();
		m_subsystemMap.clear();
        m_accessibleSubsystems.clear();
        m_associatedLocationKeys.clear();

        LOG ( SourceInfo, DebugUtil::FunctionExit, "ProfileHelper::invalidate" );
    }


    void ProfileHelper::writeData()
    {
        FUNCTION_ENTRY("writeData");

		//TD17817 marc_bugfix
		//Check first if associated locations have been inserted into the m_defaultProfileParams map
		//There was a case in which the available button was checked, but the user did not supply
		//the mandatory values
		//This is additional checking for these types of scenarios.

		for (std::vector<unsigned long>::iterator iter_keys = m_associatedLocationKeys.begin(); iter_keys != m_associatedLocationKeys.end(); iter_keys++)
		{
			ProfileParameters pProfileParameters;
			DefaultProfileParamMap::iterator resultIter = m_defaultProfileParams.find((unsigned long)(*iter_keys));	

			if (resultIter != m_defaultProfileParams.end())
			{
				//Location is already in default profile params, no need to insert
			}
			else if((*iter_keys) != 0 && m_localProfile) //Do not insert location with key value of zero (All locations)
			{
				//Add new entry in  default profile parameters
				memset(&pProfileParameters, 0 , sizeof(pProfileParameters));
				m_defaultProfileParams[*iter_keys] = pProfileParameters;
			}
			else if ((*iter_keys) == 0 && !m_localProfile)
			{
				memset(&pProfileParameters, 0 , sizeof(pProfileParameters));
				m_defaultProfileParams[0] = pProfileParameters;
     		//If all locations found, break immediately since all locations is only for central profile
				break;

			}
		}
	

        // Check all values exist. If not add them to the dataConfiguraitonException
        std::vector<std::string> fieldNames;
        if (m_name.empty())
        {
            fieldNames.push_back("Name");
        }

        if (INVALID_ACCESS_CONTROL_GROUP_KEY == m_accessControlGroup)
        {
            fieldNames.push_back("Profile Group");
        }

        const std::string DISPLAY_NAME(DISPLAY_1);
        
        for ( DefaultProfileParamMap::iterator iter = m_defaultProfileParams.begin(); iter != m_defaultProfileParams.end(); ++iter)
        {
            std::vector<unsigned long>::iterator resultIter = std::find(m_associatedLocationKeys.begin(),
                m_associatedLocationKeys.end(),iter->first);


			// TD #12987
			// we only care if the *associated* locations haven't been completely specified,
			// so we need this condition here
			if ( resultIter != m_associatedLocationKeys.end() )
			{
				//TD17817 - marc_bugfix
				if (iter->first == 0 && m_localProfile) continue;
				if (iter->first != 0 && !m_localProfile) continue;

				// The display has not been set if we can't find the DISPLAY_NAME variable for this location,
				// but if this is NOT an associated location then we don't care...
				if ( iter->second.display1 == 0 )
				{
					// No display 1 has been set for this location. It is mandatory so we add it to the list
					ILocation* location = LocationAccessFactory::getInstance().getLocationByKey( iter->first );
					std::string field = "Location ";
					field += location->getName();
					field += ", ";
                    field += DISPLAY_1;
					fieldNames.push_back( field );

					delete location;
					location = NULL;
				}

				// TD #12987
				// Left Banner is now also a required parameter
				if ( (iter->second.leftBanner).empty() )
				{
					// No left banner has been set for this location. It is mandatory so we add it to the list
					ILocation* location = LocationAccessFactory::getInstance().getLocationByKey( iter->first );
					std::string field = "Location ";
					field += location->getName();
                    field += ", ";
					field += LeftBannerDefaultPage;
					fieldNames.push_back( field );

					delete location;
					location = NULL;
				}
			}
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Profile data not fully specified. Profile cannot be written to database",
                                                         fieldNames));
        }

        // Write the Profile to the database

        // Need to check if this is a new profile or an existing profile - 
        // as a new profile will be inserted, while an existing profile will be updated
        if (m_isNew) // This is a new profile
        {
            addNewProfile();
        }

        else // This is an existing profile
        {
            modifyExistingProfile();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isValidDisplayData = true;
        m_isNew = false;
        FUNCTION_EXIT;
    }


    void ProfileHelper::deleteProfile()
    {
        FUNCTION_ENTRY("deleteProfile");

        TA_ASSERT(!m_isNew, "This operator does not yet exist in the database, and therefore cannot be deleted");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
        
        // We must retrieve the new name and description for the operator when it is deleted
        std::string newProfileName = retrieveDeletedField(m_name, "NAME", m_nameMaxLength);

        /*std::ostringstream sql;
        sql << "update SE_PROFILE set DELETED = 1, name = '" << databaseConnection->escapeInsertString(newProfileName);
        sql << "' where SEPROF_ID = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_STD_UPDATE_7651, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_UPDATE_7651, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_UPDATE_7651,
			databaseConnection->escapeInsertString(newProfileName), m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::deleteProfile(): Prepare SQL Statement error");

        databaseConnection->executeModification(strSql);

        /*sql.str("");
        sql << "delete SE_PROFILE_PARAMETERS where SEPROF_ID = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_PARAMETERS_STD_DELETE_22401, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_PARAMETERS_DELETE_22401, m_key);

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_PARAMETERS_DELETE_22401, m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::deleteProfile(): Prepare SQL Statement error");

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


	void ProfileHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

        columnNames.push_back("SEPROF_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("PROFILE_TYPE");
        columnNames.push_back("PROFILE_GROUP");
        columnNames.push_back("PROFILE_ITSI");
        //columnNames.push_back("PAPGRO_NAME");
        columnNames.push_back("IS_SYSTEM");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
		columnNames.push_back("DISPLAY_NAME");
	}	


    void ProfileHelper::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "ProfileHelper::reload" );
        
		// Ensure the gui has already been written to the database. It doesn't make
        // any sense to reload an gui that doesn't exist in the database.
        TA_ASSERT( !m_isNew, "This profile does not yet exist in the database. Therefore it cannot be reloaded.");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_SELECT_7556, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7556, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_SELECT_7556, m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::reload(): Prepare SQL Statement error");

        // create the SQL string to retrieve the data of the gui
        // based upon the key
        /*std::ostringstream sql;
		sql << "select SEPROF_ID, NAME, SEPTYP_ID, SEPGRO_ID, RAIGRO_NAME, IS_SYSTEM, ";
        sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')";
        sql << " from SE_PROFILE_V where SEPROF_ID = " << m_key;*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql,columnNames));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reasonMessage;
			reasonMessage << "No profile found for pkey = " << m_key;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of profile records."); 
        reloadUsing(0, *data);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "ProfileHelper::reload" );
    }


	void ProfileHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key  = data.getUnsignedLongData( row, columnNames[0] ); 
		m_name = data.getStringData( row, columnNames[1] );
		//TD17081++
        //m_type = (LOCAL_PROFILE_TYPE == data.getUnsignedLongData(row, columnNames[2])) ? true : false;
		m_type = data.getUnsignedLongData(row, columnNames[2]) ;
		m_localProfile = (LOCAL_PROFILE_TYPE == m_type ) ? true : false;
		//++TD17081
        
		m_accessControlGroup = data.getUnsignedLongData( row, columnNames[3] );
		m_itsiGroup = data.getStringData(row, columnNames[4] );
        //m_paPriorityGroup = data.getStringData(row, columnNames[5]);
        m_isSystemProfile = data.getBooleanData(row, columnNames[5] );

        m_dateModified = data.getDateData(row,columnNames[6], 0);
        m_dateCreated = data.getDateData( row, columnNames[7]);
        m_displayName = data.getStringData( row, columnNames[8]);

        initialiseMaxFieldLengths();

        m_isValidData = true;
        FUNCTION_EXIT;
    }
	

	void ProfileHelper::reloadDisplayData()
	{
        FUNCTION_ENTRY("ProfileHelper::reloadDisplayData" );

		// Make sure the main profile data is loaded!
		if ( ! m_isValidData && ! m_isNew )
		{
			reload();
		}

		// Clear the displays map, so that it contains an entry for each location, but
		// none of them are mapped to a display.
		resetLocationDisplays();
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_PARAMETERS_STD_SELECT_22001, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_PARAMETERS_SELECT_22001, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_PARAMETERS_SELECT_22001, m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::reloadDisplayData(): Prepare SQL Statement error");

		// Load the profile Default Displays
		/*std::ostringstream query;
		query << "SELECT locationkey, display_1, display_2, display_3, left_banner, centre_banner, right_banner ";
        query << "FROM SE_PROFILE_PARAMETERS ";
        query << "WHERE SEPROF_ID = " << m_key ;*/

		// Set up the columns
		std::vector<std::string> columnNames;
		std::string locationColumn( "locationKey" );
		std::string display1Column( "display_1" );
		std::string display2Column( "display_2" );
		std::string display3Column( "display_3" );
		std::string leftBannerColumn( "left_banner" );
		std::string centreBannerColumn( "centre_banner" );
		std::string rightBannerColumn( "right_banner" );
		columnNames.push_back( locationColumn );
		columnNames.push_back( display1Column );
		columnNames.push_back( display2Column );
		columnNames.push_back( display3Column );
        columnNames.push_back( leftBannerColumn );
        columnNames.push_back( centreBannerColumn );
        columnNames.push_back( rightBannerColumn );

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
		IData* data = databaseConnection->executeQuery( strSql, columnNames);
		
		if ( NULL == data || 0 == data->getNumRows() ) // No default displays configured!
        {
            delete data;
            data = NULL;
            m_isValidDisplayData = true;
            FUNCTION_EXIT;
            return;
        }

		// If a local profile then we can have many profiles, up to one per location
		// This has been handled above

		// Loop for each row returned (note: There will only be one row returned if this is a central profile
		do
		{
			unsigned int numRows = data->getNumRows();
        
			for (unsigned long i = 0; i < numRows; i++)
			{
				// The getUnsignedLongData() call can throw an exception. Need to catch
				// it to do pointer clean up.
               
				try
				{
                    // Check to make sure the locationField is not NULL
					// If it returns ULONG_MAX (this is what we inserted in the NVL statement )
					// then the profiles are not configured correctly
					unsigned long locationKey = data->getUnsignedLongData( i, locationColumn );
                    
                    unsigned long display1 = data->getUnsignedLongData( i, display1Column );
                    unsigned long display2 = data->getUnsignedLongData( i, display2Column );
                    unsigned long display3 = data->getUnsignedLongData( i, display3Column );

                    std::string leftBanner = data->getStringData( i, leftBannerColumn );
                    std::string centreBanner = data->getStringData( i, centreBannerColumn );
                    std::string rightBanner = data->getStringData( i, rightBannerColumn );
                    
                    DefaultProfileParamMap::iterator matching = m_defaultProfileParams.find(locationKey);
                   
					if (matching == m_defaultProfileParams.end())
                    {
                        //Not found so we must insert it new
                        ProfileParameters profileParameters;
                        profileParameters.display1 = display1;
                        profileParameters.display2 = display2;
                        profileParameters.display3 = display3;
                        profileParameters.leftBanner = leftBanner;
                        profileParameters.centreBanner = centreBanner;
                        profileParameters.rightBanner = rightBanner;
    					m_defaultProfileParams[locationKey] = profileParameters;
                    }

                    else
                    {
                        // Found so just update the map
                        matching->second.display1 = display1;
                        matching->second.display2 = display2;
                        matching->second.display3 = display3;
                        matching->second.leftBanner = leftBanner;
                        matching->second.centreBanner = centreBanner;
                        matching->second.rightBanner = rightBanner;
                    }
				}

				// data exception
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data;
					data = NULL;
					throw;
				}
			}

			delete data;
			data = NULL;
		}

		while (databaseConnection->moreData(data));

        m_isValidDisplayData = true;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "ProfileHelper::reloadDisplayData" );
	}


	void ProfileHelper::reloadAccessibleSubsystemData()
	{
        LOG ( SourceInfo, DebugUtil::FunctionExit, "ProfileHelper::reloadAccessibleSubsystemData" );

		// Purge the accessible subsystem list.
		m_accessibleSubsystems.clear();
        
        // Get a connection to the database.
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_profile_access_STD_SELECT_12506, getKey());
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_ACCESS_SELECT_12506, getKey());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_ACCESS_SELECT_12506, getKey());
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::reloadAccessibleSubsystemData(): Prepare SQL Statmenet error");

		// Set up the required columns.
		const std::string SUBSYSTEM_COLUMN("SUBSYSTEMKEY");
		std::vector<std::string> columnNames;
		columnNames.push_back(SUBSYSTEM_COLUMN);

		// Set up the SQL query.
		/*std::ostringstream sql;
        sql << "select unique " << SUBSYSTEM_COLUMN << " from SE_PROFILE_ACCESS where SEPROF_ID = " << getKey();*/

        // Load data from the database. executeQuery() can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're done with it.
		IData* data = databaseConnection->executeQuery(strSql, columnNames);
		
		do
		{
			unsigned int numRows = data->getNumRows();

            // Iterate over the returned rows        
			for (unsigned long i = 0; i < numRows; i++)
			{
				// getUnsignedLongData() can throw an exception. Need to catch it to do pointer clean up.
                try
				{
					// Add the subsystem key to the list.
                    m_accessibleSubsystems.push_back(data->getUnsignedLongData(i, SUBSYSTEM_COLUMN));
				}
				//exception
				catch (...)
				{
					delete data;
					data = NULL;

					throw;
				}
			}

			delete data;
			data = NULL;
		}

		while (databaseConnection->moreData(data));

        m_isValidAccessibleSubsystemData = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "ProfileHelper::reloadAccessibleSubsystemData" );
	}

    void ProfileHelper::reloadAssociatedLocationData()
    {
        FUNCTION_ENTRY("reloadAssociatedLocationData");

        // Clear the current list of keys
        m_associatedLocationKeys.clear();

        // Can only load the associated locations if this in not a "new" profile.
        if (!m_isNew)
        {
            // Get the associated locations
            std::vector<ILocation*> locations = LocationAccessFactory::getInstance().getLocationsByProfile(m_key);

            for (unsigned int i = 0;i < locations.size();i++)
            {
                m_associatedLocationKeys.push_back(locations[i]->getKey());

                // And delete the location objects, now they are no longer of use.
                delete locations[i];
                locations[i] = NULL;
            }

            // Set the valid flag to true
            m_isValidAssociatedLocationData = true;
        }

        FUNCTION_EXIT;
    }


    void ProfileHelper::modifyExistingProfile()
    {
        FUNCTION_ENTRY("modifyExistingProfile");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "Profile %s already exists. It's data will be updated.",m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_STD_SELECT_7501, m_name);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7501, m_name);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_SELECT_7501, m_name);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::modifyExistingProfile(): Prepare SQL Statement error");

        // Create a select statement that will ensure the uniqueness of the entity name.
        /*std::string formatSQL = "select SEPROF_ID from SE_PROFILE_V where NAME = '"
            + databaseConnection->escapeQueryString(m_name) + "'";*/

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "KEY";
        columnNames.push_back(keyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 rows returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0,keyColumn))
            {   // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "A profile already exists with name = " << m_name;
                throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name");
            }
            // else - it matches, so that's okay - it just means the name wasn't changed.
        }

        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one profile exists with name = " << m_name;
            throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name");
        }

        /*std::ostringstream sql;
        sql << "update SE_PROFILE set NAME = '";
        sql << (databaseConnection->escapeInsertString(m_name)).c_str();
        sql << "', SEPTYP_ID = " << m_type;
        sql << " where SEPROF_ID = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_STD_UPDATE_7652,
//		strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_UPDATE_7652,

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_UPDATE_7652,
			(databaseConnection->escapeInsertString(m_name)), m_type, m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::modifyExistingProfile(): Prepare SQL Statement error");

        databaseConnection->executeModification(strSql);


        //now need to update the date fields
       /* sql.str("");
        sql << " select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS')";
        sql << " from SE_PROFILE_V where SEPROF_ID = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_SELECT_7557, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7557, m_key);

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_SELECT_7557, m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::modifyExistingProfile(): Prepare SQL Statement error");

        // Can use the same columnNames vector from above.
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }

        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one entry found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name or description") );
        }

        try
        {
            m_dateCreated = data->getDateData(0,createdColumn);
            m_dateModified = data->getDateData(0,modifiedColumn);
		}
		//exception
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Now update displays in the database
        updateLocationDisplays();

        // Update the access control and radio login groups
        updateProfileGroups();

        // Update the associated location details
        updateLocationAssociations();

        FUNCTION_EXIT;
    }

        
    void ProfileHelper::addNewProfile()
    {
        FUNCTION_ENTRY("addNewProfile");
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Profile %s is a new profile. It will be added to the database.",m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_STD_SELECT_7501, databaseConnection->escapeQueryString(m_name));
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7501, databaseConnection->escapeQueryString(m_name));
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_SELECT_7501, databaseConnection->escapeQueryString(m_name));
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::addNewProfile(): Prepare SQL Statement error");

        // First check there is not alreay an operator with this name or 
        // description (it is a uniqueness constraint).

        /*std::ostringstream formatSQL;
        formatSQL << "select SEPROF_ID from SE_PROFILE_V where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";*/

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Check the number of rows returned (the actual data is not important) - it
        // should be 0
        if (0 != data->getNumRows()) // Name or description already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "A profile already exists with name " << m_name;
            using TA_Base_Core::DataException;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name"));
        }

        // delte the IData pointer now that it's finised with
        delete data;
        data = NULL;

        /*std::ostringstream sql;
        sql << "insert into SE_PROFILE_V (SEPROF_ID, NAME, SEPTYP_ID) ";
        sql << "values (SECURITY_SEQ.NEXTVAL,'";
        sql << (databaseConnection->escapeInsertString(m_name)).c_str();
        sql << "'," << m_type << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_INSERT_7851,
//		strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_INSERT_7851,

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_INSERT_7851,
            databaseConnection->getNextPkey("SECURITY_SEQ"),
			databaseConnection->escapeInsertString(m_name), m_type, m_accessControlGroup, m_itsiGroup);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::addNewProfile(): Prepare SQL Statement error");

        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this location and update the date fields
        /*sql.str("");
        sql << "select SEPROF_ID, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from SE_PROFILE_V where NAME = '";
        sql << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_SELECT_7558,databaseConnection->escapeQueryString(m_name));
//		strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7558,databaseConnection->escapeQueryString(m_name));

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_SELECT_7558,databaseConnection->escapeQueryString(m_name));
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::addNewProfile(): Prepare SQL Statement error");

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage <<  "No data found for name = " << m_name.c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage <<  "More than one entry found for name = " << m_name.c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name or description"));
        }

        // Retrieve the pkey as an unsigned long - the name of this operator - from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,keyColumn);
            m_dateCreated = data->getDateData(0,createdColumn);
		}
		//exception
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

		// TD #12987
		// the data has been written to the database, so we can set this flag to false now.
		// otherwise, the ASSERT when we call updateLocationAssociations() will fail
		m_isNew = false;

        // Now update the displays
        updateLocationDisplays();

        // Update the access control and radio login groups
        updateProfileGroups();

        // Update the associated locations
        updateLocationAssociations();

        FUNCTION_EXIT;
    }



    void ProfileHelper::updateLocationDisplays()
    {
        FUNCTION_ENTRY("updateLocationDisplays");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_PARAMETERS_STD_SELECT_22002, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_PARAMETERS_SELECT_22002, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_PARAMETERS_SELECT_22002, m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::updateLocationDisplays(): Prepare SQL Statement error");

        // First we build up a list of all locations already in the database. This way we know whether
        // we need to add or update.

        /*std::ostringstream query;
        query << "select LOCATIONKEY from SE_PROFILE_PARAMETERS where SEPROF_ID = " << m_key;*/

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string locationKeyColumn = "KEY";
        columnNames.push_back(locationKeyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        std::map<unsigned long, int> locationKeysAlreadyInDatabase;
       
		do 
        {

            for (unsigned long i = 0; i < data->getNumRows(); ++i)
            {
                unsigned long locationKey = data->getUnsignedLongData(i, locationKeyColumn);

                std::map<unsigned long, int>::iterator iter = locationKeysAlreadyInDatabase.find(locationKey);

                if(iter != locationKeysAlreadyInDatabase.end())
                {
                    // exists
                }

                else
                {
                    locationKeysAlreadyInDatabase.insert(std::make_pair(locationKey, 1));
                }
            }

            delete data;
            data = NULL;
        }

		while (databaseConnection->moreData( data ));

        // Ok now we know what is already in the database we can go about updating it.

        for (DefaultProfileParamMap::iterator iter = m_defaultProfileParams.begin(); iter != m_defaultProfileParams.end(); ++iter)
        {
            std::vector<unsigned long>::iterator resultIter = std::find(m_associatedLocationKeys.begin(),
                m_associatedLocationKeys.end(),iter->first);

			// TD #12987
			// need this condition here, so that only the *associated* locations have their parameters written
			if ( resultIter != m_associatedLocationKeys.end() )
			{

				if (!m_localProfile && iter->first != 0)
				{
					// If this is a central profile then we are only interested in the 0 location.
					// If it is any other location we can ignore it
					continue;
				}

				else if (m_localProfile && iter->first == 0)
				{
					//TD17817 marc_bugfix
					//If this is a local profile, ignore the 0 location (all locations)
					continue;
				}

				// Determine if location is already in database
				std::map<unsigned long, int>::iterator matchingLocation = locationKeysAlreadyInDatabase.find(iter->first);
    
				if (matchingLocation != locationKeysAlreadyInDatabase.end())
				{
					// Update
					/*std::ostringstream sql;
					sql << "update SE_PROFILE_PARAMETERS set ";
					sql << " display_1 = " << iter->second.display1;
					sql << ", display_2 = " << iter->second.display2;
					sql << ", display_3 = " << iter->second.display3;
					sql << ", left_banner = '" << databaseConnection->escapeInsertString(iter->second.leftBanner) << "' ";
					sql << ", centre_banner = '" << databaseConnection->escapeInsertString(iter->second.centreBanner) << "' ";
					sql << ", right_banner = '" << databaseConnection->escapeInsertString(iter->second.rightBanner) << "' ";
					sql << " where SEPROF_ID = " << m_key << " and LOCATIONKEY = " << iter->first;*/
//					strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_PARAMETERS_STD_UPDATE_22151, iter->second.display1,
//					strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_PARAMETERS_UPDATE_22151, iter->second.display1,

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_PARAMETERS_UPDATE_22151, iter->second.display1,
						iter->second.display2, iter->second.display3, databaseConnection->escapeInsertString(iter->second.leftBanner),
						databaseConnection->escapeInsertString(iter->second.centreBanner), databaseConnection->escapeInsertString(iter->second.rightBanner),
						m_key, iter->first);
//					TA_ASSERT(strSql.size() > 0, "ProfileHelper::updateLocationDisplays(): Prepare SQL Statement error");

					databaseConnection->executeModification(strSql);
				}

				else
				{
					// Nothing exists for this location key. Add all the new parameters.
					/*std::ostringstream sql;
					sql << "insert into SE_PROFILE_PARAMETERS(SEPROF_ID, LOCATIONKEY, DISPLAY_1, DISPLAY_2, DISPLAY_3, left_banner, centre_banner, right_banner) ";
					sql << "values ( " << m_key << ", " << iter->first ;
					sql << "," << iter->second.display1;
					sql << "," << iter->second.display2;
					sql << "," << iter->second.display3;
					sql << ", '" << databaseConnection->escapeInsertString(iter->second.leftBanner) << "' ";
					sql << ", '" << databaseConnection->escapeInsertString(iter->second.centreBanner) << "' ";
					sql << ", '" << databaseConnection->escapeInsertString(iter->second.rightBanner) << "' ";
					sql << ")";*/
//					strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_PARAMETERS_STD_INSERT_22301, m_key, iter->first,
//					strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_PARAMETERS_INSERT_22301, m_key, iter->first,

		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_PARAMETERS_INSERT_22301, m_key, iter->first,
						iter->second.display1, iter->second.display2,iter->second.display3,databaseConnection->escapeInsertString(iter->second.leftBanner),
						databaseConnection->escapeInsertString(iter->second.centreBanner),databaseConnection->escapeInsertString(iter->second.rightBanner));
//					TA_ASSERT(strSql.size() > 0, "ProfileHelper::updateLocationDisplays(): Prepare SQL Statement error");

					databaseConnection->executeModification(strSql);
				}
			}
        }

        FUNCTION_EXIT;
    }


	void ProfileHelper::resetLocationDisplays()
	{
		m_defaultProfileParams.clear();

		// TD #12987
		// initialise the display keys to 0, otherwise they will try and get
		// written as 0xCCCCCCCC and crash the config editor
		if(!m_localProfile)
		{
			ProfileParameters tempParameters;
			tempParameters.display1 = 0;
			tempParameters.display2 = 0;
			tempParameters.display3 = 0;
			m_defaultProfileParams.insert(DefaultProfileParamMap::value_type( 0, tempParameters ));
		}

		else
		{
			// Load all locations and add them to the map of default displays
			std::vector<ILocation*> locations = LocationAccessFactory::getInstance().getAllLocations();
			
			for (std::vector<ILocation*>::iterator iter = locations.begin(); iter != locations.end(); ++iter)
			{
				ProfileParameters tempParameters;
				tempParameters.display1 = 0;
				tempParameters.display2 = 0;
				tempParameters.display3 = 0;
				m_defaultProfileParams.insert(DefaultProfileParamMap::value_type( (*iter)->getKey(), tempParameters ));

				// Done with this location now
				delete *iter;
				*iter = NULL;
			}
		}
	}


    void ProfileHelper::updateProfileGroups()
    {
        FUNCTION_ENTRY("updateProfileGroups");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_STD_UPDATE_7653, m_accessControlGroup,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_UPDATE_7653, m_accessControlGroup,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_UPDATE_7653, m_accessControlGroup,
			databaseConnection->escapeInsertString(m_itsiGroup), m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::updateProfileGroups(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << "update SE_PROFILE set SEPGRO_ID = " << m_accessControlGroup;
        sql << ", RAIGRO_ID = (select RAIGRO_ID from RA_ITSI_GROUP where NAME = '";
        sql << databaseConnection->escapeInsertString(m_itsiGroup) << "') where SEPROF_ID = " << m_key;*/

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void ProfileHelper::setAccessControlGroup(unsigned long accessControlGroupKey)
    {
        m_accessControlGroup = accessControlGroupKey;
    }

    void ProfileHelper::setItsiGroup(const std::string& itsiGroupName)
    {
        m_itsiGroup = itsiGroupName;
    }
    void ProfileHelper::setPaPriorityGroup( const std::string& paGroupName )
    {
        //m_paPriorityGroup = paGroupName;
    }

    void ProfileHelper::addLocationAssocation(unsigned long locationId)
    {
        FUNCTION_ENTRY("addLocationAssocation");

        if (!m_isValidAssociatedLocationData && !m_isNew)
        {
            reloadAssociatedLocationData();
        }

        // Make sure the new association is not already there...
        std::vector<unsigned long>::iterator searchResult = std::find(m_associatedLocationKeys.begin(),
            m_associatedLocationKeys.end(),locationId);

        // If not, then add it, otherwise, it's already correct!
        if (searchResult == m_associatedLocationKeys.end())
        {
            m_associatedLocationKeys.push_back(locationId);
        }

        FUNCTION_EXIT;
    }

    void ProfileHelper::removeLocationAssociaiton(unsigned long locationId)
    {
        FUNCTION_ENTRY("removeLocationAssociaiton");

        if (!m_isValidAssociatedLocationData && !m_isNew)
        {
            reloadAssociatedLocationData();
        }

        // Make sure the association is actually there...
        std::vector<unsigned long>::iterator searchResult = std::find(m_associatedLocationKeys.begin(),
            m_associatedLocationKeys.end(),locationId);

        // If so, then remove it, otherwise, it's already correct!
        if (searchResult != m_associatedLocationKeys.end())
        {
            m_associatedLocationKeys.erase(searchResult);
        }

        FUNCTION_EXIT;
    }

    std::string ProfileHelper::retrieveDeletedField(const std::string& currentField, const std::string& columnName, unsigned long maxFieldSize)
    {
        FUNCTION_ENTRY("retrieveDeletedField");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        std::string textToSearchFor = currentField + DELETED_FLAG;

        if(textToSearchFor.size() > maxFieldSize)
        {
            // Remove the character before the delete flag
            textToSearchFor.erase(textToSearchFor.size() - 3, 2);
        }

        /*std::ostringstream sql;
        sql << "select " << databaseConnection->escapeInsertString(columnName) << " from SE_PROFILE where " 
            << databaseConnection->escapeInsertString(columnName) << " like '" << databaseConnection->escapeInsertString(textToSearchFor) 
            << "%'";
        sql << " and DELETED <> 0";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_STD_SELECT_7502,databaseConnection->escapeInsertString(columnName),
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7502,databaseConnection->escapeInsertString(columnName),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_SELECT_7502,databaseConnection->escapeInsertString(columnName),
			databaseConnection->escapeInsertString(columnName), databaseConnection->escapeInsertString(textToSearchFor));
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::retrieveDeletedField(): Prepare SQL Statement error");

        std::vector<std::string> columnNames;
        columnNames.push_back(columnName);

        // We only want to select about 10 rows. Any more child entities than that and we don't 
        // want to list them.
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        TA_ASSERT( data != NULL, "The data returned was NULL");

        // If no rows are returned then we haven't got any deleted items by teh same name. This means
        // we can just append the first number.
        if (data->getNumRows() == 0)
        {
            std::string newField(currentField);

            // If the field has already reached its max length, remove the last character so it can still
            // be inserted in to the database.
            if(newField.size() + 1 > maxFieldSize)
            {
                newField.erase(newField.size() - 2, 2);
            }

            newField += DELETED_FLAG;
            newField += "1";

            delete data;
            data= NULL;

            FUNCTION_EXIT;
            return newField;
        }

        // If we get to here then we have other entities by the same name that have already been
        // deleted. We must determine the next number to append to the end. We must step through
        // each returned item and find the highest number.
        unsigned long highestAppendedNumber = 0;

        do
        {

            for (unsigned int i = 0; i < data->getNumRows(); ++i)
            {
                std::string field = data->getStringData(i,columnName);
                std::istringstream appendedNumber;

                // Retrieve all the text on the string after the text matching the nameToSearchFor
                appendedNumber.str( field.substr( textToSearchFor.size(), field.length() - textToSearchFor.length() ) );
                
                // Convert the string into an unsigned long
                unsigned long number;
                appendedNumber >> number;

                if (number > highestAppendedNumber)
                {
                    highestAppendedNumber = number;
                }
            }

            delete data;
            data = NULL;
        }
		while ( databaseConnection->moreData(data) );

        // If the field has already reached its max length, remove the last few characters so it can still
        // be inserted in to the database.
        std::ostringstream numberString;
        numberString << highestAppendedNumber+1;

        if(textToSearchFor.size() + numberString.str().size() > maxFieldSize)
        {
            unsigned long trimLength = maxFieldSize - (textToSearchFor.size() + numberString.str().size());
			std::string::size_type deletedFlagPos = textToSearchFor.find(DELETED_FLAG);
           
			if(deletedFlagPos != std::string::npos)
            {
                textToSearchFor.erase(deletedFlagPos - trimLength, trimLength);
            }
        }

        std::ostringstream newField;
        newField << textToSearchFor << highestAppendedNumber+1;

        return newField.str();
        FUNCTION_EXIT;
    }

	ProfileHelper::ActionGroupEntry::ActionGroupEntry() :
		m_actionGroupKey(0),
		m_isControl(false)
	{
	}

	ProfileHelper::ActionGroupEntry::ActionGroupEntry( unsigned long actionGroupKey, bool isControl ) :
		m_actionGroupKey(actionGroupKey),
		m_isControl(isControl)
	{
	}
	
	ProfileHelper::ActionGroupEntry::ActionGroupEntry( const ProfileHelper::ActionGroupEntry& rhs ) :
		m_actionGroupKey(rhs.m_actionGroupKey),
		m_isControl(rhs.m_isControl)
	{
	}

	ProfileHelper::ActionGroupEntry& ProfileHelper::ActionGroupEntry::operator=(const ProfileHelper::ActionGroupEntry& rhs )
	{
		m_actionGroupKey = rhs.m_actionGroupKey;
		m_isControl   = rhs.m_isControl;
		return( *this );
	}

    void ProfileHelper::initialiseMaxFieldLengths()
    {
        FUNCTION_ENTRY("initialiseMaxFieldLengths");

        TA_Base_Core::ThreadGuard guard(m_nameMaxLengthLock);
        if (m_nameMaxLength > 0)
        {
            // we've already gotten the max length from last time, so don't need to fetch it again
            return;
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_Oracle_SELECT_7559);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7559);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_SELECT_7559);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::initialiseMaxFieldLengths(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << "select COLUMN_NAME, DATA_LENGTH from USER_TAB_COLS where TABLE_NAME = 'SE_PROFILE' ";
        sql << "and COLUMN_NAME = 'NAME'";*/

        std::string columnName = "COLUMN_NAME";
        std::string columnLength = "COLUMN_LENGTH";
        std::vector<std::string> columnNames;
        columnNames.push_back(columnName);
        columnNames.push_back(columnLength);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (NULL == data || 0 == data->getNumRows())
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve column lengths. Setting to default values");

            m_nameMaxLength = DEFAULT_NAME_MAX_LENGTH;

            FUNCTION_EXIT;
            return;
        }
        else if (1 < data->getNumRows()) 
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                "More than expected entries retrieved while retrieving column lengths. Setting to default values");

            m_nameMaxLength = DEFAULT_NAME_MAX_LENGTH;

            FUNCTION_EXIT;
            return;
        }
        
        try
        {
            m_nameMaxLength = data->getUnsignedLongData(0,columnLength);
		}
		//exception
        catch ( ... )
        {
            delete data;
            data = NULL;
            
            m_nameMaxLength = DEFAULT_NAME_MAX_LENGTH;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }


    std::vector<unsigned long> ProfileHelper::getLocationKeys()
    {
        // Get the currently associated locations...// get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_LOCATION_STD_SELECT_8001, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_LOCATION_SELECT_8001, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_LOCATION_SELECT_8001, m_key);
//		TA_ASSERT(strSql.size() > 0, "ProfileHelper::getLocationKeys(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << "select LOCATIONKEY from SE_PROFILE_LOCATION where SEPROF_ID = " << m_key;*/

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string profileKeyColumn = "KEY";
        columnNames.push_back(profileKeyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        std::vector<unsigned long> locationKeys;
        for (unsigned int i = 0;i < data->getNumRows();i++)
        {            
            locationKeys.push_back(data->getUnsignedLongData(i,profileKeyColumn));
        }
        delete data;

        return locationKeys;
    }


    void ProfileHelper::updateLocationAssociations()
    {
        FUNCTION_ENTRY("updateLocationAssociations");

        TA_ASSERT(!m_isNew,"This method cannot be called until the entity has an entry in the database");

        std::vector<unsigned long> locationKeys = getLocationKeys();
        // Every item in m_associatedLocationKyes and NOT in the database is to be inserted, while
        // every item in the database and NOT in m_assocaitedLocationKeys is to be deleted
        // Need to figure out what those items are
        std::vector<unsigned long> itemsToDelete;
        std::vector<unsigned long> itemsToInsert(m_associatedLocationKeys);
        for (unsigned int i = 0;i < locationKeys.size();i++)
        {            
            std::vector<unsigned long>::iterator searchResult = std::find(m_associatedLocationKeys.begin(),
                m_associatedLocationKeys.end(),locationKeys[i]);

            if (searchResult != m_associatedLocationKeys.end())
            {
                // The key was in the database and in the vector = no change
                // In this case, it doesn't need to be inserted
                itemsToInsert.erase(std::remove(itemsToInsert.begin(),itemsToInsert.end(),locationKeys[i]),
                    itemsToInsert.end());
            }
            else
            {
                // The key was in the database, but NOT in the vector = delete
                itemsToDelete.push_back(locationKeys[i]);
            }
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);
        // Now that the items to insert and delete have been determined, can carry out the operations
        if (itemsToDelete.size() > 0)
        {
            std::ostringstream sql;
            /*sql << "delete from SE_PROFILE_LOCATION where SEPROF_ID = " << m_key
                << " and LOCATIONKEY in (";*/
            for (unsigned int i = 0;i < itemsToDelete.size();i++)
            {
                sql << itemsToDelete[i];

                // If this is not the last item...
                if (i != itemsToDelete.size() - 1)
                {
                    // add a comma
                    sql << ",";
                }
            }

            // And close the brackets
            /*sql << ")";*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_LOCATION_STD_DELETE_8401, m_key, sql.str());
//			std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_LOCATION_SELECT_8401, m_key, sql.str());
			SQLStatement strSql;
			databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_LOCATION_SELECT_8401, m_key, sql.str());
//			TA_ASSERT(strSql.size() > 0, "ProfileHelper::updateLocationAssociations(): Prepare SQL Statement error");

            databaseConnection->executeModification(strSql);
        }

        // And the inserts...
		size_t uInsertSize = itemsToInsert.size();
        if (uInsertSize > 0)
        {
			//int nDbType = databaseConnection->getDbServerType();
            std::ostringstream sql;	
			std::string strItem;
			SQLTypeAdapter sqlParam;
			std::vector<std::string> vecItems;
            
            for (unsigned int i = 0;i < uInsertSize;i++)
            {
				/*switch ( nDbType )
				{
				case enumOracleDb:
					sql << " into se_profile_location (SEPROF_ID,LOCATIONKEY) values (" << m_key
						<< "," << itemsToInsert[i] << ")";
					break;
				case enumMysqlDb:
					sql << "(" << m_key <<"," <<itemsToInsert[i] << ") ";
					if ( i != itemsToInsert.size() - 1 )
					{
						sql << ",";
					}
					break;
				default:
					LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
						"the type of the database server is not supported currently.");
				}*/

				vecItems.clear();
				sql << m_key;
				strItem = sql.str(); sql.str("");
				vecItems.push_back(strItem);

				sql << itemsToInsert[i];
				strItem = sql.str(); sql.str("");
				vecItems.push_back(strItem);

				if ( i != uInsertSize - 1 )
				{
					sqlParam.add(vecItems, enumSyntax_Format_2, false);
				}
				else
				{
					sqlParam.add(vecItems, enumSyntax_Format_2, true);
				}						
            }			
            
            // Need to add a select statement to the end of this, as the multi-row insert requires a select
            // Even though it's not going to be used
            //sql << " select * from dual"; // the dual is a special temporary table that contains no real data
		//	std::string strSql = defPrepareSQLStatement(nDbType, SE_PROFILE_LOCATION_Oracle_INSERT_8351, sql.str());
//			std::string strSql = databaseConnection->prepareSQLStatement(SE_PROFILE_LOCATION_INSERT_8351, sql.str());
			SQLStatement strSql;
			databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_LOCATION_INSERT_8351, sqlParam);
//			TA_ASSERT(strSql.size() > 0, "ProfileHelper::updateLocationAssociations(): Prepare SQL Statement error");

            databaseConnection->executeModification(strSql);
        }

        FUNCTION_EXIT;
    }

} // closes TA_Base_Core

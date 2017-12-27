/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ControlStation.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ControlStation holds the data specific to a ControlStation entity from the
  * database and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include <sstream>
#include "boost/tokenizer.hpp"
#include "core/data_access_interface/entity_access/src/ControlStation.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const unsigned long ControlStation::NO_SCREENS  = 0;
    const unsigned long ControlStation::ALL_SCREENS = ULONG_MAX;
    const unsigned long ControlStation::FULL_SCREENS = ULONG_MAX - 1;
	const unsigned long ControlStation::INVALID_TIMEOUT = 0;

    const std::string ControlStation::ENTITY_TYPE = "ControlStation";
    const std::string ControlStation::DEBUG_LOG_DIR = "DebugLogDir";
    const std::string ControlStation::STARTUP_DISPLAY = "StartupDisplay";
    const std::string ControlStation::BACKGROUND_DISPLAY = "BackgroundDisplay";
    const std::string ControlStation::GRAPHWORX_SCREEN = "GraphWorxScreen";
    const std::string ControlStation::GRAPHWORX_HELP = "GraphWorxHelp";
    const std::string ControlStation::LAUNCH_BAR_SCREEN = "LaunchBarScreen";
    const std::string ControlStation::LAUNCH_BAR_POSITION = "LaunchBarPosition";
    const std::string ControlStation::USER_PREFERENCES = "UserPreferences";
    const std::string ControlStation::ACCESS_CONTROL_RESTART = "AccessControlRestart";
    const std::string ControlStation::ACCESS_CONTROL_EXIT = "AccessControlExit";
    const std::string ControlStation::EXT_SCHEMATIC = "ExtSchematic";
	const std::string ControlStation::POPUP_TIMEOUT = "AccessControlPopupTimeout";
    const std::string ControlStation::TERMINATE_APPLICATIONS = "TerminateApplications";
    const std::string ControlStation::LOGOUT_APPLICATIONS = "LogoutApplications";
    const std::string ControlStation::OPERATOR_ID = "UserId";
    const std::string ControlStation::PASSWORD = "Password";
    const std::string ControlStation::PROFILE_ID = "ProfileId";
    const std::string ControlStation::LOCATION_ID = "LocationId";
    const std::string ControlStation::SHOW_RESTART = "ShowRestart";
    const std::string ControlStation::SHOW_EXIT = "ShowExit";
    const std::string ControlStation::SHOW_LOGOUT = "ShowLogout";
    const std::string ControlStation::PING_TIMEOUT_SECONDS = "PingTimeoutSeconds";
    const std::string ControlStation::PING_RETRIES = "PingRetries";
    const std::string ControlStation::CORBA_RETRIES = "CorbaRetries";
	const std::string ControlStation::DISABLE_CAD = "DisableCAD";

	//TD16491++
	const std::string ControlStation::MMS_SUBMITTER_TYPE = "MmsSubmitterType";
	const std::string ControlStation::SCREEN_SIZE_PREFIX = "ScreenSize";	// Raymond Pau++ TD16471

    // The operator, profile and location Ids should all start from 0 or 1 so it should 
    // be safe to assume that those values are unset if they are set to ULONG_MAX
    const unsigned long ControlStation::NO_OPERATOR = ULONG_MAX;
    const unsigned long ControlStation::NO_PROFILE = ULONG_MAX;
	const unsigned long ControlStation::NO_LOCATION = ULONG_MAX;


    ControlStation::ControlStation(unsigned long key) 
        : EntityData(key, getStaticType())
    {
    }

	
    ControlStation::~ControlStation()
    { // Nothing to clean up
    }


    std::string ControlStation::getDebugLogDir()
    {
		return getHelper()->getParameter(DEBUG_LOG_DIR);
    }


    std::string ControlStation::getStartupDisplay()
    {
		return getHelper()->getParameter(STARTUP_DISPLAY);
    }


    std::string ControlStation::getBackgroundDisplay()
    {
		return getHelper()->getParameter(BACKGROUND_DISPLAY);
    }


    bool ControlStation::getUserPreferences()
    {
		std::string value = getHelper()->getParameter(USER_PREFERENCES);
		bool useUserSettings = false;
		if (!value.empty())
		{
			useUserSettings = true;
		}

        return useUserSettings;
    }


    unsigned long ControlStation::getGraphWorxScreen()
    {
		std::string value = getHelper()->getParameter(GRAPHWORX_SCREEN);
		unsigned long graphworxScreen = NO_SCREENS;
		if (!value.empty())
		{
			graphworxScreen = convertParameterToScreen(value);
		}

        return graphworxScreen;
    }


    std::string ControlStation::getGraphWorxHelpfile()
    {
		return getHelper()->getParameter(GRAPHWORX_HELP);
    }


    unsigned long ControlStation::getLaunchBarScreen()
    {
		std::string value = getHelper()->getParameter(LAUNCH_BAR_SCREEN);
		unsigned long launchBarScreen = NO_SCREENS;
		if (!value.empty())
		{
			launchBarScreen = convertParameterToScreen(value);
		}

        return launchBarScreen;
    }


    ControlStation::LaunchBarPosition ControlStation::getLaunchBarPosition()
    {
		std::string value = getHelper()->getParameter(LAUNCH_BAR_POSITION);
		LaunchBarPosition launchBarPosition = HIDDEN;
		if (!value.empty())
		{
			launchBarPosition = convertParameterToLaunchBarPosition(value);
		}
		
        return launchBarPosition;
    }


    bool ControlStation::isRestartAccessControlled()
    {
		std::string value = getHelper()->getParameter(ACCESS_CONTROL_RESTART);
		bool isRestartAccessControlled = true;
		if (!value.empty())
		{
			isRestartAccessControlled = EntityHelper::getBooleanData(value);
		}

        return isRestartAccessControlled;
    }


    bool ControlStation::isExitAccessControlled()
    {
		std::string value = getHelper()->getParameter(ACCESS_CONTROL_EXIT);
		bool isExitAccessControlled = true;
		if (!value.empty())
		{
			isExitAccessControlled = EntityHelper::getBooleanData(value);
		}

        return isExitAccessControlled;
    }


    std::string ControlStation::getExtSchematic()
    {
		return getHelper()->getParameter(EXT_SCHEMATIC);
    }


	unsigned long ControlStation::getAccessControlPopupTimeout()
	{
		std::string value = getHelper()->getParameter(POPUP_TIMEOUT);
		unsigned long popupTimeout = INVALID_TIMEOUT;
		if (!value.empty())
		{
			popupTimeout = EntityHelper::getUnsignedLongData(value);
		}

        return popupTimeout;
	}


    std::string ControlStation::getApplicationsToTerminate()
    {
		return getHelper()->getParameter(TERMINATE_APPLICATIONS);
    }


    std::string ControlStation::getApplicationsToTerminateOnLogout()
    {
		return getHelper()->getParameter(LOGOUT_APPLICATIONS);
    }


    IOperator* ControlStation::getLoginOperator()
    {
		unsigned long loginOperatorId = NO_OPERATOR;
		try
		{
			std::string value = getHelper()->getParameter(OPERATOR_ID);
			
			if (!value.empty())
			{
				loginOperatorId = EntityHelper::getUnsignedLongData(value);
			}
		}
		catch(...)
		{

		}

		// If no operator is specified, then just return NULL
		if (loginOperatorId == NO_OPERATOR)
		{
            return NULL;
        }

        return TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(loginOperatorId, false);
    }

	
    std::string ControlStation::getLoginPassword()
    {
		return getHelper()->getParameter(PASSWORD);
    }


    IProfile* ControlStation::getLoginProfile()
    {
		unsigned long loginProfileId = NO_PROFILE;

		try
		{
			std::string value = getHelper()->getParameter(PROFILE_ID);
		
			if (!value.empty())
			{
				loginProfileId = EntityHelper::getUnsignedLongData(value);
			}
		}
		catch(...)
		{

		}

        // If no profile is specified, then just return NULL
        if (loginProfileId == NO_PROFILE)
        {
            return NULL;
        }

        return TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(loginProfileId);
    }


    ILocation* ControlStation::getLoginLocation()
    {
		unsigned long loginLocationId = NO_LOCATION;

		try
		{
			std::string value = getHelper()->getParameter(LOCATION_ID);
		
			if (!value.empty())
			{
				loginLocationId = EntityHelper::getUnsignedLongData(value);
			}
		}
		catch(...)
		{

		}
		
        // If no location is specified, then just return NULL
        if (loginLocationId == NO_LOCATION)
        {
            return NULL;
        }

        return TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(loginLocationId);
    }


    bool ControlStation::canLoginAutomatically()
    {
        // Can only log in automatically when the operator, profile are specified.  
        // It's ok to have NULL location as central profiles don't need location

		std::string value = getHelper()->getParameter(OPERATOR_ID);
		unsigned long loginOperatorId = NO_OPERATOR;
		if (!value.empty())
		{
			loginOperatorId = EntityHelper::getUnsignedLongData(value);
		}

		if (loginOperatorId == NO_OPERATOR)
		{
			return false;
		}

		value = "";
		value = getHelper()->getParameter(PROFILE_ID);
		unsigned long loginProfileId = NO_PROFILE;
		if (!value.empty())
		{
			loginProfileId = EntityHelper::getUnsignedLongData(value);
		}

		if (loginProfileId == NO_PROFILE)
		{
			return false;
		}

        return true;
    }

    
    bool ControlStation::shouldShowExitButton()
    {
		std::string value = getHelper()->getParameter(SHOW_EXIT);
		bool isExitButtonShown = true;
		if (!value.empty())
		{
			isExitButtonShown = EntityHelper::getBooleanData(value);
		}
		
        return isExitButtonShown;
    }


    bool ControlStation::shouldShowRestartButton()
    {
		std::string value = getHelper()->getParameter(SHOW_RESTART);
		bool isRestartButtonShown = true;
		if (!value.empty())
		{
			isRestartButtonShown = EntityHelper::getBooleanData(value);
		}

        return isRestartButtonShown;
    }


    bool ControlStation::shouldShowLogoutButton()
    {
		std::string value = getHelper()->getParameter(SHOW_LOGOUT);
		bool isLogoutButtonShown = true;
		if (!value.empty())
		{
			isLogoutButtonShown = EntityHelper::getBooleanData(value);
		}

        return isLogoutButtonShown;
    }


    unsigned int ControlStation::getPingTimeoutSeconds()
    {
		std::string value = getHelper()->getParameter(PING_TIMEOUT_SECONDS);
		unsigned int pingTimeoutSeconds = 10;
		if (!value.empty())
		{
			pingTimeoutSeconds = EntityHelper::getUnsignedLongData(value);
		}

        return pingTimeoutSeconds;
    }


    unsigned int ControlStation::getPingRetries()
    {
		std::string value = getHelper()->getParameter(PING_RETRIES);
		unsigned int pingRetries = 2;
		if (!value.empty())
		{
			pingRetries = static_cast < unsigned int > (EntityHelper::getUnsignedLongData(value));
		}

        return pingRetries;
    }


    unsigned int ControlStation::getCorbaRetries()
    {
		std::string value = getHelper()->getParameter(CORBA_RETRIES);
		unsigned int corbaRetries = 2;
		if (!value.empty())
		{
			corbaRetries = static_cast < unsigned int > (EntityHelper::getUnsignedLongData(value));
		}

        return corbaRetries;
    }


	//TD16491++
	std::string ControlStation::getMmsSubmitterType()
	{
		return getHelper()->getParameter(MMS_SUBMITTER_TYPE);
	}
	//++TD16491
	
	// Raymond Pau++ TD16471
	bool ControlStation::isScreenSizeSpecified(unsigned int screen)
	{
		std::ostringstream screenSizeParameter;
		screenSizeParameter << SCREEN_SIZE_PREFIX;
		screenSizeParameter << screen;

		std::string value = getHelper()->getParameter(screenSizeParameter.str());

		return !value.empty();
	}


	TA_RECT ControlStation::getScreenSize(unsigned int screen)
	{
		std::ostringstream screenSizeParameter;
		screenSizeParameter << SCREEN_SIZE_PREFIX;
		screenSizeParameter << screen;

		std::string value = getHelper()->getParameter(screenSizeParameter.str());

		return convertParameterToScreenSize(value);
	}
	// ++Raymond Pau TD16471


    IEntityData* ControlStation::clone(unsigned long key)
    {
        return new ControlStation(key);        
    }


    void ControlStation::invalidate()
    {
        getHelper()->invalidate();
    }


    void ControlStation::reload()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ControlStation::reload");
        // Get the parameters for this entity
		EntityHelper* helper = getHelper();
		std::string value("");
        LOG( SourceInfo, DebugUtil::FunctionExit, "ControlStation::reload");
    }


	/*
	// Obsolete codes
    unsigned long ControlStation::convertParameterToScreen(IData& data, const int index)
    {
        try
        {
            // First see if the data is a number (ie a screen number)
            // If it is a screen number then this will successfully return that number
            // and we're done.
            return data.getUnsignedLongData( index, getHelper()->PARAMETERVALUE );
        }
            catch ( const TA_Base_Core::DataException& )
        {
            // Doh! We don't have a number so we need to see what the user has chosen.
        }

        std::string parameter = data.getStringData(index, getHelper()->PARAMETERVALUE );

        convertStringToUpperCase(parameter);

        if (parameter == "ALL")
        {
            return ALL_SCREENS;
        }
        else if (parameter == "FULL")
        {
            return FULL_SCREENS;
        }

        // Ok if we get to here then the user has either specified "None" or they
        // haven't specified anything. So basically we can just return that NO_SCREENS.

        return NO_SCREENS;
    }
	*/

    unsigned long ControlStation::convertParameterToScreen(std::string parameter)
    {

        convertStringToUpperCase(parameter);

        if (parameter == "ALL")
        {
            return ALL_SCREENS;
        }
		
		if (parameter == "FULL")
        {
            return FULL_SCREENS;
        }

		unsigned long screenNum;
		std::stringstream ss;
		ss.str(parameter);
		ss >> screenNum;

		if( screenNum > 0 )
		{
			return screenNum;
		}

        // Ok if we get to here then the user has either specified "None" or they
        // haven't specified anything. So basically we can just return that NO_SCREENS.

        return NO_SCREENS;
    }

	/*
	// Obsolete codes
    ControlStation::LaunchBarPosition ControlStation::convertParameterToLaunchBarPosition(IData& data,
                                                                                    const int index)
    {
        std::string parameter = data.getStringData(index, getHelper()->PARAMETERVALUE);

        convertStringToUpperCase(parameter);

        if (parameter == "LEFT")
        {
            return LEFT;
        }
        else if (parameter == "RIGHT")
        {
            return RIGHT;
        }
        
        return HIDDEN;
    }
	*/

	
	// Raymond Pau++ TD16471
	TA_RECT ControlStation::convertParameterToScreenSize( const std::string& parameterValue )
	{
		typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

		TA_RECT rect = { 0, 0, 0, 0 };
        boost::char_separator<char> separator(",");
        tokenizer tokens(parameterValue, separator);
        
		std::vector<std::string> parts;    
        for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        {
            parts.push_back(*tok_iter);
        }
        
		if (parts.size() != 4)
        {
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Ignoring invalid Screen Size parameter [%s]",
				parameterValue.c_str() );
			return rect;
        }
        
		std::stringstream ss;
		ss.str( parts[0] );
		ss >> rect.left;

		ss.clear();
		ss.str( parts[1] );
		ss >> rect.top;
		
		ss.clear();
		ss.str( parts[2] );
		ss >> rect.right;
		rect.right += rect.left;

		ss.clear();
		ss.str( parts[3] );
		ss >> rect.bottom;
		rect.bottom += rect.top;

		return rect;
	}
	// ++Raymond Pau TD16471


    ControlStation::LaunchBarPosition ControlStation::convertParameterToLaunchBarPosition(std::string parameter)
    {

        convertStringToUpperCase(parameter);

        if (parameter == "LEFT")
        {
            return LEFT;
        }
        else if (parameter == "RIGHT")
        {
            return RIGHT;
        }
        
        return HIDDEN;
    }

    void ControlStation::convertStringToUpperCase(std::string& strToConvert)
    {
        std::string charToInsert("?");
        for(unsigned int i = 0; i < strToConvert.length(); ++i)
        {
	        charToInsert[0] = toupper( strToConvert[i] );
	        strToConvert.replace(i,1,charToInsert);
        }
    }


    std::string ControlStation::getType()
    {
        return getStaticType();
    }


    std::string ControlStation::getStaticType()
    {
        return ENTITY_TYPE;
    }


	bool ControlStation::isDisableCAD()
	{
		std::string value = getHelper()->getParameter(DISABLE_CAD);
		bool isDisableCAD = true;
		if (!value.empty())
		{
			isDisableCAD = EntityHelper::getBooleanData(value);
		}

        return isDisableCAD;
	}

} //close namespace TA_Base_Core


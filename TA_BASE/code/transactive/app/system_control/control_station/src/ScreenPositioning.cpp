/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ScreenPositioning.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/25 15:01:15 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * Calculates the screen positions of launch pad (if exist), banner and applications
  * 
  */
#pragma warning(disable:4786)
#include "app/system_control/control_station/src/StdAfx.h"
#include "app/system_control/control_station/src/ScreenPositioning.h"
#include "app/system_control/control_station/src/ControlStationRootDialog.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include <math.h>

#include "core/data_access_interface/entity_access/src/banner.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::ControlStation;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::IEntityData;
using TA_Base_Core::Banner;

const RECT ScreenPositioning::NULL_RECT= {0, 0, 0, 0};              // This is used to indicate a rectangle whose dimension is zero
const unsigned long ScreenPositioning::LAUNCH_BAR_WIDTH = 171;      // This is the width of the launch bar 

ScreenPositioning::ScreenPositioning()
: m_isLaunchBarShown(false)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "Constructor" );

    RECT area;
    SystemParametersInfo(SPI_GETWORKAREA,0,&area,0);
    m_screenBottom = area.bottom;
    m_screenTop = area.top;

    m_numberOfMonitors = GetSystemMetrics(SM_CMONITORS);
    // Note: now, we trust the system api and video card.
    //if (m_numberOfMonitors == 1)
    //{
    //    // It may be a video card that does not report the number of monitors correctly.
    //    // In that case we must make sure.
    //    m_numberOfMonitors = calculateNumberOfMonitors();
    //}

    //if (m_numberOfMonitors != 1)
    //{
    //    // We need to check if the width we have been given is across the whole
    //    // screen or just a single monitor.
    //    m_eachScreenWidth = calculateScreenWidth();
    //}
    //else
    {
        m_eachScreenWidth = GetSystemMetrics(SM_CXSCREEN);    
    }

    int left = 0;

    // initialise the screen layout values
    for (unsigned int i = 1; i <= m_numberOfMonitors; i++)
    {
        RECT tmp = {left, m_screenTop, left=left+m_eachScreenWidth, m_screenBottom};
        m_screenLayout[i].screenRect = tmp;
        m_screenLayout[i].bannerRect = NULL_RECT;
        m_screenLayout[i].launchBarRect = NULL_RECT;
        m_screenLayout[i].applicationRect = NULL_RECT;
        m_screenLayout[i].schematicRect = NULL_RECT;
        m_screenLayout[i].bannerPosition = ControlStation::NEITHER;
        m_screenLayout[i].launchBarPosition = ControlStation::HIDDEN;
        m_screenLayout[i].hasGraphWorx = false;
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "Constructor" );
}


ScreenPositioning::~ScreenPositioning()
{
}


unsigned long ScreenPositioning::calculateNumberOfMonitors()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateNumberOfMonitors" );

    using TA_Base_App::TA_ControlStation::APPLICATION_NAME;

    unsigned long totalWidth = GetSystemMetrics(SM_CXSCREEN);
    unsigned long totalHeight = GetSystemMetrics(SM_CYSCREEN);

    //see if the ratio makes sence
	double standardRatio = 4.0/3.0;
	double otherRatio = 5.0/4.0;

    double doubleScreenWidth = totalHeight * standardRatio;
    // This will round the screen width to an unsigned long
	unsigned long screenWidth = static_cast<unsigned long>( floor(doubleScreenWidth + 0.5) ); 
    int remainder = totalWidth % screenWidth;

	//If a 4/3 ratio does not seem to fit...
	if(remainder != 0)
	{
		//try the only other ratio I think computers support
		doubleScreenWidth = totalHeight * otherRatio;
        // This will round the screen width to an unsigned long
        screenWidth = static_cast<unsigned long>( floor(doubleScreenWidth + 0.5) ); 
		remainder = totalWidth % screenWidth;

		//If a 5/4 ratio does not seem to fit...
		if(remainder != 0)
		{
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_020017);

            doubleScreenWidth = totalHeight * standardRatio;
            // This will round the screen width to an unsigned long
            screenWidth = static_cast<unsigned long>( floor(doubleScreenWidth + 0.5) ); 
		}
	}

    unsigned int numMonitors = totalWidth / screenWidth;

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Number of monitors = %lu", numMonitors);
    LOG( SourceInfo, DebugUtil::FunctionExit, "calculateNumberOfMonitors" );
    return numMonitors;
}


unsigned long ScreenPositioning::calculateScreenWidth()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateScreenWidth" );

    unsigned long totalWidth = GetSystemMetrics(SM_CXSCREEN);
    unsigned long totalHeight = GetSystemMetrics(SM_CYSCREEN);

    //see if the ratio makes sence
	double standardRatio = 4.0/3.0;
	double otherRatio = 5.0/4.0;

    double actualRatio = static_cast<double>(totalWidth) / static_cast<double>(totalHeight);

    if( (actualRatio < (standardRatio+0.001)) && (actualRatio > (standardRatio - 0.001)) )
    {
        // The ratio is correct which means we have a small resolution and multiple monitors
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Each screen width = %lu", totalWidth);
        LOG( SourceInfo, DebugUtil::FunctionExit, "calculateScreenWidth" );
        return totalWidth;
    }
    else if( (actualRatio < (otherRatio+0.001)) && (actualRatio > (otherRatio - 0.001)) )
    {
        // The ratio is correct which means we have a small resolution and multiple monitors
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Each screen width = %lu", totalWidth);
        LOG( SourceInfo, DebugUtil::FunctionExit, "calculateScreenWidth" );
        return totalWidth;
    }

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Each screen width = %lu", totalWidth / m_numberOfMonitors);
    LOG( SourceInfo, DebugUtil::FunctionExit, "calculateScreenWidth" );
    return totalWidth / m_numberOfMonitors;
}


void ScreenPositioning::loadScreenLayout(TA_Base_Core::ControlStation& controlStationDatabaseEntry,std::vector<IEntityData*>& banners)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loadScreenLayout" );

    m_displaySpanAllScreens = (controlStationDatabaseEntry.getGraphWorxScreen() == ControlStation::FULL_SCREENS);

	// Raymond Pau++ TD16471
	// load customize size/pos of each screen
	for (unsigned int screen = 1; screen <= m_numberOfMonitors; ++screen)
	{
		if (true == controlStationDatabaseEntry.isScreenSizeSpecified(screen))
		{
			TA_Base_Core::TA_RECT rect = controlStationDatabaseEntry.getScreenSize(screen);
			m_screenLayout[screen].screenRect.left		= rect.left;
			m_screenLayout[screen].screenRect.top		= rect.top;
			m_screenLayout[screen].screenRect.right		= rect.right;
			m_screenLayout[screen].screenRect.bottom	= rect.bottom;
		}
	}
	// ++Raymond Pau TD16471

    loadBannerScreenLayout(controlStationDatabaseEntry,banners);

    IF_LOCAL_TEST
    {
        m_isLaunchBarShown = true;
        loadLaunchBarScreenLayout(controlStationDatabaseEntry);
    }

    if (controlStationDatabaseEntry.getLaunchBarScreen() != ControlStation::NO_SCREENS)
    {
        m_isLaunchBarShown = true;
        loadLaunchBarScreenLayout(controlStationDatabaseEntry);
    }

    loadGraphworxScreenLayout(controlStationDatabaseEntry);

    // set the application dimension
    loadApplicationScreenLayout();

    LOG( SourceInfo, DebugUtil::FunctionExit, "loadScreenLayout" );
}


void ScreenPositioning::loadBannerScreenLayout(TA_Base_Core::ControlStation& controlStationDatabaseEntry,std::vector<IEntityData*>& banners)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loadBannerScreenLayout" );

    try
    {
        // Step through each banner and set up the m_screenLayout map with the banner positioning
        for (unsigned int i = 0; i < banners.size(); i++)
        {
            Banner* thisBanner = dynamic_cast<Banner*>(banners[i]);

            // First figure out which screen the banner is on.
            // The leftposition can be negative so if it is change this to 0 so we can compare unsigned longs
            int leftPositionOfBanner = thisBanner->getLeftPosition();
            if (thisBanner->getLeftPosition() < 1)
            {
                leftPositionOfBanner = 1;
            }

            int screen = getWhichScreenCoordinateIsOn(leftPositionOfBanner);
            if (screen < 1)
            {
                // The leftposition of the banner was not within our screen range. Therefore it will not be shown
                LOG_GENERIC ( SourceInfo, DebugUtil::DebugWarn, "The position of banner '%s' does not fall on any of the monitors", thisBanner->getName().c_str() );
				m_isBannerVisible.push_back(false);
                continue;
            }

			m_isBannerVisible.push_back(true);

            RECT& bannerRect = m_screenLayout[screen].bannerRect;
            bannerRect.left = leftPositionOfBanner;
            bannerRect.right = m_screenLayout[screen].screenRect.right;

            unsigned long topPositionOfBanner = thisBanner->getTopPosition();
            if (topPositionOfBanner < m_screenTop)
            {
                topPositionOfBanner = m_screenTop;
            }

            bannerRect.top = topPositionOfBanner;
            if (topPositionOfBanner < 10)
            {
                // If the position is less than 10 then we assume we are at the top of the screen
                m_screenLayout[screen].bannerPosition = TA_Base_Core::ControlStation::TOP;
                bannerRect.bottom = topPositionOfBanner + thisBanner->getHeight();
            }
            else if (topPositionOfBanner > (m_screenBottom - thisBanner->getHeight() - 10) )
            {
                // This means we are at the bottom of the screen.
                m_screenLayout[screen].bannerPosition = TA_Base_Core::ControlStation::BOTTOM;
                bannerRect.bottom = m_screenBottom;
            }
            else
            {
                // Banner is not at the top or bottom so we just pretend it does not exist
                m_screenLayout[screen].bannerPosition = TA_Base_Core::ControlStation::NEITHER;
                bannerRect.bottom = bannerRect.top = m_screenTop;
                bannerRect.right = bannerRect.left;
            }
        }
    }
    catch ( const TA_Base_Core::DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Exception thrown while trying to retrieve the banner details. Will not be displayed");
    }
    catch ( const TA_Base_Core::DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Exception thrown while trying to retrieve the banner details. Will not be displayed");
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "loadBannerScreenLayout" );
}


void ScreenPositioning::loadGraphworxScreenLayout(TA_Base_Core::ControlStation& controlStationDatabaseEntry)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loadGraphworxScreenLayout" );

    unsigned long graphWorxScreen = ControlStation::NO_SCREENS;
    try
    {
        graphWorxScreen = controlStationDatabaseEntry.getGraphWorxScreen();
    }
    catch ( const TA_Base_Core::DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Exception thrown while trying to retrieve the screens GraphWorx should display on. Will default to NONE");
    }
    catch ( const TA_Base_Core::DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Exception thrown while trying to retrieve the screens GraphWorx should display on. Will default to NONE");
    }

    if (graphWorxScreen == ControlStation::NO_SCREENS)
    {
        // User doesn't want any GraphWorX instances so leave the default settings
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "GraphWorX not to be displayed on any screens");
        LOG( SourceInfo, DebugUtil::FunctionExit, "loadGraphworxScreenLayout" );
        return;
    }

    if (graphWorxScreen == ControlStation::ALL_SCREENS)
    {
        ThreadGuard guard(m_layoutLock);
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "GraphWorX to be shown on all screens");
        for (unsigned int monitor = 1; monitor <= m_numberOfMonitors; ++monitor)
        {
            m_screenLayout[monitor].hasGraphWorx = true;
        }
        return;
    }

    // launching it on a particular screen
    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "GraphWorX to be shown on screen %lu",graphWorxScreen);
    ThreadGuard guard(m_layoutLock);
    ScreenLayout::iterator iter = m_screenLayout.find(graphWorxScreen);
    if (iter != m_screenLayout.end() )
    {
         iter->second.hasGraphWorx = true;
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "loadGraphworxScreenLayout" );
}


void ScreenPositioning::loadLaunchBarScreenLayout(TA_Base_Core::ControlStation& controlStationDatabaseEntry)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loadLaunchBarScreenLayout" );

    unsigned long launchBarScreen = ControlStation::NO_SCREENS;
    TA_Base_Core::ControlStation::LaunchBarPosition launchBarPosition = ControlStation::HIDDEN;
    try
    {
        launchBarScreen = controlStationDatabaseEntry.getLaunchBarScreen();
        launchBarPosition = controlStationDatabaseEntry.getLaunchBarPosition();
    }
    catch ( const TA_Base_Core::DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Exception thrown while trying to retrieve the screen the launch bar should display on. Will default to NONE");
    }
    catch ( const TA_Base_Core::DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Exception thrown while trying to retrieve the screen the launch bar should display on. Will default to NONE");
    }

    IF_LOCAL_TEST
    {
        launchBarScreen = 1;
        launchBarPosition = TA_Base_Core::ControlStation::RIGHT;
    }

    {
        ThreadGuard guard(m_layoutLock);

        if (launchBarScreen == ControlStation::NO_SCREENS)
        {
            // User doesn't want the launch bar so leave the default settings
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Launch bar not to be shown on any screens");
        }
        else if (launchBarScreen == ControlStation::ALL_SCREENS)
        {
            // Trying to launch on all screens, but we could only launch on one screen
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Cannot display launch bar on all screens, can only launch it on 1 screen or hide it");
        }
        else
        {
            // on a specific screen
            ScreenLayout::iterator iter = m_screenLayout.find(launchBarScreen);
            if (iter != m_screenLayout.end() )
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Launch bar to be shown on screen %lu",launchBarScreen);
                iter->second.launchBarPosition = launchBarPosition;
            }
        }

        // we have to go through all launch bar rects to initialise the values.
        for (unsigned int screen = 1; screen <= m_numberOfMonitors; screen++)
        {
            RECT& launchBar = m_screenLayout[screen].launchBarRect;
            RECT banner = m_screenLayout[screen].bannerRect;
    
            // Is there a banner on this screen? If so we need to position and resize
            // around this.
            switch (m_screenLayout[screen].bannerPosition)
            {
            case(ControlStation::TOP):
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Launch bar must allow for the banner at the top of the screen");
                launchBar.top = banner.bottom;
                launchBar.bottom = m_screenBottom;
                break;

            case(ControlStation::BOTTOM):
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Launch bar must allow for the banner at the bottom of the screen");
                launchBar.top = m_screenTop;
                launchBar.bottom = banner.top;
                break;

            case(ControlStation::NEITHER):
            default:
                launchBar.top = m_screenTop;
                launchBar.bottom = m_screenBottom;
                break;
            }

            // Now which side is the launch bar meant to be on? We need to set up the topLeft
            // position based on this.
            switch (m_screenLayout[screen].launchBarPosition)
            {
            case(ControlStation::LEFT):
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Launch bar to be shown on the left of the screen");
                launchBar.left = m_eachScreenWidth * (screen-1);
                launchBar.right = launchBar.left + LAUNCH_BAR_WIDTH;
                break;

            case(ControlStation::RIGHT):
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Launch bar to be shown on the right of the screen");
                launchBar.right = m_eachScreenWidth * screen;
                launchBar.left = launchBar.right - LAUNCH_BAR_WIDTH;
                break;

            case(ControlStation::HIDDEN):   // we still want to get the coordinates right even if the launch bar is hidden
            default:
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Launch bar is hidden");
                launchBar.left = m_screenLayout[screen].screenRect.left;
                launchBar.right = launchBar.left;
                break;

            }
        }
    }
    LOG( SourceInfo, DebugUtil::FunctionExit, "loadLaunchBarScreenLayout" );
}


void ScreenPositioning::loadApplicationScreenLayout()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loadApplicationScreenLayout" );

    ThreadGuard guard(m_layoutLock);

    for (unsigned int screen = 1; screen <= m_numberOfMonitors; screen++)
    {
        RECT& appRect = m_screenLayout[screen].applicationRect;

        // Calculate the x position and possible width. The appRect width is affected by the launch bar if it
        // is on the same screen so we must consider this.  

        // If the launch pad is hidden, then the width of the appRect screen should cover the whole screen
        // horizontally and cover the rest of the space left by the banner.
        // If it has launch bar, then it should have the same height as launch bar and have the remaining horizontal space.

        if (m_isLaunchBarShown)
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Setting top and bottom of application area to be the same as launch bar's");
            appRect.top = m_screenLayout[screen].launchBarRect.top;
            appRect.bottom = m_screenLayout[screen].launchBarRect.bottom;
            switch (m_screenLayout[screen].launchBarPosition)
            {
            case(ControlStation::LEFT):
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "App area must allow for the launch bar on the left");
                appRect.left = m_screenLayout[screen].screenRect.left + LAUNCH_BAR_WIDTH;
                appRect.right = m_screenLayout[screen].screenRect.right;
                break;

            case(ControlStation::RIGHT):
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "App area must allow for the launch bar on the right");
                appRect.left = m_screenLayout[screen].screenRect.left;
                appRect.right = m_screenLayout[screen].screenRect.right - LAUNCH_BAR_WIDTH;
                break;

            case(ControlStation::HIDDEN):
            default: // No launch bar so use default
                appRect.left = m_screenLayout[screen].screenRect.left;
                appRect.right = m_screenLayout[screen].screenRect.right;
                break;
            }
        }
        else // No launch bar
        {
            RECT banner = m_screenLayout[screen].bannerRect;
            switch (m_screenLayout[screen].bannerPosition)
            {
            case(ControlStation::TOP):
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Launch bar must allow for the banner at the top of the screen");
                appRect.top = banner.bottom;
                appRect.bottom = m_screenBottom;
                break;

            case(ControlStation::BOTTOM):
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Launch bar must allow for the banner at the bottom of the screen");
                appRect.top = m_screenTop;
                appRect.bottom = banner.top;
                break;

            case(ControlStation::NEITHER):
            default:
                appRect.top = m_screenTop;
                appRect.bottom = m_screenBottom;
                break;
            }
            appRect.left = m_screenLayout[screen].screenRect.left;
            appRect.right = m_screenLayout[screen].screenRect.right;
        }

        // initialise the schematic display to having the same dimension as the applicatoin/main navigator area
        m_screenLayout[screen].schematicRect = m_screenLayout[screen].applicationRect;  
    }
    LOG( SourceInfo, DebugUtil::FunctionExit, "loadApplicationScreenLayout" );
}


int ScreenPositioning::getScreenNumber(TA_Base_Bus::TA_GenericGui::EScreen targetScreen, long val)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getScreenNumber" );
    int retval = -1;
    switch (targetScreen)
    {
    case TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT:         // Area within the current monitor that currentX lies on 
        retval = getWhichScreenCoordinateIsOn(val);
        break;
    case TA_Base_Bus::TA_GenericGui::SCREEN_PREVIOUS:        // Area within the monitor on the left of the one that currentX lies on.  If there is no screen on the left of this, it will just use the current monitor.
        retval = getWhichScreenCoordinateIsOn(val) - 1;
        break;
    case TA_Base_Bus::TA_GenericGui::SCREEN_NEXT:            // Area within the monitor on the right of the one that currentX lies on.  If there is no screen on the right of this, it will just use the current monitor.
        retval = getWhichScreenCoordinateIsOn(val) + 1;
        break;
    case TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC:        // Area within the specified monitor
        retval = val;
        break;
    case TA_Base_Bus::TA_GenericGui::SCREEN_FIRST:           // Area within the first monitor (ignores val)
        retval = 1;
        break;
    case TA_Base_Bus::TA_GenericGui::SCREEN_LAST:            // Area within the last monitor (ignores val)
        retval = m_numberOfMonitors;
        break;
    case TA_Base_Bus::TA_GenericGui::SCREEN_ALL:             // Area within all screens
    default:
        break;
    }

    if (targetScreen != TA_Base_Bus::TA_GenericGui::SCREEN_ALL)
    {
        // make sure the screen is within range.
        if (retval > static_cast<int>(m_numberOfMonitors))
        {
            retval = m_numberOfMonitors;
        }
        else if (retval < 1)
        {
            retval = 1;
        }
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "getScreenNumber" );
    return retval;
}


int ScreenPositioning::getWhichScreenCoordinateIsOn(long xCoord) const
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getWhichScreenCoordinateIsOn" );

    for(unsigned int monitor = 1; monitor<= m_numberOfMonitors; ++monitor)
    {
        if( xCoord < static_cast<long>(m_eachScreenWidth*monitor) )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,"X coordinate %lu can be found on monitor %lu",xCoord,monitor);
            LOG( SourceInfo, DebugUtil::FunctionExit, "getWhichScreenCoordinateIsOn" );
            return monitor;
        }
    }

    // Not found
    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,"X coordinate %lu can not be found on any monitors.",xCoord);
    LOG( SourceInfo, DebugUtil::FunctionExit, "getWhichScreenCoordinateIsOn" );
    return -1;
}


RECT ScreenPositioning::getRect( const TA_Base_Bus::TA_GenericGui::EScreen targetScreen, 
                                 const TA_Base_Bus::TA_GenericGui::EArea targetArea, 
                                 long val /*= -1*/)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getRect" );
    int screenNum = getScreenNumber(targetScreen, val);
    RECT rect;
    if (screenNum > 0) // if it is getting a rect of a particular screen, then just return the rect as it is
    {
        ScreenArrangement& layout = m_screenLayout[screenNum];
        switch(targetArea)
        {
        case TA_Base_Bus::TA_GenericGui::AREA_SCREEN:
            rect =  layout.screenRect;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Getting the screen area of monitor %d", screenNum);
            break;
        case TA_Base_Bus::TA_GenericGui::AREA_BANNER:
            rect = layout.bannerRect;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Getting the banner area of monitor %d", screenNum);
            break;
        case TA_Base_Bus::TA_GenericGui::AREA_MAINNAVIGATOR:
            rect = layout.applicationRect;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Getting the main navigator/application area of monitor %d", screenNum);
            break;
        case TA_Base_Bus::TA_GenericGui::AREA_LAUNCHPAD:
            rect = layout.launchBarRect;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Getting the launchbar area of monitor %d", screenNum);
            break;
        case TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC:
            rect = layout.schematicRect;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Getting the schematic display area of monitor %d", screenNum);
            break;
        default:
            break;
        }
    }
    else 
    {
        // we're spanning over all 3 screens, then we're getting the left, top, bottom coordinates
        // of the first screen and the right coordinate of the last screen.
        ScreenArrangement& layoutFirst = m_screenLayout[1];
        ScreenArrangement& layoutLast = m_screenLayout[m_numberOfMonitors];
        switch(targetArea)
        {
        case TA_Base_Bus::TA_GenericGui::AREA_SCREEN:
            rect =  layoutFirst.screenRect;
            rect.right = layoutLast.screenRect.right;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Getting the combined screen area of all monitors");
            break;
        case TA_Base_Bus::TA_GenericGui::AREA_BANNER:
            rect = layoutFirst.bannerRect;
            rect.right = layoutLast.bannerRect.right;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Getting the combined banner area of all monitors");
            break;
        case TA_Base_Bus::TA_GenericGui::AREA_MAINNAVIGATOR:
            rect = layoutFirst.applicationRect;
            rect.right = layoutLast.applicationRect.right;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Getting the combined main navigator/application area of all monitors");
            break;
        case TA_Base_Bus::TA_GenericGui::AREA_LAUNCHPAD:
            rect = layoutFirst.launchBarRect;
            rect.right = layoutLast.launchBarRect.right;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Getting the combined launchbar area of all monitors");
            break;
        case TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC:
            rect = layoutFirst.schematicRect;
            rect.right = layoutLast.schematicRect.right;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Getting the combined schematic display area of all monitors");
            break;
        default:
            break;
        }
    }
    LOG( SourceInfo, DebugUtil::FunctionExit, "getRect" );

    return rect;
}


RECT ScreenPositioning::getLaunchBarRect()
{
    RECT rect = NULL_RECT;
    for (ScreenLayout::iterator layout = m_screenLayout.begin(); layout != m_screenLayout.end(); layout++)
    {
        if (layout->second.launchBarPosition != ControlStation::HIDDEN)
        {
            rect = layout->second.launchBarRect;
        }
    }
    return rect;
}


RECT ScreenPositioning::getAppRectNextToLaunchBar()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getAppRectNextToLaunchBar" );

    for (ScreenLayout::iterator layout = m_screenLayout.begin(); layout != m_screenLayout.end(); layout++)
    {
        if (layout->second.launchBarPosition != ControlStation::HIDDEN)
        {
            return layout->second.applicationRect;
        }
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "getAppRectNextToLaunchBar" );
    // just make it launch on the first screen
    return m_screenLayout[1].applicationRect;
}


ScreenPositioning::WindowPositionVector ScreenPositioning::getGraphworxPositions()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getGraphworxPositions" );

    WindowPositionVector windowPosition;
    if (m_displaySpanAllScreens)
    {
        RECT graphworx = getRect(TA_Base_Bus::TA_GenericGui::SCREEN_ALL, TA_Base_Bus::TA_GenericGui::AREA_SCREEN);
        Coordinate topLeft;
        topLeft.first = graphworx.left;
        topLeft.second = graphworx.top;
        WindowPosition pos;
        pos.topLeft = topLeft;
        pos.width = graphworx.right - graphworx.left;
        pos.height = graphworx.bottom - graphworx.top;
        windowPosition.push_back(pos);
        LOG( SourceInfo, DebugUtil::FunctionExit, "getGraphworxPositions" );
        return windowPosition;
    }

    for (unsigned int screen = 1; screen <= m_numberOfMonitors; screen++)
    {
        // if the right and bottom coordinates are 0 (no need to check left and top
        // as they would undoubtedly be 0 too, then it means that we dont' want to
        // display graphworx, otherwise we do.
        if (m_screenLayout[screen].hasGraphWorx)
        {
            RECT& graphworx = m_screenLayout[screen].applicationRect;
            Coordinate topLeft;
            topLeft.first = graphworx.left;
            topLeft.second = graphworx.top;
            WindowPosition pos;
            pos.topLeft = topLeft;
            pos.width = graphworx.right - graphworx.left;
            pos.height = graphworx.bottom - graphworx.top;
            windowPosition.push_back(pos);
        }
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "getGraphworxPositions" );
    return windowPosition;
}


void ScreenPositioning::setDisplayDimension(const RECT dim)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "setDisplayDimension" );
    // see which screen we're setting
    int screenNum = getWhichScreenCoordinateIsOn(dim.left);
    if (screenNum > 0)
    {
        m_screenLayout[screenNum].schematicRect = dim;
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,"Setting the schematic dimension as %d, %d, %d, %d", dim.left, dim.top, dim.right, dim.bottom);
    }
    LOG( SourceInfo, DebugUtil::FunctionExit, "setDisplayDimension" );
}



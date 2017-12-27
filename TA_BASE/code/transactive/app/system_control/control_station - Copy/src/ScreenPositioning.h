/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ScreenPositioning.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Calculates the screen positions of launch pad (if exist), banner and applications
  * 
  */


#if !defined(ScreenPositioning_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_)
#define ScreenPositioning_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_

#include <vector>
#include <utility>
#include <map>
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "core/data_access_interface/entity_access/src/ControlStation.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

// Forward declaration
namespace TA_Base_Core
{
    class IEntityData;
}


class ScreenPositioning
{
public:

    // x, y coordinate pair
    typedef std::pair<unsigned long, unsigned long> Coordinate;

    // an array of coordinates
    typedef std::vector<Coordinate>  CoordinatesVector;

    struct WindowPosition
    {
        Coordinate topLeft;     // The top left corner coordinate of the window
        unsigned long width;    // The width of the window
        unsigned long height;   // The height of the window
    };

    typedef std::vector<WindowPosition> WindowPositionVector;

    // This structure keeps all the dimension and any other necessary flags for indicating
    // how to position the banner, launch bar and graph worx screen.  Except for the schematic
    // dimension, these rects are only set once after the initialisation.
    struct ScreenArrangement
    {
        RECT screenRect;        // The dimension of the screen area
        RECT bannerRect;        // The dimension of the banner area
        RECT launchBarRect;     // The dimension of the launch bar area
        RECT applicationRect;   // The dimension of the application area - i.e. whatever's left after placing the launch bar and banner
        RECT schematicRect;     // The dimension of the subdisplay area
        TA_Base_Core::ControlStation::BannerPosition bannerPosition;         // The positioning type of the banner
        TA_Base_Core::ControlStation::LaunchBarPosition launchBarPosition;   // The positioning type of the launch bar
        bool hasGraphWorx;      // True if graphworx is to be displayed on this screen
    };

    // This map is keyed on screen number (1+) and holds the layout for
    // each screen.
    typedef std::map<unsigned int, ScreenArrangement> ScreenLayout;


    /**
     * Constructor
     */
    ScreenPositioning( );


    /**
     * Destructor
     */
    virtual ~ScreenPositioning();


    /**
     * loadScreenLayout
     *
     * This will load the layout of each screen and store it. This should be called each time configuration
     * change is made to the ControlStation entity.
     *
     * @param ControlStation& - A reference to a ControlStation database object. This will be used
     *                          to retrieve the layout options from the Control Station entity's configuration.
     *
     */
    void loadScreenLayout(TA_Base_Core::ControlStation& controlStationDatabaseEntry,
                          std::vector<TA_Base_Core::IEntityData*>& banners);


    /**
     * getGraphworxPositions
     *
     * This will return a vector of window positions for launching, moving and resizing GraphWorx.
     * A call to loadScreenLayout() must have been done sometime before this is called.
     *
     * @return WindowPositionVector - The window positions of all GraphWorX instances required.
     */
    WindowPositionVector getGraphworxPositions();


    /**
     * getLaunchBarRect
     *
     * This will return the dimension of the launch bar.
     *
     * @return RECT - The window dimension of the launch bar. If all fields are 0 then no launch 
     *                bar should be displayed.
     */
    RECT getLaunchBarRect();


    /**
     * isLaunchBarDisplayed
     *
     * This will indicate whether the user has configured for the launch bar to be shown at the side
     * of the screen.
     *
     * @return bool - true indicates the launch bar should be shown
     */
    bool isLaunchBarDisplayed() const
    {
        return m_isLaunchBarShown;
    }


    /**
     * getAppRectNextToLaunchBar
     *
     * This will return the dimension of where the application will launch next to the launch bar.  
     *
     * NOTE: This should only be used by the launch pad to launch the application next to it.  
     *       It should not be used by any other applications unless what's intended is to
     *       launch the application next to launch bar
     *
     * @return RECT - The window dimension of the launch bar. If all fields are 0 then no launch 
     *                bar should be displayed.
     */
    RECT getAppRectNextToLaunchBar();

    
    /**
     * getWhichScreenCoordinateIsOn
     *
     * This calculates which screen a particular x-coordinate falls on.
     *
     * @param unsigned long - This is the x-coordinate to find
     * 
     * @return unsigned long - This is the monitor/screen number starting at 1. It will return
     *                         0 if the point did not fall on any of the screens.
     */
    int getWhichScreenCoordinateIsOn(long xCoord) const;


    /**
     * getNumberOfMonitors
     *
     * @return unsigned long - The number of monitors connected to this console
     */
    unsigned int getNumberOfMonitors() const
    {
        return m_numberOfMonitors;
    }


    /**
     * getEachScreenWidth
     *
     * @return unsigned long - The width of each individual monitor (in pixels)
     */
    unsigned long getEachScreenWidth() const
    {
        return m_eachScreenWidth;
    }


    /** 
      * setDisplayDimension
      *
      * Sets the dimension of the display area within the main navigator on screen 1.
      *
      * @param dim - the dimension of the display area within the main navigator on screen 1.
      */
    void setDisplayDimension(const RECT dim);


    /** 
      * getRect
      *
      * Returns the dimension of the specified area on the specified screen.
      *
      * @param targetScreen - Which screen
      * @param targetArea - The area on the screen
      * @param val - Optional value.  Stores the coordinate if targetScreen is 
      *              SCREEN_CURRENT, SCREEN_PREVIOUS, SCREEN_NEXT.
      *              Stores the screen number if targetScreen is SCREEN_SPECIFIC 
      *
      * @return The dimension of the specified area on the specified screen.
      *
      */
    RECT getRect(const TA_Base_Bus::TA_GenericGui::EScreen targetScreen, 
                 const TA_Base_Bus::TA_GenericGui::EArea targetArea, 
                 long val = -1);

	std::vector<bool> getBannerVisibility() const
	{ 
		return m_isBannerVisible; 
	}

private:

    //
    // These are made private as we do not need them.
    //
    ScreenPositioning( const ScreenPositioning& sd){ };
    ScreenPositioning& operator=(const ScreenPositioning&){ };


    /**
     * calculateNumberOfMonitors
     *
     * This will retrieve the number of monitors based on the screen resolution retrieved.
     *
     * @param unsigned long - The number of monitors the computer has.
     */
    unsigned long calculateNumberOfMonitors();

    
    /**
     * calculateScreenWidth
     *
     * This will retrieve the screen width of each screen. This is based on the fact that
     * the system has multiple monitors. The width we retrieve could be the width across
     * all monitors or the width across one. We must determine what the width of each
     * screen is.
     *
     * @param unsigned long - The width of a single monitor
     */
    unsigned long calculateScreenWidth();

    
    /**
     * loadBannerScreenLayout
     *
     * This will load the layout of the banner for each screen and store it.
     *
     * @param ControlStation& - A reference to a ControlStation database object. This will be used
     *                          to retrieve the layout options from the Control Station entity's configuration.
     */
    void loadBannerScreenLayout(TA_Base_Core::ControlStation& controlStationDatabaseEntry,
                                std::vector<TA_Base_Core::IEntityData*>& banners);

    /**
     * loadGraphworxScreenLayout
     *
     * This will load the layout of GraphWorx for each screen and store it.
     *
     * @param ControlStation& - A reference to a ControlStation database object. This will be used
     *                          to retrieve the layout options from the Control Station entity's configuration.
     */
    void loadGraphworxScreenLayout(TA_Base_Core::ControlStation& controlStationDatabaseEntry);

    /**
     * loadLaunchBarScreenLayout
     *
     * This will load the layout of the launch bar for each screen and store it. The launch bar will only
     * ever appear on one screen maximum.
     *
     * @param ControlStation& - A reference to a ControlStation database object. This will be used
     *                          to retrieve the layout options from the Control Station entity's configuration.
     */
    void loadLaunchBarScreenLayout(TA_Base_Core::ControlStation& controlStationDatabaseEntry);


    /**
     * loadApplicationScreenLayout
     *
     * This sets the graphworx/application area to fill up the remaining space after placing the banner and launch pad.
     *
     */
    void loadApplicationScreenLayout();


    /** 
      * getScreenNumber
      *
      * Returns the screen number of the targetScreen.  Returns -1 if it's over all screens
      *
      * @param targetScreen - Which screen
      * @param val - Optional value.  Stores the coordinate if targetScreen is 
      *              SCREEN_CURRENT, SCREEN_PREVIOUS, SCREEN_NEXT.
      *              Stores the screen number if targetScreen is SCREEN_SPECIFIC 
      *
      * @return the screen number of the targetScreen
      *
      */
    int getScreenNumber(TA_Base_Bus::TA_GenericGui::EScreen targetScreen, long val);


    TA_Base_Core::NonReEntrantThreadLockable m_layoutLock; // This thread locks the m_screenLayout member variable.
    ScreenLayout m_screenLayout;         // This holds the screen layout for each monitor.
    bool m_isLaunchBarShown;             // This indicates whether the side launch bar is being used or not
    bool m_displaySpanAllScreens;        // True if a single graphworx display is to span across all screen

    unsigned long m_totalScreenWidth;    // This is the total width of the screen across all monitors
    unsigned long m_screenBottom;        // This is the bottom of the screen where the useable area ends
    unsigned long m_screenTop;           // This is the top position of the screen where the useable area begins
    unsigned long m_eachScreenWidth;     // This is the width of each screen (totalScreenWidth / numberOfMonitors)
    unsigned long m_numberOfMonitors;    // This is the number of monitors the machine has   

    static const RECT NULL_RECT;                    // This is used to indicate a rectangle whose dimension is zero
    static const unsigned long LAUNCH_BAR_WIDTH;    // This is the width of the launch bar 

	std::vector<bool> m_isBannerVisible;
};

#endif // !defined(ScreenPositioning_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_)

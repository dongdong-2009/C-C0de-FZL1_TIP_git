/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ControlStationCorbaImpl.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This receives callbacks from the ManagedProcesses and will allow applications to be launched.
  * It also receives callbacks when the services have been restarted.
  * This implements the IControlStationCorbaDef corba definition which means this
  * object is what the ManagedProcess's get when they resolve the Control Station.
  */

#if !defined(ControlStationCorbaImpl_C550A2B9_9026_4f94_9F48_174625E800BF__INCLUDED_)
#define ControlStationCorbaImpl_C550A2B9_9026_4f94_9F48_174625E800BF__INCLUDED_

#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "core\process_management\IDL\src\IControlStationCorbaDef.h"
#include "core\corba\src\ServantBase.h"


class ControlStationCorbaImpl : public virtual POA_TA_Base_Core::IControlStationCorbaDef,
                                public virtual TA_Base_Core::ServantBase
{
public:

    /**
     * Constructor
     *
     * @param string - The name of the console this Control Station is running on
     */
    ControlStationCorbaImpl(const std::string& consoleName);


    /**
     * Destructor
     */
    virtual ~ControlStationCorbaImpl();


public:

    /** 
      * getRect
      *
      * Gets the RECT value of the specified area on the specified screen.
      *
      * @param targetScreen - The screen on the MFT
      * @param targetArea - The target area within the screen
      * @param val - Optional value.  This value should to the x coordinate value 
      *              if targetScreen is SCREEN_CURRENT, SCREEN_PREVIOUS or SCREEN_NEXT.  
      *              It should contain the screen number if target screen is 
      *              SCREEN_SPECIFIC.  By default this is set to -1.
      *
      * @return The CRect value of the targetArea that lies on targetScreen.
      *
      */
    TA_Base_Core::IControlStationCorbaDef::RECT getRect( unsigned long targetScreen, 
                                                    unsigned long targetArea, 
                                                    long val);


    /** 
      * launchAnApplication
      *
      * This will launch an application in a specified position. Any additional arguments
      * to provide to the application can be specified.
      *
      * @param appType - The application to launch
      * @param extraCommandLine - The command line to be passed to the application.
      * @param posFlag - How the application is going to be positioned relative to boundary paramter
      * @param alignFlag - How the application is going to be positioned relative to point parameter
      * @param objectDim - The dimension of the object that this application will be launched relative
      *                    to (if POS_RELATIVE or POS_NOOVERLAP is specified).
      * @param boundaryDim - The dimension of the object that this application will be bounded in
      *                      (if POS_BOUNDED is specified)
      *
      */
    void launchAnApplication(  unsigned long appType, 
                               const char* extraCommandLine, 
                               unsigned long posFlag,
                               unsigned long alignFlag,
                               const TA_Base_Core::IControlStationCorbaDef::RECT& objectDim,
                               const TA_Base_Core::IControlStationCorbaDef::RECT& boundaryDim);


    /**
     * launchGraphWorxDisplay
     *
     * This attempts to load the specified display in GraphWorX. The GraphWorX instance used
     * will be dependent on the screen passed in.
     *
     * @param string - The name and path of the display to load
     * @param string - The asset in the display that is to be centered on the screen.  
     * @param long -   The left position of the application that made the request. This must be used
     *                 to determine which screen to load the display on.
     *
     * @exception ApplicationException Indicates an error occurred while attempting to launch the display.
     */
	virtual void launchDisplay(const char* display, const char* asset, long leftPosition);


    /**
     * Does nothing - just used to see if the Control Station is *there
     */
    void poll() {}

private:
    // Hidden as they are not needed
	ControlStationCorbaImpl( const ControlStationCorbaImpl& theControlStationCorbaImpl);
	ControlStationCorbaImpl& operator=(const ControlStationCorbaImpl& pm);
};

#endif // !defined(ControlStationCorbaImpl_C550A2B9_9026_4f94_9F48_174625E800BF__INCLUDED_)

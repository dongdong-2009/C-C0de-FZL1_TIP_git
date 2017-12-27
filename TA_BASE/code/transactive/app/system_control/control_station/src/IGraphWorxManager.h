/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/IGraphWorxManager.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *  
  * This interface advertises the calls the AutoProxy will need to callback.
  */

#if !defined(IGraphWorxManager_36889BB4_BAF3_4bc1_B189_C5757C976786__INCLUDED_)
#define IGraphWorxManager_36889BB4_BAF3_4bc1_B189_C5757C976786__INCLUDED_

#include <map>
#include <vector>
#include <string>

class IGwx32;

class IGraphWorxManager
{

public:

    struct GraphWorXInstance
    {
        IGwx32* graphworx;      // The instance of graphworx display
        unsigned long left;     // The left position of the display
        unsigned long top;      // The top position of the display
        unsigned long width;    // The width of the display
        unsigned long height;   // The height of the display
        std::string background; // The display that will be launched before the startup display if specified.  Displays that are launched after this will possess the frame properties of this background display
        std::string display;    // The startup display
    };


    /**
     * Destructor
     */
    virtual ~IGraphWorxManager() { };


    /**
      * copyGraphWorxDisplay
      * 
      * This will launch a new GraphWorX display on the screen specified. This newly
      * launched display will hold the same display as that shown on the GraphWorX
      * instance running beside the launch bar.
      *
      * @param unsigned long screenNumber - The screen the GraphWorX instance should launch on.
      *
      * @exception GraphWorxException - This indicates that the GraphWorx display could not be launched.
      */
    virtual void copyGraphWorxDisplay(const unsigned long screenNumber) =0;


    /** 
      * getCurrentDisplay
      *
      * Returns the name of the current display on the specified screen
      *
      * @param screenNumber The screen number
      *
      * @return the name of the current display on the specified screen
      */
    virtual std::string getCurrentDisplay(unsigned int screenNumber) =0;


    /** 
      * loadComplete
      *
      * This indicates that the display has been completely loaded on the specified screen.
      *
      * @param leftPosition the left position of the display screen
      */
    virtual void loadComplete(const unsigned long leftPosition) =0;


    /**
     * waitUntilReady
     *
     * This waits until all instances of Graphworx are ready before returning. This can be used by
     * the login dialog to ensure that GraphWorx isn't grabbing the screen focus before displaying the login
     * dialog
     */
    virtual void waitUntilReady() =0;

};

#endif // !defined(IGraphWorxManager_36889BB4_BAF3_4bc1_B189_C5757C976786__INCLUDED_)

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/GraphWorxManager.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *  
  * This class will control the instances of the GraphWorx threads and allow the session manager to
  * control GraphWorx.
  */

#if !defined(GraphWorxManager_CDF2CE12_706C_411a_B627_B2DFBC17F7D7__INCLUDED_)
#define GraphWorxManager_CDF2CE12_706C_411a_B627_B2DFBC17F7D7__INCLUDED_

#include "app\system_control\control_station\src\IGraphWorxManager.h"

#include <string>
#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

class GraphWorxCommunicator;
class GraphWorxApplication;
class ScreenPositioning;

class GraphWorxManager : public IGraphWorxManager
{
public:

	/**
    * Constructor
    *
    * @param background - The background display for GraphWorx to load that stays on
    *                     the background and display the display specified.
    * @param display - The display for GraphWorx to load on startup
    * @param ScreenPositioning& - This holds the information on how many GraphWorx instances
    *                             should be loaded and where they should be positioned.
    */
    GraphWorxManager(const std::string& background, const std::string& display, ScreenPositioning& screenPositions);


	/**
    * Destructor
    */
	virtual ~GraphWorxManager();


    /**
    * launchGraphWorx
    *
    * This launches three instances of GraphWorX on the left, centre and right monitor.
    *
    * @exception GraphWorxException This is thrown if GraphWorx cannot be loaded or a schematic
    *            cannot be displayed. If GraphWorX could not be run then the exception will have
    *            a type NOT_RUN in it and the caller needs to take action. If the type is
    *            FILE_OPEN_ERROR then GraphWorx was launched but errors occurred when trying to
    *            load a schematic.
    */
    void launchGraphWorx();


    /**
    * shutdownGraphWorx
	* This will tell all three GraphWorX applications to hide.
	*/
	void shutdownGraphWorx();


    /**
     * waitUntilReady
     *
     * This waits until all instances of Graphworx are ready before returning. This can be used by
     * the login dialog to ensure that GraphWorx isn't grabbing the screen focus before displaying the login
     * dialog
     */
    void waitUntilReady();


    /**
     * incrementNumberReady
     *
     * This is called by each instance of graphworx when it is ready.
     */
    void incrementNumberReady();


    /**
      * copyGraphWorxDisplay
      * 
      * This will launch a new GraphWorX display on the screen specified. This newly
      * launched display will hold the same display as that shown on the GraphWorX
      * instance running beside the launch bar.
      * PRECONDITION: screenNumber <= number of monitors on console
      *
      * @param unsigned long screenNumber - The screen the GraphWorX instance should launch on.
      *
      * @exception GraphWorxException - This indicates that the GraphWorx display could not be launched.
      */
    virtual void copyGraphWorxDisplay(const unsigned long screenNumber);


    /** 
      * getCurrentDisplay
      *
      * Returns the name of the current display on the specified screen
      *
      * @param screenNumber The screen number
      *
      * @return the name of the current display on the specified screen
      */
    std::string getCurrentDisplay(unsigned int screenNumber);


private:
    // Made private as these are not needed.
	GraphWorxManager( const GraphWorxManager& theGraphWorxManager);
    GraphWorxManager& operator=(const GraphWorxManager& gwc);
    

    /**
      * calculateDisplayToLoad
      * 
      * This method determines which GraphWorX instance is running on the screen with
      * the launch bar and retrieves the display it is currently showing.
      *
      * @return std::string - The name and path of the display for the new GraphWorx
      *                       instance to load.
      */
    std::string calculateDisplayToLoad();


    /**
    * 
    * These set and get methods are used to access the member variables in a thread safe
    * manner.
    */
    unsigned long getNumberOfInstancesReady();


    /** 
      * loadComplete
      *
      * This indicates that the display has been completely loaded on the specified screen.
      *
      * @param leftPosition the left position of the display screen
      */
    void loadComplete(const unsigned long leftPosition);


    // The number of seconds to wait for the GraphWorX instances to come back ready.
    static const int SECONDS_TO_WAIT;

    // Each instance of graphworx that is to be launched and must continue to run
    std::vector<GraphWorxCommunicator*>   m_graphWorxCommunicators;

    // Special GraphWorX launches performed by the user. These are not maintained as
    // per the other instances and are closed permanently when the user logs out.
    std::vector<GraphWorxApplication*> m_graphWorxApplications;

    // This is incremented each time a GraphWorx thread is ready.
    volatile unsigned long m_numberOfGraphWorxInstancesReady;

    ScreenPositioning& m_screenPositions;

    // Used to lock the member variables that can be accessed from several points at once.
    mutable TA_Base_Core::ReEntrantThreadLockable m_instancesReadyLock;  
};

#endif // !defined(GraphWorxManager_CDF2CE12_706C_411a_B627_B2DFBC17F7D7__INCLUDED_)

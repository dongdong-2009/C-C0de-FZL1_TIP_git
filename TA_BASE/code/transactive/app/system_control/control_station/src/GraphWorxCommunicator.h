/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/GraphWorxCommunicator.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *  
  * This class will communicate with GraphWorX. It will launch a GraphWorX instance and
  * continue to send it messages through its lifetime.
  * This is different to the GraphWorXApplication in that it ensures the GraphWorX instance
  * is always running and only hides it when the user logs out.
  */

#if !defined(GraphWorxCommunicator_CDF2CE12_706C_411a_B627_B2DFBC17F7D7__INCLUDED_)
#define GraphWorxCommunicator_CDF2CE12_706C_411a_B627_B2DFBC17F7D7__INCLUDED_

#include "app\system_control\control_station\src\IGraphWorxManager.h"
#include <string>
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

// Forward declare. Header included in the cpp file.
class IGwx32;
class GraphWorxManager;


class GraphWorxCommunicator : public TA_Base_Core::Thread
{
public:

	/**
    * Constructor
    */
    GraphWorxCommunicator(IGraphWorxManager::GraphWorXInstance& instance, GraphWorxManager& manager);


	/**
    * Destructor
    */
	virtual ~GraphWorxCommunicator();


    /**
    * run
    *
	* This will block until terminate() is called.
    * NOTE: do not call this method directly. It needs to be public so that it can be called from
    * the thread-spawning function
	*/
	virtual void run();


	/**
    * terminate
    *
	* The terminate() method will cause run() to return. 
    * NOTE: probably not a good idea to call this method directly - call terminateAndWait() instead
	*/
	virtual void terminate();


    /**
    * launchGraphWorx
    *
    * This launches three instances of GraphWorX on the left, centre and right monitor. It doesn't
    * actually launch GraphWorx but just displays the already loaded version on the screen.
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
    *
	* This will tell all the GraphWorX application to hide (not actually shutdown, 
    * just hide).
	*/
	void shutdownGraphWorx();


    /**
    * isGraphWorxInstanceBesideLaunchBar
    *
	* This will determine if this GraphWorx instance is running beside the launch bar.
    * This is done by checking to see if it is the full width of the screen. If it
    * is not then it is next to the launch bar.
    * 
    * @return bool - True if this instance is running on the same screen as GraphWorx.
	*/
    bool isGraphWorxInstanceBesideLaunchBar(const unsigned long screenWidth);


    /**
    * getDisplay
    *
	* This will return the name and path of the display this GraphWorx instance
    * currently has loaded.
    */
    std::string getDisplay();


    /**
    * getChildDisplay
    *
	* This will return the name and path of the popup display this GraphWorx instance
    * currently has loaded.
    */
    std::string getChildDisplay();


    /** 
      * loadComplete
      *
      * This indicates that the display has been completely loaded
      *
      * @param leftPosition the left position of the display screen
      *
      * @return true if this graphworx screen's left position is as specified
      */
    bool loadComplete(const unsigned long leftPosition);


    /**
     * printDisplay
     *
     * Prints the graphworx display held by this instance of graphworx.
     */
    void printDisplay();


private:
    // Made private as these are not needed.
    GraphWorxCommunicator( const GraphWorxCommunicator& theGraphWorxCommunicator);
    GraphWorxCommunicator& operator=(const GraphWorxCommunicator& rhs);


    /** 
     * runGraphWorx
     *
     * This will actually kick off an instance of GraphWorX, load the display and position it.
     */
    void runGraphWorx();


    /**
     * loadBaseDisplay
     *
     * This will create the GraphWorX instance and open the base display
     *
     * @exception TransactiveException Will throw this when there is a problem
     *            creating a GraphWorX instance or when any file error occured.
     */
    void loadBaseDisplay(const std::string& baseDisplay);


    /** 
     * terminateGraphWorx
     *
     * This will nicely terminate the instance of GraphWorX.
     */
    void terminateGraphWorx();
    

    /**
     * showGraphWorx
     * 
     * When told to launch GraphWorx this will just display it on the screen. We will have already loaded
     * it.
     */
    void showGraphWorx();


    /**
     * hideGraphWorx
     * 
     * When told to shutdown GraphWorx this will just hide it on the screen. We will shut it down in the
     * destructor.
     */
    void hideGraphWorx();

    
    /**
    * 
    * These set and get methods are used to access the member variables in a thread safe
    * manner. 'Shutdown' in this instance means 'hide'.
    */
    void setIsTerminating(const bool isTerminating);
    bool isTerminating();
    void setShouldLaunchGraphWorx(const bool shouldLaunch);
    bool shouldLaunchGraphWorx();
    void setShouldShutdownGraphWorx(const bool shouldShutdown);
    bool shouldShutdownGraphWorx();
    static void waitService( const char* serviceName, const DWORD targetState, unsigned long queryInterval = 1000 );


    // This holds each GraphWorX window, configuration information for it and whether
    // it is running or not.
    IGraphWorxManager::GraphWorXInstance m_graphWorXWindow;

    // This is the GraphWorxManager we use to callback on when we reach certain milestones.
    // eg successfully loaded, successfully displayed etc etc.
    GraphWorxManager& m_manager;

    // This is the path and name of the child popup display of the GraphWorX window.
    std::string m_currentChildDisplay;

    TA_Base_Core::NonReEntrantThreadLockable m_loadingLock;
    TA_Base_Core::NonReEntrantThreadLockable m_variableLock;  // Used to lock the member variables that can be accessed from several points at once.

    // Indicates when the thread is terminated
    volatile bool m_isTerminating;  

    // Indicates when we should launch GraphWorx. We have to launch GraphWorx
    // as it has to be done in the same thread it is created in.
    volatile bool m_shouldLaunchGraphWorx; 

    // Indicates when we should shutdown GraphWorx. We have to shutdown GraphWorx
    // as it has to be done in the same thread it is created in.
    volatile bool m_shouldShutdownGraphWorx; 
                                    
    // This indicates if GraphWorx loaded correctly
    volatile bool m_isValidLoad;               

    // This indicates if GraphWorx is currently displaying to the user.
    volatile bool m_isShowing;           
};

#endif // !defined(GraphWorxCommunicator_CDF2CE12_706C_411a_B627_B2DFBC17F7D7__INCLUDED_)

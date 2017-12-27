/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ApplicationMover.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This thread is linked to a single instance of a RunningApplication. After a set period
  * of time it will attempt to move the application into the specified position.
  * 
  */


#if !defined(ApplicationMover_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_)
#define ApplicationMover_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_

#include "core/threads/src/Thread.h"

class ApplicationMover : public TA_Base_Core::Thread
{
public:

    struct WindowMoveInformation
    {
        unsigned long processId;    // The processId of the application to move
        unsigned long posFlag;      // Contains the positioning flags
        unsigned long alignFlag;    // Contains the alignment flags
        RECT boundaryDim;           // The dimension of the boundary to bind the application within
        RECT objectDim;             // The dimension of the object
        RECT appDim;                // The calculated dimension of the object
        volatile bool finishedRepositioning; // True if the ApplicationMover has finished repositioning this app
    };


    /**
      * Constructor
      */
    ApplicationMover();


    /**
      * Destructor
      */
    virtual ~ApplicationMover();


    /**
      * setProcessDetails
      *
      * This is used to set the details of the process before starting the thread.
      *
      * @param processId - The processId of the application to move
      * @param posFlag - Contains the positioning flags
      * @param alignFlag - Contains the alignment flags
      * @param objectDim - The dimension of the object
      * @param boundaryDim - The dimension of the boundary to bind the application within
      */
    void setProcessDetails(const unsigned long processId,
                            const unsigned long posFlag,
                            const unsigned long alignFlag,
                            const RECT& objectDim,
                            const RECT& boundaryDim);


    /** 
      * getProcessDetails
      *
      * Returns the positioning details for this process
      *
      * @return the positioning details for this process
      */
    WindowMoveInformation getProcessDetails() const
    {
        return m_moveInformation;
    }


    /**
      * run
      *
      * This will block until terminate() is called or the application is succesfully moved into
      * place.
      * NOTE: do not call this method directly. It needs to be public so that it can be called from
      * the thread-spawning function
      */
    void run();


    /**
      * terminate
      *
      * The terminate() method will cause run() to return. 
      * NOTE: probably not a good idea to call this method directly - call terminateAndWait() instead
      */
    void terminate();


    /** 
      * resetThread
      *
      * Reset the thread so that it is no longer terminating
      *
      */
    void resetThread();

private:
   
    // Made private as they are not needed
    ApplicationMover( const ApplicationMover& theApplicationMover);
    ApplicationMover& operator=(const ApplicationMover& pm);


    /**
      * WndPositionEnumHandler
      *
      * This handles callbacks from ::EnumWindows()
      * This is a handler for top level window enumeration, which lets us
      * determine which window needs to be positioned.
      *
      * @return TRUE to continue enumerating windows, FALSE to stop.
      *
      */
    static BOOL CALLBACK WndPositionEnumHandler(HWND hWnd, LPARAM lParam);


    // The information required to move the correct application into the correct position.
    WindowMoveInformation m_moveInformation;  

    // Indicates whether we are terminating or not.
    volatile bool m_isTerminating;                     
};

#endif // !defined(ApplicationMover_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_)

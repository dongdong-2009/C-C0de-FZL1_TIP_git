/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/GraphWorxManager.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *  
  * This class will control the instances of the GraphWorx threads and allow the session manager to
  * control GraphWorx.
  */


#include <algorithm>   // For std::remove

#include "app\system_control\control_station\src\StdAfx.h"
#include "app\system_control\control_station\src\GraphWorxCommunicator.h"
#include "app\system_control\control_station\src\GraphWorxApplication.h"
#include "app\system_control\control_station\src\GraphWorxManager.h"
#include "app\system_control\control_station\src\ScreenPositioning.h"

#include "core\exceptions\src\GraphWorxException.h"
#include "core\utilities\src\DebugUtil.h"

using TA_Base_Core::DebugUtil;

const int GraphWorxManager::SECONDS_TO_WAIT = 30;


GraphWorxManager::GraphWorxManager(const std::string& background, const std::string& display, ScreenPositioning& screenPositions) 
 :  m_numberOfGraphWorxInstancesReady(0), m_screenPositions(screenPositions)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "Constructor");

    // Now retrieve how many GraphWorx instances and their positions.
    ScreenPositioning::WindowPositionVector graphWorxPositions = m_screenPositions.getGraphworxPositions();

    // Iterate through and store the details for each GraphWorx instance. As well as this start each
    // instance as we go
    ScreenPositioning::WindowPositionVector::iterator iter = graphWorxPositions.begin();
    while (iter != graphWorxPositions.end() )
    {
        GraphWorXInstance gw;
        gw.graphworx = NULL;
        gw.left = iter->topLeft.first;
        gw.top = iter->topLeft.second;
        gw.width = iter->width;
        gw.height = iter->height;
        gw.display = display;
        gw.background = background;

        GraphWorxCommunicator* gwc = new GraphWorxCommunicator(gw, *this);
        if( gwc == NULL )
        {
            using TA_Base_Core::GraphWorxException;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on GraphWorxCommunicator returned NULL");
            TA_THROW( GraphWorxException("GraphWorx instances could not be created. new returned NULL.", GraphWorxException::NOT_RUN) );
        }

        gwc->start();
        m_graphWorxCommunicators.push_back( gwc );

       ++iter;
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "Constructor");
}

GraphWorxManager::~GraphWorxManager()
{
    FUNCTION_ENTRY("~GraphWorxManager");
    try
    {
        // Delete all GraphWorxApplication's that have been launched. Deletion will cause them
        // to be terminated.
        for (std::vector<GraphWorxApplication*>::iterator iter1 = m_graphWorxApplications.begin();
                                                          iter1 != m_graphWorxApplications.end();
                                                          ++iter1)
        {
            if ((*iter1) != NULL)
            {
                delete *iter1;
                *iter1 = NULL;
            }
        }

        // Delete all GraphWorxCommunicators that were started when the Control Station started.
        for (std::vector<GraphWorxCommunicator*>::iterator iter2 = m_graphWorxCommunicators.begin();
                                                           iter2 != m_graphWorxCommunicators.end();
                                                           ++iter2)
        {
            (*iter2)->terminateAndWait();
            delete *iter2;
            *iter2 = NULL;
        }
    }
    catch ( ... )
    {
        // Catch all exceptions so we don't get errors filtering out
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor of GraphWorxManager");
    }
    FUNCTION_EXIT;
}

void GraphWorxManager::launchGraphWorx()
{
    m_numberOfGraphWorxInstancesReady = 0;

    for (std::vector<GraphWorxCommunicator*>::iterator iter = m_graphWorxCommunicators.begin();
                                                       iter != m_graphWorxCommunicators.end();
                                                       ++iter)
    {
        (*iter)->launchGraphWorx();
    }
}

void GraphWorxManager::shutdownGraphWorx()
{
    // Delete all GraphWorxApplication's that have been launched. Deletion will cause them
    // to be terminated. This is what we want as the user has logged out.
    for (std::vector<GraphWorxApplication*>::iterator iter = m_graphWorxApplications.begin();
                                                      iter != m_graphWorxApplications.end();
                                                      ++iter)
    {
        if ((*iter) != NULL)
        {
            delete *iter;
            *iter = NULL;
        }
    }
    // Empty the vector
    m_graphWorxApplications.erase( std::remove(m_graphWorxApplications.begin(), m_graphWorxApplications.end(), static_cast<GraphWorxApplication*>(NULL)), m_graphWorxApplications.end() );


    // Now hide all other instances so they are ready next time the user logs in.
    m_numberOfGraphWorxInstancesReady = 0;

    for (std::vector<GraphWorxCommunicator*>::iterator iter2 = m_graphWorxCommunicators.begin();
                                                       iter2 != m_graphWorxCommunicators.end();
                                                       ++iter2)
    {
        (*iter2)->shutdownGraphWorx();
    }
}


void GraphWorxManager::waitUntilReady()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "waitUntilReady");

    // After SECONDS_TO_WAIT seconds we will give up
    int numSeconds = 0;
    while ( ( getNumberOfInstancesReady() < m_graphWorxCommunicators.size() ) &&
            ( numSeconds < SECONDS_TO_WAIT ) )
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Waited %d second for all instances of graphworx to be ready; going to wait another second.", numSeconds);
        TA_Base_Core::Thread::sleep(1000);
        ++numSeconds;
    }

    // Reset for next time
    m_numberOfGraphWorxInstancesReady = 0;

    LOG( SourceInfo, DebugUtil::FunctionExit, "waitUntilReady");
}

void GraphWorxManager::incrementNumberReady()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "incrementNumberReady");

    TA_THREADGUARD(m_instancesReadyLock);
    ++m_numberOfGraphWorxInstancesReady;

    LOG( SourceInfo, DebugUtil::FunctionExit, "incrementNumberReady");
}


unsigned long GraphWorxManager::getNumberOfInstancesReady()
{
    TA_THREADGUARD(m_instancesReadyLock);
    return m_numberOfGraphWorxInstancesReady;
}


void GraphWorxManager::copyGraphWorxDisplay(const unsigned long screenNumber)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "copyGraphWorxDisplay");

    // Now retrieve the position information for a GraphWorX instance on this screen.
    RECT graphWorxPosition = m_screenPositions.getRect(TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC, TA_Base_Bus::TA_GenericGui::AREA_MAINNAVIGATOR, screenNumber);

    GraphWorXInstance gw;
    gw.graphworx = NULL;
    gw.left = graphWorxPosition.left;
    gw.top = graphWorxPosition.top;
    gw.width = graphWorxPosition.right - graphWorxPosition.left;
    gw.height = graphWorxPosition.bottom - graphWorxPosition.top;
    gw.display = calculateDisplayToLoad();

    GraphWorxApplication* gwc = new GraphWorxApplication(gw);
    if( gwc == NULL )
    {
        using TA_Base_Core::GraphWorxException;
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on GraphWorxApplication returned NULL");
        TA_THROW( GraphWorxException("GraphWorx instances could not be created. new returned NULL.", GraphWorxException::NOT_RUN) );
    }
    m_graphWorxApplications.push_back(gwc);

    LOG( SourceInfo, DebugUtil::FunctionExit, "copyGraphWorxDisplay");
}


std::string GraphWorxManager::calculateDisplayToLoad()
{
    unsigned long screenWidth = m_screenPositions.getEachScreenWidth();

    for (std::vector<GraphWorxCommunicator*>::iterator iter = m_graphWorxCommunicators.begin();
         iter != m_graphWorxCommunicators.end();
         ++iter)
    {
        if( (*iter) != NULL )
        {
            if( (*iter)->isGraphWorxInstanceBesideLaunchBar(screenWidth) )
            {
                return (*iter)->getDisplay();
            }
        }
    }
    
    // If we get to here then we didn't find a matching GraphWorX instance.
    // Throw an exception
    using TA_Base_Core::GraphWorxException;
    TA_THROW( GraphWorxException("Could not find a GraphWorx instance running on the same screen as the launch bar.", GraphWorxException::NOT_RUN) );
}


std::string GraphWorxManager::getCurrentDisplay(unsigned int screenNumber)
{
    if ((screenNumber < 1) || (screenNumber > m_graphWorxCommunicators.size()))
    {
        return "";  // otherwise, not displaying any graphworx on the specified screen
    }
    // TD8433: Get the child display (the popup) instead of the initial display loaded in the communicator.
    return m_graphWorxCommunicators[screenNumber - 1]->getChildDisplay();
}


void GraphWorxManager::loadComplete(const unsigned long leftPosition)
{
    for (std::vector<GraphWorxCommunicator*>::iterator iter = m_graphWorxCommunicators.begin();
         iter != m_graphWorxCommunicators.end();
         ++iter)
    {
        if( (*iter) != NULL )
        {
            // if we found the graphworx that resides on that position, then we can return
            if((*iter)->loadComplete(leftPosition))
            {
                return;
            }
        }
    }
}



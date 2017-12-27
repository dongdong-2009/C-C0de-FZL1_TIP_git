/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/GraphWorxApplication.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *  
  * This class will communicate with GraphWorX. It will launch a single instance of GraphWorx which
  * will be terminated when the user logs out. This is different to the GraphWorXCommunicator in that
  * it does not ensure the GraphWorX instance continues to run and it immediately launches and 
  * displays it.
  */

#include "app\system_control\control_station\src\StdAfx.h"
#include "app\system_control\control_station\src\GraphWorxApplication.h"
#include "app\system_control\control_station\src\gwx32.h"
#include "core\exceptions\src\GraphWorxException.h"
#include "core\utilities\src\DebugUtil.h"

using TA_Base_Core::DebugUtil;

GraphWorxApplication::GraphWorxApplication(IGraphWorxManager::GraphWorXInstance instance) 
 : m_graphWorXWindow(instance)
{
    runGraphWorx();
}


GraphWorxApplication::~GraphWorxApplication()
{
    try
    {
        terminateGraphWorx();
        CoUninitialize();
    }
    catch ( ... )
    {
        // Catch all exceptions so we don't get errors filtering out
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor of GraphWorxApplication");
    }
}


void GraphWorxApplication::runGraphWorx()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "runGraphWorx");

    CoInitialize(NULL);

    using TA_Base_Core::GraphWorxException;

    try
    {
        // If the display is empty then there is no point continuing
        if (m_graphWorXWindow.display.empty() )
        {
            TA_THROW( GraphWorxException("Display passed to be loaded is empty", GraphWorxException::INVALID_FILE) );
        }

        m_graphWorXWindow.graphworx = new IGwx32;
        if (m_graphWorXWindow.graphworx == NULL)
        {
            TA_THROW( GraphWorxException("new IGwx32 returned NULL. Cannot continue to load GraphWorx",GraphWorxException::NOT_RUN) );
        }

        COleException pError; 
        if (!m_graphWorXWindow.graphworx->CreateDispatch("Gwx32.Display",&pError) )
        {
            char msg[256];
            pError.GetErrorMessage(msg,256);
            std::string error = "CreateDispatch failed with error: ";
            error += msg;
            TA_THROW( GraphWorxException(error.c_str(),GraphWorxException::NOT_RUN) );
        }

        if ( !m_graphWorXWindow.graphworx->FileNew())
        {
            TA_THROW( GraphWorxException("Failed to perform a FileNew on the GraphWorX instance",GraphWorxException::FILE_OPEN_ERROR) );
        }

        if (!m_graphWorXWindow.graphworx->FileOpen(m_graphWorXWindow.display.c_str()))
        {
            TA_THROW( GraphWorxException("Failed to perform a FileOpen on the GraphWorX instance",GraphWorxException::FILE_OPEN_ERROR) );
        }

        m_graphWorXWindow.graphworx->SetWindowDimensionsPixels(m_graphWorXWindow.left,m_graphWorXWindow.top,m_graphWorXWindow.width,m_graphWorXWindow.height);
        m_graphWorXWindow.graphworx->StartRuntime();
        m_graphWorXWindow.graphworx->ShowWindow();

    }
    catch ( GraphWorxException& )
    {
        if ( m_graphWorXWindow.graphworx != NULL)
        {
            delete m_graphWorXWindow.graphworx;
            m_graphWorXWindow.graphworx = NULL;
        }
        throw;
    }
    catch ( ... )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Thrown while GraphWorX was attempting to start up.");
        if ( m_graphWorXWindow.graphworx != NULL)
        {
            delete m_graphWorXWindow.graphworx;
            m_graphWorXWindow.graphworx = NULL;
        }
        TA_THROW( GraphWorxException("Unknown exception caught. Pass this out as a known exception",GraphWorxException::NOT_RUN) );
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "runGraphWorx");
}


void GraphWorxApplication::terminateGraphWorx()
{
    try
    {
        if (m_graphWorXWindow.graphworx != NULL)
        {
            m_graphWorXWindow.graphworx->HideWindow();
            m_graphWorXWindow.graphworx->StopRuntime();
            m_graphWorXWindow.graphworx->CloseWindow();
            delete m_graphWorXWindow.graphworx;
            m_graphWorXWindow.graphworx = NULL;
        }
    }
    catch ( ... )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Thrown while GraphWorX was attempting to terminate.");
    }
}


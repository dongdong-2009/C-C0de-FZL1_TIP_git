/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/CameraStepParameter.cpp $
  * @author:  Bart Golab/Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "CameraStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStepParameterCameraEditDlg.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "VideoEnquirer.h"
#include "MessageBox.h"

#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

namespace
{
    const unsigned long DEFAULT_KEY = 0;
    const std::string UNKNOWN_NAME = "";
}

/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

CameraStepParameter::CameraStepParameter(PlanStep& parent, std::string name, unsigned long cameraKey ) :
    PlanStepParameter(parent, name),
    m_cameraKey(cameraKey),
    m_cameraName("")
{
    FUNCTION_ENTRY( "CameraStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_cameraName = enquirer.getCameraName( m_cameraKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_cameraName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


CameraStepParameter::CameraStepParameter(PlanStep& parent, const CameraStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_cameraKey(parameter.getCameraKey()),
    m_cameraName("")
{
    FUNCTION_ENTRY( "CameraStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_cameraName = enquirer.getCameraName( m_cameraKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_cameraName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


CameraStepParameter::CameraStepParameter(const CameraStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_cameraKey(parameter.getCameraKey()),
    m_cameraName("")
{
    FUNCTION_ENTRY( "CameraStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_cameraName = enquirer.getCameraName( m_cameraKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_cameraName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


CameraStepParameter::~CameraStepParameter()
{
    FUNCTION_ENTRY( "~CameraStepParameter" );
    FUNCTION_EXIT;
}


void CameraStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), m_cameraName.c_str(),this);

    FUNCTION_EXIT;
}


void CameraStepParameter::setCameraKey(unsigned long cameraKey)
{
    FUNCTION_ENTRY( "setCameraKey" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_cameraKey = cameraKey;

    FUNCTION_EXIT;
}


void CameraStepParameter::setCameraName(const std::string& cameraName)
{
    FUNCTION_ENTRY( "setCameraName" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_cameraName = cameraName;

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* CameraStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterCameraEditDlg(*this,canEdit);
}


bool CameraStepParameter::isAnyCameraAvailable ()
{
    FUNCTION_ENTRY( "isAnyCameraAvailable" );

    TA_Base_Bus::VideoSwitchAgentFactory::CameraVector cameras;
    try
    {
        cameras = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getCameras();

        FUNCTION_EXIT;
        return true;
    }
    catch( const TA_Base_Core::ObjectResolutionException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", e.what() );

        CString actionName, agent;
      actionName.LoadString(IDS_LOAD_CAMERAS);
      agent.LoadString(IDS_VIDEO_SWITCH_AGENT);
        TA_Base_Bus::TransActiveMessage userMsg;
      userMsg << actionName << agent;
      UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);
    }
    catch( const TA_Base_Bus::VideoSwitchAgentException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
        TA_Base_Bus::TransActiveMessage userMsg;
        CString actionName, agent;
        actionName.LoadString(IDS_LOAD_CAMERAS);
        agent.LoadString(IDS_VIDEO_SWITCH_AGENT);
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);
    }
    catch( const CORBA::Exception& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
        MessageBox::error(IDS_LOAD_CAMERAS);
    }
    catch( const TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        MessageBox::error(IDS_LOAD_CAMERAS);
    }
    catch( ... )
    {
      LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception" );
        MessageBox::error(IDS_LOAD_CAMERAS);
    }

    FUNCTION_EXIT;
    return false;

}


#endif // defined(STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined(STEPTYPE_ACTIVATE_CAMERA_PRESET)

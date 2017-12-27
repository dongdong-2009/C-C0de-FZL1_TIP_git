/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/CameraPresetStepParameter.cpp $
  * @author:  Bart Golab/Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * The preset information for the Activate Camera Preset step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "CameraPresetStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "VideoEnquirer.h"
#include "MessageBox.h"

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

CameraPresetStepParameter::CameraPresetStepParameter(PlanStep& parent,std::string name,
                                                     unsigned long cameraEntityKey,
                                                     unsigned long presetKey ) :
    PlanStepParameter(parent,name),
    m_cameraKey(cameraEntityKey),
    m_presetKey(presetKey),
    m_presetName("")
{
    FUNCTION_ENTRY( "CameraPresetStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_presetName = enquirer.getPresetName( m_cameraKey, m_presetKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_presetName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


CameraPresetStepParameter::CameraPresetStepParameter(PlanStep& parent, const CameraPresetStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_cameraKey(parameter.getCameraKey()),
    m_presetKey(parameter.getPresetKey()),
    m_presetName("")
{
    FUNCTION_ENTRY( "CameraPresetStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_presetName = enquirer.getPresetName( m_cameraKey, m_presetKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_presetName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


CameraPresetStepParameter::CameraPresetStepParameter(const CameraPresetStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_cameraKey(parameter.getCameraKey()),
    m_presetKey(parameter.getPresetKey()),
    m_presetName("")
{
    FUNCTION_ENTRY( "CameraPresetStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_presetName = enquirer.getPresetName( m_cameraKey, m_presetKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_presetName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


CameraPresetStepParameter::~CameraPresetStepParameter()
{
    FUNCTION_ENTRY( "~CameraPresetStepParameter" );

    // Do nothing.

    FUNCTION_EXIT;
}


void CameraPresetStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), m_presetName.c_str(), this);

    FUNCTION_EXIT;
}


void CameraPresetStepParameter::setCameraKey(unsigned long cameraKey)
{
    FUNCTION_ENTRY( "setCameraKey" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_cameraKey = cameraKey;

    FUNCTION_EXIT;
}


void CameraPresetStepParameter::setPresetKey(unsigned long presetKey)
{
    FUNCTION_ENTRY( "setPresetKey" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_presetKey = presetKey;

    FUNCTION_EXIT;
}


void CameraPresetStepParameter::setPresetName(const std::string& presetName)
{
    FUNCTION_ENTRY( "setPresetName" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_presetName = presetName;

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* CameraPresetStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this,canEdit);
}


#endif // defined(STEPTYPE_ACTIVATE_CAMERA_PRESET)

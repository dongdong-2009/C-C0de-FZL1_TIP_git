/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PAMessageStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents the PaMessage parameter of a Station PA step.
  *
  **/

#if !defined(AFX_PAMESSAGESTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)
#define AFX_PAMESSAGESTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PaMessageStepParameter

    class PaMessageStepParameter : public PlanStepParameter
    {
         // Operations
    public:
         PaMessageStepParameter(PlanStep& parent, const std::string& name, bool bPATrainMsg = false);
         PaMessageStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::PAMessage& destination, bool bPATrainMsg = false);
         PaMessageStepParameter(PlanStep& parent, const PaMessageStepParameter& parameter, bool bPATrainMsg = false);
         PaMessageStepParameter(const PaMessageStepParameter& parameter);
         virtual ~PaMessageStepParameter();
         virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

         TA_Base_Core::PAMessage getPAMessage () const { return m_paMessage; }
         void setPaMessageId(const unsigned long& messageId);
         void setPAMessageLibVersion (const unsigned short& libVersion);
         void setPAMessageLibSection (const unsigned short& libSection);
         unsigned long  getPAMessageId () { return m_paMessage.messageId; }
         unsigned short getPAMessageLibVersion () { return m_paMessage.libVersion; }
         unsigned short getPAMessageLibSection () { return m_paMessage.libSection; }
         void setPAMessageType (bool bTrain);
     protected:
         virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
         PaMessageStepParameter& operator=(const PaMessageStepParameter& parameter);

         // Attributes
    private:
         TA_Base_Core::PAMessage m_paMessage;
         bool m_bTrain;//whether this is a Station or Train PA message
    };
}

#endif // defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#endif // !defined(AFX_PAMESSAGESTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)

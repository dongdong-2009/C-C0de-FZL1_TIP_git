/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TisMessageStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents the Message parameter of a Station/Train Traveller Info System step.
  *
  **/

#if !defined(AFX_TISMESSAGEPARAMETER_H__4F9F0E3D_F61D_464C_8EE4_2B3EB5CAB10F__INCLUDED_)
#define AFX_TISMESSAGEPARAMETER_H__4F9F0E3D_F61D_464C_8EE4_2B3EB5CAB10F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"
#include "TisConfigAccess.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TisMessageStepParameter

    class TisMessageStepParameter : public PlanStepParameter
    {
    // Types
    public:
        typedef enum
                {
                    MT_STATION,
                    MT_TRAIN,
                    MT_UNDEFINED
                }
                EMessageType;

    // Operations
    public:
        TisMessageStepParameter(PlanStep& parent, const std::string& name, EMessageType messageType);
        TisMessageStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::TISMessage& message, EMessageType messageType);
        TisMessageStepParameter(PlanStep& parent, const TisMessageStepParameter& parameter);
        TisMessageStepParameter(const TisMessageStepParameter& parameter);
        virtual ~TisMessageStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);
        virtual bool editParameter(unsigned short libraryVersion, TisConfigAccess::PredefinedTisMessage& selectedMessage);

        TA_Base_Core::TISMessage getMessage() const  { return m_message; }
        EMessageType getMessageType() const          { return m_messageType; }
        std::string getMessageText() const           { return m_messageText; }

        void setMessage(const TA_Base_Core::TISMessage& message);
        void resetMessage();

    protected:
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        TisMessageStepParameter& operator=(const TisMessageStepParameter& parameter);

    // Attributes
    private:
        TA_Base_Core::TISMessage m_message;
        EMessageType m_messageType;
        std::string m_messageText;
    };
}

#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

#endif // !defined(AFX_TISMESSAGEPARAMETER_H__4F9F0E3D_F61D_464C_8EE4_2B3EB5CAB10F__INCLUDED_)

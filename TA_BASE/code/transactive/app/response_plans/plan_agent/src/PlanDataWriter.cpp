/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/WXL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanDataWriter.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #11 $
  *
  *  Last modification: $DateTime: 2014/04/04 10:54:00 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */
#include <iomanip>

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/exceptions/src/PlanStepDetailException.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataWriter.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/common/src/ProjectDefinition.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/BooleanContextImpl.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanDataWriter.h"

namespace TA_Base_App
{
    const unsigned long PLANEDITLOCK_TIMEOUT_SECONDS = 60;
    const char* const PLAN_EDIT_LOCK_SEPARATOR = ":";

    PlanDataWritingClass::PlanDataWritingClass()
    {
        UpdatePlansDBSequence();

        TA_Base_Bus::PlanDataWriterImp::CopyNameSuffix = " - 副本";
        TA_Base_Bus::PlanDataWriterImp::NewPlanName = "新预案";
        TA_Base_Bus::PlanDataWriterImp::NewCategoryName = "新目录";
    }


    void PlanDataWritingClass::UpdatePlansDBSequence()
    {
        TA_Base_Bus::PlanDataWriter::instance()->UpdatePlansDBSequence();
    }


	void PlanDataWritingClass::getEditPlanLock( const std::string& session, const std::string& planManagerId, TA_Base_Core::NodeId plan, bool noDBAccess /*=false*/ )
    {
		if ( !TA_Base_Bus::PlanDataReader::instance()->existsNode(plan) )
		{
            TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
		}

        std::string requestingLockOwner = session;
        requestingLockOwner += PLAN_EDIT_LOCK_SEPARATOR;
        requestingLockOwner += planManagerId;

        // TD8505: Merged fix from 4619.
        if (!noDBAccess)
        {
            std::string exisingLockOwner = setEditLockInDataBase(requestingLockOwner, plan);

            if ( exisingLockOwner.compare(requestingLockOwner) != 0 )
            {
                std::string owner = ActivePlanBulletinBoard::instance()->getOwner(exisingLockOwner);
            
                std::ostringstream what;
                what << "The plan is already being edited by the operator: " << owner;

                TA_THROW(TA_Base_Core::PlanAgentException(what.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(exisingLockOwner)));
            }
        }

        //Send StateUpdate for Plan being edited
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
        TA_Base_Core::EditLockUpdate_var elu = new TA_Base_Core::EditLockUpdate;

        elu->session = CORBA::string_dup(session.c_str());
		elu->pmId = CORBA::string_dup(planManagerId.c_str());
        elu->plan = plan;

        pasUpdateData->update.getLockUpdate(elu);

        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
    }


	void PlanDataWritingClass::releaseEditPlanLock(const std::string& session,TA_Base_Core::NodeId plan, bool noDBAccess /*=false*/ )
    {
		if ( !TA_Base_Bus::PlanDataReader::instance()->existsNode(plan) )
		{
            TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
		}

        // TD8505: Merged fix from 4619.
        if (!noDBAccess)
        {
            releaseEditLockInDatabase(session, plan );
        }

        //Send StateUpdate for release Plan being edited
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
        TA_Base_Core::EditLockUpdate_var elu = new TA_Base_Core::EditLockUpdate;

        elu->session = CORBA::string_dup(session.c_str());
		elu->pmId = CORBA::string_dup("no need pmid.");
        elu->plan = plan;

        pasUpdateData->update.releaseLockUpdate(elu);

        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
    }

    std::string PlanDataWritingClass::getEditLockOwner(TA_Base_Core::NodeId plan)
    {
        return TA_Base_Bus::PlanDataWriter::instance()->getEditLockOwner(plan);
    }

	bool PlanDataWritingClass::isPlanBeingEdited(TA_Base_Core::NodeId plan)
	{
        try
        {
            return TA_Base_Bus::PlanDataWriter::instance()->isPlanBeingEdited(plan);
        }
        catch(TA_Base_Core::DataException& e)
        {
            if (e.getFailType() == TA_Base_Core::DataException::NO_VALUE)
            {
                TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
            }
            TA_THROW(e);
        }
	}


    std::string PlanDataWritingClass::setEditLockInDataBase (const std::string& session, TA_Base_Core::NodeId plan )
    {
        try
        {
            return TA_Base_Bus::PlanDataWriter::instance()->setEditLockInDataBase(session, plan);
        }
        catch(TA_Base_Core::DataException& e)
        {
            if (e.getFailType() == TA_Base_Core::DataException::NO_VALUE)
            {
                TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
            }
            TA_THROW(e);
        }
    }


    void PlanDataWritingClass::releaseEditLockInDatabase(const std::string& session, TA_Base_Core::NodeId plan )
    {
        try
        {
            TA_Base_Bus::PlanDataWriter::instance()->releaseEditLockInDatabase(session, plan);
        }
        catch(TA_Base_Core::DataException& e)
        {
            if (e.getFailType() == TA_Base_Core::DataException::NO_VALUE)
            {
                std::ostringstream message;
                message << "The plan " << plan << "does not exist.";
                TA_THROW(TA_Base_App::PlanNotFoundException(message.str(), TA_Base_Core::PNFE_NODE_NOT_EXIST));
            }
            TA_THROW(e);
        }
        catch(TA_Base_Core::PlanAgentException& e)
        {
            if (e.getErrorType() == TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR)
            {
                std::string lockOwner = e.what();
                std::ostringstream message;
                message << "Error: Cannot release the edit lock owned by " << e.what() << ".";
                TA_THROW(TA_Base_Core::PlanAgentException(lockOwner, TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(lockOwner)));
            }
            TA_THROW(e);
        }
        
    }


    bool PlanDataWritingClass::refreshEditLockInDatabase(const std::string& session, TA_Base_Core::NodeId plan)
    {
        return true;
    }


	void PlanDataWritingClass::CheckPlanEditSession(TA_Base_Core::NodeId plan, const std::string& session)
	{
        std::string lockOwner;
        try
        {
            lockOwner = TA_Base_Bus::PlanDataWriter::instance()->getPlanEditSession(plan, session);
        }
        catch(TA_Base_Core::DataException& e)
        {
            if (e.getFailType() == TA_Base_Core::DataException::NO_VALUE)
            {
                TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
            }
            TA_THROW(e);
        }

        if (lockOwner.find(session) == 0 ) return;

        std::string owner = ActivePlanBulletinBoard::instance()->getOwner(lockOwner);

        std::ostringstream what;
        what << "The plan is being edited by another operator: " << owner;

        TA_THROW(TA_Base_Core::PlanAgentException(what.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(lockOwner)));
	}



	void PlanDataWritingClass::validatePlan(const TA_Base_Core::Plan& plan)
	{
        std::ostringstream message;
        TA_Base_Core::NodeId nodeId = plan.detail.plan;
        TA_Base_Core::NodeId parent = plan.detail.parent;

        const TA_Base_Core::PlanDetail* planDetail = &(plan.detail);
        TA_Base_Core::ENode parentNodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(planDetail->parent);

        if ( parentNodeType != TA_Base_Core::CATEGORY_NODE )
        {			
            message << "Error: The parent node is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_PARENT_NOT_CATEGORY));
        }

        std::string name = planDetail->name.in();
        std::string description = planDetail->description.in();
        TA_Base_Core::EApprovalState approvalState = planDetail->approvalState;

        if ( name.empty() )
        {
            TA_THROW(TA_Base_Core::PlanAgentException("The name of Plan cannot be blank.", TA_Base_Core::PAE_NO_PLAN_NAME));
        }

        //see if file has any file-path disallowed characters ( \ / : * ? " < > | )
        if (name.find ("\\") != std::string::npos ||
            name.find ("/")  != std::string::npos ||
            name.find (":")  != std::string::npos ||
            name.find ("*")  != std::string::npos ||
            name.find ("?")  != std::string::npos ||
            name.find ("\"") != std::string::npos ||
            name.find ("<")  != std::string::npos ||
            name.find (">")  != std::string::npos ||
            name.find ("|")  != std::string::npos )
        {
            TA_THROW(TA_Base_Core::PlanAgentException("The name of Plan cannot contain any of the reserved characters - \\ / : * ? \" < > |", TA_Base_Core::PAE_INVALID_PLAN_NAME));
        }


        if ( !TA_Base_Bus::PlanDataReader::instance()->canUseName(nodeId, parent, parseForApostrophe(name) ) )
        {
            message.str("");
            message << "Error: The name " << name.c_str() << " is already used by another Plan/Category.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NAME_NOT_UNIQUE));
        }

        if ( TA_Base_Core::UNAPPROVED_APPROVAL_STATE != approvalState )
        {
            TA_THROW(TA_Base_Core::PlanAgentException("Error: Plan can only be saved in an Unapproved state.", TA_Base_Core::PAE_CANNOT_SAVE_APPROVED_PLAN));
        }

        int numberOfSteps = plan.stepseq.length();
        const TA_Base_Core::StepDetail* stepDetail = NULL;
        const TA_Base_Core::StepParameters* stepParameters = NULL;

        for (int i = 0; i < numberOfSteps; i++)
        {
            stepDetail = &(plan.stepseq[i].detail);
            stepParameters = &(plan.stepseq[i].parameters);

            if ( stepDetail->type != stepParameters->_d() )
            {
                message.str("");
                message << "Error: Type mismatch between Step Type and its parameters for step position: "
                    << i+1;

                TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_STEP_TYPE_AND_PARAMETER_MISMATCH, i+1));				
            }

            std::string stepName = stepDetail->name.in();
            std::string stepDescription = stepDetail->description.in();
            unsigned long stepPosition = stepDetail->position;
            TA_Base_Core::StepId step = stepDetail->step;
            TA_Base_Core::EStep type = stepDetail->type;
            unsigned long delay = stepDetail->delay;

            if ( stepName.empty() )
            {
                TA_THROW(TA_Base_Core::PlanStepDetailException("Step name cannot be blank", TA_Base_Core::PSDT_NAME, TA_Base_Core::PSDE_UNSPECIFIED, i+1 ));
            }

            if ( stepPosition != i+1 )
            {
                message << "Error: The position parameter in step detail of step " << i+1 << " is " 
                    << stepPosition;
                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_POSITION, TA_Base_Core::PSDE_INVALID, i+1 ));
            }

			if ( type < TA_Base_Core::END_STEP || type >= TA_Base_Core::UNDEFINED_STEP)
			{
				message << "Error: Invalid step type (" << type << ") in step detail of step " << i+1;
                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TYPE, TA_Base_Core::PSDE_INVALID, i+1 ));
			}
		}

        const TA_Base_Core::DataPointValue* dpValue = NULL;

        for (int m = 0; m < numberOfSteps; m++)
        {
            stepParameters = &(plan.stepseq[m].parameters);

            switch(stepParameters->_d())
            {
            case TA_Base_Core::END_STEP:
                break;

            case TA_Base_Core::ABORT_STEP:
                break;

            case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
                {
                    const TA_Base_Core::ActivePlanCheckParameters& refParams = stepParameters->activePlanCheck();

                    if (!(refParams.plan) || !TA_Base_Bus::PlanDataReader::instance()->existsNode(refParams.plan))
                    {
                        message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " No plan has been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (TA_Base_Core::PLAN_NODE != TA_Base_Bus::PlanDataReader::instance()->getNodeType(refParams.plan))
                    {
                        message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " No plan has been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (getStepNumber(plan, refParams.yesStep) == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'Yes Step' : " << " The step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_YES, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (getStepNumber(plan, refParams.noStep) == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'No Step' : " << " The step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_NO, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
                {
                    const TA_Base_Core::AssignCameraToMonitorParameters& refParams = stepParameters->cameraMonitor();
                    if ( refParams.camera == 0 )
                    {
                        message << "Step " << m+1 << " - Parameter 'Camera Name' : " << " Camera name has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_CAMERA_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( refParams.monitor == 0 )
                    {
                        message << "Step " << m+1 << " - Parameter 'Monitor Name' : " << " Monitor name has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_MONITOR_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::DATA_POINT_CHECK_STEP:
                {
                    const TA_Base_Core::DataPointCheckParameters& refParams = stepParameters->dataPointCheck();
                    std::string dpName = refParams.dataPoint.in();

                    if (dpName.empty())
                    {
                        message << "Step " << m+1 << " - Parameter 'Data Point Name' : " << " Data Point name has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    dpValue = &(refParams.value);

                    switch ( dpValue->_d() )
                    {
                    case TA_Base_Core:: BOOLEAN_DATA_POINT:
                        {
                            int boolValue = dpValue->booleanValue();
                        }
                        break;

                    case TA_Base_Core::NUMERIC_DATA_POINT:
                        //Nothing to validate
                        break;

                    case TA_Base_Core::TEXT_DATA_POINT:
                        {
                            std::string text = dpValue->textValue();
                            if (text.empty())
                            {
                                message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Text value of the data point has not been specified";
                                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
                            }
                        }
                        break;

                    case TA_Base_Core::UNDEFINED_DATA_POINT:
                        {
                            message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Data Point value has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
                        }

                    default:
                        {
                            message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Data Point value has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
                        }
                    }

					switch (refParams.testType)
                    {
                    case TA_Base_Core::EQUAL_TO_TEST:
                    case TA_Base_Core::NOT_EQUAL_TO_TEST :
                    case TA_Base_Core::LESS_THAN_TEST:
                    case TA_Base_Core::GREATER_THAN_TEST:
                    case TA_Base_Core::LESS_THAN_OR_EQUAL_TO_TEST:
                    case TA_Base_Core::GREATER_THAN_OR_EQUAL_TO_TEST:
                        break;

                    case TA_Base_Core::UNDEFINED_TEST:
                        message << "Step " << m+1 << " - Parameter 'Data Point Test Type' : " << " Data Point Test Type has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_TEST_TYPE, TA_Base_Core::PSDE_INVALID, m+1 ));

                    default:
                        message << "Step " << m+1 << " - Parameter 'Data Point Test Type' : " << " Data Point Test Type has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_TEST_TYPE, TA_Base_Core::PSDE_INVALID, m+1 ));
                    }

                    if (getStepNumber(plan, refParams.successStep) == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'Success Step' : " << " The success step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SUCCESS, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (getStepNumber(plan, refParams.failureStep) == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'Failure Step' : " << " The Failure step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_FAILURE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }									
                }
                break;

			case TA_Base_Core::DATA_POINT_SET_STEP:
                {
                    const TA_Base_Core::DataPointSetParameters& refParams = stepParameters->dataPointSet();
					std::string dpName = refParams.dataPoint.in();
					if (dpName.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Data Point Name' : " << " Data Point Name has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1) );
					}

					dpValue = &(refParams.newValue);

					switch(dpValue->_d())
					{
					case TA_Base_Core::BOOLEAN_DATA_POINT:
						{
							int boolValue = dpValue->booleanValue();
						}
						break;

					case TA_Base_Core::NUMERIC_DATA_POINT:						
						//Nothing to validate
						break;

					case TA_Base_Core::TEXT_DATA_POINT:
						{
							std::string text = dpValue->textValue();

							if (text.empty())
							{
						        message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Text value of the data point has not been specified";
                                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
							}
						}
						break;

                    case TA_Base_Core::UNDEFINED_DATA_POINT:
                        {
						    message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Data Point value has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
                        }

                    default:
                        {
						    message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Data Point value has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
                        }
					}
				}
				break;

			case TA_Base_Core::DECISION_STEP:
                {
                    const TA_Base_Core::DecisionParameters& refParams = stepParameters->decision();
					std::string decisionMessage = refParams.decisionMessage.in();

					if (decisionMessage.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Decision Message' : " << " The message has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DECISION_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (getStepNumber(plan, refParams.yesStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'Yes Step' : " << " The Yes step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_YES, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (getStepNumber(plan, refParams.noStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'No Step' : " << " The No step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_NO, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}									

					std::string yesButtonLabel = refParams.yesButtonLabel.in();
					if (yesButtonLabel.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Yes Button Label' : " << " The label has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_YES_LABEL, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}	
					
					std::string noButtonLabel = refParams.noButtonLabel.in();
					if (noButtonLabel.empty())
					{
						message << "Step " << m+1 << " - Parameter 'No Button Label' : " << " The label has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_NO_LABEL, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}	
					
					std::string pauseButtonLabel = refParams.pauseButtonLabel.in();
					if (pauseButtonLabel.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Pause Button Label' : " << " The label has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PAUSE_LABEL, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}
				}
				break;

			case TA_Base_Core::JUMP_STEP:
                {
                    const TA_Base_Core::JumpParameters& refParams = stepParameters->jump();

					if (getStepNumber(plan, refParams.step) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'Jump Step' : " << " The Jump step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_JUMP, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}
				}
				break;

			case TA_Base_Core::LOG_STEP:
                {
                    const TA_Base_Core::LogParameters& refParams = stepParameters->log();
					std::string logMessage = refParams.message.in();

					if (logMessage.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Prompt Message' : " << " The message has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PROMPT_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}
				}
				break;

            case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
                {
                    const TA_Base_Core::ActivateCameraPresetParameters& refParams = stepParameters->cameraPreset();

                    if ( refParams.camera == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'Camera Name' : " << " Camera name has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_CAMERA_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( refParams.preset == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'Camera Preset' : " << " Camera preset has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_CAMERA_PRESET, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

			case TA_Base_Core::PROMPT_STEP:
                {
                    const TA_Base_Core::PromptParameters& refParams = stepParameters->prompt();
					std::string promptMessage = refParams.message.in();
					if (promptMessage.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Prompt Message' : " << " The message has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PROMPT_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}
				}
				break;

			case TA_Base_Core::RUN_PLAN_STEP:
                {
                    const TA_Base_Core::RunPlanParameters& refParams = stepParameters->runPlan();

					if (!(refParams.plan) || !TA_Base_Bus::PlanDataReader::instance()->existsNode(refParams.plan))
					{
						message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " No plan has been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (TA_Base_Core::PLAN_NODE != TA_Base_Bus::PlanDataReader::instance()->getNodeType(refParams.plan))
					{
						message << "Error: Step position " << m+1 << " Invalid node type for first parameter of Active Plan Check.";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_INVALID, m+1 ));
					}
				}
				break;

			case TA_Base_Core::TERMINATE_PLAN_STEP:
                {
                    const TA_Base_Core::TerminatePlanParameters& refParams = stepParameters->terminatePlan();

					if (!(refParams.plan) || !TA_Base_Bus::PlanDataReader::instance()->existsNode(refParams.plan))
					{
						message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " No plan has been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (TA_Base_Core::PLAN_NODE != TA_Base_Bus::PlanDataReader::instance()->getNodeType(refParams.plan))
					{
						message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " Specified plan is not a valid plan";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_INVALID, m+1 ));
					}
				}
				break;
			
			case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
                {
                    const TA_Base_Core::EvaluateExpressionParameters& refParams = stepParameters->evaluateExpression();
					std::string eExpression = refParams.expression.in();

					if (eExpression.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Expression' : " << " Expression to evaluate has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_EXPRESSION_VALUE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}
					else //check validity of expression 
					{
						try
						{
							//use ExpressionEvaluator class to see if the expression can be parsed
							std::auto_ptr <TA_Base_Bus::ExpressionEvaluator> expEval(new TA_Base_Bus::ExpressionEvaluator);
							BooleanContextImpl* context = new BooleanContextImpl ();
							expEval->setNewExpression(eExpression);
							expEval->addBooleanContext (context);
							expEval->evaluateBooleanExpression ();	
						}
						catch (const TA_Base_Core::MathematicalEvaluationException)
						{
							message << "Step " << m+1 << " - Parameter 'Expression Value' : " << " The expression cannot be evaluated !";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_EXPRESSION_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
						}
					}

					if (getStepNumber(plan, refParams.successStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'Success Step' : " << " The success step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SUCCESS, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (getStepNumber(plan, refParams.failureStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'Failure Step' : " << " The Failure step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_FAILURE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}									
				}
				break;

			case TA_Base_Core::VMS_SET_SCENE_STEP:
                {
                    const TA_Base_Core::VmsSetSceneParameters& refParams = stepParameters->vmsSetScene();
                    if ( refParams.vmsSign == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'VMS Sign' : " << " VMS Sign has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_VMS_SIGN, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( refParams.scene == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'VMS Scene' : " << " VMS Scene has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_VMS_SCENE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
				break;

            case TA_Base_Core::LAUNCH_GUI_APP_STEP:
                {
                    const TA_Base_Core::LaunchGuiAppParameters& refParams = stepParameters->guiAppParameters();

                    if (refParams.app == 0)
                    {
				        message << "Step " << m+1 << " - Parameter 'Application' : " << " This has not been specified or is invalid";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_APPLICATION, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( refParams.pos.width == 0 )
                    {
				        message << "Step " << m+1 << " - Parameters 'Width' : " << " Width should be greater than 0";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_WIDTH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( refParams.pos.height == 0 )
                    {
				        message << "Step " << m+1 << " - Parameters 'Height' : " << " Height should be greater than 0";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_HEIGHT, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::VMS_BLANKOUT_STEP:
                {
                    const TA_Base_Core::VmsBlankoutParameters& refParams = stepParameters->vmsBlankout();

                    if ( refParams.vmsSign == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'VMS Sign' : " << " VMS Sign has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_VMS_SIGN, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
                {
                    const TA_Base_Core::VmsRestoreScheduleParameters& refParams = stepParameters->vmsRestoreSchedule();

                    if ( refParams.vmsSign == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'VMS Sign' : " << " VMS Sign has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_VMS_SIGN, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::VIDEO_WALL_STEP:
                {
                    const TA_Base_Core::VideoWallParameters& refParams = stepParameters->videoWallParams();

                    if ( refParams.wallBoard == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'Wall Board' : " << " Wall Board has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_WALLBOARD, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( refParams.layout == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'Video Layout' : " << " Video Layout has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_VIDEO_LAYOUT, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
                {
                    const TA_Base_Core::SendSmsMessageParameters& refParams = stepParameters->sendSmsMessage();
                    std::string strIdentifier = refParams.recipientIdentifier.in();

                    if ( strIdentifier.empty() )
                    {
                        message << "Step " << m+1 << " - Parameter 'SMS Recipient' : SMS Recipient has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SMS_RECIPIENT, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    std::string strTextMsg = refParams.textMessage.in();
                    if ( strTextMsg.empty() )
                    {
                        message << "Step " << m+1 << " - Parameter 'SMS Message' : SMS Message has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SMS_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
                {
                    const TA_Base_Core::LaunchSchematicParameters& refParams = stepParameters->launchSchematic();
                    std::string strFile = refParams.schematic.in();

                    if ( strFile.empty() )
                    {
                        message << "Step " << m+1 << " - Parameter 'Schematic name' : Schematic has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SCHEMATIC_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( refParams.screen == 0 )
                    {
                        message << "Step " << m+1 << " - Parameter 'Display' : Specified screen number is not valid";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SCREEN_NUMBER, TA_Base_Core::PSDE_INVALID, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
                {
                    TA_Base_Core::StationTravellerInfoSystemParameters stationTisParams = stepParameters->stationTravellerInfoSystem();
					
					if(stationTisParams.pidList.length() == 0)
					{
						message << "Step " << m + 1 << " - destinationList '' : Not select zones";
						TA_THROW( TA_Base_Core::PlanStepDetailException( message.str(), TA_Base_Core::PSDT_STATION_DESITINATIONLIST, TA_Base_Core::PSDE_UNSPECIFIED, m + 1 ) );
					}
					

                    if(stationTisParams.messageContent.length() == 0)
                    {
                        message << "Step " << m + 1 << " - Parameter 'messageContent' : Message content is empty";
                        TA_THROW( TA_Base_Core::PlanStepDetailException( message.str(), TA_Base_Core::PSDT_STATION_MESSAGECONTENT, TA_Base_Core::PSDE_UNSPECIFIED, m + 1 ) );
                    }
                }
                break;

            case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
                {
                    const TA_Base_Core::TrainTravellerInfoSystemParameters& refParams = stepParameters->trainTravellerInfoSystem();

                    if (refParams.destination.trainId == TA_Base_Core::TIS_UNDEFINED_TRAIN_ID && 
                        refParams.destination.trainGroup == TA_Base_Core::TG_UNDEFINED)
                    {
                        message << "Step " << m+1 << " - Parameter 'Train ID' : Train ID has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TRAIN_ID, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (refParams.message.messageId == TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID)
                    {
                        message << "Step " << m+1 << " - Parameter 'Message' : Message has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TIS_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (refParams.priority == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'Priority' : Message priority has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TIS_PRIORITY, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (refParams.duration == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'Duration' : Message duration has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TIS_DURATION, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

				case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
                    {
                        const TA_Base_Core::StationPublicAddressParameters& refParams = stepParameters->stationPublicAddress();
						if (refParams.message.messageId == TA_Base_Core::PA_UNDEFINED_MESSAGE_ID)
                        {
                            message << "Step " << m+1 << " - Parameter 'Message' : Message has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_STATION_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }
						if (refParams.zones.length () == 0)
						{
							message << "Step " << m+1 << " - Parameter 'Zone' : Zone has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_STATION_ZONE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
						}

                        std::string strStation = refParams.station.station.in();
						if (strStation.empty() && !refParams.station.allStations)
						{
							message << "Step " << m+1 << " - Parameter 'Station' : Station name has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_STATION_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
						}
					}
					break;

				case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
                    {
                        const TA_Base_Core::TrainPublicAddressParameters& refParams = stepParameters->trainPublicAddress();
						if (refParams.message.messageId == TA_Base_Core::PA_UNDEFINED_MESSAGE_ID)
                        {
                            message << "Step " << m+1 << " - Parameter 'Message' : Message has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_STATION_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }
						if (refParams.destination.trainId == TA_Base_Core::TIS_UNDEFINED_TRAIN_ID && 
							refParams.destination.trainGroup == TA_Base_Core::TG_UNDEFINED)
						{
							message << "Step " << m+1 << " - Parameter 'Train ID' : Train ID has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TRAIN_ID, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
						}
						if (refParams.duration == 0)
						{
							message << "Step " << m+1 << " - Parameter 'Duration' : Message duration has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TIS_DURATION, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
						}
					}
					break;

				// Failure Step Summary
				case TA_Base_Core::FAILURE_SUMMARY_STEP:
                    {
                        const TA_Base_Core::FailureSummaryParameters& refParams = stepParameters->failureSummary();
						if ((refParams.failureCategory != TA_Base_Core::SFC_ALL_STEPS) && 
							(refParams.failureCategory != TA_Base_Core::SFC_SET_DATA_POINT_STEPS))
						{
							message << "Step " << m+1 << " - Parameter 'Failure Category' : Failure Category  has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_FAILURE_SUMMARY, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
						}
					}
					break;
				
				//WuZhongyi New Step Fire Alarm Station PA
				case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP_BY_ALARM:
					{
						break;
					}
					break;
					
                case TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP:
                    {
                        const TA_Base_Core::BackgroundLaunchSchematicParameters& refParams = stepParameters->bgLaunchSchematic();
                        std::string strSchematic = refParams.schematic.in();

                        if ( strSchematic.empty() )
                        {
                            message << "Step " << m+1 << " - Parameter 'Schematic name' : Schematic has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SCHEMATIC_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }

                        if ( refParams.screen == 0 )
                        {
                            message << "Step " << m+1 << " - Parameter 'Display' : Specified screen number is not valid";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SCREEN_NUMBER, TA_Base_Core::PSDE_INVALID, m+1 ));
                        }

                        if (TA_Base_Core::DatabaseKey::getInvalidKey() == refParams.profile)
                        {
                            message << "Step " << m+1 << " - Parameter 'Profile' : Profile number is not valid";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PROFILE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }
                    }
                    break;

                case TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP:
                    {
                        const TA_Base_Core::BackgroundLaunchGuiAppParameters& refParams = stepParameters->bgLaunchGuiApp();

                        if (refParams.app == 0)
                        {
                            message << "Step " << m+1 << " - Parameter 'Application' : " << " This has not been specified or is invalid";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_APPLICATION, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }

                        if ( refParams.pos.width == 0 )
                        {
                            message << "Step " << m+1 << " - Parameters 'Width' : " << " Width should be greater than 0";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_WIDTH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }

                        if ( refParams.pos.height == 0 )
                        {
                            message << "Step " << m+1 << " - Parameters 'Height' : " << " Height should be greater than 0";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_HEIGHT, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }
                        
                        if (TA_Base_Core::DatabaseKey::getInvalidKey() == refParams.profile)
                        {
                            message << "Step " << m+1 << " - Parameter 'Profile' : Profile number is not valid";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PROFILE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }

                    }
                    break;

                case TA_Base_Core::BACKGROUND_DECISION_STEP:
                    {
                        const TA_Base_Core::BackgroundDecisionParameters& refParams = stepParameters->bgDecision();
                        std::string decisionMessage = refParams.decisionMessage.in();

                        if (decisionMessage.empty())
                        {
                            message << "Step " << m+1 << " - Parameter 'Decision Message' : " << " The message has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DECISION_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }

                        if (getStepNumber(plan, refParams.yesStep) == 0)
                        {
                            message << "Step " << m+1 << " - Parameter 'Yes Step' : " << " The Yes step has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_YES, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }

                        if (getStepNumber(plan, refParams.noStep) == 0)
                        {
                            message << "Step " << m+1 << " - Parameter 'No Step' : " << " The No step has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_NO, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }									

                        std::string yesButtonLabel = refParams.yesButtonLabel.in();
                        if (yesButtonLabel.empty())
                        {
                            message << "Step " << m+1 << " - Parameter 'Yes Button Label' : " << " The label has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_YES_LABEL, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }	

                        std::string noButtonLabel = refParams.noButtonLabel.in();
                        if (noButtonLabel.empty())
                        {
                            message << "Step " << m+1 << " - Parameter 'No Button Label' : " << " The label has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_NO_LABEL, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }	

                        std::string pauseButtonLabel = refParams.pauseButtonLabel.in();
                        if (pauseButtonLabel.empty())
                        {
                            message << "Step " << m+1 << " - Parameter 'Pause Button Label' : " << " The label has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PAUSE_LABEL, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }

                        if (TA_Base_Core::DatabaseKey::getInvalidKey() == refParams.profile)
                        {
                            message << "Step " << m+1 << " - Parameter 'Profile' : Profile number is not valid";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PROFILE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }
                    }
                    break;

                case TA_Base_Core::BACKGROUND_LOG_STEP:
                    {
                        const TA_Base_Core::BackgroundLogParameters& refParams = stepParameters->bgLog();

                        std::string logMessage = refParams.message.in();

                        if (logMessage.empty())
                        {
                            message << "Step " << m+1 << " - Parameter 'Prompt Message' : " << " The message has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PROMPT_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }

                        if (TA_Base_Core::DatabaseKey::getInvalidKey() == refParams.profile)
                        {
                            message << "Step " << m+1 << " - Parameter 'Profile' : Profile number is not valid";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PROFILE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }
                    }
                    break;

                case TA_Base_Core::BACKGROUND_PROMPT_STEP:
                    {
                        const TA_Base_Core::BackgroundPromptParameters& refParams = stepParameters->bgPrompt();

                        std::string promptMessage = refParams.message.in();
                        if (promptMessage.empty())
                        {
                            message << "Step " << m+1 << " - Parameter 'Prompt Message' : " << " The message has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PROMPT_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }

                        if (TA_Base_Core::DatabaseKey::getInvalidKey() == refParams.profile)
                        {
                            message << "Step " << m+1 << " - Parameter 'Profile' : Profile number is not valid";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PROFILE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                        }
                    }
                    break;

			default:
				break;

			}
		}
	}


	void PlanDataWritingClass::updatePlanDetail(const std::string& session, const TA_Base_Core::PlanDetail& detail)
	{
		std::ostringstream message;
		TA_Base_Core::NodeId nodeId = detail.plan;

		//Check whether the Plan is being edited by the current session user
		CheckPlanEditSession(nodeId, session);

		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(nodeId);

		if ( nodeType != TA_Base_Core::PLAN_NODE)
		{
			message << "Error: The node [" << detail.plan << "] is not of type PLAN."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
		}

        TA_Base_Bus::PlanDataWriter::instance()->updatePlanDetail(session, detail);

		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(nodeId);
		pcud->configUpdate.updatePlan(ns);

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

        std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath( nodeId );
		AAMessageSender::instance()->submitAuditPlanUpdate( session, strPlanName );
	}


	void PlanDataWritingClass::updatePlan(const std::string& session, const TA_Base_Core::Plan& plan)
	{
		std::ostringstream message;

		TA_Base_Core::NodeId nodeId = plan.detail.plan;

		//Check whether the Plan is being edited by the current session user
		CheckPlanEditSession(nodeId, session);

		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(nodeId);

		if ( nodeType != TA_Base_Core::PLAN_NODE)
		{
			message << "Error: The node [" << plan.detail.name << "] is not of type PLAN."; 
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str());
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
		}

		//Make the Plan pass through some sanity checks before updating
		validatePlan(plan);

        TA_Base_Bus::PlanDataWriter::instance()->updatePlan(session, plan);

		int numberOfSteps = plan.stepseq.length();

		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(nodeId);
		pcud->configUpdate.updatePlan(ns);

        std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath( nodeId );

		if ( numberOfSteps == 0 )
		{
			ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

		    AAMessageSender::instance()->submitAuditPlanUpdate( session, strPlanName );
			return;			
		}

        TA_Base_Bus::PlanDataWriter::instance()->deleteAllStepParameters(session, plan);
        TA_Base_Bus::PlanDataWriter::instance()->deleteAllSteps(session, plan);


		// We are here now to insert all the steps into step_details

////-------------

		const TA_Base_Core::DataPointValue* dpValue = NULL;

		for ( int m = 0; m < numberOfSteps ; m++ )
		{	//Add Step into step_details
			const TA_Base_Core::StepDetail* stepDetail = &(plan.stepseq[m].detail);
			const TA_Base_Core::StepParameters* stepParameters = &(plan.stepseq[m].parameters);

			if ( stepDetail->type != stepParameters->_d() )
			{
				//TODO: Throw exception
				//Mismatch between Step Type and its parameters 
				return;
			}

            TA_Base_Bus::PlanDataWriter::instance()->insertStepDetail(plan, *stepDetail);

			std::vector<std::string> valueVector; //To store the modified param values

			if ( stepDetail->type != stepParameters->_d() )
			{
				//TODO: Throw exception
				//Mismatch between Step Type and its parameters 
				return;
			}

            const std::string STEP_NUMBER_PREFIX("#");
            std::ostringstream ostrstrm;

			switch(stepParameters->_d())
			{
			case TA_Base_Core::END_STEP:
				break;

			case TA_Base_Core::ABORT_STEP:
				break;

            #if defined ( STEPTYPE_ACTIVE_PLAN_CHECK )
			case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
            {
                // TD8736: Originally, the plan parameter was stored as a plan ID, and the yes/no step parameter
                //         as a step ID. These have been changed to plan path and step position, respectively.
                //         Plan Manager continues to support the Active Plan Check step in terms of plan ID and
                //         step ID. These parameters need to be converted before being written to database. 
                //         They have already been verified by the time we get here, so the conversion is safe.

                const TA_Base_Core::ActivePlanCheckParameters& refParams = stepParameters->activePlanCheck();
				ostrstrm << TA_Base_Bus::PlanDataReader::instance()->getNodePath(refParams.plan);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, refParams.yesStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, refParams.noStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                break;
            }
            #endif

            #if defined ( STEPTYPE_ASSIGN_CAMERA_TO_MONITOR )
			case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
            {
		        const TA_Base_Core::AssignCameraToMonitorParameters& refParams = stepParameters->cameraMonitor();
                ostrstrm << refParams.camera;
				valueVector.push_back( ostrstrm.str() );
                ostrstrm.str("");
                ostrstrm << refParams.monitor;
				valueVector.push_back( ostrstrm.str() );

                break;
            }
            #endif

            #if defined ( STEPTYPE_DATA_POINT_CHECK )
			case TA_Base_Core::DATA_POINT_CHECK_STEP:
            {
		        const TA_Base_Core::DataPointCheckParameters& refParams = stepParameters->dataPointCheck();
				valueVector.push_back(refParams.dataPoint.in());
				dpValue = &(refParams.value);

				switch ( dpValue->_d() )
				{
				case TA_Base_Core:: BOOLEAN_DATA_POINT:
					ostrstrm << TA_Base_Core::BOOLEAN_DATA_POINT << ",";
					ostrstrm << dpValue->booleanValue();				
					valueVector.push_back(ostrstrm.str());
					break;

				case TA_Base_Core::NUMERIC_DATA_POINT:
                    valueVector.push_back(makeNumericDataPointValueString( dpValue->numericValue() ));
					break;

				case TA_Base_Core::TEXT_DATA_POINT:
					ostrstrm << TA_Base_Core::TEXT_DATA_POINT << ",";
					ostrstrm << dpValue->textValue();
					valueVector.push_back(ostrstrm.str());
					break;

				default:
					//Throw exception
					break;
				}

				ostrstrm.str("");
				ostrstrm << refParams.testType;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                // TD8736: Originally, the success/failure step parameter was stored as a step ID. It has been changed 
                //         to step position. Plan Manager continues to support the Data Point Check step in terms of 
                //         step ID, so these parameters need to be converted before being written to database. 
                //         They have already been verified by the time we get here, so the conversion is safe.

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, refParams.successStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, refParams.failureStep);
				valueVector.push_back(ostrstrm.str());

				break;
            }
            #endif

            #if defined ( STEPTYPE_DATA_POINT_SET )
			case TA_Base_Core::DATA_POINT_SET_STEP:
            {
		        const TA_Base_Core::DataPointSetParameters& refParams = stepParameters->dataPointSet();
				valueVector.push_back(refParams.dataPoint.in());
				dpValue = &(refParams.newValue);

				switch(dpValue->_d())
				{
				case TA_Base_Core::BOOLEAN_DATA_POINT:
					ostrstrm << TA_Base_Core::BOOLEAN_DATA_POINT << ",";
					ostrstrm << dpValue->booleanValue();
					valueVector.push_back(ostrstrm.str());
					break;

				case TA_Base_Core::NUMERIC_DATA_POINT:
                    valueVector.push_back(makeNumericDataPointValueString( dpValue->numericValue() ));
					break;

				case TA_Base_Core::TEXT_DATA_POINT:
					ostrstrm << TA_Base_Core::TEXT_DATA_POINT << ",";
					ostrstrm << dpValue->textValue();
					valueVector.push_back(ostrstrm.str());
					break;
				}

				break;
            }
            #endif

            #if defined ( STEPTYPE_DECISION )
			case TA_Base_Core::DECISION_STEP:
            {
                // TD8736: Originally, the yes/no step parameter was stored as a step ID. It has been changed 
                //         to step position. Plan Manager continues to support the Decision step in terms of 
                //         step ID, so these parameters need to be converted before being written to database. 
                //         They have already been verified by the time we get here, so the conversion is safe.

                const TA_Base_Core::DecisionParameters& refParams = stepParameters->decision();
				valueVector.push_back(parseForApostrophe(refParams.decisionMessage.in()));
				ostrstrm << refParams.responseTimeout;
				valueVector.push_back(ostrstrm.str());

				ostrstrm.str("");
				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, refParams.yesStep);
				valueVector.push_back(ostrstrm.str());

				ostrstrm.str("");
				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, refParams.noStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				//DP-changes
				// add support to the new parameters for the decision step
				// yesButtonLabel
				valueVector.push_back(parseForApostrophe(refParams.yesButtonLabel.in()));

				//noButtonLabel
				valueVector.push_back(parseForApostrophe(refParams.noButtonLabel.in()));

				//pauseButtonLabel
				valueVector.push_back(parseForApostrophe(refParams.pauseButtonLabel.in()));

				//noButtonEnabled
				ostrstrm.str("");
                if ( refParams.noButtonEnabled )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());

				//pauseButtonEnabled
				ostrstrm.str("");
                if ( refParams.pauseButtonEnabled )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());
				
				ostrstrm.str("");

                break;
            }
            #endif

            #if defined ( STEPTYPE_JUMP )
			case TA_Base_Core::JUMP_STEP:
            {
                // TD8736: Originally, the target step parameter was stored as a step ID. It has been changed 
                //         to step position. Plan Manager continues to support the Jump step in terms of 
                //         step ID, so this parameter needs to be converted before being written to database. 
                //         It has already been verified by the time we get here, so the conversion is safe.

                const TA_Base_Core::JumpParameters& refParams = stepParameters->jump();
				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, refParams.step);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                break;
            }
            #endif

            #if defined ( STEPTYPE_LOG )
			case TA_Base_Core::LOG_STEP:
            {
		        const TA_Base_Core::LogParameters& refParams = stepParameters->log();
				valueVector.push_back(parseForApostrophe(refParams.message.in()));
                ostrstrm.str("");
                if ( refParams.logRequired )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());
                ostrstrm.str("");
				ostrstrm << refParams.responseTimeout;
				valueVector.push_back(ostrstrm.str());

				break;
            }
            #endif

            #if defined ( STEPTYPE_ACTIVATE_CAMERA_PRESET )
            case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
            {
                const TA_Base_Core::ActivateCameraPresetParameters& refParams= stepParameters->cameraPreset();
                ostrstrm << refParams.camera;
				valueVector.push_back( ostrstrm.str() );
                ostrstrm.str("");
                ostrstrm << refParams.preset;
                valueVector.push_back( ostrstrm.str() );

                break;
            }
            #endif

            #if defined ( STEPTYPE_PROMPT )
            case TA_Base_Core::PROMPT_STEP:
            {
                const TA_Base_Core::PromptParameters& refParams = stepParameters->prompt();
				valueVector.push_back( parseForApostrophe(refParams.message.in()) );
				ostrstrm << refParams.dismissTimeout;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                break;
            }
            #endif

            #if defined ( STEPTYPE_RUN_PLAN )
			case TA_Base_Core::RUN_PLAN_STEP:
            {
                // TD8736: Originally, the plan parameter was stored as a plan ID. This has been changed
                //         to be the plan path. The Run Plan step is still supported by the Plan Manager
                //         in terms of plan ID, so this needs to be converted to the corresponding path
                //         before being written to database. The plan ID has already been verified by the
                //         time we get here, so the conversion is safe.

                const TA_Base_Core::RunPlanParameters& refParams = stepParameters->runPlan();
				ostrstrm << TA_Base_Bus::PlanDataReader::instance()->getNodePath(refParams.plan);
				valueVector.push_back(ostrstrm.str());

                ostrstrm.str("");
				ostrstrm << refParams.manualLaunch;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << refParams.waitForCompletion;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                break;
            }
            #endif

            #if defined ( STEPTYPE_TERMINATE_PLAN )
			case TA_Base_Core::TERMINATE_PLAN_STEP:
            {
                // TD8736: Originally, the plan parameter was stored as a plan ID. This has been changed to
                //         be the plan path. The Terminate Plan step is still supported by the Plan Manager
                //         in terms of plan ID, so this needs to be converted to the corresponding path
                //         before being written to database. The plan ID has already been verified by the
                //         time we get here, so the conversion is safe.

                const TA_Base_Core::TerminatePlanParameters& refParams = stepParameters->terminatePlan();
				ostrstrm << TA_Base_Bus::PlanDataReader::instance()->getNodePath(refParams.plan);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                break;
            }
            #endif

            #if defined ( STEPTYPE_EVALUATE_EXPRESSION )
			case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
            {
                // TD8736: Originally, the success/failure step parameter was stored as a step ID. It has been changed 
                //         to step position. Plan Manager continues to support the Evaluate Expression step in terms of 
                //         step ID, so these parameters need to be converted before being written to database. 
                //         They have already been verified by the time we get here, so the conversion is safe.

                const TA_Base_Core::EvaluateExpressionParameters& refParams = stepParameters->evaluateExpression();
				valueVector.push_back(refParams.expression.in());
				ostrstrm.str("");

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, refParams.successStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, refParams.failureStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                break;
            }
            #endif

            #if defined ( STEPTYPE_VMS_SET_SCENE )
			case TA_Base_Core::VMS_SET_SCENE_STEP:
            {
		        const TA_Base_Core::VmsSetSceneParameters& refParams = stepParameters->vmsSetScene();
				ostrstrm << refParams.vmsSign;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << refParams.scene;
				valueVector.push_back(ostrstrm.str());
                ostrstrm.str("");

                break;
            }
            #endif

            #if defined ( STEPTYPE_LAUNCH_GUI_APP )
            case TA_Base_Core::LAUNCH_GUI_APP_STEP:
            {
		        const TA_Base_Core::LaunchGuiAppParameters& refParams = stepParameters->guiAppParameters();
                ostrstrm << refParams.app;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                valueVector.push_back(refParams.args.in());

                ostrstrm << refParams.pos.top   <<  ","
                         << refParams.pos.left  << ","
                         << refParams.pos.width << ","
                         << refParams.pos.height;

				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                break;
            }
            #endif

            #if defined ( STEPTYPE_VMS_BLANKOUT )
			case TA_Base_Core::VMS_BLANKOUT_STEP:
            {
                const TA_Base_Core::VmsBlankoutParameters& refParams = stepParameters->vmsBlankout();
				ostrstrm << refParams.vmsSign;
				valueVector.push_back(ostrstrm.str());
                ostrstrm.str("");

                break;
            }
            #endif

            #if defined ( STEPTYPE_VMS_RESTORE_SCHEDULE )
			case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
            {
                const TA_Base_Core::VmsRestoreScheduleParameters& refParams = stepParameters->vmsRestoreSchedule();
				ostrstrm << refParams.vmsSign;
				valueVector.push_back(ostrstrm.str());
                ostrstrm.str("");

                break;
            }
            #endif

            #if defined ( STEPTYPE_VIDEOWALL )
			case TA_Base_Core::VIDEO_WALL_STEP:
            {
                const TA_Base_Core::VideoWallParameters& refParams = stepParameters->videoWallParams();
				ostrstrm << refParams.wallBoard;
				valueVector.push_back(ostrstrm.str());

				ostrstrm.str("");
				ostrstrm << refParams.layout;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                break;
            }
            #endif

            #if defined (STEPTYPE_SEND_SMS_MESSAGE)
            case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
            {
                const TA_Base_Core::SendSmsMessageParameters& refParams = stepParameters->sendSmsMessage();
                ostrstrm << std::string(refParams.recipientIdentifier);
                valueVector.push_back( ostrstrm.str() );
                ostrstrm.str("");

                ostrstrm << std::string(refParams.textMessage);
                valueVector.push_back( ostrstrm.str() );
                ostrstrm.str("");

                break;
            }
            #endif

            #if defined (STEPTYPE_LAUNCH_SCHEMATIC)
            case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
            {
                const TA_Base_Core::LaunchSchematicParameters& refParams = stepParameters->launchSchematic();

                ostrstrm.str("");
                ostrstrm << refParams.schematic.in();
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << refParams.screen;
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << refParams.commands.in();
                valueVector.push_back( ostrstrm.str() );

                break;
            }
            #endif

            #if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)
            case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
            {
                const TA_Base_Core::StationTravellerInfoSystemParameters& refParams = stepParameters->stationTravellerInfoSystem();
                const std::string SEPARATOR(",");
                const std::string SEPARATOR2( ";" );

                //Process destinationList,one PidsDestination by one,one PidsDestination is formatted
                //like so:"stionid,downtrack,uptrack,concourse",and PidsDestinations is separated by SEPARATOR2

				//pidList
				ostrstrm.str( "" );
				for(unsigned int i = 0; i < refParams.pidList.length(); i++)
				{
					ostrstrm << refParams.pidList[i].in() << SEPARATOR;
				}
				valueVector.push_back( ostrstrm.str() );

                ostrstrm.str( "" );
                for ( unsigned int i = 0; i < refParams.destinationList.length(); i++ )
                {
                    ostrstrm << refParams.destinationList[i].station << SEPARATOR;
                    ostrstrm << refParams.destinationList[i].downTrack << SEPARATOR;
                    ostrstrm << refParams.destinationList[i].upTrack << SEPARATOR;
                    ostrstrm << refParams.destinationList[i].concourse << SEPARATOR2;
                }
                valueVector.push_back( ostrstrm.str() );

                //messageContent
                ostrstrm.str( "" );
                for ( unsigned int i = 0; i < refParams.messageContent.length(); i++ )
                {
                    ostrstrm << refParams.messageContent[i];
                }
                valueVector.push_back( ostrstrm.str() );

                //msgType
                ostrstrm.str( "" );
                ostrstrm << refParams.msgType;
                valueVector.push_back( ostrstrm.str() );

                //showType
                ostrstrm.str( "" );
                ostrstrm << refParams.showType;
                valueVector.push_back( ostrstrm.str() );

                //isTrain
                ostrstrm.str( "" );
                ostrstrm << 0;
                valueVector.push_back( ostrstrm.str() );

                //startTime,not use
                ostrstrm.str( "" );
                ostrstrm << 0 << SEPARATOR << 0 << SEPARATOR << 0 << SEPARATOR << 0 << SEPARATOR << 0 << SEPARATOR << 0;
                valueVector.push_back( ostrstrm.str() );

                //endTime, not use
                valueVector.push_back( ostrstrm.str() );

                //interval, not use
                ostrstrm.str( "" );
                ostrstrm << 0;
                valueVector.push_back( ostrstrm.str() );

                //isCyclic, not use
                valueVector.push_back( ostrstrm.str() );

                break;
            }
            #endif

            #if defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)
            case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
            {
                const TA_Base_Core::TrainTravellerInfoSystemParameters& refParams = stepParameters->trainTravellerInfoSystem();
                const std::string SEPARATOR(",");

                // The train TIS message destination parameter is formatted like so:
                // "train_id,train_group"

                ostrstrm.str("");
                ostrstrm << refParams.destination.trainId << SEPARATOR
                         << refParams.destination.trainGroup;
                 valueVector.push_back( ostrstrm.str() );

                // The train TIS message is formatted like so:
                // "library_version,library_section,message_id"
                
                ostrstrm.str("");
                ostrstrm << refParams.message.libraryVersion << SEPARATOR
                         << refParams.message.librarySection << SEPARATOR
                         << refParams.message.messageId;
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << refParams.priority;
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << refParams.duration;
                valueVector.push_back( ostrstrm.str() );

                break;
            }
            #endif

            #if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)
			case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
            {
                const TA_Base_Core::StationPublicAddressParameters& refParams = stepParameters->stationPublicAddress();
                const std::string SEPARATOR(",");

                // The station destination parameter is formatted like so:
                // "station,all_stations"
                ostrstrm.str("");
                ostrstrm << refParams.station.station.in() << SEPARATOR << refParams.station.locationKey << SEPARATOR
                         << refParams.station.allStations;
                valueVector.push_back(ostrstrm.str());

                // The station PA message is formatted like so:
                // "message_id"
                ostrstrm.str("");
                ostrstrm  << refParams.message.messageId;
                valueVector.push_back(ostrstrm.str());

                //chime 
                ostrstrm.str("");
                if ( refParams.chime )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());

                //cyclic
                ostrstrm.str("");
                if ( refParams.cyclic )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());

					 //TIS Sync
					 ostrstrm.str("");
                if ( refParams.TISSynch )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());

                // Message duration
                ostrstrm.str("");
                ostrstrm << refParams.duration;
                valueVector.push_back(ostrstrm.str());

                //Message interval
                ostrstrm.str("");
                ostrstrm << refParams.interval;
                valueVector.push_back(ostrstrm.str());

                // Cylic times
                ostrstrm.str("");
                ostrstrm << refParams.cyclicTimes;
                valueVector.push_back(ostrstrm.str());

                // the parameter value column in the database has a limited size.
                // The zones are therefore split into a number of parameters rows, each holding a small 
                // subset of the  zoneID values, in the format "zoneid1,zoneid2, ... ,zoneidN".
                const unsigned long ZONES_PER_ROW(15);
                unsigned long zoneIdx(0);

                while (zoneIdx < refParams.zones.length())
                {
                    ostrstrm.str("");

                    for (unsigned long i = 0;
                         i < ZONES_PER_ROW && zoneIdx < refParams.zones.length();
                         i++, zoneIdx++)
                    {
                        ostrstrm << refParams.zones[zoneIdx] << SEPARATOR;
                    }

                    valueVector.push_back(ostrstrm.str());
                }

                break;
            }
            #endif

            #if defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)
			case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
            {
                const TA_Base_Core::TrainPublicAddressParameters& refParams = stepParameters->trainPublicAddress();
                const std::string SEPARATOR(",");

                 // The train PA destination parameter is formatted like so:
                // "train_id,train_group"
                ostrstrm.str("");
                ostrstrm << refParams.destination.trainId << SEPARATOR
                         << refParams.destination.trainGroup;
                 valueVector.push_back( ostrstrm.str() );

               
                // The station PA message is formatted like so:
                // "library_version,library_section,message_id"
                ostrstrm.str("");
                ostrstrm << refParams.message.libVersion << SEPARATOR
                         << refParams.message.libSection << SEPARATOR
                         << refParams.message.messageId;
                valueVector.push_back(ostrstrm.str());

               
			    //cyclic
				ostrstrm.str("");
                if ( refParams.cyclic )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());

                // Message duration
                ostrstrm.str("");
                ostrstrm << refParams.duration;
                valueVector.push_back(ostrstrm.str());

                break;
            }
            #endif

            #if defined (STEPTYPE_FAILURE_SUMMARY_STEP) 	
			case TA_Base_Core::FAILURE_SUMMARY_STEP:
			{
		        const TA_Base_Core::FailureSummaryParameters& refParams = stepParameters->failureSummary();

				ostrstrm.str("");				
				ostrstrm << refParams.failureCategory;
				valueVector.push_back(ostrstrm.str());
                ostrstrm.str("");

                break;
			}
            #endif // STEPTYPE_FAILURE_SUMMARY_STEP

			//WuZhongyi New Step Fire Alarm Station PA
            #if defined (STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM)
			case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP_BY_ALARM:
			{
				const TA_Base_Core::StationPublicAddressByAlarmParameters& refParams = stepParameters->stationPublicAddrssByAlarm();
				std::string strValue = refParams.message.in();
                valueVector.push_back(strValue);

                break;
			}
            #endif //STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM

            #if defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)
            case  TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP:
            {
                const TA_Base_Core::BackgroundLaunchSchematicParameters& refParams = stepParameters->bgLaunchSchematic();

                ostrstrm.str("");
                ostrstrm << refParams.schematic.in();
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << refParams.screen;
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << refParams.commands.in();
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << refParams.actionTypeKey;
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << refParams.dataPoint;
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << refParams.profile;
                valueVector.push_back(ostrstrm.str());

                break;
            }
            #endif // STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC


            #if defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)
            case  TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP:
                {
                    const TA_Base_Core::BackgroundLaunchGuiAppParameters& refParams = stepParameters->bgLaunchGuiApp();

                    ostrstrm << refParams.app;
                    valueVector.push_back(ostrstrm.str());
                    ostrstrm.str("");

                    valueVector.push_back(refParams.args.in());

                    ostrstrm << refParams.pos.top   <<  ","
                        << refParams.pos.left  << ","
                        << refParams.pos.width << ","
                        << refParams.pos.height;

                    valueVector.push_back(ostrstrm.str());
                    ostrstrm.str("");
                    
                    if ( refParams.bAddTriggerEntity )                
                        ostrstrm << 1;
                    else
                        ostrstrm << 0;
                    valueVector.push_back(ostrstrm.str());

                    ostrstrm.str("");
                    ostrstrm << refParams.actionTypeKey;
                    valueVector.push_back( ostrstrm.str() );

                    ostrstrm.str("");
                    ostrstrm << refParams.dataPoint;
                    valueVector.push_back( ostrstrm.str() );

                    ostrstrm.str("");
                    if ( refParams.bTriggerEntityCheckAction )                
                        ostrstrm << 1;
                    else
                        ostrstrm << 0;
                    valueVector.push_back(ostrstrm.str());

                    ostrstrm.str("");
                    ostrstrm << refParams.profile;
                    valueVector.push_back(ostrstrm.str());

                    break;
                }
                #endif // STEPTYPE_BACKGROUND_LAUNCH_GUI_APP

            #if defined ( STEPTYPE_BACKGROUND_DECISION )
            case TA_Base_Core::BACKGROUND_DECISION_STEP:
                {
                    // TD8736: Originally, the yes/no step parameter was stored as a step ID. It has been changed 
                    //         to step position. Plan Manager continues to support the Decision step in terms of 
                    //         step ID, so these parameters need to be converted before being written to database. 
                    //         They have already been verified by the time we get here, so the conversion is safe.

                    const TA_Base_Core::BackgroundDecisionParameters& refParams = stepParameters->bgDecision();
                    valueVector.push_back(parseForApostrophe(refParams.decisionMessage.in()));
                    ostrstrm << refParams.responseTimeout;
                    valueVector.push_back(ostrstrm.str());

                    ostrstrm.str("");
                    ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, refParams.yesStep);
                    valueVector.push_back(ostrstrm.str());

                    ostrstrm.str("");
                    ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, refParams.noStep);
                    valueVector.push_back(ostrstrm.str());
                    ostrstrm.str("");

                    //DP-changes
                    // add support to the new parameters for the decision step
                    // yesButtonLabel
                    valueVector.push_back(parseForApostrophe(refParams.yesButtonLabel.in()));

                    //noButtonLabel
                    valueVector.push_back(parseForApostrophe(refParams.noButtonLabel.in()));

                    //pauseButtonLabel
                    valueVector.push_back(parseForApostrophe(refParams.pauseButtonLabel.in()));

                    //noButtonEnabled
                    ostrstrm.str("");
                    if ( refParams.noButtonEnabled )                
                        ostrstrm << 1;
                    else
                        ostrstrm << 0;
                    valueVector.push_back(ostrstrm.str());

                    //pauseButtonEnabled
                    ostrstrm.str("");
                    if ( refParams.pauseButtonEnabled )                
                        ostrstrm << 1;
                    else
                        ostrstrm << 0;
                    valueVector.push_back(ostrstrm.str());

                    ostrstrm.str("");
                    ostrstrm << refParams.profile;
                    valueVector.push_back(ostrstrm.str());

                    break;
                }
            #endif

            #if defined ( STEPTYPE_BACKGROUND_LOG )
            case TA_Base_Core::BACKGROUND_LOG_STEP:
                {
                    const TA_Base_Core::BackgroundLogParameters& refParams = stepParameters->bgLog();
                    valueVector.push_back(parseForApostrophe(refParams.message.in()));
                    ostrstrm.str("");
                    if ( refParams.logRequired )                
                        ostrstrm << 1;
                    else
                        ostrstrm << 0;
                    valueVector.push_back(ostrstrm.str());
                    ostrstrm.str("");
                    ostrstrm << refParams.responseTimeout;
                    valueVector.push_back(ostrstrm.str());

                    ostrstrm.str("");
                    ostrstrm << refParams.profile;
                    valueVector.push_back(ostrstrm.str());
                    break;
                }
            #endif

            #if defined ( STEPTYPE_BACKGROUND_PROMPT )
            case TA_Base_Core::BACKGROUND_PROMPT_STEP:
                {
                    const TA_Base_Core::BackgroundPromptParameters& refParams = stepParameters->bgPrompt();
                    valueVector.push_back( parseForApostrophe(refParams.message.in()) );

                    ostrstrm.str("");
                    ostrstrm << refParams.dismissTimeout;
                    valueVector.push_back(ostrstrm.str());

                    ostrstrm.str("");
                    ostrstrm << refParams.profile;
                    valueVector.push_back(ostrstrm.str());

                    break;
                }
                #endif // STEPTYPE_BACKGROUND_LAUNCH_GUI_APP

			default:
				break;
			}

	//	update step_parameters 

            TA_Base_Bus::PlanDataWriter::instance()->updateStepParameters(*stepDetail, valueVector);
		}

        AAMessageSender::instance()->submitAuditPlanUpdate( session, strPlanName );

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );		
	}

	void PlanDataWritingClass::copyPlan(const std::string& session, TA_Base_Core::NodeId plan, TA_Base_Core::NodeId parent, bool useForce)
	{
		std::ostringstream message;

		TA_Base_Core::NodeSummary_var oldSummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(plan);

		if ( oldSummary->type != TA_Base_Core::PLAN_NODE)
		{
			message << "Error: The node [" << plan << "] is not of type PLAN."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
		}

		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(parent);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
		{
			message << "Error: The given parent node [" << parent << "] is not of type CATEGORY."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
		}

		std::string name = oldSummary->name.in();
		bool overwriting = false;
		TA_Base_Core::NodeId existingPlan = 0;

		if ( oldSummary->parent != parent )
		{
			// if we have a conflicting plan
			if ( !TA_Base_Bus::PlanDataReader::instance()->canUseName(plan, parent, parseForApostrophe(name)) )
			{
				// get the ID of the existing plan
				existingPlan = getPlanToReplace( name, parent, useForce );		
				overwriting = true;
			}
		}
		else
		{
			name = getNodeNameToCopy(plan, parent);
		}

		TA_Base_Core::NodeId newPlan = 0;
		try
		{
			if (overwriting)
			{
                TA_Base_Bus::PlanDataWriter::instance()->replacePlan(session, existingPlan, plan);
			}
			else
			{
                TA_Base_Bus::PlanDataWriter::instance()->copyPlan(session, plan, parent, name);
			}

		    // We have successfully copied the plan, let us retrieve its NodeId and send the update	        
			newPlan = TA_Base_Bus::PlanDataReader::instance()->getNodeId( parent, parseForApostrophe(name) );
		}
		catch(...)
		{
            std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath( plan );
            std::string destinationPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(parent);
            destinationPlanName += "/";
            destinationPlanName += TA_Base_Bus::PlanDataReader::instance()->getNodeName( plan );
			AAMessageSender::instance()->submitAuditPlanCopyFailure(session, strPlanName, destinationPlanName );

			std::ostringstream alarmMessage;
			alarmMessage << "Plan Copy Failure:" << strPlanName << "[Id:" << plan << "]";
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", alarmMessage.str().c_str());

			TA_THROW( TA_Base_Core::DataException( "Failed to copyPlan", TA_Base_Core::DataException::NO_VALUE, "" ) );
        }


		TA_Base_Core::PlanConfigUpdateData pcud;
		pcud.session = CORBA::string_dup(session.c_str());
        std::string sourcePlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan);
        std::string targetPlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(newPlan);

        // When replacing an existing plan, send a delete update before the copy update.
        if (overwriting)
        {
		    TA_Base_Core::DeleteNodeUpdate dnu;
		    dnu.parent = parent;
		    dnu.nodes.length(1);
            dnu.nodes[0] = existingPlan;

			if ( parent != TA_Base_Core::ROOT_NODE_ID )
            {
				pcud.configUpdate.deletePlans(dnu);
            }
			else
            {
				pcud.configUpdate.deletePlansFromRoot(dnu.nodes);
            }

            ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( pcud );

		    AAMessageSender::instance()->submitAuditPlanDelete( session, targetPlanPath );

		    std::ostringstream auditMessage;
		    auditMessage << targetPlanPath << " [Id:" << existingPlan << "] has been deleted by overwriting " << sourcePlanPath << " [Id:"  << plan << "].";
		    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
        }

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(newPlan);

		if ( parent != TA_Base_Core::ROOT_NODE_ID )
        {
			pcud.configUpdate.copyPlan(ns);
        }
		else
        {
			pcud.configUpdate.copyPlanToRoot( ns );
        }

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( pcud );

		AAMessageSender::instance()->submitAuditPlanCopy( session, sourcePlanPath, targetPlanPath );

		std::ostringstream auditMessage;
		auditMessage << sourcePlanPath << " [Id:" << plan << "] has been copied to " << targetPlanPath << " [Id:" << newPlan << "].";

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
	}


	 TA_Base_Core::AuxPlanDetail* PlanDataWritingClass::exportPlan (const std::string& session, TA_Base_Core::NodeId& planId)
	 {
         return TA_Base_Bus::PlanDataWriter::instance()->exportPlan(session, planId);
	 }

	void PlanDataWritingClass::importPlan (const std::string& session, const TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId)
	{
        TA_Base_Core::NodeId planId = TA_Base_Bus::PlanDataWriter::instance()->importPlan(session, plan, parentId);

		//Send Update for insertPlan
		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(planId);

		if ( parentId != TA_Base_Core::ROOT_NODE_ID )
        {
			pcud->configUpdate.insertPlan(ns);
        }
		else
        {
			pcud->configUpdate.insertPlanIntoRoot(ns);
        }

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

        std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath( planId );
		AAMessageSender::instance()->submitAuditPlanInsert( session, strPlanName );

		std::ostringstream auditMessage;
		auditMessage << strPlanName << "[Id:" << planId << "] has been inserted.";
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
	}

	void PlanDataWritingClass::movePlan(const std::string& session, TA_Base_Core::NodeId plan, 
                                        TA_Base_Core::NodeId parent, bool useForce)
	{
		TA_Base_Core::NodeSummary_var oldSummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(plan);
		std::ostringstream message;

		if ( oldSummary->type != TA_Base_Core::PLAN_NODE)
		{
			message << "Error: The node [" << plan << "] is not of type PLAN."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
		}

		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(parent);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
		{
			message << "Error: The node [" << parent << "] is not of type CATEGORY."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_CATEGORY));
		}

		if( isPlanBeingEdited(plan) )
		{
			std::string editLockSession = getEditLockOwner(plan);
			message << "Error: The plan with name [" << oldSummary->name.in()
					<< "] is being edited by "
					<<	ActivePlanBulletinBoard::instance()->getOwner(editLockSession);
			TA_THROW( TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)) );

		}

        std::string sourcePlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan);
		bool overwriting = false;
		TA_Base_Core::NodeId existingPlan = 0;

        // if we have a duplicate name
		if ( !TA_Base_Bus::PlanDataReader::instance()->canUseName(plan, parent, parseForApostrophe(oldSummary->name.in())) )
		{
			// get the ID of the existing plan
			existingPlan = getPlanToReplace( oldSummary->name.in(), parent, useForce );		
			overwriting = true;
		}

		if (overwriting)
		{
            TA_Base_Bus::PlanDataWriter::instance()->replacePlan(session, existingPlan, plan);
		}
		else
		{
            TA_Base_Bus::PlanDataWriter::instance()->updateNodeParent(session, parent, plan);
		}

		TA_Base_Core::PlanConfigUpdateData pcud;
		pcud.session = CORBA::string_dup(session.c_str());
        std::string targetPlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan); // same plan ID different parent now

        // When replacing an existing plan, send a delete update before the move update.
        if (overwriting)
        {
		    TA_Base_Core::DeleteNodeUpdate dnu;
		    dnu.parent = parent;
		    dnu.nodes.length(1);
            dnu.nodes[0] = existingPlan;

			if ( parent != TA_Base_Core::ROOT_NODE_ID )
            {
				pcud.configUpdate.deletePlans(dnu);
            }
			else
            {
				pcud.configUpdate.deletePlansFromRoot(dnu.nodes);
            }

            ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( pcud );

		    AAMessageSender::instance()->submitAuditPlanDelete( session, targetPlanPath );

		    std::ostringstream auditMessage;
		    auditMessage << targetPlanPath << " [Id:" << existingPlan << "] has been deleted by overwriting " << sourcePlanPath << " [Id:"  << plan << "].";
		    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
        }

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(plan);
		TA_Base_Core::TransferNodeUpdate tnu;

		tnu.oldParent = oldSummary->parent;
		tnu.summary.node = ns->node;
		tnu.summary.parent = parent;
		tnu.summary.name = CORBA::string_dup(ns->name);
		tnu.summary.type = ns->type;
		tnu.summary.approvalState = ns->approvalState;
		tnu.summary.hasChildren = ns->hasChildren;
		tnu.summary.hasApprovedDescendant = ns->hasApprovedDescendant;
		tnu.summary.hasUnapprovedDescendant = ns->hasUnapprovedDescendant;

		if ( parent == TA_Base_Core::ROOT_NODE_ID )
        {
			pcud.configUpdate.movePlanToRoot( tnu );
        }
		else if ( oldSummary->parent == TA_Base_Core::ROOT_NODE_ID )
        {
			pcud.configUpdate.movePlanFromRoot( tnu );
        }
		else
        {
			pcud.configUpdate.movePlan( tnu );
        }

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( pcud );

		AAMessageSender::instance()->submitAuditPlanMove( session, sourcePlanPath, targetPlanPath );

		std::ostringstream auditMessage;
		auditMessage << sourcePlanPath << " [Id:" << plan << "] has been moved to " << targetPlanPath << ".";
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
	}

	void PlanDataWritingClass::insertPlan(const std::string& session, const TA_Base_Core::NodeId parent)
	{
        TA_Base_Core::NodeId plan = TA_Base_Bus::PlanDataWriter::instance()->insertPlan(session, parent);

		//Send Update for insertPlan
		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(plan);

		if ( parent != TA_Base_Core::ROOT_NODE_ID )
        {
			pcud->configUpdate.insertPlan(ns);
        }
		else
        {
			pcud->configUpdate.insertPlanIntoRoot(ns);
        }

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

        std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan);
		AAMessageSender::instance()->submitAuditPlanInsert( session, strPlanName );

		std::ostringstream auditMessage;
		auditMessage << strPlanName << "[Id:" << plan << "] has is moved.";
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
	}

	//This method used to delete plans which could belong to different parents. But now it is being 
	//changed so that all plans are assumed to belong to the same parent.

	void PlanDataWritingClass::deletePlans(const std::string& session, const TA_Base_Core::NodeIds& plans)
	{
		int numberOfPlans = plans.length();
		TA_Base_Core::NodeId parent = TA_Base_Bus::PlanDataReader::instance()->getParent(plans[0]);

		TA_Base_Core::DeleteNodeUpdate_var dnu = new TA_Base_Core::DeleteNodeUpdate;
		dnu->parent = parent;
		dnu->nodes.length(numberOfPlans);

		TA_Base_Core::NodeId plan = 0;

		std::ostringstream message;

		for (int i=0; i < numberOfPlans; i++)
		{
			plan = plans[i];
			TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(plan);
			message << "Error: The plan ";

			if ( nodeType != TA_Base_Core::PLAN_NODE)
			{
				message << "is not of type PLAN.";
                TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
			}


			if ( TA_Base_Core::APPROVED_APPROVAL_STATE == TA_Base_Bus::PlanDataReader::instance()->getPlanApprovalState(plan) )
			{
				message << "is approved and cannot be deleted.";
                TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_DELETE_APPROVED_PLAN));
			}


			if ( isPlanBeingEdited(plan) )
			{
                std::string editLockSession = getEditLockOwner(plan);
				message << "is being edited by " << ActivePlanBulletinBoard::instance()->getOwner(editLockSession) << ".";
                TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)));
			}

			//dnu.operator [](i).parent = TA_Base_Bus::PlanDataReader::instance()->getParent(plan);
			dnu->nodes[i] = plan;
		}

		//We are here means we can delete all the given plans
		int undeletedPlans = 0;
		std::vector <int> VectorUndeletedPlans;

		for ( int j = 0; j < numberOfPlans; j++)
		{
            std::string planPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plans[j]);
			plan = plans[j];
            // TD11505: Stored procedure DELETE_PLAN() will raise an application error if the plan
            // cannot be deleted for whatever reason. This will be thrown from executeProcedure() 
            // as a DatabaseException.
            try
            {
                TA_Base_Bus::PlanDataWriter::instance()->deletePlan(session, plan);

		        AAMessageSender::instance()->submitAuditPlanDelete( session, planPath );
            }
            catch (const TA_Base_Core::DatabaseException& ex)
            {
                // Check for error -20022.
                const std::string DELETE_PLAN_ERROR("-20022");
                std::string exceptionError(ex.what());

                if (exceptionError.find(DELETE_PLAN_ERROR) == std::string::npos)
                {
					TA_THROW(ex);
                }

				undeletedPlans++;
				VectorUndeletedPlans.push_back(j);

			    AAMessageSender::instance()->submitAuditPlanDeleteFailure( session, planPath );

				//Log alarm
				std::ostringstream alarmMessage;
				alarmMessage << "PlanDeleteFailure:" << planPath << "[Id:" << plans[j] << "] could not be deleted.";
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", alarmMessage.str().c_str());
            }

            try
            {
                TA_Base_Bus::PlanDataWriter::instance()->deletePlanType(plan);
            }
            catch ( ... )
            {
            }
		}

		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		if ( undeletedPlans )
		{
			TA_Base_Core::DeleteNodeUpdate_var newDnu = new TA_Base_Core::DeleteNodeUpdate;
			dnu->parent = parent;
			int actualDeletedPlans = numberOfPlans-undeletedPlans;
			newDnu->nodes.length(actualDeletedPlans);
			int oldIndex = 0;
			int newIndex = 0;
			bool foundUndeletedPlan = false;

			std::vector<int>::iterator it;

			for ( it = VectorUndeletedPlans.begin(); it != VectorUndeletedPlans.end() ; it++)
			{
				int nUndeleted = *it;
				while(newIndex != nUndeleted)
				{
					newDnu->nodes[newIndex++] = dnu->nodes[oldIndex++];
				}

				oldIndex++;
			}

			if ( parent != TA_Base_Core::ROOT_NODE_ID )
            {
				pcud->configUpdate.deletePlans(newDnu);
            }
			else
            {
				pcud->configUpdate.deletePlansFromRoot(newDnu->nodes);
            }
		}
		else
		{
			if ( parent != TA_Base_Core::ROOT_NODE_ID )
            {
				pcud->configUpdate.deletePlans(dnu);
            }
			else
            {
				pcud->configUpdate.deletePlansFromRoot(dnu->nodes);
            }
		}

        if (undeletedPlans != numberOfPlans)
        {
		    //Finally send the update
		    ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );
        }

		if (undeletedPlans )
		{
			std::string message = "Error: The Plans nodes could not be deleted: ";
            TA_THROW(TA_Base_Core::PlanAgentException(message, TA_Base_Core::PAE_DATA_ERROR));
		}
	}

    void PlanDataWritingClass::updateCategory(const std::string& session, const TA_Base_Core::CategoryDetail& detail)
	{
		std::ostringstream message;

		TA_Base_Core::NodeId parent = detail.parent;

		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(parent);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
		{
			message << "Error: The parent[" << detail.parent << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_PARENT_NOT_CATEGORY));
		}

		TA_Base_Core::NodeId category = detail.category;

		nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(category);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
		{
			message << "Error: The category[" << category << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_CATEGORY));
		}

		std::string name = parseForApostrophe(detail.name.in());
		std::string description = parseForApostrophe(detail.description.in());

		if ( !TA_Base_Bus::PlanDataReader::instance()->canUseName(category, parent, name) )
		{
			message << "Error: Category with name [" << detail.name.in() << "] already exists in the parent category."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NAME_NOT_UNIQUE));
		}

        std::string oldPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(category);

        TA_Base_Bus::PlanDataWriter::instance()->updateCategory(session, name, description, category);
		//We have successfully updated the category.
		//Send Update for insertCategory
		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(category);
		pcud->configUpdate.updateCategory(ns);
		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

        std::string strNewName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(category);
		AAMessageSender::instance()->submitAuditCategoryUpdate( session, oldPath, strNewName );

		std::ostringstream auditMessage;
		auditMessage << "Category details of " << strNewName << "[Id:" << category << "] have been updated."; 
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
	}


    void PlanDataWritingClass::insertCategory(const std::string& session, TA_Base_Core::NodeId parent)
	{
        std::string name = getNewNodeName(parent, TA_Base_Core::CATEGORY_NODE);
        TA_Base_Core::NodeId category = TA_Base_Bus::PlanDataWriter::instance()->insertCategory(session, parent);

		//Send Update for insertCategory
		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(category);

		if ( parent != TA_Base_Core::ROOT_NODE_ID )
        {
			pcud->configUpdate.insertCategory(ns);
        }
		else
        {
			pcud->configUpdate.insertCategoryIntoRoot(ns);
        }

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

        std::string strName = TA_Base_Bus::PlanDataReader::instance()->getNodePath( category );
		std::string strParent = TA_Base_Bus::PlanDataReader::instance()->getNodeName( parent );
		AAMessageSender::instance()->submitAuditCategoryInsert( session, name, strParent );

		std::ostringstream auditMessage;
		auditMessage << "Category " << strName << "[Id:" << category << "] has been inserted into Category["  << strParent << "].";
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
	}

	void PlanDataWritingClass::deleteCategory(const std::string& session, TA_Base_Core::NodeId category)
	{
        std::string categoryPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(category);
		std::ostringstream message;

		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(category);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
		{
			message << "Error: The node[" << category << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_CATEGORY));
		}

		TA_Base_Core::NodeSummary * categorySummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(category);

		if ( categorySummary->hasChildren )
		{
            TA_THROW(TA_Base_Core::PlanAgentException("Error: Non empty category cannot be deleted.", TA_Base_Core::PAE_CANNOT_DELETE_NON_EMPTY_CATAGORY));
		}

		TA_Base_Core::NodeId parent = categorySummary->parent;

		std::string parentName = "ROOT";

		if ( parent != TA_Base_Core::ROOT_NODE_ID )
		{
			TA_Base_Core::NodeSummary* parentSummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(parent);
			parentName = parentSummary->name.in();
		}

        TA_Base_Bus::PlanDataWriter::instance()->deleteCategory(category);
		//Send Update
		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		if ( parent != TA_Base_Core::ROOT_NODE_ID )
		{
			TA_Base_Core::DeleteNodeUpdate_var dnu = new TA_Base_Core::DeleteNodeUpdate;
			dnu->parent = parent;
			dnu->nodes.length(1);
			dnu->nodes[0] = category;
			pcud->configUpdate.deleteCategory(dnu);
		}
		else
        {
			pcud->configUpdate.deleteCategoryFromRoot(category);
        }

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );			

		AAMessageSender::instance()->submitAuditCategoryDelete( session, categoryPath );

		std::ostringstream auditMessage;
		auditMessage << categorySummary->name.in() << "[Id:" << category << "] in Category" << parentName << "[" << parent << "] has been deleted.";
	    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
	}


    void PlanDataWritingClass::setPlanApprovalState(const std::string& session, TA_Base_Core::NodeId nodeId, TA_Base_Core::EApprovalState approvalState)
    {
        //Check that the Plan is not active now
        std::list<TA_Base_Core::ActivePlanId> planList;
        ActivePlanBulletinBoard::instance()->getActiveInstances( nodeId, planList );

        std::ostringstream what;

        if ( planList.size() != 0 )
        {
            what << "Plan: " << TA_Base_Bus::PlanDataReader::instance()->getNodeName(nodeId).c_str() <<" has at least one instance active";

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", what.str().c_str());
            TA_THROW(TA_Base_Core::PlanAgentException(what.str(), TA_Base_Core::PAE_PLAN_HAS_ACTIVE_INSTANCE));
        }

        if ( isPlanBeingEdited(nodeId) )
        {
			std::string editLockSession = getEditLockOwner(nodeId);
            what << "The Plan cannot be approved while being edited by " << ActivePlanBulletinBoard::instance()->getOwner(editLockSession) << ".";
            TA_THROW(TA_Base_Core::PlanAgentException(what.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)));
        }

        unsigned long planPKey = nodeId;

        try
        {
            TA_Base_Bus::PlanDataWriter::instance()->setPlanApprovalState(session, nodeId, approvalState);
        }
		catch ( ... )
		{
            std::ostringstream auditMessage;
            std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath( nodeId );

			if ( approvalState == TA_Base_Core::APPROVED_APPROVAL_STATE)
			{
			    AAMessageSender::instance()->submitAuditPlanApprovalFailure( session, strPlanName );

				auditMessage << "PlanApprovalFailure:" << strPlanName << "[Id:" << nodeId << "] could not be approved.";
			    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", auditMessage.str().c_str());
			}
			else if ( approvalState == TA_Base_Core::UNAPPROVED_APPROVAL_STATE)
			{
				auditMessage << "PlanUnApprovalFailure:" << strPlanName << "[Id:" << nodeId << "] could not be unapproved.";
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", auditMessage.str().c_str());
                AAMessageSender::instance()->submitAuditPlanUnapprovalFailure( session, strPlanName );
				std::string message = "Failed to unapprove a plan.";
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", message.c_str() );
				TA_THROW(  TA_Base_Core::PlanAgentException (message.c_str() ,TA_Base_Core::PAE_PLAN_UNAPPROVE_ERROR) );
			}
            return;
		}

		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		TA_Base_Core::ApprovalStateUpdate_var asu = new TA_Base_Core::ApprovalStateUpdate;
		asu->plan = nodeId;
		asu->approvalState = approvalState;

		pcud->configUpdate.approvalState(asu);

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );
        //Generate auditMessage for Approval/Unapproval

        std::string strName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(nodeId);

        std::ostringstream auditMessage;
        auditMessage << "has ";

		if ( approvalState == TA_Base_Core::APPROVED_APPROVAL_STATE)
		{
		    AAMessageSender::instance()->submitAuditPlanApproval( session, strName );
			auditMessage << "approved ";
		}
		else if ( approvalState == TA_Base_Core::UNAPPROVED_APPROVAL_STATE)
		{
		    AAMessageSender::instance()->submitAuditPlanUnapproval( session, strName );
			auditMessage << "unapproved ";
		}

        auditMessage << "the plan: " << strName << "[Id:" << nodeId << "]";
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
    }


	std::string PlanDataWritingClass::getNodeNameToCopy(TA_Base_Core::NodeId node, TA_Base_Core::NodeId parent)
	{
        return TA_Base_Bus::PlanDataWriter::instance()->getNodeNameToCopy(node, parent);
	}

	std::string PlanDataWritingClass::getNewNodeName(TA_Base_Core::NodeId parent, TA_Base_Core::ENode nodeType)
	{
		return TA_Base_Bus::PlanDataWriter::instance()->getNewNodeName(parent, nodeType);
	}


	std::string PlanDataWritingClass::parseForApostrophe(const std::string& inString)
	{
		std::string retValue="";
		int iPos = 0;
		int nextPos = 0;
		std::string temp="";

		while ( (nextPos = inString.find_first_of("'",iPos)) != std::string::npos ) 
		{
			temp = inString.substr(iPos, nextPos-iPos);
			retValue += temp;
			retValue += "''";
			iPos = nextPos+1;
		}

		if (retValue.empty())
			retValue = inString;
		else
			retValue += inString.substr(iPos, inString.length());

		return retValue;
	}

    std::string PlanDataWritingClass::makeNumericDataPointValueString(const TA_Base_Core::NumericDataPoint& ndp)
    {
        std::stringstream strmValue;
        strmValue << ndp.value;
        std::string strValue = strmValue.str();
        unsigned long precision = 0;

		std::string::size_type beginPos = 0;
		beginPos = strValue.find_first_of(".", beginPos);

		if (beginPos == std::string::npos)
        {
            precision = strValue.size();
        }
        else
        {
            precision = beginPos;
        }

	    std::ostringstream ostrstrm;
		ostrstrm << TA_Base_Core::NUMERIC_DATA_POINT << ",";
		ostrstrm << std::setprecision(ndp.precision + precision) << ndp.value << ",";
		ostrstrm << ndp.precision;

        return ostrstrm.str();
    }


    TA_Base_Core::NodeId PlanDataWritingClass::getPlanToReplace(const std::string& planName, TA_Base_Core::NodeId parent, bool useForce)
    {
		// get the details for the duplicate plan
		TA_Base_Core::NodeId duplicatePlan = TA_Base_Bus::PlanDataReader::instance()->getNodeId( parent, parseForApostrophe(planName) );
	
		TA_Base_Core::NodeSummary_var dupSummary =
			TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(duplicatePlan);

        // check that the existing plan is in fact a plan
        if (TA_Base_Core::CATEGORY_NODE == dupSummary->type)
        {
            std::ostringstream message;
			message << "Error: A subcategory with name ["
					<< planName
					<< "] already exists in the destination category.";
			TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_OVERWRITE_CATEGORY));
        }

		// check that the existing plan is not approved
		if (TA_Base_Core::APPROVED_APPROVAL_STATE == dupSummary->approvalState)
		{
            std::ostringstream message;
			message << "Error: An approved plan with name ["
					<< planName
					<< "] already exists in the destination category.";
			TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_OVERWRITE_APPROVED_PLAN));
		}

		// check that the existing plan is not being edited
		if (isPlanBeingEdited(duplicatePlan))
		{
            std::ostringstream message;
			std::string editLockSession = getEditLockOwner(duplicatePlan);
			message << "Error: The plan with name [" 
                    << planName
					<< "already exists in the destination category and it is being edited by "
					<<	ActivePlanBulletinBoard::instance()->getOwner(editLockSession);
			TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_OVERWRITE_EDITED_PLAN, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)));
		}

		if (!useForce)
		{
            std::ostringstream message;
			message << "Error: Plan with name ["
					<< planName
					<< "] already exists in the destination category."; 
			TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NAME_NOT_UNIQUE));
		}

        return duplicatePlan;
    }


    TA_Base_Core::StepNumber PlanDataWritingClass::getStepNumber(const TA_Base_Core::Plan& plan, TA_Base_Core::StepId step) const
    {
        unsigned long stepCount(plan.stepseq.length());

        for (unsigned long i(0); i < stepCount; i++)
		{
            const TA_Base_Core::StepDetail& stepDetail = plan.stepseq[i].detail;

			if (stepDetail.step == step)
			{
				return stepDetail.position;
			}
		}

        return 0;
    }

    void PlanDataWritingClass::setPlanType( const std::string& session, TA_Base_Core::NodeId nodeId, TA_Base_Core::EPlanType visibleState )
    {
        try
        {
            TA_Base_Bus::PlanDataWriter::instance()->deletePlanType(nodeId);
        }
        catch ( ... )
        {
        }

        bool bSuccess = false;

        try
        {
            bSuccess = TA_Base_Bus::PlanDataWriter::instance()->insertPlanType( session, nodeId, visibleState );
        }
        catch ( ... )
        {
        }

        if ( !bSuccess )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Set plan type fail for node %d and type %d", nodeId, visibleState);
        }
    }
}

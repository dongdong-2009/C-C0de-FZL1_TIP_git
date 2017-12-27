/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/WXL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanAccessFactory.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2014/04/04 10:54:00 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */

#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActiveStepsHeader.h"
#include "app/response_plans/plan_agent/src/PlanAccessFactory.h"

namespace TA_Base_App
{

	PlanAccessFactoryImpl::PlanAccessFactoryImpl()
	{
	}
	
    // Improve performance, load all steps in one sql
	void PlanAccessFactoryImpl::getActiveStepList( const unsigned long plan, IActivePlanFlow& flowControl, std::string& reason, std::list<ActiveStep*>& lstSteps ) const
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to load steps" );

		lstSteps.clear();

//  		TA_Base_Core::StepDetails_var stepDetails = TA_Base_Bus::PlanDataReader::instance()->getAllStepDetails( plan );
        TA_Base_Core::Steps_var varAllSteps = TA_Base_Bus::PlanDataReader::instance()->getAllSteps( plan );

		// Return an empty step list.
//         if( 0 == stepDetails->length() )
        if( 0 == varAllSteps->length() )
		{
			return;
		}

		TA_Base_Core::StepNumber stepIndex = 1;

        bool stepDefined = false;

		//TD20350 xinsong++
		int GRC3PrevStepIndex = -1; //the previous GRC3 step index 
		GRC3DataPointSetComplexStep * currentGRCType3Step = NULL;

// 		for( stepIndex = 1; stepIndex<= stepCount; stepIndex++ )
		for( stepIndex = 1; stepIndex<= varAllSteps->length(); stepIndex++ )
		{
// 			TA_Base_Core::StepParameters_var stepParameters;
            stepDefined = false;

// 			stepParameters = TA_Base_Bus::PlanDataReader::instance()->getStepParameters( plan, stepIndex, true );

//             switch( stepParameters->_d() )
            switch( varAllSteps[stepIndex-1].parameters._d() )
			{
				case TA_Base_Core::END_STEP:
				{
//                     lstSteps.push_back( new ActiveEndStep(stepDetails[stepIndex-1], flowControl) );
                    lstSteps.push_back( new ActiveEndStep(varAllSteps[stepIndex-1].detail, flowControl) );
                    stepDefined = true;

                    //if ( !stepDefined )
                    //{
                    //    reason = "Error: TA_Base_Core::END_STEP has not been defined.";
                    //}
				}
				break;

				case TA_Base_Core::ABORT_STEP:
				{
                    #if defined ( STEPTYPE_ABORT )

//                     lstSteps.push_back( new ActiveAbortStep(stepDetails[stepIndex-1], flowControl) );
                    lstSteps.push_back( new ActiveAbortStep(varAllSteps[stepIndex-1].detail, flowControl) );
                    stepDefined = true;

                    #else
					reason = "Error: TA_Base_Core::ABORT_STEP has not been defined.";
				    #endif // STEPTYPE_ABORT

                }
				break;

				case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
				{
					#if defined ( STEPTYPE_ACTIVE_PLAN_CHECK )

//                     lstSteps.push_back( new ActiveActivePlanCheckStep( stepDetails[stepIndex-1], stepParameters->activePlanCheck(), flowControl) );
                    lstSteps.push_back( new ActiveActivePlanCheckStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.activePlanCheck(), flowControl) );
                    stepDefined = true;
					#else
                    reason = "Error: TA_Base_Core::ACTIVE_PLAN_CHECK_STEP has not been defined.";
					#endif //STEPTYPE_ACTIVE_PLAN_CHECK
                }
				break;

				case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
				{
				    #if defined ( STEPTYPE_ASSIGN_CAMERA_TO_MONITOR )

//                     lstSteps.push_back( new ActiveAssignCameraToMonitorStep( stepDetails[stepIndex-1], stepParameters->cameraMonitor(), flowControl) );
                    lstSteps.push_back( new ActiveAssignCameraToMonitorStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.cameraMonitor(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP has not been defined.";

                    #endif //ASSIGN_CAMERA_TO_MONITOR_STEP
                }
				break;

				case TA_Base_Core::DATA_POINT_CHECK_STEP:
				{
					#if defined ( STEPTYPE_DATA_POINT_CHECK )

//                     lstSteps.push_back( new ActiveDataPointCheckStep(stepDetails[stepIndex-1], stepParameters->dataPointCheck(), flowControl) );
                    lstSteps.push_back( new ActiveDataPointCheckStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.dataPointCheck(), flowControl) );
                    stepDefined = true;
  

					#else
					reason = "Error: TA_Base_Core::DATA_POINT_CHECK_STEP has not been defined.";
					#endif //STEPTYPE_DATA_POINT_CHECK

                }
				break;

				case TA_Base_Core::DATA_POINT_SET_STEP:
				{
					#if defined ( STEPTYPE_DATA_POINT_SET )

                    ActiveStep * pActiveStep = NULL;

                    TA_Base_Core::StepDetail& refDetail = varAllSteps[stepIndex-1].detail;
                    bool bGroupStep = ( !refDetail.skip && refDetail.ignoreFailure && refDetail.nowait ); 

//                     ActiveDataPointSetStep * pDataPointSetStep = new ActiveDataPointSetStep(stepDetails[stepIndex-1], stepParameters->dataPointSet(), flowControl, fGRCType3Step );
					ActiveDataPointSetStep * pDataPointSetStep = new ActiveDataPointSetStep(refDetail, varAllSteps[stepIndex-1].parameters.dataPointSet(), flowControl, bGroupStep );
					if ( bGroupStep )
                    {
						if ( GRC3PrevStepIndex + 1 < stepIndex ) 
						{
//                             currentGRCType3Step = new GRC3DataPointSetComplexStep(stepDetails[stepIndex-1], flowControl);
                            currentGRCType3Step = new GRC3DataPointSetComplexStep(refDetail, flowControl);
                            currentGRCType3Step->setChildStep( pDataPointSetStep );
                            pActiveStep = currentGRCType3Step;
						}
						else
						{
//                             pActiveStep = new ActiveNullStep(stepDetails[stepIndex-1], flowControl);
                            pActiveStep = pDataPointSetStep;
						}
						currentGRCType3Step->addChildStep(pDataPointSetStep);
						GRC3PrevStepIndex = stepIndex;
					}
					else
					{
						pActiveStep = pDataPointSetStep;
					}

                    lstSteps.push_back( pActiveStep );
                    stepDefined = true;

                    #else
					reason = "Error: TA_Base_Core::DATA_POINT_SET_STEP has not been defined.";
				    #endif //STEPTYPE_DATA_POINT_SET

                }
				break;

				case TA_Base_Core::DECISION_STEP:
				{
					#if defined ( STEPTYPE_DECISION )
                    
//                     lstSteps.push_back( new ActiveDecisionStep(stepDetails[stepIndex-1], stepParameters->decision(), flowControl) );
                    lstSteps.push_back( new ActiveDecisionStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.decision(), flowControl) );
                    stepDefined = true;

                    #else
					reason = "Error: TA_Base_Core::DECISION_STEP has not been defined.";
                    #endif //STEPTYPE_DECISION

				}
				break;

				case TA_Base_Core::JUMP_STEP:
				{
					#if defined ( STEPTYPE_JUMP )

//                     lstSteps.push_back( new ActiveJumpStep(stepDetails[stepIndex-1], stepParameters->jump(), flowControl) );
                    lstSteps.push_back( new ActiveJumpStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.jump(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::JUMP_STEP has not been defined.";
                    #endif //STEPTYPE_JUMP
				}
				break;

				case TA_Base_Core::LOG_STEP:
				{
					#if defined ( STEPTYPE_LOG )

//                     lstSteps.push_back( new ActiveLogStep(stepDetails[stepIndex-1], stepParameters->log(), flowControl) );
                    lstSteps.push_back( new ActiveLogStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.log(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::LOG_STEP has not been defined.";
                    #endif //STEPTYPE_LOG
				}
				break;

				case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
				{
				    #if defined ( STEPTYPE_ACTIVATE_CAMERA_PRESET )

//                     lstSteps.push_back( new ActiveAssignCameraToPresetStep( stepDetails[stepIndex-1], stepParameters->cameraPreset(), flowControl) );
                    lstSteps.push_back( new ActiveAssignCameraToPresetStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.cameraPreset(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP has not been defined.";
                    #endif //STEPTYPE_ACTIVATE_CAMERA_PRESET
                }
				break;

				case TA_Base_Core::PROMPT_STEP:
				{
					#if defined ( STEPTYPE_PROMPT )

//                     lstSteps.push_back( new ActivePromptStep(stepDetails[stepIndex-1], stepParameters->prompt(), flowControl) );
                    lstSteps.push_back( new ActivePromptStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.prompt(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::PROMPT_STEP has not been defined.";

                    #endif //STEPTYPE_PROMPT
				}
				break;

				case TA_Base_Core::RUN_PLAN_STEP:
				{
					#if defined ( STEPTYPE_RUN_PLAN )

//                     lstSteps.push_back( new ActiveRunPlanStep( stepDetails[stepIndex-1], stepParameters->runPlan(), flowControl) );
                    lstSteps.push_back( new ActiveRunPlanStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.runPlan(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::RUN_PLAN_STEP has not been defined.";

                    #endif //STEPTYPE_RUN_PLAN
				}
				break;

				case TA_Base_Core::TERMINATE_PLAN_STEP:
				{
					#if defined ( STEPTYPE_TERMINATE_PLAN )

//                     lstSteps.push_back( new ActiveTerminatePlanStep( stepDetails[stepIndex-1], stepParameters->terminatePlan(), flowControl) );
                    lstSteps.push_back( new ActiveTerminatePlanStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.terminatePlan(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::TERMINATE_PLAN_STEP has not been defined.";
                    #endif //STEPTYPE_TERMINATE_PLAN
				}
				break;

				case TA_Base_Core::VMS_SET_SCENE_STEP:
				{
                    #if defined ( STEPTYPE_VMS_SET_SCENE )
//                     lstSteps.push_back( new ActiveVmsSetSceneStep( stepDetails[stepIndex-1], stepParameters->vmsSetScene(), flowControl) );
                    lstSteps.push_back( new ActiveVmsSetSceneStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.vmsSetScene(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::VMS_SET_SCENE_STEP has not been defined.";
                    #endif //STEPTYPE_VMS_SET_SCENE
				}
				break;

				case TA_Base_Core::LAUNCH_GUI_APP_STEP:
				{
                    #if defined ( STEPTYPE_LAUNCH_GUI_APP )
//                     lstSteps.push_back( new ActiveLaunchGuiAppStep( stepDetails[stepIndex-1], stepParameters->guiAppParameters(), flowControl) );
                    lstSteps.push_back( new ActiveLaunchGuiAppStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.guiAppParameters(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::LAUNCH_GUI_APP_STEP has not been defined.";
                    #endif //STEPTYPE_LAUNCH_GUI_APP
				}
				break;

                case TA_Base_Core::VMS_BLANKOUT_STEP:
                {
                    #if defined ( STEPTYPE_VMS_BLANKOUT )
//                     lstSteps.push_back( new ActiveVmsBlankoutStep( stepDetails[stepIndex-1], stepParameters->vmsBlankout(), flowControl) );
                    lstSteps.push_back( new ActiveVmsBlankoutStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.vmsBlankout(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::VMS_BLANKOUT_STEP has not been defined.";
                    #endif //STEPTYPE_VMS_BLANKOUT
                }
                break;

                case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
                {
                    #if defined ( STEPTYPE_VMS_RESTORE_SCHEDULE )
//                     lstSteps.push_back( new ActiveVmsRestoreScheduleStep( stepDetails[stepIndex-1], stepParameters->vmsRestoreSchedule(), flowControl) );
                    lstSteps.push_back( new ActiveVmsRestoreScheduleStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.vmsRestoreSchedule(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP has not been defined.";
                    #endif //STEPTYPE_VMS_RESTORE_SCHEDULE
                }
                break;

                case TA_Base_Core::VIDEO_WALL_STEP:
                {
                    #if defined ( STEPTYPE_VIDEOWALL )
//                     lstSteps.push_back( new ActiveVideoWallStep( stepDetails[stepIndex-1], stepParameters->videoWallParams(), flowControl) );
                    lstSteps.push_back( new ActiveVideoWallStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.videoWallParams(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::VIDEO_WALL_STEP has not been defined.";
                    #endif //STEPTYPE_VIDEOWALL
                }
                break;

                case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
                {
                    #if defined ( STEPTYPE_STATION_PUBLIC_ADDRESS )
//                     lstSteps.push_back( new ActiveStationPaStep( stepDetails[stepIndex-1], stepParameters->stationPublicAddress(), flowControl) );
                    lstSteps.push_back( new ActiveStationPaStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.stationPublicAddress(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP has not been defined.";
                    #endif //STEPTYPE_STATION_PUBLIC_ADDRESS
                }
                break;

                case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
                {
                    #if defined ( STEPTYPE_TRAIN_PUBLIC_ADDRESS )
//                     lstSteps.push_back( new ActiveTrainPaStep( stepDetails[stepIndex-1], stepParameters->trainPublicAddress(), flowControl) );
                    lstSteps.push_back( new ActiveTrainPaStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.trainPublicAddress(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP has not been defined.";
                    #endif //STEPTYPE_TRAIN_PUBLIC_ADDRESS
                }
                break;

                case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
                {
                    #if defined ( STEPTYPE_SEND_SMS_MESSAGE )
//                     lstSteps.push_back( new ActiveSendSmsMessageStep( stepDetails[stepIndex-1], stepParameters->sendSmsMessage(), flowControl ) );
                    lstSteps.push_back( new ActiveSendSmsMessageStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.sendSmsMessage(), flowControl ) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::SEND_SMS_MESSAGE_STEP has not been defined.";
                    #endif //STEPTYPE_SEND_SMS_MESSAGE
                }
                break;

				case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
				{
                    #if defined ( STEPTYPE_LAUNCH_SCHEMATIC )
//                     lstSteps.push_back( new ActiveLaunchSchematicStep( stepDetails[stepIndex-1], stepParameters->launchSchematic(), flowControl) );
                    lstSteps.push_back( new ActiveLaunchSchematicStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.launchSchematic(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::LAUNCH_SCHEMATIC_STEP has not been defined.";
                    #endif //STEPTYPE_LAUNCH_SCHEMATIC
				}
				break;

				case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
				{
					#if defined ( STEPTYPE_EVALUATE_EXPRESSION)
//                     lstSteps.push_back( new ActiveEvaluateExpressionStep( stepDetails[stepIndex-1], stepParameters->evaluateExpression(), flowControl) );
                    lstSteps.push_back( new ActiveEvaluateExpressionStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.evaluateExpression(), flowControl) );
					stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::EVALUATE_EXPRESSION_STEP has not been defined.";
					#endif //STEPTYPE_EVALUATE_EXPRESSION
				}
				break;

				case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
				{
					#if defined ( STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM )
//                     lstSteps.push_back( new ActiveStationTisStep(stepDetails[stepIndex-1], stepParameters->stationTravellerInfoSystem(), flowControl) );
                    lstSteps.push_back( new ActiveStationTisStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.stationTravellerInfoSystem(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP has not been defined.";
				    #endif //STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM
                }
				break;

				case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
				{
					#if defined ( STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM )
//                     lstSteps.push_back( new ActiveTrainTisStep(stepDetails[stepIndex-1], stepParameters->trainTravellerInfoSystem(), flowControl) );
                    lstSteps.push_back( new ActiveTrainTisStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.trainTravellerInfoSystem(), flowControl) );
                    stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP has not been defined.";
				    #endif //STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM
                }
				break;
				// lizettejl++ Failure Step Summary (DP-changes)
				case TA_Base_Core::FAILURE_SUMMARY_STEP:
				{
					#if defined ( STEPTYPE_FAILURE_SUMMARY_STEP )
//                     lstSteps.push_back( new  ActiveFailureSummaryStep(stepDetails[stepIndex-1], stepParameters->failureSummary(), flowControl) );
                    lstSteps.push_back( new  ActiveFailureSummaryStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.failureSummary(), flowControl) );
					stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::FAILURE_SUMMARY_STEP has not been defined.";
					#endif // STEPTYPE_FAILURE_SUMMARY_STEP 
				}
				break;
				//++lizettejl
				//WuZhongyi New Step Fire Alarm Station PA
				case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP_BY_ALARM:
				{
					#if defined ( STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM )
//                     stepList->push_back( new ActiveStationPaByAlarmStep(stepDetails[stepIndex-1], stepParameters->stationPublicAddrssByAlarm(), flowControl) );
                    lstSteps.push_back( new  ActiveStationPaByAlarmStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.stationPublicAddrssByAlarm(), flowControl) );
					stepDefined = true;
                    #else
					reason = "Error: TA_Base_Core::STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM has not been defined.";
                    #endif // STATION_PUBLIC_ADDRESS_STEP_BY_ALARM 
				}
				break; 
				
                case TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP:
                {
                #if defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)
                lstSteps.push_back(new ActiveBackgroundLaunchSchematicStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.bgLaunchSchematic(), flowControl));
                stepDefined = true;
                #else
                reason = "Error: TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP has not been defined.";
                #endif // STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC 
                }
                break;

                case TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP:
                {
                #if defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)
                lstSteps.push_back(new ActiveBackgroundLaunchGuiAppStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.bgLaunchGuiApp(), flowControl));
                stepDefined = true;
                #else
                reason = "Error: TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP has not been defined.";
                #endif // STEPTYPE_BACKGROUND_LAUNCH_GUI_APP 
                }
                break;

                case TA_Base_Core::BACKGROUND_DECISION_STEP:
                {
                #if defined (STEPTYPE_BACKGROUND_DECISION)
                lstSteps.push_back(new ActiveBackgroundDecisionStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.bgDecision(), flowControl));
                stepDefined = true;
                #else
                reason = "Error: TA_Base_Core::BACKGROUND_DECISION_STEP has not been defined.";
                #endif // STEPTYPE_BACKGROUND_DECISION 
                }
                break;

                case TA_Base_Core::BACKGROUND_LOG_STEP:
                {
                #if defined (STEPTYPE_BACKGROUND_LOG)
                lstSteps.push_back(new ActiveBackgroundLogStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.bgLog(), flowControl));
                stepDefined = true;
                #else
                reason = "Error: TA_Base_Core::BACKGROUND_LOG_STEP has not been defined.";
                #endif // STEPTYPE_BACKGROUND_LOG 
                }
                break;

                case TA_Base_Core::BACKGROUND_PROMPT_STEP:
                {
                #if defined (STEPTYPE_BACKGROUND_PROMPT)
                lstSteps.push_back(new ActiveBackgroundPromptStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.bgPrompt(), flowControl));
                stepDefined = true;
                #else
                reason = "Error: TA_Base_Core::BACKGROUND_PROMPT_STEP has not been defined.";
                #endif // STEPTYPE_BACKGROUND_PROMPT 
                }
                break;

				case TA_Base_Core::UNDEFINED_STEP:
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "found undefined step" );
                    stepDefined = true;
				}
				break; 
			}

            if ( !stepDefined )
            {
                break;
            }
		}

        if ( stepDefined )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loaded %lu steps", lstSteps.size() );
        }
        else
        {
		    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Failed to load all steps, unusable steptype encountered: %s", reason.c_str() );
        }
	}

} // TA_Base_App

//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/common/src/StringConstants.h $
// @author:  Mahaveer Pareek
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2015/01/23 16:58:54 $
// Last modified by:  $Author: qi.huang $
//
// This file contains project-specific preprocessor definitions used 
// for conditional compilation of Plan Agent and Plan Manager.
//
// Each project should uncomment the relevant PROJECT_XXXX definition.
//

#ifndef STRING_CONSTANTS_HEADER_INCLUDED
#define STRING_CONSTANTS_HEADER_INCLUDED

const char* const RemarkPlanManagerIsNotRegistered = "Ԥ��������û�гɹ�ע��";
const char* const RemarkWaitingForOperatorResponse = "�Ⱥ����Ա����";
const char* const RemarkPlanPausedByOperator = "Ԥ��������Ա��ͣ";
const char* const RemarkPlanCompletedWithFailedSteps = "Ԥ����ɵ���ʧ�ܲ���";
const char* const RemarkPlanCompletedSuccess = "Ԥ���ɹ����";
const char* const RemarkStepExecutionSucceed = "����ִ�гɹ�";
const char* const RemarkStepExecutionFailed = "����ִ��ʧ��";
const char* const RemarkStepExecutionTimeout = "����ִ�г�ʱ";
const char* const RemarkWaitingForAppToBeLaunched = "�ȴ�Ӧ�ó���ִ��";
const char* const RemarkNoPlanManagerForOperatorInput = "û������Ԥ����������������߽���";
const char* const RemarkExecutingStep = "����ִ�в���";
const char* const RemarkExecutingNoWaitSteps = "����ִ�в���";
const char* const RemarkPlanStepExecuted = "����ִ�����";
const char* const RemarkPlanAbort = "Ԥ����ֹ";
const char* const RemarkPlanPaused = "Ԥ����ͣ";
const char* const RemarkPlanResumed = "Ԥ���ָ�";
const char* const RemarkPlanExpired = "Ԥ������";
const char* const RemarkCanNotFoundVideoSwitchAgent = "�Ҳ�����Ƶ����";
const char* const RemarkCanNotLockCamera = "�޷��õ�����ͷ";
const char* const RemarkAttemptCompareDPwithNonfloatValue = "���Ը������������ݵ���Ǹ���ֵ�Ƚ�";
const char* const RemarkAttemptCompareDPwithNonBoolValue = "���Բ����������ݵ���ǲ���ֵ�Ƚ�";
const char* const RemarkAttemptCompareDPwithNonTextValue = "�����ı��������ݵ�����ı�ֵ�Ƚ�";
const char* const RemarkInvalidDatapointValue = "��Ч�����ݵ�ֵ";
const char* const RemarkBadDatapointQuality = "��Ч�����ݵ�ֵ";
const char* const RemarkDataPointTimeLaterThanNow = "�豸ϵͳʱ������ۺϼ��ϵͳʱ��";
const char* const RemarkInvalidDatapoint = "���ݵ��޷�����";
const char* const RemarkInvalidStepParameter = "��Ч��Ԥ������";
const char* const RemarkPlanManagerInteractiveActionError = "Ԥ��������ִ�н�������ʧ��";
const char* const RemarkInvalidExpress = "��Ч�ı��ʽ";
const char* const RemarkApplicationUnableToLaunch = "�����޷�ִ��";
const char* const RemarkWaitingSchematicLaunch = "�ȴ�HMI�˻�����ִ��/�л�";
const char* const RemarkSchematicCanNotLaunch = "HMI�˻������޷�ִ��/�л�";
const char* const RemarkWaitPlanControllerLaunch = "�ȴ�Ԥ�����������ִ��";
const char* const RemarkPlanControllerChildPlanCanNotLaunch = "Ԥ������������޷�����";
const char* const RemarkNoFoundPlan = "û�з���Ԥ��";
const char* const RemarkRunPlanFailed = "ִ��Ԥ��ʧ��";
const char* const RemarkPlanStepSkipped = "���豻����";
const char* const RemarkWriteDataPointFailed = "д���ݵ�ʧ��";
const char* const RemarkAccessDataPointDenied = "��Ȩ�޷������ݵ�";
const char* const RemarkCustomizationInProgress = "������";
const char* const RemarkPlanStarted = "Ԥ����ʼ";
const char* const RemarkNoMftResponse = "û�п���̨��Ӧ����";
const char* const RemarkOperatorStopPlan = "ֹͣ��Ԥ��";
const char* const RemarkOperatorTookControl = "�ӹ���Ԥ��";
const char* const RemarkDssEngineStopPlan = "����Ԥ������ֹͣ��Ԥ��";
const char* const RemarkSystemStopPlan = "Ԥ�����ڷ���˱��رն���ֹ";

#endif // STRING_CONSTANTS_HEADER_INCLUDED

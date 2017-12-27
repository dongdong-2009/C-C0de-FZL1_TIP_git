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

const char* const RemarkPlanManagerIsNotRegistered = "预案管理器没有成功注册";
const char* const RemarkWaitingForOperatorResponse = "等候操作员操作";
const char* const RemarkPlanPausedByOperator = "预案被操作员暂停";
const char* const RemarkPlanCompletedWithFailedSteps = "预案完成但有失败步骤";
const char* const RemarkPlanCompletedSuccess = "预案成功完成";
const char* const RemarkStepExecutionSucceed = "步骤执行成功";
const char* const RemarkStepExecutionFailed = "步骤执行失败";
const char* const RemarkStepExecutionTimeout = "步骤执行超时";
const char* const RemarkWaitingForAppToBeLaunched = "等待应用程序被执行";
const char* const RemarkNoPlanManagerForOperatorInput = "没有运行预案管理器可与操作者交互";
const char* const RemarkExecutingStep = "正在执行步骤";
const char* const RemarkExecutingNoWaitSteps = "并发执行步骤";
const char* const RemarkPlanStepExecuted = "步骤执行完毕";
const char* const RemarkPlanAbort = "预案中止";
const char* const RemarkPlanPaused = "预案暂停";
const char* const RemarkPlanResumed = "预案恢复";
const char* const RemarkPlanExpired = "预案过期";
const char* const RemarkCanNotFoundVideoSwitchAgent = "找不到视频服务";
const char* const RemarkCanNotLockCamera = "无法得到摄像头";
const char* const RemarkAttemptCompareDPwithNonfloatValue = "尝试浮点数类型数据点与非浮点值比较";
const char* const RemarkAttemptCompareDPwithNonBoolValue = "尝试布尔类型数据点与非布尔值比较";
const char* const RemarkAttemptCompareDPwithNonTextValue = "尝试文本类型数据点与非文本值比较";
const char* const RemarkInvalidDatapointValue = "无效的数据点值";
const char* const RemarkBadDatapointQuality = "无效的数据点值";
const char* const RemarkDataPointTimeLaterThanNow = "设备系统时间迟于综合监控系统时间";
const char* const RemarkInvalidDatapoint = "数据点无法访问";
const char* const RemarkInvalidStepParameter = "无效的预案参数";
const char* const RemarkPlanManagerInteractiveActionError = "预案管理器执行交互操作失败";
const char* const RemarkInvalidExpress = "无效的表达式";
const char* const RemarkApplicationUnableToLaunch = "程序无法执行";
const char* const RemarkWaitingSchematicLaunch = "等待HMI人机界面执行/切换";
const char* const RemarkSchematicCanNotLaunch = "HMI人机界面无法执行/切换";
const char* const RemarkWaitPlanControllerLaunch = "等待预案管理控制器执行";
const char* const RemarkPlanControllerChildPlanCanNotLaunch = "预案管理控制器无法启动";
const char* const RemarkNoFoundPlan = "没有发现预案";
const char* const RemarkRunPlanFailed = "执行预案失败";
const char* const RemarkPlanStepSkipped = "步骤被跳过";
const char* const RemarkWriteDataPointFailed = "写数据点失败";
const char* const RemarkAccessDataPointDenied = "无权限访问数据点";
const char* const RemarkCustomizationInProgress = "定制中";
const char* const RemarkPlanStarted = "预案开始";
const char* const RemarkNoMftResponse = "没有控制台响应请求";
const char* const RemarkOperatorStopPlan = "停止了预案";
const char* const RemarkOperatorTookControl = "接管了预案";
const char* const RemarkDssEngineStopPlan = "根据预案规则停止了预案";
const char* const RemarkSystemStopPlan = "预案由于服务端被关闭而终止";

#endif // STRING_CONSTANTS_HEADER_INCLUDED

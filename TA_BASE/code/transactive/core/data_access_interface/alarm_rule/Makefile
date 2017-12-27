# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/alarm_rule
PROJ_NAME	= TA_AlarmRuleDAI
PROJ_SOURCE	= \
		  AlarmRuleAccessFactory.cpp \
		  AlarmRuleActionParameterHelper.cpp \
		  AlarmRuleData.cpp \
		  AlarmRuleHelper.cpp \
		  AutomaticAcknowledgementRuleHelper.cpp \
		  AutomaticAcknowlegementRuleData.cpp \
		  AvalancheSuppressionRuleData.cpp \
		  AvalancheSuppressionRuleHelper.cpp \
		  IAlarmRuleData.cpp \
		  PlanExecutionRuleData.cpp \
		  PlanExecutionRuleHelper.cpp \
		  SuppressionRuleData.cpp \
                  SuppressionByDescriptionRuleData.cpp \
		  SuppressionRuleHelper.cpp \
		  MMSRuleData.cpp \
		  MMSRuleHelper.cpp \
		  CCTVRuleData.cpp \
		  CCTVRuleHelper.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing

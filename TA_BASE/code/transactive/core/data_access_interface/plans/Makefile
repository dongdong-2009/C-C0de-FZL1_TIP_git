# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/plans
PROJ_NAME	= TA_PlanDAI
PROJ_SOURCE	= AlarmPlanAssocData.cpp \
		  AlarmPlanAssocHelper.cpp \
		  AlarmPlanAssocAccessFactory.cpp \
	 	  ConfigAlarmPlanAssoc.cpp \
		  PlanAccessFactory.cpp \
		  PlanDAI.cpp\
                  ConfigEventPlanAssocData.cpp \
                  EventPlanAssocData.cpp \
                  EventPlanAssocHelper.cpp \
                  EventPlanAssocAccessFactory.cpp\
                  PlanDssDpRuleHelper.cpp\
                  PlanDssDpRule.cpp\
                  ConfigPlanDssDpRule.cpp\
                  PlanDssDpRuleAccessFactory.cpp
                  
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
                      $(PROJ_ROOT_DIR)/code/transactive/core/message/IDL

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing

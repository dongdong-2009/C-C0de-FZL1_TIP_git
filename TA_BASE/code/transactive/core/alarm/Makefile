# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
# Make file for the Alarm classes.
#

PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/alarm
PROJ_NAME	= TA_Alarm
PROJ_SOURCE	= \
    		AlarmHelper.cpp \
    		AlarmConfig.cpp \
    		AlarmHelperBase.cpp \
    		AlarmHelperManager.cpp \
    		AlarmPublicationManager.cpp \
    		AlarmSender.cpp \
    		AlarmSubscriptionManager.cpp \
    		NonUniqueAlarmHelper.cpp \
			AlarmModificationHelper.cpp \
			AlarmReceiver.cpp \
			CtaAlarms.cpp \
			Utilities.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
    $(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

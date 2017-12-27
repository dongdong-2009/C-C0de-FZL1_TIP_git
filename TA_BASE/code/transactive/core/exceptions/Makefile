# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
# This file builds the exceptions library.
#
#
PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/exceptions
PROJ_NAME	= TA_Exceptions
PROJ_SOURCE	= \
		AlarmActionException.cpp \
		AlarmAgentException.cpp \
		AlarmCacheCapacityExceededException.cpp \
		ArchiveException.cpp \
		AssertException.cpp \
		AtsAgentException.cpp \
		AuthenticationAgentException.cpp \
		AuthenticationSecurityException.cpp \
		CannotUseSysControllerException.cpp \
		CtaRuntimeEntityParameterNotFoundException.cpp \
		DatabaseException.cpp \
		DataConfigurationException.cpp \
		DataException.cpp \
		DbConnectionFailed.cpp \
		DbConnectionNoFile.cpp \
		DbUniqueConstraintViolated.cpp \
		DutyAgentException.cpp \
		EntityTypeException.cpp \
		FailoverException.cpp \
		GenericAgentException.cpp \
		GenericApplicationException.cpp \
		InterlockingException.cpp \
		InvalidCommandLineException.cpp \
		ManagedProcessException.cpp \
		MathematicalEvaluationException.cpp \
		NameNotFoundException.cpp \
		NoAlarmAgentException.cpp \
		ObjectResolutionException.cpp \
		PlanAgentException.cpp \
		PlanStepDetailException.cpp \
		PlanStepException.cpp \
		PolicyDecisionPointException.cpp \
		ProcessAlreadyRunningException.cpp \
		RightsException.cpp \
		ScadaCommonException.cpp \
		ScadaProxyException.cpp \
		SocketException.cpp \
		SystemControllerException.cpp \
		TcpSocketException.cpp \
		TransactiveException.cpp \
		UserCancelException.cpp \
		UtilitiesException.cpp \
		VideoSwitchAgentInternalException.cpp \
		FatalConfigurationException.cpp \
		ApplicationException.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
    $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

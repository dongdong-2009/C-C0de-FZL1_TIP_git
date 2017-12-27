# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/security_utilities/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
# This file builds the TA_SecurityUtilities library
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/security/security_utilities
PROJ_NAME	= TA_SecurityUtilities
PROJ_SOURCE	= ISessionUpdateObserver.cpp \
		  SessionUpdateSubject.cpp \
            	  IDutyUpdateObserver.cpp \
            	  DutyUpdateSubject.cpp \
                  SessionDetailsCache.cpp \
                  TimePair.cpp \
                  TimePairMapBuilder.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_library/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/security/rights_library
PROJ_NAME	= TA_RightsLibrary
PROJ_SOURCE	= CorbaRightsLibrary.cpp \
	  	  RightsLibrary.cpp \
		  DutyCommsSubscriber.cpp \
		  LocSubDbCache.cpp \
		  RightsCache.cpp \
		  RightsQuery.cpp \
		  CtaRights.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
                      $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
                      $(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
		      $(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
		      $(PROJ_ROOT_DIR)/code/transactive/core/naming

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

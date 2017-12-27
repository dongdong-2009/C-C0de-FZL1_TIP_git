# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/PRIV_RealTimeEvent/TA_BASE/transactive/bus/event/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2013/07/10 17:37:28 $
# Last modified by:  $Author: CM $
#
# Make file for the GenericAgent classes.
#

PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/bus/event
PROJ_NAME	= TA_Bus_Event
PROJ_SOURCE	= SessionHelper.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS	=TA_AuthenticationCorbaDef

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

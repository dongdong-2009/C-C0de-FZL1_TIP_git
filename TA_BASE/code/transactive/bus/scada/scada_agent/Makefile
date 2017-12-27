# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
#	Last modification: $DateTime: 2017/02/06 10:52:23 $
#	Last mofified by:  $Author: Dhanshri $
#
# Make file for the GenericAgent classes.
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/scada_agent
PROJ_NAME	= TA_ScadaAgent
PROJ_SOURCE	=  \
			ScadaAgentUser.cpp \
			ScadaAgent.cpp 

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES =   $(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
                        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL/ \
	                $(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
	                $(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
	                $(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/event_triggered

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

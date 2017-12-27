# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/4669_Review_Branch/TA_BASE/transactive/core/datalog/IDL/Makefile $ 
# Author:  Ripple
# Version: $Revision: #3 $
#
# Last modification: $DateTime: 2010/12/17 11:17:43 $
# Last modified by:  $Author: grace.koh $

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/datalog/IDL
PROJ_NAME	= TA_DataLogAgentCorbaDef
PROJ_IDL	= \
       		  DataLogMessageCorbaDef.idl \
       		  DataLogAgentCorbaDef.idl
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

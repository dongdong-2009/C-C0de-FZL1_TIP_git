# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/CODE_TA_BASE_DDB/transactive/core/uuid/Makefile $ 
# Author:  Ripple
# Version: $Revision: #2 $
#
# Last modification: $DateTime: 2006/10/17 17:37:13 $
# Last modified by:  $Author: grace.koh $
#
# This file builds the Miuuid library.
#
PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/ocl
PROJ_NAME	= TA_ocl
PROJ_SOURCE	= crdef.cpp \
		crdb.cpp \
		oracall.cpp \
		ora.cpp \
		crparser.cpp \
		orapars.cpp \
		oramonitor.cpp \
		oraloader.cpp \
		orascript.cpp



PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing


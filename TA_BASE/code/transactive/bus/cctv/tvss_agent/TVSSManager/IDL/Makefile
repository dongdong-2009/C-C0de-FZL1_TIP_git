# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/cctv/video_switch_agent/IDL/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2009/12/29 14:17:48 $
# Last modified by:  $Author: grace.koh $
#
# Makefile for the TVSSManagerCorbaDef IDL classes.
#

PROJ_ROOT_DIR	= ../../../../../../..
PROJ_CODE_DIR	= transactive/bus/cctv/tvss_agent/TVSSManager/IDL
PROJ_NAME		= TA_TVSSManagerCorbaDef
PROJ_IDL		= \
           	  	TVSSManagerCorbaDef.idl 
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive
PROJ_TYPE		= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
                $(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

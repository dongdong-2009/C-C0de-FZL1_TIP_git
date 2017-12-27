# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/PRIV_RealTimeEvent/TA_BASE/transactive/core/alarm/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2013/07/10 17:37:28 $
# Last modified by:  $Author: CM $
#
# Make file for the Alarm classes.
#

PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/event
PROJ_NAME	= TA_Event
PROJ_SOURCE	= \
    		EventHelper.cpp \
    		EventHelperException.cpp \
    		EventUtilities.cpp \
			EventDeSerialize.cpp \
			EventSerialize.cpp \
			EventZlibDeflate.cpp \
			EventZlibInflate.cpp
			
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
    $(PROJ_ROOT_DIR)/code/transactive/core/event/IDL \
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

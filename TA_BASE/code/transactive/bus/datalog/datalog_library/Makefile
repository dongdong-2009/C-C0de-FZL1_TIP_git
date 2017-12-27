# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3002_TIP/TA_BASE/transactive/bus/data/datalog_library/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2011/01/04 14:58:58 $
# Last modified by:  $Author: luohuirong $
#
# This file builds the TA_DataLogLibrary library.
#
#
PROJ_ROOT_DIR   = ../../../../..
PROJ_CODE_DIR   = transactive/bus/datalog/datalog_library
PROJ_NAME   = TA_DataLogLibrary
PROJ_SOURCE = \
            DataLogManager.cpp \
            DataLogAgentImpl.cpp \
	    DataLogValue.cpp DataLogQueue.cpp DataLogDPItem.cpp DataLogCategory.cpp \
	    ScadaDataProxy.cpp 

PROJ_TYPE   = lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
 

DEPENDENT_LIBRARIES = \
$(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
$(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL \
$(PROJ_ROOT_DIR)/code/transactive/core/naming \
$(PROJ_ROOT_DIR)/code/transactive/core/datalog/IDL \
$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL \
$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL



PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
    #Do Nothing

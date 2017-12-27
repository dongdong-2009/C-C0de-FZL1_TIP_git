# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/corba/IDL/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
# Make file for the TA_Corba CorbaDef classes.
#
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/corba/IDL
PROJ_NAME	= TA_CorbaDef
PROJ_IDL	= IChannelLocatorCorbaDef.idl IFederatableEventChannel.idl
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)


# We need to compile IFederatableEventChannel.idl with different options to other idl files, as we need to 
# reference AttNotification.idl which a) is not in the normal OmniNotify header file location, and b) needs
# to be compiled with -Wbh=.hh (because OmniNotify headers with .hh suffix need to be included by 
# AttNotification.hh.
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefileDefs.mak

$(PROJ_IDL_DIR)/IFederatableEventChannel.cpp: $(PROJ_SRC_DIR)/IFederatableEventChannel.idl
	@echo "Generating skeletons for $@"
	@test -d $(PROJ_IDL_DIR) || $(MD) $(PROJ_IDL_DIR)
	$(OMNI_BIN)/omniidl -C$(PROJ_IDL_DIR) -bcxx -Wbkeep_inc_path -Wba -I$(PROJ_SRC_DIR) -I$(PROJ_IDL_INC) -I. -I$(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/src/services/omniNotify/idl -I$(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/idl/COS -I$(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/src/lib/omniORB -Wbh=.hh -Wbs=.cpp -Wbd=_skel.cpp $<
	@echo "Done"

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefileRules.mak

install:
	#Do Nothing

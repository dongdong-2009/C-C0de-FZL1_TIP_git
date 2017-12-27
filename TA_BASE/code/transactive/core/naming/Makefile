# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/naming/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
# Make file for the Naming classes.
#
#

PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/naming
PROJ_NAME	= TA_Naming
PROJ_SOURCE	= Naming.cpp \
    		  BindingRequestListener.cpp \
		  BindingHandler.cpp \
		  AbstractNamedObject.cpp \
		  NamingThreadPoolSingleton.cpp \
		  NamingHeartbeatMonitor.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
	#Do Nothing

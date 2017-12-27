# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/PRIV_4669_TRAIN_ECP/CODE_TA_BASE_DDB/transactive/bus/trains/TrainAgentCorba/idl/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2007/11/02 10:17:14 $
# Last modified by:  $Author: buildteam $
#
# Make file for the Train Corba Def classes.
#
PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/bus/trains/TrainAgentCorba/idl
PROJ_NAME	= TA_TrainAgentCorba
PROJ_IDL	= \
				ITrainCommonCorba.idl \
				ITrainExceptionsCorba.idl \
				ITrainCctvCorba.idl \
				ITrainDriverCallCorba.idl \
				ITrainInformationCorba.idl \
				ITrainOperationModeCorba.idl \
				ITrainPaCorba.idl \
				ITrainPecCorba.idl \
				ITrainTisCorba.idl \
				ITrainRadioGroupCorba.idl \
				ITrainStateUpdateCorba.idl

PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/radio/RadioAgentCorbaDef_4669/idl 	

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

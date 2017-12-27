# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/pa
PROJ_NAME	= TA_PADAI


PROJ_SOURCE	= \
ConfigPaAtsTriggeredBroadcast.cpp  \
ConfigPaBroadcastShortcut.cpp  \
ConfigPaDvaMessage.cpp  \
ConfigPaDvaMessageVersion.cpp  \
ConfigPaFasTriggeredBroadcast.cpp  \
ConfigPaScheduleBroadcast.cpp  \
ConfigPaStation.cpp  \
ConfigPaTisMessageMap.cpp  \
ConfigPaTisZoneMap.cpp  \
ConfigPaTrainDvaMessage.cpp  \
ConfigPaZone.cpp  \
ConfigPaZoneGroup.cpp  \
DatabaseQueryHelper.cpp  \
PaAtsTriggeredBroadcast.cpp  \
PaAtsTriggeredBroadcastAccessFactory.cpp  \
PaAtsTriggeredBroadcastHelper.cpp  \
PaBroadcastShortcut.cpp  \
PaBroadcastShortcutAccessFactory.cpp  \
PaBroadcastShortcutHelper.cpp  \
PaDvaMessage.cpp  \
PaDvaMessageAccessFactory.cpp  \
PaDvaMessageHelper.cpp  \
PaDvaMessageVersion.cpp  \
PaDvaMessageVersionAccessFactory.cpp  \
PaDvaMessageVersionHelper.cpp  \
PaFasTriggeredBroadcast.cpp  \
PaFasTriggeredBroadcastAccessFactory.cpp  \
PaFasTriggeredBroadcastHelper.cpp  \
PaFireCountdownAccessFactory.cpp  \
PaPagingConsoleAccessFactory.cpp  \
PaPriorityGroupAccessFactory.cpp  \
PaScheduleBroadcast.cpp  \
PaScheduleBroadcastAccessFactory.cpp  \
PaScheduleBroadcastHelper.cpp  \
PaStation.cpp  \
PaStationAccessFactory.cpp  \
PaStationHelper.cpp  \
PaSysParamAccessFactory.cpp  \
PaTisMessageMap.cpp  \
PaTisMessageMapHelper.cpp  \
PaTisMessageMappingAccessFactory.cpp  \
PaTisZoneMap.cpp  \
PaTisZoneMapHelper.cpp  \
PaTisZoneMappingAccessFactory.cpp  \
PaTrainDvaMessage.cpp  \
PaTrainDvaMessageAccessFactory.cpp  \
PaTrainDvaMessageHelper.cpp  \
PaZone.cpp  \
PaZoneAccessFactory.cpp  \
PaZoneGroup.cpp  \
PaZoneGroupAccessFactory.cpp  \
PaZoneGroupHelper.cpp  \
PaZoneHelper.cpp  \

		  
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/build/SOLARIS_n/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS	= TA_DAI

DEPENDENT_LIBRARIES = \
    	$(PROJ_ROOT_DIR)/code/transactive/core/event/IDL

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
        #Do Nothing

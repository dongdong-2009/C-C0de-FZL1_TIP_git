# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/Makefile $
# @author:  Ripple
# @version: $Revision: #4 $
#
# Last modification: $DateTime: 2017/05/24 19:49:16 $
# Last modified by:  $Author: yong.liu $
#
# This file builds the ScadaCommonLib library.
#
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/scada/rtu_library
PROJ_NAME	= TA_RTULib
PROJ_SOURCE	= StationSystemOutputWriter.cpp \
				RTURedundancyWorker.cpp \
				RTUStatusWorker.cpp \
				SteRTUStatusWorker.cpp \
				RTUPollingWorker.cpp \
				STDModbusPollingWorker.cpp \
				RTUOutputWorker.cpp \
				ModbusDpProcessor.cpp \
				SteRTUDpProcessor.cpp \
				CommonDpProcessor.cpp \
				BaseRTUManager.cpp \
				SteRTUManager.cpp \
				StandardRTUManager.cpp \
				RTUDownloadWorker.cpp \
				RTUCommandWorker.cpp \
				RTU.cpp \
				CommandQueue.cpp \
				ModbusPollingScheduler.cpp \
				TimerWorker.cpp \
				HeartBeatWorker.cpp \
				NumberConversionUtil.cpp \
				SteRTU.cpp \
				StandardRTU.cpp \
				RTUFactory.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL \
											$(PROJ_ROOT_DIR)/code/transactive/bus/scada/datapoint_library
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

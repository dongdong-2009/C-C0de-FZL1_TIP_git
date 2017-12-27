# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/Makefile $
# @author:  Ripple
# @version: $Revision: #3 $
#
# Last modification: $DateTime: 2015/02/03 16:02:44 $
# Last modified by:  $Author: haijun.li $
#
# This file builds the TA_DataPointLibrary library.
#
#
PROJ_ROOT_DIR   = ../../../../..
PROJ_CODE_DIR   = transactive/bus/scada/datapoint_library
PROJ_NAME   = TA_DataPointLibrary
PROJ_SOURCE = \
            AnalogueDataPoint.cpp \
            AnswerBackErrorUser.cpp \
            EventDetailsFactory.cpp \
            BooleanDataPoint.cpp \
            CalculatedExpression.cpp \
            CombinedOutputDataPoint.cpp \
            CommonContext.cpp \
            DataPoint.cpp \
            DataPointAlarmDetails.cpp \
            DataPointAlarmDetailsFactory.cpp \
            DataPointFactory.cpp \
            DataPointStateChangeEvent.cpp \
            DerivedDataPoint.cpp \
            DerivedState.cpp \
            EnumDataPoint.cpp \
            InputAssociation.cpp \
            LaunchingConditionCheck.cpp \
            MeteredDataPoint.cpp \
            NotifyEventProcessor.cpp \
            PersistProcessor.cpp \
            SafetyOutputDataPoint.cpp \
            SingletonAnswerBackErrorUtil.cpp \
            TextDataPoint.cpp \
            OverScaleHiAlarmDetails.cpp \
            OverScaleLoAlarmDetails.cpp \
            HIAlarmDetails.cpp \
            HIHIAlarmDetails.cpp \
            LOAlarmDetails.cpp \
            LOLOAlarmDetails.cpp \
            TRUEAlarmDetails.cpp \
            FALSEAlarmDetails.cpp \
            GeneralAlarmDetails.cpp \
	    AlarmAckStateProcessor.cpp \
	    DataPointStateUpdateSender.cpp \
	    DataPointStateUpdateServant.cpp \
	    CommandProcessor.cpp \
	    MultiBitDataPoint.cpp \
	    LFThreadPoolSingleton.cpp

PROJ_TYPE   = lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES = \
    $(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
    $(PROJ_ROOT_DIR)/code/transactive/bus/maintenance_management/mmsCorbaDef/idl \
    $(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library \
    $(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
    #Do Nothing

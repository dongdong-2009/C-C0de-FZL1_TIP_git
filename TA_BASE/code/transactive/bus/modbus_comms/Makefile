# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/Makefile $
# @author:  Ripple
# @version: $Revision: #3 $
#
# Last modification: $DateTime: 2011/10/24 07:36:20 $
# Last modified by:  $Author: dhanshri $

PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/bus/modbus_comms
PROJ_NAME	= TA_ModbusComms
PROJ_SOURCE	= ModbusException.cpp \
		  ModbusConnection.cpp \
		  ModbusTcpConnection.cpp \
		  ModbusSerialConnection.cpp \
    		  SerialPortException.cpp \
		  SerialPort.cpp \
		  CrcCalculator.cpp \
		  SteModbusTcpConnection.cpp \
		  DuplexModbusTcpConnection.cpp \
		  ModbusInClient.cpp \
		  ModbusService.cpp 
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

#BR TODO: Assuming that the following isn't needed. Should delete it once confirmed
#PROJ_CFLAGS = -DUNIX -DSYSV 

# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

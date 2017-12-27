# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/104_library/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/21 16:51:05 $
# Last modified by:  $Author: haijun.li $
#
# This file builds the TA_104Library library.
#
#
PROJ_ROOT_DIR   = ../../../../..
PROJ_CODE_DIR   = transactive/bus/scada/104_library
PROJ_NAME   = TA_104Library
PROJ_SOURCE = \
            FepDataStruct.cpp \
            FtpException.cpp \
            Command.cpp \
            Iec104.cpp \
            IecSocket.cpp \
            Iec104Event.cpp \
            Adaptee.cpp \
            Iec104Api.cpp \
            ProtocolBase.cpp \
            ConnectThread.cpp \
            Bexception.cpp \
            Bf.cpp \
            Object.cpp \
            Threads.cpp

PROJ_TYPE   = lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

# Libraries that this library is dependent upon.
# DEPENDENT_LIBRARIES = \
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
    #Do Nothing

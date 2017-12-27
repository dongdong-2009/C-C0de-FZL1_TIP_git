# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/database/Makefile $ 
# Author:  Ripple
# Version: $Revision: #2 $
#
# Last modification: $DateTime: 2015/01/23 16:19:20 $
# Last modified by:  $Author: Ouyang $
#
# Make file for the DataAccessLib classes.
#
#
PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/database
PROJ_NAME	= TA_DataAccess
PROJ_SOURCE	= DBException.cpp \
                  ConnectionReaper.cpp \
                  OCIConnection.cpp \
		OCIOTLConnection.cpp \
		ODBCOTLConnection.cpp \
		  mysqlconnection.cpp \
                  DbServerLocator.cpp \
                  SimpleDb.cpp \
                  OCIQueryAdapter.cpp \
		OCIOTLQueryAdapter.cpp \
		ODBCOTLQueryAdapter.cpp \
		  MysqlQueryAdapter.cpp \
                  OCIAccessStatusMonitor.cpp \
                  OCIConnectionStatusMonitor.cpp \
                  SimpleDbStatusMonitor.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing

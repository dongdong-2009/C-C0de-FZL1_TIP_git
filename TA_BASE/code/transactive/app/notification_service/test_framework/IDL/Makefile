# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/IDL/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $
#
# Make file for the Test Framework IDL.
#

PROJ_ROOT_DIR = ../../../..
PROJ_NAME = FrameworkCorbaDef
PROJ_IDL = \
       framework.idl

PROJ_IDL_INC = ../../..
PROJ_TYPE	= lib
PROJ_OBJDIR = ./obj
PROJ_INCDIR = ./src
PROJ_INCDIRS = \
        $(PROJ_ROOT_DIR) \
        $(PROJ_ROOT_DIR)/core/message/IDL/src

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/core/message/IDL


##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/tools/build/CommonMakefile.mak


install:
	#Do Nothing

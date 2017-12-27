# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/simulator/example/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $
#

PROJ_ROOT_DIR = ../../..
PROJ_NAME     = simulator_example
PROJ_SOURCE   =	main.cpp \
                VersionGen.cpp \

PROJ_TYPE     = exe
PROJ_OBJDIR   = ./obj
PROJ_INCDIR   = ./src \
		$(PROJ_ROOT_DIR)

PROJ_LIBDIRS  =	$(PROJ_ROOT_DIR)/core/versioning/obj \
		$(PROJ_ROOT_DIR)/core/synchronisation/obj \
		$(PROJ_ROOT_DIR)/core/exceptions/obj \
		$(PROJ_ROOT_DIR)/core/utilities/obj \
		$(PROJ_ROOT_DIR)/core/threads/obj \
		$(PROJ_ROOT_DIR)/core/logging/obj \
		$(PROJ_ROOT_DIR)/core/sockets/obj \
		$(PROJ_ROOT_DIR)/bus/generic_server_simulator/examples/echo_server/obj \
		$(PROJ_ROOT_DIR)/bus/generic_server_simulator/obj

PROJ_LIBS = \
		EchoServerExample \
		TA_GenericServerSimulator \
		TA_Utilities \
		TA_Version \
		TA_Thread \
		TA_Sockets \
		TA_Synchronisation \
		TA_LoggingLibrary \
		TA_Exceptions \
		TA_Utilities

PROJ_IDL_INC  = $(PROJ_ROOT_DIR)

PROJ_INCDIRS  = \
		$(PROJ_ROOT_DIR)/core/logging/src \
		$(PROJ_ROOT_DIR)/core/utilities/src \
		$(PROJ_ROOT_DIR)/core/threads/src \
		$(PROJ_ROOT_DIR)/core/sockets/src \
		$(PROJ_ROOT_DIR)/core/synchronisation/src \
		$(PROJ_ROOT_DIR)/core/exceptions/src \
		$(PROJ_ROOT_DIR)/bus/generic_simulator/examples/echo_server/src \
		$(PROJ_ROOT_DIR)/bus/generic_simulator/src


DEPENDENT_LIBRARIES =	$(PROJ_ROOT_DIR)/core/exceptions \
			$(PROJ_ROOT_DIR)/core/versioning \
			$(PROJ_ROOT_DIR)/core/synchronisation \
			$(PROJ_ROOT_DIR)/core/threads \
			$(PROJ_ROOT_DIR)/core/utilities \
			$(PROJ_ROOT_DIR)/core/logging \
			$(PROJ_ROOT_DIR)/core/sockets \
			$(PROJ_ROOT_DIR)/bus/generic_server_simulator/examples/echo_server \
			$(PROJ_ROOT_DIR)/bus/generic_server_simulator
 
include $(PROJ_ROOT_DIR)/tools/build/CommonMakefile.mak

install:
	#Do Nothing

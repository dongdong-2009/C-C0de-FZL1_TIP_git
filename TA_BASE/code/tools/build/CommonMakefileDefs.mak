################################################################################
# The source code in this file is the property of Ripple Systems and is not for 
# redistribution in any form.
#
# Source:   $File: $ 
# Author:  Grace Koh
# Version: $Revision: $
#
# Last modification: $DateTime: $
# Last modified by:  $Author: $
#
################################################################################
#
#					  Project base make file.
#
# This file was adapted from the Citylink.mak file by Nicola Ritter: some
# lines are currently commented out that may be required later
#
# All TransActive projects must include this base makefile at the end of
# the project makefile.
#
# C++ source file names must end with the ".cpp", and header file names must
# end with ".h".
#
# Project makefile parameters:
# ============================
# - Mandatory:
# PROJ_ROOT_DIR	The relative location of repository root directory
# PROJ_CODE_DIR The directory location of Makefile relative to 'code'
# PROJ_NAME		The name of the file created by the build for the project.
#			      Also the name of the main program .cpp file
# PROJ_TYPE		The type of file created (exe = binary, lib = (static) library,
#                 dlib = dynamically linked library).
# PROJ_SRC_DIR   The root src file directory
# PROJ_INC_DIR   The include directory: some want ./src, some want ./inc
# PROJ_OBJ_DIR   The object directory: some want ./src, somew want ./obj
# PROJ_OUT_DIR   The directory where the target is created.
# PROJ_SOURCE   The source files included in this project
# - Optional:
# BR TODO: Some of these are probably obsolete and can be deleted
# PROJ_IDL	    The idl file to be compiled by omniidl
# PROJ_IDL_INC	Any dir to be included in omniidl call
# PROJ_INCDIRS	The list of additional include file directories.
# PROJ_LIB_DIRS	The list of additional library directories.
# PROJ_LIBS		The list of additional libraries used by this project.
# PROJ_LIBDEPS  The list of other libraries we are developing on which
#			      this particular project depends.  Should be put in with
#			      a root directory of Solaris7/lib  e.g. libAlarmUtil.a or
#				  dmif/libAlarmProcessorDmIf.a.  There may be double up between
#			      this constant and the PROJ_LIBS constant - I cant avoid this
#				  at this point
# PROJ_DEFS		The list of additional #defines specified to the project build.
# PROJ_CFLAGS	The list of additional compiler command line options.
#				 for example -g for debugging
# PROJ_LFLAGS	The list of additional linker command line options.
# PROJ_X_H	    The list of header files to export out of this project.
# PROJ_LIB_TYPE The type of library (dmif = DmIf library, util = util library)
#
################################################################################


############################################################
# The various executables used during the build process.
############################################################
CC		= gcc
CXX		= g++
COMPILE		= $(CXX)
LINK		= $(CXX)
DEP		= $(CC) -traditional-cpp
AR		= ar -ruv
RM		= rm -f
MD		= mkdir -p
CP		= cp -f
SED		= sed -e
FIX_CORBA_INCLUDES = $(PROJ_ROOT_DIR)/code/tools/build/fix_corba_includes.sh


############################################################
# Software Versions
############################################################
# OMNI_VER: 	Must match the bit of the directory name in code tree that varies
# BOOST_VER: 	Must match the bit of the directory name in code tree that varies
# ORACLE_VER: 	Must match the bit of the directory name in code tree that varies
# ACE_VER: 	Must match the bit of the directory name in code tree that varies
#-----
OMNI_VER	= omniORB_4.1.6
BOOST_VER	= boost_1_56_0
ORACLE_VER	= 11.2.0
MYSQL_VER   = 5.5.15
MCL_VER     = 1.1.1
ACE_VER	= 6_0_4
OPENSSL_VER = openssl-1.0.0d
CURL_VER=curl-7.21.7
SSH_VER=libssh2-1.2.9
############################################################
# Hardware versions
############################################################
# PROCESSOR: Processor name
# PLATFORM_CPU: as above without underscores?
#-----
PROCESSOR := $(shell uname -p | tr A-Z a-z)
ifeq ($(PROCESSOR), sparc)
	PROCESSOR := __sparc__
	PLATFORM_CPU := sparc
endif
ifeq ($(PROCESSOR), i386)
	PROCESSOR := __x86__
	PLATFORM_CPU := x86
endif
ifeq ($(PROCESSOR), i586)
	PROCESSOR := __x86__
	PLATFORM_CPU := x86
endif
ifeq ($(PROCESSOR), i686)
	PROCESSOR := __x86__
	PLATFORM_CPU := x86
endif
ifeq ($(PROCESSOR), unknown) # Assume X86 if unknown
	PROCESSOR := __x86__
	PLATFORM_CPU := x86
endif

ARCHITECTURE := $(shell arch)

############################################################
# OS Versions & OS-specific flags
############################################################
#OPERATING_SYSTEM: O/S
#PLATFORM_OS: O/S
#OP_SYS_VERSION: O/S version (major revision)
#OS_VERSION: O/S version (major revision) used for CFLAGS
#ARCHITECTURE: O/S version (major revision) used for CFLAGS
#BASE_CFLAGS: Base compiler flags
#OMNI_BIN: Directory where OmniORB binaries are found
#OMNI_LIB: Directory where OmniORB libs are found
#BOOST_LIB: Directory where Boost libs are found
#ACE_LIB_DIR: Directory where ACE libs are found
#ORACLE_DIR: Root directory for Oracle
#MYSQL_DIR: Root directory for Mysql
#MCL_LIB_DIR: Directory where mysql connection library(MCL) libs are found
#MCL_SRC_DIR: Root directory for MCL source files
#-----

# Get OP_SYS_VERSION for Solaris (BRTODO: what about Linux?)
# TODO: the "sun4_sosV_X.Y" dir names are based on naming hangover from 3.0X releases
#       One possible fix, to avoid having to rename the dir in P4/cvs is that the
#       build.xml script could create a soft link from the sun4_sosV directory to
#       the appropriate config.guess output
#       The omni config.guess script can be used to name the linux directories, too
#
OP_SYS_VERSION := $(shell uname -r)
ifeq ($(OP_SYS_VERSION), 5.8)
	OS_VERSION := __OSVERSION__=5
	OMNI_OS_IDENTIFIER := sun4_sosV_$(OP_SYS_VERSION)
endif
ifeq ($(OP_SYS_VERSION), 5.9)
	OS_VERSION := __OSVERSION__=5
	OMNI_OS_IDENTIFIER := $(OMNI_OS_IDENTIFIER)
endif
ifeq ($(OP_SYS_VERSION), 5.10)
	OS_VERSION := __OSVERSION__=5
	OMNI_OS_IDENTIFIER := $(shell $(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/bin/scripts/config.guess)
    ifeq ($(ACE_VER), 5_3)
        ACE_VER := 5_5
    endif
    ifeq ($(ORACLE_VER), 920)
        ifeq ($(PLATFORM_CPU), x86)
            ORACLE_VER := 10.2.0
        endif
    endif
endif

OPERATING_SYSTEM := $(shell uname -s | tr -s '[:lower:]' '[:upper:]')
# Solaris (version 5.8 or 5.9)
ifeq ($(OPERATING_SYSTEM), SUNOS)
	OPERATING_SYSTEM := SOLARIS
	BASE_CFLAGS += -D__sunos__
	OMNI_BIN = $(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/$(OMNI_OS_IDENTIFIER)/bin
	OMNI_LIB = $(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/$(OMNI_OS_IDENTIFIER)/lib
	BOOST_LIB = $(PROJ_ROOT_DIR)/code/cots/boost/$(BOOST_VER)/libs/$(OPERATING_SYSTEM)_$(OP_SYS_VERSION)_$(ARCHITECTURE)
	ACE_LIB_DIR = $(PROJ_ROOT_DIR)/code/cots/ACE/$(ACE_VER)/lib/$(OPERATING_SYSTEM)_$(OP_SYS_VERSION)_$(ARCHITECTURE)
	ORACLE_DIR = $(PROJ_ROOT_DIR)/code/cots/oracle/$(ORACLE_VER)/$(OPERATING_SYSTEM)_$(OP_SYS_VERSION)_$(ARCHITECTURE)
        MYSQL_DIR = $(PROJ_ROOT_DIR)/code/cots/mysql/$(MYSQL_VER)/$(OPERATING_SYSTEM)_$(OP_SYS_VERSION)_$(ARCHITECTURE)
	MCL_LIB_DIR = $(PROJ_ROOT_DIR)/code/cots/mcl/$(MCL_VER)/lib/$(OPERATING_SYSTEM)_$(OP_SYS_VERSION)_$(ARCHITECTURE)_mysql$(MYSQL_VER)_boost1.56.0
	MCL_SRC_DIR = $(PROJ_ROOT_DIR)/code/cots/mcl/$(MCL_VER)
	CURL_DIR = $(PROJ_ROOT_DIR)/code/cots/curl/$(CURL_VER)/build/lib/$(OPERATING_SYSTEM)_$(OP_SYS_VERSION)_$(ARCHITECTURE)
	OPENSSL_DIR = $(PROJ_ROOT_DIR)/code/cots/openssl/$(OPENSSL_VER)/build/lib/$(OPERATING_SYSTEM)_$(OP_SYS_VERSION)_$(ARCHITECTURE)
	OPENSSL_BIN = $(PROJ_ROOT_DIR)/code/cots/openssl/$(OPENSSL_VER)/build/bin/$(OPERATING_SYSTEM)_$(OP_SYS_VERSION)_$(ARCHITECTURE)
	PROJ_SQLGEN = $(PROJ_ROOT_DIR)/code/cots/sqlgen/1.1.1/bin/$(OPERATING_SYSTEM)_$(OP_SYS_VERSION)_$(ARCHITECTURE)/sqlgen
	PLATFORM_OS := solaris
endif
# Linux
# BR TODO: Note that the Linux directories are just guesses for the moment
ifeq ($(OPERATING_SYSTEM), LINUX)
	OMNI_OS_IDENTIFIER := $(shell $(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/bin/scripts/config.guess)
	BASE_CFLAGS += -D__linux__
	OMNI_BIN = $(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/$(OMNI_OS_IDENTIFIER)/bin
	OMNI_LIB = $(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/$(OMNI_OS_IDENTIFIER)/lib
	BOOST_LIB = $(PROJ_ROOT_DIR)/code/cots/boost/$(BOOST_VER)/libs/$(OMNI_OS_IDENTIFIER)
	ACE_LIB_DIR = $(PROJ_ROOT_DIR)/code/cots/ACE/$(ACE_VER)/lib/$(OMNI_OS_IDENTIFIER)
	ORACLE_DIR = $(PROJ_ROOT_DIR)/code/cots/oracle/$(ORACLE_VER)/$(OMNI_OS_IDENTIFIER)
	MYSQL_DIR = $(PROJ_ROOT_DIR)/code/cots/mysql/$(MYSQL_VER)/$(OMNI_OS_IDENTIFIER)
	MCL_LIB_DIR = $(PROJ_ROOT_DIR)/code/cots/mcl/$(MCL_VER)/lib/$(OMNI_OS_IDENTIFIER)_mysql$(MYSQL_VER)_boost1.56.0
	MCL_SRC_DIR = $(PROJ_ROOT_DIR)/code/cots/mcl/$(MCL_VER)
	CURL_DIR = $(PROJ_ROOT_DIR)/code/cots/curl/$(CURL_VER)/build/lib/$(OMNI_OS_IDENTIFIER)
	OPENSSL_DIR = $(PROJ_ROOT_DIR)/code/cots/openssl/$(OPENSSL_VER)/build/lib/$(OMNI_OS_IDENTIFIER)
	OPENSSL_BIN = $(PROJ_ROOT_DIR)/code/cots/openssl/$(OPENSSL_VER)/build/bin/$(OMNI_OS_IDENTIFIER)
	PROJ_SQLGEN = $(PROJ_ROOT_DIR)/code/cots/sqlgen/1.1.1/bin/$(OMNI_OS_IDENTIFIER)/sqlgen
	PLATFORM_OS := linux
endif

############################################################
# Main compiler options
############################################################
# RELEASE_TYPE: DEBUG_BUILD or RELEASE_BUILD
# RELEASE_FLAG: 'd' or 'r'
# STRING_TYPE:  ASCII_STRINGS or UNICODE_STRINGS
# STRING_FLAG:  'n' or 'w'
#-----

ifdef DEBUG
  RELEASE_TYPE := DEBUG_BUILD
  RELEASE_FLAG := d
else
  RELEASE_TYPE := RELEASE_BUILD
  RELEASE_FLAG := r
endif

ifdef UNICODE
  STRING_TYPE := UNICODE_STRINGS
  STRING_FLAG := w
else
  STRING_TYPE := ASCII_STRINGS
  STRING_FLAG := n
endif


############################################################
# Directories & files.
############################################################
# The vars set here are documented at the top of this file...
#-----
# Base directories for the project being worked on.
# Don't overwrite this if it is already defined
ifndef PROJ_SRC_DIR
  PROJ_SRC_DIR	= ./src
endif
PROJ_DEP_DIR	= $(PROJ_ROOT_DIR)/dep/$(PROJ_CODE_DIR)
PROJ_OBJ_DIR	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)/$(PROJ_CODE_DIR)
# IDL files need to be generated into a 'src' dir or the #includes will be broken
PROJ_IDL_DIR	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)/$(PROJ_CODE_DIR)/src

# This defines what object files are generated from an IDL
PROJ_IDL_SKELETONS = $(PROJ_IDL:%.idl=$(PROJ_IDL_DIR)/%_skel.cpp)
IDL_OBJECTS = $(PROJ_IDL:%.idl=$(PROJ_OBJ_DIR)/%.o) \
				$(PROJ_IDL:%.idl=$(PROJ_OBJ_DIR)/%_skel.o) 

# The project object files.  Any common code ( specified in COMMON_SOURCE ) is
# also included.
CPP_OBJECTS = $(PROJ_SOURCE:%.cpp=$(PROJ_OBJ_DIR)/%.o) \
				$(addprefix $(PROJ_OBJ_DIR)/,$(notdir $(COMMON_SOURCE:%.cpp=$(PROJ_OBJ_DIR)/%.o)))

MV_C_OBJECTS = $(PROJ_MV_C_SOURCE:%.c=$(PROJ_OBJ_DIR)/%.o)
MV_CPP_OBJECTS = $(PROJ_MV_CPP_SOURCE:%.cxx=$(PROJ_OBJ_DIR)/%.o)

PC_OBJECTS  = $(PROJ_PC_SOURCE:%.pc=$(PROJ_OBJ_DIR)/%.o)

ALL_SOURCE  = $(PROJ_SOURCE) $(PROJ_PC_SOURCE) $(PROJ_MV_C_SOURCE) $(PROJ_MV_CPP_SOURCE)

OBJECTS  = $(PC_OBJECTS) $(CPP_OBJECTS) $(MV_C_OBJECTS) $(MV_CPP_OBJECTS) $(IDL_OBJECTS)

ALL_SOURCE_FULL_PATH = $(addprefix $(PROJ_SRC_DIR)/,$(ALL_SOURCE))

# The project dependencies file.
DEPS_FILE	= $(PROJ_DEP_DIR)/deps

DEPENDENCY_FILES := $(PROJ_SOURCE:%.cpp=$(PROJ_DEP_DIR)/%.d)

# Any exported project headers.
ifdef PROJ_X_H
PROJ_X_H_PATH = $(addprefix $(PROJ_INC_DIR)/,$(PROJ_X_H))
endif

# The final target.
# Static libraries are special because debug versions have an '_d' appended.
# NOT!!! (yet) this can not be done until the _d is added to the libs in all
# the makefiles automagicly. The Makefiles have to change to ensure that only
# those libs that are transactive and not cots etc are split out
ifeq ($(PROJ_TYPE), exe)
	PROJ_TARGET		= $(PROJ_OUT_DIR)/$(PROJ_NAME)
endif
ifeq ($(PROJ_TYPE), dlib)
	PROJ_TARGET		= $(PROJ_OUT_DIR)/lib$(PROJ_NAME).so
endif
ifeq ($(PROJ_TYPE), lib)
ifeq ($(RELEASE_TYPE), DEBUG_BUILD)
	PROJ_TARGET		= $(PROJ_OUT_DIR)/lib$(PROJ_NAME).a
else
	PROJ_TARGET		= $(PROJ_OUT_DIR)/lib$(PROJ_NAME).a
endif
endif

# Need to hack the names so that the :s for sed works properly in the
# makedepend below
PROJ_OBJ_DIR_HACKED = $(subst /,\/,$(PROJ_OBJ_DIR))\/
PROJ_SRC_DIR_HACKED = $(subst /,\/,$(PROJ_SRC_DIR))\/


############################################################
# Compiler flags.
############################################################

# Note that if you want -g for debug purposes, add it as a PROJ_CFLAGS
#
# Bruce Fountain 12-Jun-2001:
#	Modified the flags for omniORB (vs ORBacus). I still don't know what
#	 they do
#
# BASE_CFLAGS	= -c -fPIC -Wall -fpermissive \
#			   -fno-gnu-keywords -frtti -fexceptions
ifeq ($(OPERATING_SYSTEM), LINUX)
BASE_CFLAGS	+= -pthread -fexceptions -c -Wall -DSTL_HAS_DEFAULT_ARGS -DUsePthread \
					-D_REENTRANT \
					-D$(PROCESSOR) \
					-D$(OPERATING_SYSTEM) 
					#-DTA_ASSERT_FAIL=DEFAULT
					#-D$(OS_VERSION)
else
BASE_CFLAGS	+= -pthreads -c -Wall -DSTL_HAS_DEFAULT_ARGS -DUsePthread \
					-D_REENTRANT \
					-D$(PROCESSOR) \
					-D$(OPERATING_SYSTEM) \
					-D$(OS_VERSION) 
					#-DTA_ASSERT_FAIL=DEFAULT
endif

# Add support for Purify.
# BRTODO: This is solaris-only. It should be generalised (or removed).
ifdef PURIFY
	PURIFY_CMD=/opt/rational/releases/PurifyPlus.2003a.06.00/sun4_solaris2/bin/purify -g++=yes -collector=/usr/local/lib/gcc-lib/sparc-sun-solaris2.8/3.2.1/collect2
endif

# Add support for Quantify.
# BRTODO: This is solaris-only. It should be generalised (or removed).
ifdef QUANTIFY
	QUANTIFY_CMD=/opt/rational/releases/PurifyPlus.2003a.06.00/sun4_solaris2/bin/quantify -g++=yes -collector=/usr/local/lib/gcc-lib/sparc-sun-solaris2.8/3.2.1/collect2
endif

# Automatically add debug compiler switch if the DEBUG argument is passed
# on the command line.
# also make sure that recursive make calls also have the debug argument
ifeq ($(RELEASE_TYPE), DEBUG_BUILD)
  PROJ_CFLAGS += -g
  PROJ_LFLAGS += -g
  DEFINITIONS = DEBUG=$$DEBUG
else
  PROJ_CFLAGS += -g
  PROJ_LFLAGS += -g
  PROJ_CFLAGS += -O2
  PROJ_LFLAGS += -O2
endif

# Shared libraries require position independent code
ifeq ($(PROJ_TYPE), dlib)
  PROJ_CFLAGS += -fPIC
endif

# add omni and oracle include directories
ifeq ($(OPERATING_SYSTEM), SOLARIS)
  STD_INC_DIRS += \
	$(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/$(OMNI_OS_IDENTIFIER)/include \
	$(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/$(OMNI_OS_IDENTIFIER)/include/COS \
	$(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/$(OMNI_OS_IDENTIFIER)/include/omniNotify \
	$(PROJ_ROOT_DIR)/code/cots/ACE/$(ACE_VER)/ACE_wrappers \
	$(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)/transactive \
	$(PROJ_ROOT_DIR)/code/cots/openssl/$(OPENSSL_VER)/build/include \
	$(PROJ_ROOT_DIR)/code/cots/curl/$(CURL_VER)/include \
	$(PROJ_ROOT_DIR)/code/cots/libssh2/$(SSH_VER)/include \
	$(ORACLE_DIR)/inc \
        $(MYSQL_DIR)/inc  \
	$(MCL_SRC_DIR)

endif
ifeq ($(OPERATING_SYSTEM), LINUX)
  STD_INC_DIRS += \
	$(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/$(OMNI_OS_IDENTIFIER)/include \
	$(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/$(OMNI_OS_IDENTIFIER)/include/COS \
	$(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/$(OMNI_OS_IDENTIFIER)/include/omniNotify \
	$(PROJ_ROOT_DIR)/code/cots/ACE/$(ACE_VER)/ACE_wrappers \
	$(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)/transactive \
	$(PROJ_ROOT_DIR)/code/cots/openssl/$(OPENSSL_VER)/build/include \
	$(PROJ_ROOT_DIR)/code/cots/curl/$(CURL_VER)/include \
	$(PROJ_ROOT_DIR)/code/cots/libssh2/$(SSH_VER)/include \
	$(ORACLE_DIR)/inc \
	$(MYSQL_DIR)/inc  \
	$(MCL_SRC_DIR)
endif

# add boost support
STD_INC_DIRS += $(PROJ_ROOT_DIR)/code/cots/boost/$(BOOST_VER)

# All include dirs
INC_DIRS = $(addprefix -I,$(PROJ_INC_DIR) $(PROJ_INCDIRS) $(STD_INC_DIRS))

# The built-up command line to the compiler.
CFLAGS	= $(BASE_CFLAGS) $(PROJ_CFLAGS) $(INC_DIRS)

############################################################
# Linker flags.
############################################################

# Linker Options.
ifeq ($(OPERATING_SYSTEM), SOLARIS)
	BASE_LFLAGS	+= -misalign
endif

ifeq ($(OPERATING_SYSTEM), LINUX)
	BASE_LFLAGS	+= -pthread
	BASE_LFLAGS	+= -fexceptions
endif

# Library directories.
STD_LIB_DIRS	=	 $(PROJ_OBJ_DIR)

ifeq ($(OPERATING_SYSTEM), SOLARIS)
    STD_LIB_DIRS += $(OMNI_LIB) \
		    $(BOOST_LIB) \
		    $(ORACLE_DIR)/lib \
		    $(MYSQL_DIR)/lib \
		    $(MCL_LIB_DIR)   \
		    $(ACE_LIB_DIR) \
		    $(CURL_DIR)  \
			$(OPENSSL_DIR)
endif
ifeq ($(OPERATING_SYSTEM), LINUX)
    STD_LIB_DIRS += $(OMNI_LIB) \
		    $(BOOST_LIB) \
		    $(ORACLE_DIR)/lib \
                 $(MYSQL_DIR)/lib \
       		$(MCL_LIB_DIR)   \
		    $(ACE_LIB_DIR) \
		    $(CURL_DIR)  \
			$(OPENSSL_DIR)
endif

PROJ_LIB_DIRS	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

LIB_DIRS = $(addprefix -L,$(PROJ_LIB_DIRS) $(STD_LIB_DIRS))

# The built-up command line to the linker.
LFLAGS	+= $(BASE_LFLAGS) $(PROJ_LFLAGS) $(LIB_DIRS)

############################################################
# Libraries.
############################################################


# third party libraries that we will almost always need:
#TP_LIBS +=       omniORB4 omniDynamic4 omnithread COS4 boost_system-gcc34-mt-1_39 boost_filesystem-gcc34-mt-1_39 boost_regex-gcc34-mt-1_39 crypto
TP_LIBS +=       omniORB4 omniDynamic4 omnithread COS4 boost_system boost_filesystem boost_regex boost_date_time crypto

# standard libraries required by Nexus or TA_Util
STD_LIBS += pthread nsl

# For Linux OUR_LIBS is empty. All libraries should be entered in the
# makefile
ifeq ($(OPERATING_SYSTEM), SOLARIS)
	STD_LIBS += posix4 socket
endif


# Add -l to each library for the command line
LIBS		 = $(addprefix -l,$(PROJ_LIBS) $(TP_LIBS) $(STD_LIBS))

# Search path for library dependencies.
# Project path is searched first because development on a library may be
# occurring. Nicola: not sure what this is doing!
# BRTODO: Find out what this is and if we still need it
vpath %.a $(PROJ_LIB_DIRS) $(STD_LIB_DIRS)


############################################################
# Library dependencies.
############################################################
# Add path to each other dependency
LIBDEP = $(addprefix $(PLATFORM_LIB_DIR)/,$(PROJ_LIBDEPS))


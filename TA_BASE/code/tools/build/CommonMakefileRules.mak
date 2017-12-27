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
############################################################
# Rules.
############################################################

all:	validate_makefile make_obj_directories make_dependencies $(PROJ_TARGET)

BUILD:	validate_makefile make_obj_directories $(PROJ_TARGET)

# This is a bit of a hack to validate the project makefile parameters,
# but it works!  Is there a better way to do this?
#
validate_makefile:
	@echo "==> VALIDATING MAKEFILE....."
ifndef PROJ_ROOT_DIR
	@echo "PROJ_ROOT_DIR has not been specified in the makefile"
	@false
endif
ifndef PROJ_CODE_DIR
	@echo "PROJ_CODE_DIR has not been specified in the makefile"
	@false
endif
ifndef PROJ_NAME
	@echo "PROJ_NAME has not been specified in the makefile"
	@false
endif
ifndef PROJ_OBJ_DIR
	@echo "PROJ_OBJ_DIR has not been specified in the makefile"
	@false
endif
ifndef PROJ_OUT_DIR
	@echo "PROJ_OUT_DIR has not been specified in the makefile"
	@false
endif
ifndef PROJ_INC_DIR
	@echo "PROJ_INC_DIR has not been specified in the makefile"
	@false
endif
ifndef PROJ_TYPE
	@echo "PROJ_TYPE has not been specified in the makefile"
	@false
else
ifneq ($(PROJ_TYPE), exe)
ifneq ($(PROJ_TYPE), lib)
ifneq ($(PROJ_TYPE), dlib)
	@echo "Invalid value of PROJ_TYPE should be exe, lib or dlib"
	@false
endif
endif
endif
endif
ifndef ALL_SOURCE
	@echo "ALL_SOURCE has not been specified in the makefile"
	@false
endif
ifeq ($(OPERATING_SYSTEM), SOLARIS)
ifdef PROJ_IDL
	# dos2unix $(OMNI_BIN)/omniidl $(OMNI_BIN)/omniidl
	@echo "Ensuring execute permissions set for omniidl lib"
	@chmod +x $(OMNI_BIN)/omnicpp $(OMNI_BIN)/omniidl
endif
ifdef PROJ_SQLGEN_INPUT
#	@echo "remove object files"
#	@$(RM) $(PROJ_TARGET) $(OBJECTS) $(DEPS_FILE) $(PROJ_IDL_SKELETONS) $(DEPENDENCY_FILES)
	@echo "GENERATING sql.h sqlmacrodef.h by sqlcode.txt ..."
	@echo "Ensuring execute permissions set for PROJ_SQLGEN lib"
	@chmod +x $(PROJ_SQLGEN)
	@$(PROJ_SQLGEN) --inputfile=$(PROJ_SQLGEN_INPUT) --outpath=$(PROJ_SQLGEN_OUTPUT)
endif
endif
	@echo "	DONE"

# if this dependency is placed under "all" then we get some output that
# helps debug the makefile
debug_makefile:
	@echo "DEBUGGING  MAKEFILE...."
	@echo "  OPERATING_SYSTEM = '"$(OPERATING_SYSTEM)"'"
	@echo "  OBJECTS = '" $(OBJECTS) "'"
	@echo "  LINK = '" $(LINK) "'"
	@echo "  PROJ_TARGET = '" $(PROJ_TARGET) "'"
	@echo "  LIBS = '" $(LIBS) "'"
	@echo "  LIBDEP1 = '" $(LIBDEP1) "'"
	@echo "  LIBDEP2 = '" $(LIBDEP2) "'"
	@echo "  LIBDEP3 = '" $(LIBDEP3) "'"
	@echo "  LIBDEP = '" $(LIBDEP) "'"
	@echo "  CFLAGS = '" $(CFLAGS) "'"
	@echo "  PROJ_OBJ_DIR = '" $(PROJ_OBJ_DIR) "'"
	@echo "  PROJ_DEP_DIR = '" $(PROJ_DEP_DIR) "'"
	@echo "  BASE_LFLAGS = '" $(BASE_LFLAGS) "'"
	@echo "  PROJ_LFLAGS = '" $(PROJ_LFLAGS) "'"
	@echo "  PROJ_LIB_BUILD_DIR = '" $(PROJ_LIB_BUILD_DIR) "'"
	@echo "  PROJ_LIB_DIRS = '" $(PROJ_LIB_DIRS) "'"
	@echo "  STD_LIB_DIRS = '" $(STD_LIB_DIRS) "'"
	@echo "  LIB_DIRS = '" $(LIB_DIRS) "'"
	@echo "  LFLAGS = '" $(LFLAGS) "'"
	@echo ""

# Either it is an exe file that needs to be compiled, or it is a lib that is
# being compiled.  Either way the results go in ./obj.  make install, will
# copy finished libraries and headers to the appropriate places
ifeq ($(PROJ_TYPE), exe)
$(PROJ_TARGET):	$(OBJECTS) $(LIBDEP)
	@echo "==> LINKING....."
	@if [ ! -d $(PROJ_OBJ_DIR) ]; then $(MD) $(PROJ_OBJ_DIR); fi
	@if [ ! -d $(PROJ_OUT_DIR) ]; then $(MD) $(PROJ_OUT_DIR); fi
	$(QUANTIFY_CMD) $(PURIFY_CMD) $(LINK) -o $(PROJ_TARGET) $(OBJECTS) $(LFLAGS) $(LIBS)
	@echo "	DONE"
endif
ifeq ($(PROJ_TYPE), lib)
$(PROJ_TARGET):	$(OBJECTS)
	@echo "==> BUILDING LIBRARY....."
	@if [ ! -d $(PROJ_OBJ_DIR) ]; then $(MD) $(PROJ_OBJ_DIR); fi
	@if [ ! -d $(PROJ_OUT_DIR) ]; then $(MD) $(PROJ_OUT_DIR); fi
	$(AR) $(PROJ_TARGET) $(OBJECTS)
	@echo "	DONE"
endif
ifeq ($(PROJ_TYPE), dlib)
$(PROJ_TARGET):	$(OBJECTS)
	@echo "==> BUILDING DYNAMIC LIBRARY....."
	@if [ ! -d $(PROJ_OBJ_DIR) ]; then $(MD) $(PROJ_OBJ_DIR); fi
	@if [ ! -d $(PROJ_OUT_DIR) ]; then $(MD) $(PROJ_OUT_DIR); fi
	$(LINK) -shared -o $(PROJ_TARGET) $(OBJECTS) $(LFLAGS) 
	@echo "	DONE"
endif

#
# This target is used to build depenendent libraries ( if defined ).
# Applications and libraries will specify the libraries that they are
# dependent upon in their Makefile, and when the dependencies for the
# application / library are determined, the dependent libraries will
# be automagically built and installed.
#
# the $DEFINITIONS is for any definitions that should be passed to make
# ie DEBUG=1
make_dependent_libraries: $(DEPENDENT_LIBRARIES)
ifdef DEPENDENT_LIBRARIES
	@for LIBRARY in $(DEPENDENT_LIBRARIES); \
	do \
		echo Building dependent library $$LIBRARY; \
		echo $(MAKE) -C $$LIBRARY $(DEFINITIONS) all install; \
		$(MAKE) -C $$LIBRARY $(DEFINITIONS) all install; \
	done;
endif

# BR TODO: This is broken. It assumes that .o files will go into ./obj, but they now go into $(PROJ_ROOT_DIR)/build/...
# This has to be fixed to take that into accout (I think system_status_agent is only agent affected)
make_obj_directories:
ifdef ADDITIONAL_OBJ_DIRECTORIES
	($(MD) $(PROJ_OBJ_DIR));
	($(MD) $(PROJ_DEP_DIR));
	@for DIRECTORY in $(ADDITIONAL_OBJ_DIRECTORIES); \
	do \
		($(MD) $(PROJ_OBJ_DIR)/$$DIRECTORY); \
		($(MD) $(PROJ_DEP_DIR)/$$DIRECTORY); \
	done;
endif

deep_clean: $(DEPENDENT_LIBRARIES)
ifdef DEPENDENT_LIBRARIES
	@for LIBRARY in $(DEPENDENT_LIBRARIES); \
	do \
		echo cleaning dependent library $$LIBRARY; \
		echo $(MAKE) -C $$LIBRARY deep_clean; \
		$(MAKE) -C $$LIBRARY deep_clean; \
		$(MAKE) -C $$LIBRARY clean; \
	done;
endif

# Generates the dependencies that ensure the code is compiled if it
# should be, does not automatically list library dependencies!
# Note that makedepend assumes that .o files go in src, so
# there is a hack: makedepend writes to temp and then sed fixes
# up the path to use the obj directory instead
# Note also that makedepend expects the file it is writing to to
# already exist!  Hence the touch to temp.
#
# NOTE: The file VersionGen.cpp contains a preprocessor directive
#       that gets expanded to contain the date and time that the
#       file was compiled.  The object file is removed during this
#       target so that the source file is compiled resulting in the
#       application having the correct build date and time.
make_dependencies: make_dependent_libraries $(DEPENDENCY_FILES)
	@if [ -f $(PROJ_OBJ_DIR)/VersionGen.o ]; then $(RM) $(PROJ_OBJ_DIR)/VersionGen.o;fi

# The SED regular expression uses the | character as a delimiter 
# because it is modifying a path which may contain the / character.
$(PROJ_DEP_DIR)/%.d: $(PROJ_SRC_DIR)/%.cpp
	@echo "==> Generating dependencies for $<"
	@if [ ! -d $(PROJ_DEP_DIR) ]; then $(MD) $(PROJ_DEP_DIR); fi
	@-gcc -M $(CFLAGS) $< | $(SED) "s|\(.*\.o\)|"$(PROJ_OBJ_DIR)"\/\1|" > $@

###
# Check header validity 
# Todo: only re-run for those which failed (ie. preserve stub on successful check)
###
VALIDATE_HEADER_FILES := $(wildcard $(PROJ_SRC_DIR)/*.h)
VALIDATE_HEADER_STUBS := $(VALIDATE_HEADER_FILES:$(PROJ_SRC_DIR)/%.h=$(PROJ_DEP_DIR)/Check_%.cpp)

validate_headers: $(VALIDATE_HEADER_STUBS)

# note: removing stub to ensure rule is executed again
$(PROJ_DEP_DIR)/Check_%.cpp:	$(PROJ_SRC_DIR)/%.h
	@echo "==> Checking validity of $(PROJ_CODE_DIR:transactive/%=%)/$<" 
	@if [ ! -d $(dir $@) ]; then $(MD) $(dir $@); fi 
	@echo '#include "~INCLUDE~"' | $(SED) "s|~INCLUDE~|$(PROJ_CODE_DIR:transactive/%=%)/$<|" > $@
	-$(CXX) -S $(CFLAGS) $@ 
	$(RM) $@ 
	@echo "	DONE"

# The compile command for object files
$(PROJ_OBJ_DIR)/%.o:	$(PROJ_SRC_DIR)/%.cxx
	@echo "==> COMPILING" $@
	@if [ ! -d $(PROJ_OBJ_DIR) ]; then $(MD) $(PROJ_OBJ_DIR); fi
	$(CXX) -o $@ $(CFLAGS) $<
	@echo "	DONE"

# The compile command for object files
$(PROJ_OBJ_DIR)/%.o:	$(PROJ_SRC_DIR)/%.cpp
	@echo "==> COMPILING" $@
	@if [ ! -d $(PROJ_OBJ_DIR) ]; then $(MD) $(PROJ_OBJ_DIR); fi
	$(CXX) -o $@ $(CFLAGS) $<
	@echo "	DONE"

# The compile command for common source object files
# BR TODO: What the hell is this anyway?
$(PROJ_OBJ_DIR)/%.o:	../src/%.cpp
	@echo "==> COMPILING common source" $@
	@if [ ! -d $(PROJ_OBJ_DIR) ]; then $(MD) $(PROJ_OBJ_DIR); fi
	$(CXX) -o $@ $(CFLAGS) $<
	@echo "	DONE"

# The compile command for object files (generated from IDLs)
$(PROJ_OBJ_DIR)/%.o:	$(PROJ_IDL_DIR)/%.cpp
	@echo "==> COMPILING IDL-generated cpp file" $@
	@if [ ! -d $(PROJ_OBJ_DIR) ]; then $(MD) $(PROJ_OBJ_DIR); fi
	$(CXX) -o $@ $(CFLAGS) $<
	@echo "	DONE"

# The compile command for object files
$(PROJ_OBJ_DIR)/%.o:	$(PROJ_SRC_DIR)/%.c
	@echo "==> COMPILING" $@
	@if [ ! -d $(PROJ_OBJ_DIR) ]; then $(MD) $(PROJ_OBJ_DIR); fi
	$(CC) -o $@ $(CFLAGS) $<
	@echo "	DONE"

# The compile command for compiling IDL files
# BR TODO: I've deleted the code to fix CORBA includes - I think this is no longer required for omniORB 4.0.5
#    They can be rescued from an earlier revision of this file if necessary
$(PROJ_IDL_DIR)/%.cpp: $(PROJ_SRC_DIR)/%.idl
	@echo "Generating skeletons for $@"
	@test -d $(PROJ_IDL_DIR) || $(MD) $(PROJ_IDL_DIR)
	$(OMNI_BIN)/omniidl -C$(PROJ_IDL_DIR) -bcxx -Wbkeep_inc_path -Wba -I$(PROJ_SRC_DIR) -I$(PROJ_IDL_INC) -I. -I$(PROJ_ROOT_DIR)/code/cots/omniORB/$(OMNI_VER)/src/lib/omniORB -Wbh=.h -Wbs=.cpp -Wbd=_skel.cpp $<
	@echo "Done"

# The following rule is required to correctly build the _skel.cpp
# DO NOT DELETE THE BODY OF THIS RULE OR IT WILL NOT WORK!
$(PROJ_IDL_DIR)/%_skel.cpp: $(PROJ_IDL_DIR)/%.cpp
	@echo "Done"

# The compile command for .pc files
# The compile command for .pc files
#
# BCF 26-Sep-2001 PR734: tell PRO*C to generate thread-safe code
$(PROJ_SRC_DIR)/%.c:	$(PROJ_SRC_DIR)/%.pc
	@echo "==> COMPILING" $@
	@if [ ! -d $(PROJ_OBJ_DIR) ]; then $(MD) $(PROJ_OBJ_DIR); fi
	ORACLE_HOME=$(ORACLE_HOME);$(ORACLE_HOME)/bin/proc  $< include=$(PROJ_INC_DIR) THREADS=YES
	@echo "	DONE"

clean:
	@$(RM) $(PROJ_TARGET) $(OBJECTS) $(DEPS_FILE) $(PROJ_IDL_SKELETONS) $(DEPENDENCY_FILES)

# Show all libraries automatically added in to the linking
showlibdirs:
	@echo " LIBS = '" $(STD_LIB_DIRS) "'"

# Show all libraries automatically added in to the linking
showlibs:
	@echo " LIBS = '" $(OUR_LIBS) "'"

# Show all includes automatically added in to the compile line
showincs:
	@echo " INCLUDES  = '" $(STD_INC_DIRS)"'"

# Show all include directories automatically added in to the compile line
showincdirs:
	@echo " INCLUDE DIRS = '" $(STD_INC_DIRS) $(PROJ_INC_DIR)"'"

# include the file of dependencies
# ifdef DEPENDENCY_FILES
# 	ifneq ($(DEPENDENCY_FILES),"")
# 		-include $(DEPENDENCY_FILES)
# 	endif
# endif


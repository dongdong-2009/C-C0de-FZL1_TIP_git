PROJ_ROOT_DIR = ../../../../..
PROJ_NAME     = TA_MAQ
PROJ_CODE_DIR = transactive/app/dbsynch/MAQLib
PROJ_SOURCE   = Agent.cpp \
				Consumer.cpp \
				ConsumerImpl.cpp \
				DatabaseImpl.cpp \
				DbDataImpl.cpp \
				DBException.cpp \
				Listener.cpp \
				MAQCmd.cpp \
				Message.cpp \
				COracleEnv.cpp \
				MessageImpl.cpp \
				MysqlConnection.cpp \
				MysqlQueryAdapter.cpp \
				OracleConnection.cpp \
				OracleQueryAdapter.cpp \
				Producer.cpp \
				ProducerImpl.cpp \
				SQLCode.cpp \
				SQLCodeHelper.cpp \
				SQLTypeAdapter.cpp \
				SQLVarParms.cpp \
				UtilityFun.cpp  \
				CMysqlEnv.cpp

PROJ_TYPE    = lib
PROJ_INC_DIR = ./src \
				$(PROJ_ROOT_DIR)/code/transactive \
				$(PROJ_ROOT_DIR)/code/cots/ocilib/3.9.3/src
PROJ_OUT_DIR = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)


##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing


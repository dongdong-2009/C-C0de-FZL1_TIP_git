PROJ_ROOT_DIR = ../../../../..
PROJ_NAME     = SynchEngine
PROJ_CODE_DIR = transactive/app/dbsynch/SynchEngine
PROJ_SOURCE   = AQMsgThreadPool.cpp \
				Buffer.cpp \
				ClientComWorker.cpp \
				ComProtocol.cpp \
				ConfigInfoHelper.cpp \
				DeqMsgJob.cpp \
				MsgDispatcher.cpp \
				MsgHelper.cpp \
				MsgItemImp.cpp \
				MsgPackageImp.cpp \
				MysqlSrvWorkSingle.cpp \
				ProcessRcvMsg.cpp \
				PropagateMsg.cpp \
				SrvComWorker.cpp \
				SrvDeqWorker.cpp \
				SrvEnqWorker.cpp \
				SrvInitWorker.cpp \
				SrvWorkMutil.cpp \
				SyncAQMsgManager.cpp \
				SynModeMonitor.cpp \
				TcpConnectionActor.cpp \
				TcpConnectionListen.cpp \
				TcpConnectionPool.cpp \
				IServerTcpComType.cpp  \
				UniqueCheck.cpp \
				VersionGen.cpp \
				ConnectionActorReleaser.cpp \
				MysqlSrvEnqWorker.cpp \
				WorkTime.cpp \
				MonitorWorker.cpp \
				main.cpp
PROJ_TYPE    = exe
PROJ_INC_DIR = ./src \
				$(PROJ_ROOT_DIR)/code/transactive \
				$(PROJ_ROOT_DIR)/code/transactive/app/dbsynch/MAQLib/src \
				$(PROJ_ROOT_DIR)/code/cots/ocilib/3.9.3/src
PROJ_OUT_DIR = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     =\
		TA_Thread \
		TA_Sockets \
        TA_Utilities \
		TA_Synchronisation \
        TA_Thread \
        TA_Uuid \
		TA_MAQ \
		TA_Exceptions \
        TA_Version \
		clntsh  \
		TA_Sockets \
		mysqlcppconn \
		mysqlclient \
		zlib \
		ocilib \
		ACE

DEPENDENT_LIBRARIES = \
 $(PROJ_ROOT_DIR)/code/transactive/app/dbsynch/MAQLib\
 $(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
 $(PROJ_ROOT_DIR)/code/transactive/core/versioning \
 $(PROJ_ROOT_DIR)/code/transactive/core/threads \
 $(PROJ_ROOT_DIR)/code/transactive/core/utilities \
 $(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
 $(PROJ_ROOT_DIR)/code/transactive/core/ocl \
 $(PROJ_ROOT_DIR)/code/transactive/core/uuid \
 $(PROJ_ROOT_DIR)/code/transactive/core/sockets

PROJ_CFLAGS = -DTA_ASSERT_LOG=1

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak
STD_LIB_DIRS += $(PROJ_ROOT_DIR)/code/cots/ocilib/3.9.3/lib/$(OPERATING_SYSTEM)_$(OP_SYS_VERSION)_$(ARCHITECTURE)_Oracle_$(ORACLE_VER)  \
				$(PROJ_ROOT_DIR)/code/cots/ZipArchive/zlib

install:
	#Do Nothing


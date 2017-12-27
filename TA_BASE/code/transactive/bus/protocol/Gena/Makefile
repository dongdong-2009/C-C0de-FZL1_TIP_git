# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/protocol/gena/Makefile $
# @author:  Ripple
# @version: $Revision: #4 $
#
# Last modification: $DateTime: 2012/07/24 11:03:41 $
# Last modified by:  $Author: Shiva $


PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/protocol/gena
PROJ_NAME	= TA_GenaLibrary
PROJ_SOURCE	= AbstractGENARequestProccessor.cpp \
			AbstractTopicListener.cpp \
			GenaBridge.cpp \
			GenaCallbackProccessor.cpp \
			GenaClient.cpp \
			GENAClientProxy.cpp \
			GenaConfigException.cpp \
			GenaConfigHelper.cpp \
			GENAConnectionFactory.cpp \
			GENAEvent.cpp \
			GENAHttpClient.cpp \
			GENAHttpServer.cpp \
			GENALibrary.cpp \
			GENAPublicationManager.cpp \
			GenaServer.cpp \
			GenaServerImp.cpp \
			GenaSubscriber.cpp \
			GENATopicData.cpp \
			GENATopicDataStore.cpp \
			GENATopicPusblisherImp.cpp \
			HTTPAppRequestHandler.cpp \
			HTTPAppRequestHandlerFactory.cpp \
			NoActiveConException.cpp \
			ServerConnectionManager.cpp \
			ServerModeException.cpp \
			ServerProxy.cpp \
			SIDManager.cpp \
			SIDNotFoundException.cpp \
			SubscriberMonitor.cpp \
			SubscriptionManager.cpp \
			ThreadConnResolve.cpp \
			TopicNotFoundException.cpp \
			TopicPublisherFactory.cpp \
			XmlUtility.cpp
						
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
        
DEPENDENT_LIBRARIES = \
 $(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
 $(PROJ_ROOT_DIR)/code/transactive/core/versioning \
 $(PROJ_ROOT_DIR)/code/transactive/core/corba \
 $(PROJ_ROOT_DIR)/code/transactive/core/threads \
 $(PROJ_ROOT_DIR)/code/transactive/core/utilities \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
 $(PROJ_ROOT_DIR)/code/transactive/core/database \
 $(PROJ_ROOT_DIR)/code/transactive/core/message \
 $(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
 $(PROJ_ROOT_DIR)/code/transactive/core/uuid \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/Gena \
 $(PROJ_ROOT_DIR)/code/transactive/core/ocl

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neat compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

STD_INC_DIRS += $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/Foundation/include \
 $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/Util/include \
 $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/XML/include \
 $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/Net/include \

LIB_DIRS += $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/lib

TP_LIBS += PocoUtil PocoNet PocoXML PocoFoundation
install:
        #Do Nothing

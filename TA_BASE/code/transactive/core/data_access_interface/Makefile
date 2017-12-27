# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/Makefile $ 
# Author:  Ripple
# Version: $Revision: #4 $
#
# Last modification: $DateTime: 2017/08/07 13:27:25 $
# Last modified by:  $Author: dhanshri $
#
#
PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface
PROJ_NAME	= TA_DAI
PROJ_SOURCE	= SimpleDbDatabase.cpp \
			ConfigOperator.cpp \
			DbConnection.cpp \
			DbConnectionStrings.cpp \
			DbStatus.cpp \
			DatabaseFactory.cpp \
			Operator.cpp \
			OperatorAccessFactory.cpp \
			OperatorHelper.cpp \
			SimpleDbData.cpp \
			ConfigMessageType.cpp \
			Gui.cpp \
			ConfigGui.cpp \
			GuiAccessFactory.cpp \
			GuiHelper.cpp \
			MessageTypeAccessFactory.cpp \
			MessageTypeData.cpp \
			MessageTypeHelper.cpp \
			MessageTypeEventData.cpp \
			ConfigProfile.cpp \
			Profile.cpp \
			ProfileHelper.cpp \
			ProfileAccessFactory.cpp \
			ProfileGroup.cpp \
			ConfigProfileGroup.cpp \
			ProfileGroupHelper.cpp \
			ProfileGroupAccessFactory.cpp \
			Action.cpp \
			ActionHelper.cpp \
			ActionAccessFactory.cpp \
			AlarmTypeAccessFactory.cpp \
			AlarmTypeData.cpp \
			AlarmTypeHelper.cpp \
			ConfigAlarmType.cpp \
			CombinedEventAccessFactory.cpp \
			CombinedEventData.cpp \
			T6AccessFactory.cpp \
			AlarmSeverityAccessFactory.cpp \
			AlarmSeverityData.cpp \
			AlarmSeverityHelper.cpp \
			ConfigAlarmSeverityData.cpp \
			EventColourAccessFactory.cpp \
			EventColourData.cpp \
			EventColourHelper.cpp \
			ConfigEventColourData.cpp \
			Session.cpp \
			SessionAccessFactory.cpp \
			PowerDemandAccessFactory.cpp \
			PowerDemandData.cpp \
			AlarmAccessFactory.cpp \
			AlarmData.cpp \
			MessageAccessFactory.cpp \
			MessageData.cpp \
			ConfigLocation.cpp \
			LocationAccessFactory.cpp \
			Location.cpp \
			LocationHelper.cpp \
			Subsystem.cpp \
			SubsystemHelper.cpp \
			SubsystemAccessFactory.cpp \
			ResourceHelper.cpp \
			Resource.cpp \
			ResourceAccessFactory.cpp \
			ScadaDisplayAccessFactory.cpp \
			SchematicDisplayMapAccessFactory.cpp \
			ScadaDisplay.cpp \
			ConfigScadaDisplay.cpp \
			ScadaDisplayHelper.cpp \
			DataPointStateChangeAccessFactory.cpp \
			DataPointStateChangeData.cpp \
			ConfigSubsystem.cpp \
			ActionGroup.cpp \
			ConfigActionGroup.cpp \
			ActionGroupHelper.cpp \
			ActionGroupAccessFactory.cpp \
			SubsystemState.cpp \
			SubsystemStateAccessFactory.cpp \
			ConfigGlobalParameter.cpp \
			GlobalParameter.cpp \
			GlobalParameterAccessFactory.cpp \
			GlobalParameterHelper.cpp \
			ConfigOnlinePrintCfg.cpp \
			OnlinePrintCfg.cpp \
			OnlinePrintCfgHelper.cpp \
			OnlinePrintCfgAccessFactory.cpp \
			ConfigOnlinePrinter.cpp \
			OnlinePrinter.cpp \
			OnlinePrinterHelper.cpp \
			OnlinePrinterAccessFactory.cpp \
			Region.cpp \
			RegionAccessFactory.cpp \
			RegionHelper.cpp \
			ConfigChangesTypes.cpp \
			ScadaBusinessItem.cpp \
			ScadaBusinessManagerAccessFactory.cpp \
			ScadaBusinessManagerAccessFactoryImpl.cpp \
			NamingPortAccessFactory.cpp \
			NamingPort.cpp \
			RightsAccessFactory.cpp \
			MmsRequestAlarmData.cpp \
			MmsRequestScheduledData.cpp \
			MmsRequestSchematicData.cpp \
			MmsRequestAccessFactory.cpp \
			ConfigRights.cpp \
			SecuritySettingAccessFactory.cpp \
			OperatorPassword.cpp \
			OperatorPasswordAccessFactory.cpp \
			SQLCode.cpp \
			SQLFileHelper.cpp \
			SQLTypeAdapter.cpp \
			SQLUtilityFun.cpp \
			SQLParamsHelper.cpp \
			SQLVarParms.cpp \
			AlarmDisplayFilterData.cpp \
			AlarmDisplayFilterAccessFactory.cpp \
			ModbusServerPointAccessFactory.cpp \
			ModbusServerPoint.cpp \
			ModbusServerAnaloguePoint.cpp
			
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)


PROJ_SQLGEN_INPUT = ./src/sqlcode.txt
PROJ_SQLGEN_OUTPUT = ./src

# BR TODO: These project flags look very suss - need to investigate
PROJ_CFLAGS = -DTA_ASSERT_LOG=1 -DPROJECT_4669
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing

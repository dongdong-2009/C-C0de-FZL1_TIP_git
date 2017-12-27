using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MulLanguage;
using TrendViewer.View;

namespace TrendViewer
{
    class LanguageHelper
    {
        private const string CLASS_NAME = "LanguageHelper";


        /////
        //caption name:
        public const string TrendViewer_CancelBtn = "TrendViewer_CancelBtn";
        public const string TrendViewer_SubmitBtn = "TrendViewer_SubmitBtn";
        public const string TrendViewer_ConfirmBtn = "TrendViewer_ConfirmBtn";
        public const string TrendViewer_TotalPageLbl = "TrendViewer_TotalPageLbl";
        public const string TrendViewer_LintTypeLine = "TrendViewer_LintTypeLine";
        public const string TrendViewer_LintTypeSpline = "TrendViewer_LintTypeSpline";
        public const string TrendViewer_EditHistDPTitle = "TrendViewer_EditHistDPTitle";
        public const string TrendViewer_EditDPTitle = "TrendViewer_EditDPTitle";
        public const string TrendViewer_DPLbl = "TrendViewer_DPLbl";
        public const string TrendViewer_ColorLbl = "TrendViewer_ColorLbl";
        public const string TrendViewer_LineTypeLbl = "TrendViewer_LineTypeLbl";
        public const string TrendViewer_SelectColorBtn = "TrendViewer_SelectColorBtn";
        public const string TrendViewer_BrowseBtn = "TrendViewer_BrowseBtn";
        public const string TrendViewer_ServerNameLbl = "TrendViewer_ServerNameLbl";
        public const string TrendViewer_LableLbl = "TrendViewer_LableLbl";
        public const string TrendViewer_Host = "TrendViewer_Host";
        public const string TrendViewer_DPVisibleCb = "TrendViewer_DPVisibleCb";
        public const string TrendViewer_LblEnabledCb = "TrendViewer_LblEnabledCb";
        public const string TrendViewer_DPEnabledCb = "TrendViewer_DPEnabledCb";
        public const string TrendViewer_HistDataPointData_StartDateLbl = "TrendViewer_HistDataPointData_StartDateLbl";
        public const string TrendViewer_HistDataPointData_EndDateLbl = "TrendViewer_HistDataPointData_EndDateLbl";
        public const string TrendViewer_HistDataPointData_AddDPTitle = "TrendViewer_HistDataPointData_AddDPTitle";
        public const string TrendViewer_MarkData_EditMarkerTitle = "TrendViewer_MarkData_EditMarkerTitle";
        public const string TrendViewer_MarkData_AddMarkerTitle = "TrendViewer_MarkData_AddMarkerTitle";
        public const string TrendViewer_MarkData_MarkerNameLbl = "TrendViewer_MarkData_MarkerNameLbl";
        public const string TrendViewer_MarkData_MarkerBColorLbl = "TrendViewer_MarkData_MarkerBColorLbl";
        public const string TrendViewer_MarkData_MarkerFColorLbl = "TrendViewer_MarkData_MarkerFColorLbl";
        public const string TrendViewer_MarkData_StartValLbl = "TrendViewer_MarkData_StartValLbl";
        public const string TrendViewer_MarkData_RangeLbl = "TrendViewer_MarkData_RangeLbl";
        public const string TrendViewer_MarkData_EnableMarkerCb = "TrendViewer_MarkData_EnableMarkerCb";
        public const string TrendViewer_FormulaData_FormulaLbl = "TrendViewer_FormulaData_FormulaLbl";
        public const string TrendViewer_FormulaData_EgLbl = "TrendViewer_FormulaData_EgLbl";
        public const string TrendViewer_FormulaData_AddTitle = "TrendViewer_FormulaData_AddTitle";
        public const string TrendViewer_FormulaData_EditTitle = "TrendViewer_FormulaData_EditTitle";
        public const string TrendViewer_EditChartForm_Title = "TrendViewer_EditChartForm_Title";
        public const string TrendViewer_EditChartForm_ChartTitleLbl = "TrendViewer_EditChartForm_ChartTitleLbl";
        public const string TrendViewer_EditChartForm_XTitleLbl = "TrendViewer_EditChartForm_XTitleLbl";
        public const string TrendViewer_EditChartForm_YTitleLbl = "TrendViewer_EditChartForm_YTitleLbl";
        public const string TrendViewer_FormulaList_Title = "TrendViewer_FormulaList_Title";
        public const string TrendViewer_FormulaList_GroupBox = "TrendViewer_FormulaList_GroupBox";
        public const string TrendViewer_FormulaList_AddMenu = "TrendViewer_FormulaList_AddMenu";
        public const string TrendViewer_FormulaList_LoadMenu = "TrendViewer_FormulaList_LoadMenu";
        public const string TrendViewer_FormulaList_SaveMenu = "TrendViewer_FormulaList_SaveMenu";
        public const string TrendViewer_MarkerList_Title = "TrendViewer_MarkerList_Title";
        public const string TrendViewer_MarkerList_GroupBox = "TrendViewer_MarkerList_GroupBox";
        public const string TrendViewer_MarkerList_AddMenu = "TrendViewer_MarkerList_AddMenu";
        public const string TrendViewer_MarkerList_LoadMenu = "TrendViewer_MarkerList_LoadMenu";
        public const string TrendViewer_MarkerList_SaveMenu = "TrendViewer_MarkerList_SaveMenu";
        public const string TrendViewer_LoadMarker_Title = "TrendViewer_LoadMarker_Title";
        public const string TrendViewer_SaveMarker_Title = "TrendViewer_SaveMarker_Title";
        public const string TrendViewer_HistDPList_Title = "TrendViewer_HistDPList_Title";
        public const string TrendViewer_HistDPList_GroupBox = "TrendViewer_HistDPList_GroupBox";
        public const string TrendViewer_HistDPList_AddMenu = "TrendViewer_HistDPList_AddMenu";
        public const string TrendViewer_HistDPList_LoadMenu = "TrendViewer_HistDPList_LoadMenu";
        public const string TrendViewer_HistDPList_SaveMenu = "TrendViewer_HistDPList_SaveMenu";
        public const string TrendViewer_DPList_Title = "TrendViewer_DPList_Title";
        public const string TrendViewer_DPList_GroupBox = "TrendViewer_DPList_GroupBox";
        public const string TrendViewer_DPList_AddMenu = "TrendViewer_DPList_AddMenu";
        public const string TrendViewer_DPList_LoadMenu = "TrendViewer_DPList_LoadMenu";
        public const string TrendViewer_DPList_SaveMenu = "TrendViewer_DPList_SaveMenu";
        public const string TrendViewer_GroupConfigurationLbl = "TrendViewer_GroupConfigurationLbl";
        public const string TrendViewer_HistDPGroup_TitleLoad = "TrendViewer_HistDPGroup_TitleLoad";
        public const string TrendViewer_HistDPGroup_TitleSave = "TrendViewer_HistDPGroup_TitleSave";
        public const string TrendViewer_AboutFormTitle = "TrendViewer_AboutFormTitle";
        public const string TrendViewer_OPCDataSelector_GrpBoxName = "TrendViewer_OPCDataSelector_GrpBoxName";
        public const string TrendViewer_OPCDataSelector_OPCDPColumnName = "TrendViewer_OPCDataSelector_OPCDPColumnName";
        public const string TrendViewer_OPCDataSelector_DesColumnName = "TrendViewer_OPCDataSelector_DesColumnName";
        public const string TrendViewer_OPCDataSelector_OPCServerLbl = "TrendViewer_OPCDataSelector_OPCServerLbl";
        public const string TrendViewer_OPCDataSelector_FormTitle = "TrendViewer_OPCDataSelector_FormTitle";
        public const string TrendViewer_TrendView_Title = "TrendViewer_TrendView_Title";
        public const string TrendViewer_TrendView_Second = "TrendViewer_TrendView_Second";
        public const string TrendViewer_TrendView_Minute = "TrendViewer_TrendView_Minute";
        public const string TrendViewer_TrendView_Hour = "TrendViewer_TrendView_Hour";
        public const string TrendViewer_TrendView_Day = "TrendViewer_TrendView_Day";
        public const string TrendViewer_TrendView_Start = "TrendViewer_TrendView_Start";
        public const string TrendViewer_TrendView_Stop = "TrendViewer_TrendView_Stop";
        public const string TrendViewer_TrendView_GraphLblHist = "TrendViewer_TrendView_GraphLblHist";
        public const string TrendViewer_TrendView_GraphLblRT = "TrendViewer_TrendView_GraphLblRT";
        public const string TrendViewer_TrendView_GraphLblMixed = "TrendViewer_TrendView_GraphLblMixed";
        public const string TrendViewer_TrendView_MixModeCb = "TrendViewer_TrendView_MixModeCb";
        public const string TrendViewer_TrendView_PlotIntervalLbl = "TrendViewer_TrendView_PlotIntervalLbl";
        public const string TrendViewer_TrendView_TimeFrameLbl = "TrendViewer_TrendView_TimeFrameLbl";
        public const string TrendViewer_TrendView_3DModeCb = "TrendViewer_TrendView_3DModeCb";
        public const string TrendViewer_TrendView_GrpBox = "TrendViewer_TrendView_GrpBox";
        public const string TrendViewer_TrendView_ChartTitle = "TrendViewer_TrendView_ChartTitle";
        public const string TrendViewer_TrendView_StartTimeLbl = "TrendViewer_TrendView_StartTimeLbl";
        public const string TrendViewer_TrendView_EndTimeLbl = "TrendViewer_TrendView_EndTimeLbl";
        public const string TrendViewer_TrendView_RTDataGrpBox = "TrendViewer_TrendView_RTDataGrpBox";
        public const string TrendViewer_TrendView_HistDataGrpBox = "TrendViewer_TrendView_HistDataGrpBox";
        public const string TrendViewer_TrendView_MenuUtiCommand = "TrendViewer_TrendView_MenuUtiCommand";
        public const string TrendViewer_TrendView_MenuExport = "TrendViewer_TrendView_MenuExport";
        public const string TrendViewer_TrendView_MenuScreenShort = "TrendViewer_TrendView_MenuScreenShort";
        public const string TrendViewer_TrendView_MenuConfig = "TrendViewer_TrendView_MenuConfig";
        public const string TrendViewer_TrendView_MenuDPConfig = "TrendViewer_TrendView_MenuDPConfig";
        public const string TrendViewer_TrendView_MenuHistDPConfig = "TrendViewer_TrendView_MenuHistDPConfig";
        public const string TrendViewer_TrendView_MenuMarkerConfig = "TrendViewer_TrendView_MenuMarkerConfig";
        public const string TrendViewer_TrendView_MenuChangeChartTitle = "TrendViewer_TrendView_MenuChangeChartTitle";
        public const string TrendViewer_TrendView_MenuSampleGrpConfig = "TrendViewer_TrendView_MenuSampleGrpConfig";
        public const string TrendViewer_TrendView_MenuFormulaConfig = "TrendViewer_TrendView_MenuFormulaConfig";
        public const string TrendViewer_TrendView_MenuPrint = "TrendViewer_TrendView_MenuPrint";
        public const string TrendViewer_TrendView_MenuAbout = "TrendViewer_TrendView_MenuAbout";
        public const string TrendViewer_ColDPEnabled = "TrendViewer_ColDPEnabled";
        public const string TrendViewer_ColLblEnabled = "TrendViewer_ColLblEnabled";
        public const string TrendViewer_ColDispDP = "TrendViewer_ColDispDP";
        public const string TrendViewer_ColDPName = "TrendViewer_ColDPName";
        public const string TrendViewer_ColLengend = "TrendViewer_ColLengend";
        public const string TrendViewer_ColDPType = "TrendViewer_ColDPType";
        public const string TrendViewer_ColDPColor = "TrendViewer_ColDPColor";
        public const string TrendViewer_ColServer = "TrendViewer_ColServer";
        public const string TrendViewer_ColHost = "TrendViewer_ColHost";
        public const string TrendViewer_ColEdit = "TrendViewer_ColEdit";
        public const string TrendViewer_ColDelete = "TrendViewer_ColDelete";
        public const string TrendViewer_ColStartDate = "TrendViewer_ColStartDate";
        public const string TrendViewer_ColEndDate = "TrendViewer_ColEndDate";
        public const string TrendViewer_ColMarkerEnabled = "TrendViewer_ColMarkerEnabled";
        public const string TrendViewer_ColMarkerName = "TrendViewer_ColMarkerName";
        public const string TrendViewer_ColMarkerWidth = "TrendViewer_ColMarkerWidth";
        public const string TrendViewer_ColMarkerBcolor = "TrendViewer_ColMarkerBcolor";
        public const string TrendViewer_ColMarkerFontColor = "TrendViewer_ColMarkerFontColor";
        public const string TrendViewer_ColMarkerStartVal = "TrendViewer_ColMarkerStartVal";
        public const string TrendViewer_ColFormula = "TrendViewer_ColFormula";
        public const string TrendViewer_Msg_SysMsg = "TrendViewer_Msg_SysMsg";
        public const string TrendViewer_Msg_DelConfirm = "TrendViewer_Msg_DelConfirm";
        public const string TrendViewer_Msg_StopRT2Continue = "TrendViewer_Msg_StopRT2Continue";
        public const string TrendViewer_Msg_DuplicateName = "TrendViewer_Msg_DuplicateName";
        public const string TrendViewer_Msg_EmptyName = "TrendViewer_Msg_EmptyName";
        public const string TrendViewer_Msg_SelectConfirm = "TrendViewer_Msg_SelectConfirm";
        public const string TrendViewer_Msg_ScreenShotOK = "TrendViewer_Msg_ScreenShotOK";
        public const string TrendViewer_Msg_ExportOK = "TrendViewer_Msg_ExportOK";
        public const string TrendViewer_Msg_ErrTitle = "TrendViewer_Msg_ErrTitle";
        public const string TrendViewer_Msg_WarningTitle = "TrendViewer_Msg_WarningTitle";
        public const string TrendViewer_Msg_SelectConfigForLoad = "TrendViewer_Msg_SelectConfigForLoad";
        public const string TrendViewer_Msg_NoConn2OPC = "TrendViewer_Msg_NoConn2OPC";
        public const string TrendViewer_Msg_AddOPCItemErr = "TrendViewer_Msg_AddOPCItemErr";
        public const string TrendViewer_Msg_RemoveOPCItemErr = "TrendViewer_Msg_RemoveOPCItemErr";
        public const string TrendViewer_Msg_NoDB = "TrendViewer_Msg_NoDB";
        public const string TrendViewer_Msg_UnhandledError = "TrendViewer_Msg_UnhandledError";
        public const string TrendViewer_Msg_LoadDataError = "TrendViewer_Msg_LoadDataError";
        public const string TrendViewer_Msg_SaveError = "TrendViewer_Msg_SaveError";
        public const string TrendViewer_Msg_ExportNoData = "TrendViewer_Msg_ExportNoData";
        public const string TrendViewer_Msg_OPCNOK = "TrendViewer_Msg_OPCNOK";
        public const string TrendViewer_Msg_NoDB2 = "TrendViewer_Msg_NoDB2";
        public const string TrendViewer_Msg_ScreenShotNOK_1 = "TrendViewer_Msg_ScreenShotNOK_1";
        public const string TrendViewer_Msg_NoConfigToLoad = "TrendViewer_Msg_NoConfigToLoad";
        public const string TrendViewer_Msg_NoConfigToSave = "TrendViewer_Msg_NoConfigToSave";
        public const string TrendViewer_Msg_StartDateMoreThenEnd = "TrendViewer_Msg_StartDateMoreThenEnd";
        public const string TrendViewer_AddDPTitle = "TrendViewer_AddDPTitle";
        public const string TrendViewer_HistDataPointData_EditDPTitle = "TrendViewer_HistDataPointData_EditDPTitle";


        ///////
        //caption string CH an EN
        public const string TrendViewer_CancelBtn_EN = "Cancel";
        public const string TrendViewer_CancelBtn_CH = "取消";
        public const string TrendViewer_SubmitBtn_EN = "OK";
        public const string TrendViewer_SubmitBtn_CH = "呈交";
        public const string TrendViewer_ConfirmBtn_EN = "OK";
        public const string TrendViewer_ConfirmBtn_CH = "确认";
        public const string TrendViewer_TotalPageLbl_EN = "Total Page:";
        public const string TrendViewer_TotalPageLbl_CH = "总页数：";
        public const string TrendViewer_LintTypeLine_EN = "Line";
        public const string TrendViewer_LintTypeLine_CH = "直线";
        public const string TrendViewer_LintTypeSpline_EN = "Spline";
        public const string TrendViewer_LintTypeSpline_CH = "曲线";

        public const string TrendViewer_EditHistDPTitle_EN = "Edit Historical Datapoint";
        public const string TrendViewer_EditHistDPTitle_CH = "编辑历史数据点";
        public const string TrendViewer_EditDPTitle_EN = "Edit Datapoint";
        public const string TrendViewer_EditDPTitle_CH = "编辑数据点";
        public const string TrendViewer_AddDPTitle_EN = "Add Datapoint";
        public const string TrendViewer_AddDPTitle_CH = "添加数据点";
        public const string TrendViewer_DPLbl_EN = "DataPoint:";
        public const string TrendViewer_DPLbl_CH = "数据点：";
        public const string TrendViewer_ColorLbl_EN = "Color:";
        public const string TrendViewer_ColorLbl_CH = "颜色：";
        public const string TrendViewer_LineTypeLbl_EN = "Line Type:";
        public const string TrendViewer_LineTypeLbl_CH = "系列类型：";
        public const string TrendViewer_SelectColorBtn_EN = "Select Color";
        public const string TrendViewer_SelectColorBtn_CH = "颜色选择";
        public const string TrendViewer_BrowseBtn_EN = "Browse";
        public const string TrendViewer_BrowseBtn_CH = "浏览";
        public const string TrendViewer_ServerNameLbl_EN = "Server:";
        public const string TrendViewer_ServerNameLbl_CH = "服务器名：";
        public const string TrendViewer_LableLbl_EN = "Legend:";
        public const string TrendViewer_LableLbl_CH = "图例";
        public const string TrendViewer_Host_EN = "Hostname:";
        public const string TrendViewer_Host_CH = "主机：";
        public const string TrendViewer_DPVisibleCb_EN = "Display";
        public const string TrendViewer_DPVisibleCb_CH = "显示数据点";
        public const string TrendViewer_LblEnabledCb_EN = "Enable Tag";
        public const string TrendViewer_LblEnabledCb_CH = "标签值允许";
        public const string TrendViewer_DPEnabledCb_EN = "Enable Datapoint";
        public const string TrendViewer_DPEnabledCb_CH = "数据点允许";

        public const string TrendViewer_HistDataPointData_StartDateLbl_EN =  "Start Date:";
        public const string TrendViewer_HistDataPointData_StartDateLbl_CH =  "开始日期：";
        public const string TrendViewer_HistDataPointData_EndDateLbl_EN =  "End Date:";
        public const string TrendViewer_HistDataPointData_EndDateLbl_CH =  "结束日期：";
        public const string TrendViewer_HistDataPointData_AddDPTitle_EN =  "Add Datapoint History";
        public const string TrendViewer_HistDataPointData_AddDPTitle_CH =  "添加历史数据点";
        public const string TrendViewer_HistDataPointData_EditDPTitle_EN = "Edit Datapoint History";
        public const string TrendViewer_HistDataPointData_EditDPTitle_CH = "编辑历史数据点";
        public const string TrendViewer_MarkData_EditMarkerTitle_EN =  "Edit Data Marker";
        public const string TrendViewer_MarkData_EditMarkerTitle_CH =  "编辑数据标记";
        public const string TrendViewer_MarkData_AddMarkerTitle_EN =  "Add Data Marker";
        public const string TrendViewer_MarkData_AddMarkerTitle_CH =  "添加数据标记";
        public const string TrendViewer_MarkData_MarkerNameLbl_EN =  "Marker Name:";
        public const string TrendViewer_MarkData_MarkerNameLbl_CH =  "标签名：";
        public const string TrendViewer_MarkData_MarkerBColorLbl_EN =  "Background Color:";
        public const string TrendViewer_MarkData_MarkerBColorLbl_CH =  "背景颜色：";
        public const string TrendViewer_MarkData_MarkerFColorLbl_EN =  "Font Color:";
        public const string TrendViewer_MarkData_MarkerFColorLbl_CH =  "字体颜色：";
        public const string TrendViewer_MarkData_StartValLbl_EN =  "Start Value:";
        public const string TrendViewer_MarkData_StartValLbl_CH =  "值：";
        public const string TrendViewer_MarkData_RangeLbl_EN =  "Range:";
        public const string TrendViewer_MarkData_RangeLbl_CH =  "线宽度：";
        public const string TrendViewer_MarkData_EnableMarkerCb_EN =  "Enable Marker";
        public const string TrendViewer_MarkData_EnableMarkerCb_CH =  "数据标记允许";
        public const string TrendViewer_FormulaData_FormulaLbl_EN =  "Formula:";
        public const string TrendViewer_FormulaData_FormulaLbl_CH =  "公式：";
        public const string TrendViewer_FormulaData_EgLbl_EN =  "Eg : 2*::OPCDataPointName::+(100/5)";
        public const string TrendViewer_FormulaData_EgLbl_CH =  "例如: 2*::OPCDataPointName::+(100/5)";
        public const string TrendViewer_FormulaData_AddTitle_EN =  "Add Formula";
        public const string TrendViewer_FormulaData_AddTitle_CH =  "添加数据点公式";
        public const string TrendViewer_FormulaData_EditTitle_EN =  "Edit Formula";
        public const string TrendViewer_FormulaData_EditTitle_CH =  "编辑数据点公式";
        public const string TrendViewer_EditChartForm_Title_EN =  "Trend Title";
        public const string TrendViewer_EditChartForm_Title_CH =  "修改趋势图标题";
        public const string TrendViewer_EditChartForm_ChartTitleLbl_EN =  "Trend Title:";
        public const string TrendViewer_EditChartForm_ChartTitleLbl_CH =  "趋势图标题：";
        public const string TrendViewer_EditChartForm_XTitleLbl_EN =  "X-axis Label:";
        public const string TrendViewer_EditChartForm_XTitleLbl_CH =  "X轴标签：";
        public const string TrendViewer_EditChartForm_YTitleLbl_EN =  "Y-axis Label:";
        public const string TrendViewer_EditChartForm_YTitleLbl_CH =  "Y轴标签：";
        public const string TrendViewer_FormulaList_Title_EN =  "Datapoint Formula Viewer";
        public const string TrendViewer_FormulaList_Title_CH =  "数据点公式阅读器";
        public const string TrendViewer_FormulaList_GroupBox_EN =  "Datapoint Formula List";
        public const string TrendViewer_FormulaList_GroupBox_CH =  "数据点公式列表";
        public const string TrendViewer_FormulaList_AddMenu_EN =  "Add Formula Configuration";
        public const string TrendViewer_FormulaList_AddMenu_CH =  "添加公式配置";
        public const string TrendViewer_FormulaList_LoadMenu_EN =  "Load Formula Configuration";
        public const string TrendViewer_FormulaList_LoadMenu_CH =  "负载公式配置";
        public const string TrendViewer_FormulaList_SaveMenu_EN =  "Save Formula Configuration";
        public const string TrendViewer_FormulaList_SaveMenu_CH =  "保存公式配置";
        public const string TrendViewer_MarkerList_Title_EN =  "Data Marker Viewer";
        public const string TrendViewer_MarkerList_Title_CH =  "数据标记阅读器";
        public const string TrendViewer_MarkerList_GroupBox_EN =  "Data Marker List";
        public const string TrendViewer_MarkerList_GroupBox_CH =  "数据标记列表";
        public const string TrendViewer_MarkerList_AddMenu_EN =  "Add Data Marker";
        public const string TrendViewer_MarkerList_AddMenu_CH =  "添加数据标记";
        public const string TrendViewer_MarkerList_LoadMenu_EN =  "Load Data Marker Configuration";
        public const string TrendViewer_MarkerList_LoadMenu_CH =  "负载数据标记配置";
        public const string TrendViewer_MarkerList_SaveMenu_EN =  "Save Data Marker Configuration";
        public const string TrendViewer_MarkerList_SaveMenu_CH =  "保存数据标记配置";
        public const string TrendViewer_LoadMarker_Title_EN =  "Load Marker Configuration";
        public const string TrendViewer_LoadMarker_Title_CH =  "负载数据标记配置";
        public const string TrendViewer_SaveMarker_Title_EN =  "Save Marker Configuration";
        public const string TrendViewer_SaveMarker_Title_CH =  "保存数据标记配置";
        public const string TrendViewer_HistDPList_Title_EN =  "Historical Datapoint Viewer";
        public const string TrendViewer_HistDPList_Title_CH =  "数据点历史阅读器";
        public const string TrendViewer_HistDPList_GroupBox_EN =  "Historical Datapoint List";
        public const string TrendViewer_HistDPList_GroupBox_CH =  "历史数据点列表";
        public const string TrendViewer_HistDPList_AddMenu_EN =  "Add Datapoint Configuration";
        public const string TrendViewer_HistDPList_AddMenu_CH =  "添加数据点配置";
        public const string TrendViewer_HistDPList_LoadMenu_EN =  "Load Datapoint Configuration";
        public const string TrendViewer_HistDPList_LoadMenu_CH =  "负载历史数据配置";
        public const string TrendViewer_HistDPList_SaveMenu_EN =  "Save Datapoint Configuration";
        public const string TrendViewer_HistDPList_SaveMenu_CH =  "保存历史数据配置";
        public const string TrendViewer_DPList_Title_EN =  "Datapoint Viewer";
        public const string TrendViewer_DPList_Title_CH =  "数据点阅读器";
        public const string TrendViewer_DPList_GroupBox_EN =  "Datapoint List";
        public const string TrendViewer_DPList_GroupBox_CH =  "数据点列表";
        public const string TrendViewer_DPList_AddMenu_EN =  "Add Datapoint Configuration";
        public const string TrendViewer_DPList_AddMenu_CH =  "添加数据点配置";
        public const string TrendViewer_DPList_LoadMenu_EN =  "Load Datapoint Configuration";
        public const string TrendViewer_DPList_LoadMenu_CH =  "负载数据点配置";
        public const string TrendViewer_DPList_SaveMenu_EN =  "Save Datapoint Configuration";
        public const string TrendViewer_DPList_SaveMenu_CH =  "保存数据点配置";
        public const string TrendViewer_GroupConfigurationLbl_EN =  "Configuration Name:";
        public const string TrendViewer_GroupConfigurationLbl_CH =  "配置名：";
        public const string TrendViewer_HistDPGroup_TitleLoad_EN =  "Load Historical Data Configuration";
        public const string TrendViewer_HistDPGroup_TitleLoad_CH =  "负载历史数据配置";
        public const string TrendViewer_HistDPGroup_TitleSave_EN =  "Save Historical Data Configuration";
        public const string TrendViewer_HistDPGroup_TitleSave_CH =  "保存历史数据配置";
        public const string TrendViewer_AboutFormTitle_EN =  "About";
        public const string TrendViewer_AboutFormTitle_CH =  "关于";
        public const string TrendViewer_OPCDataSelector_GrpBoxName_EN =  "Datapoint List";
        public const string TrendViewer_OPCDataSelector_GrpBoxName_CH =  "数据点列表";
        public const string TrendViewer_OPCDataSelector_OPCDPColumnName_EN =  "OPC Datapoint";
        public const string TrendViewer_OPCDataSelector_OPCDPColumnName_CH =  "OPC 数据点";
        public const string TrendViewer_OPCDataSelector_DesColumnName_EN =  "Description";
        public const string TrendViewer_OPCDataSelector_DesColumnName_CH =  "描述";
        public const string TrendViewer_OPCDataSelector_OPCServerLbl_EN =  "OPC Server:";
        public const string TrendViewer_OPCDataSelector_OPCServerLbl_CH =  "OPC 服务器：";
        public const string TrendViewer_OPCDataSelector_FormTitle_EN =  "OPC Browser";
        public const string TrendViewer_OPCDataSelector_FormTitle_CH =  "OPC 浏览器";
        public const string TrendViewer_TrendView_Title_EN =  "OPC TrendViewer";
        public const string TrendViewer_TrendView_Title_CH = "趋势图阅读器";
        public const string TrendViewer_TrendView_Second_EN =  "Second";
        public const string TrendViewer_TrendView_Second_CH =  "秒";
        public const string TrendViewer_TrendView_Minute_EN =  "Minute";
        public const string TrendViewer_TrendView_Minute_CH =  "分";
        public const string TrendViewer_TrendView_Hour_EN =  "Hour";
        public const string TrendViewer_TrendView_Hour_CH =  "时";
        public const string TrendViewer_TrendView_Day_EN =  "Day";
        public const string TrendViewer_TrendView_Day_CH =  "天";
        public const string TrendViewer_TrendView_Start_EN =  "Start";
        public const string TrendViewer_TrendView_Start_CH =  "开始";
        public const string TrendViewer_TrendView_Stop_EN =  "Stop";
        public const string TrendViewer_TrendView_Stop_CH =  "停止";
        public const string TrendViewer_TrendView_GraphLblHist_EN =  "HISTORICAL";
        public const string TrendViewer_TrendView_GraphLblHist_CH = "历史";
        public const string TrendViewer_TrendView_GraphLblRT_EN =  "REALTIME";
        public const string TrendViewer_TrendView_GraphLblRT_CH =  "实时";
        public const string TrendViewer_TrendView_GraphLblMixed_EN =  "MIXED";
        public const string TrendViewer_TrendView_GraphLblMixed_CH =  "混合模式";
        public const string TrendViewer_TrendView_MixModeCb_EN = "Mixed Mode";
        public const string TrendViewer_TrendView_MixModeCb_CH = "混合模式";
        public const string TrendViewer_TrendView_PlotIntervalLbl_EN =  "Plotting Interval:";
        public const string TrendViewer_TrendView_PlotIntervalLbl_CH = "时间间隔：";
        public const string TrendViewer_TrendView_TimeFrameLbl_EN =  "Time Frame:";
        public const string TrendViewer_TrendView_TimeFrameLbl_CH = "时帧：";
        public const string TrendViewer_TrendView_3DModeCb_EN =  "3D";
        public const string TrendViewer_TrendView_3DModeCb_CH = "3D图标";
        public const string TrendViewer_TrendView_GrpBox_EN =  "Trend";
        public const string TrendViewer_TrendView_GrpBox_CH =  "趋势图";
        public const string TrendViewer_TrendView_ChartTitle_EN =  "Data Trend";
        public const string TrendViewer_TrendView_ChartTitle_CH =  "数据点趋势";
        public const string TrendViewer_TrendView_StartTimeLbl_EN = "Start Time:";
        public const string TrendViewer_TrendView_StartTimeLbl_CH =  "开始时间：";
        public const string TrendViewer_TrendView_EndTimeLbl_EN = "End Time:";
        public const string TrendViewer_TrendView_EndTimeLbl_CH =  "结束时间：";
        public const string TrendViewer_TrendView_RTDataGrpBox_EN =  "Realtime Datapoint";
        public const string TrendViewer_TrendView_RTDataGrpBox_CH =  "实时数据";
        public const string TrendViewer_TrendView_HistDataGrpBox_EN =  "Historical Datapoint";
        public const string TrendViewer_TrendView_HistDataGrpBox_CH =  "历史数据";
        public const string TrendViewer_TrendView_MenuUtiCommand_EN =  "Utility Command";
        public const string TrendViewer_TrendView_MenuUtiCommand_CH = "文件";
        public const string TrendViewer_TrendView_MenuExport_EN =  "Export History Data";
        public const string TrendViewer_TrendView_MenuExport_CH = "导出历史数据";
        public const string TrendViewer_TrendView_MenuScreenShort_EN = "Screenshot Trend";
        public const string TrendViewer_TrendView_MenuScreenShort_CH = "屏幕显示趋势";
        public const string TrendViewer_TrendView_MenuConfig_EN =  "Configuration";
        public const string TrendViewer_TrendView_MenuConfig_CH = "配置设定";
        public const string TrendViewer_TrendView_MenuDPConfig_EN =  "Datapoint Configuration";
        public const string TrendViewer_TrendView_MenuDPConfig_CH =  "数据点配置";
        public const string TrendViewer_TrendView_MenuHistDPConfig_EN =  "Historical Data Configuration";
        public const string TrendViewer_TrendView_MenuHistDPConfig_CH =  "历史数据配置";
        public const string TrendViewer_TrendView_MenuMarkerConfig_EN =  "Marker Data Configuration";
        public const string TrendViewer_TrendView_MenuMarkerConfig_CH = "标记数据配置";
        public const string TrendViewer_TrendView_MenuChangeChartTitle_EN = "Change Trend Title";
        public const string TrendViewer_TrendView_MenuChangeChartTitle_CH = "修改趋势图标题";
        public const string TrendViewer_TrendView_MenuSampleGrpConfig_EN =  "OPC DataLog Configuration";
        public const string TrendViewer_TrendView_MenuSampleGrpConfig_CH = "数据列表配置";
        public const string TrendViewer_TrendView_MenuFormulaConfig_EN =  "Formula Configuration";
        public const string TrendViewer_TrendView_MenuFormulaConfig_CH = "公式配置";
        public const string TrendViewer_TrendView_MenuPrint_EN =  "Print";
        public const string TrendViewer_TrendView_MenuPrint_CH =  "打印";
        public const string TrendViewer_TrendView_MenuAbout_EN =  "About";
        public const string TrendViewer_TrendView_MenuAbout_CH =  "关于";
        public const string TrendViewer_ColDPEnabled_EN =  "Enable Datapoint";
        public const string TrendViewer_ColDPEnabled_CH =  "数据点允许";
        public const string TrendViewer_ColLblEnabled_EN =  "Enable Tag";
        public const string TrendViewer_ColLblEnabled_CH = "标签允许";
        public const string TrendViewer_ColDispDP_EN =  "Display Datapoint";
        public const string TrendViewer_ColDispDP_CH =  "显示数据点";
        public const string TrendViewer_ColDPName_EN =  "Datapoint Name";
        public const string TrendViewer_ColDPName_CH = "数据点名";
        public const string TrendViewer_ColLengend_EN = "Legend";
        public const string TrendViewer_ColLengend_CH =  "图例";
        public const string TrendViewer_ColDPType_EN =  "Line Type";
        public const string TrendViewer_ColDPType_CH =  "数据点类型";
        public const string TrendViewer_ColDPColor_EN =  "Color";
        public const string TrendViewer_ColDPColor_CH =  "数据点颜色";
        public const string TrendViewer_ColServer_EN =  "Server";
        public const string TrendViewer_ColServer_CH = "数据点服务器";
        public const string TrendViewer_ColHost_EN =  "Host";
        public const string TrendViewer_ColHost_CH = "数据点服务器";
        public const string TrendViewer_ColEdit_EN =  "Edit";
        public const string TrendViewer_ColEdit_CH =  "编辑";
        public const string TrendViewer_ColDelete_EN =  "Delete";
        public const string TrendViewer_ColDelete_CH =  "删除";
        public const string TrendViewer_ColStartDate_EN =  "Start";
        public const string TrendViewer_ColStartDate_CH =  "开始日期";
        public const string TrendViewer_ColEndDate_EN =  "End";
        public const string TrendViewer_ColEndDate_CH =  "结束日期";
        public const string TrendViewer_ColMarkerEnabled_EN = "Enable Marker";
        public const string TrendViewer_ColMarkerEnabled_CH =  "数据标记允许";
        public const string TrendViewer_ColMarkerName_EN =  "Marker Name";
        public const string TrendViewer_ColMarkerName_CH =  "数据标记名";
        public const string TrendViewer_ColMarkerWidth_EN =  "Range";
        public const string TrendViewer_ColMarkerWidth_CH =  "宽度";
        public const string TrendViewer_ColMarkerBcolor_EN =  "Background Color";
        public const string TrendViewer_ColMarkerBcolor_CH = "背景颜色";
        public const string TrendViewer_ColMarkerFontColor_EN =  "Font Color";
        public const string TrendViewer_ColMarkerFontColor_CH =  "字体颜色";
        public const string TrendViewer_ColMarkerStartVal_EN =  "Start Value";
        public const string TrendViewer_ColMarkerStartVal_CH =  "值";
        public const string TrendViewer_ColFormula_EN =  "Formula";
        public const string TrendViewer_ColFormula_CH =  "数据点公式";
        public const string TrendViewer_Msg_SysMsg_EN =  "System Message";
        public const string TrendViewer_Msg_SysMsg_CH =  "系统消息";
        public const string TrendViewer_Msg_DelConfirm_EN =  "Delete Current Selection?";
        public const string TrendViewer_Msg_DelConfirm_CH =  "确实要删除当前选择项?";
        public const string TrendViewer_Msg_StopRT2Continue_EN =  "Please stop the realtime trend in order to continue";
        public const string TrendViewer_Msg_StopRT2Continue_CH =  "请先停止实时趋势图再继续";
        public const string TrendViewer_Msg_DuplicateName_EN =  "The name shouldn't be the same as existing ones.";
        public const string TrendViewer_Msg_DuplicateName_CH =  "名称不能与列表中的重复。";
        public const string TrendViewer_Msg_EmptyName_EN =  "The name shouldn't be empty.";
        public const string TrendViewer_Msg_EmptyName_CH =  "名称不能为空";
        public const string TrendViewer_Msg_SelectConfirm_EN =  "Please select a configuration to load";
        public const string TrendViewer_Msg_SelectConfirm_CH =  "请选择一个配置再开始画图";
        public const string TrendViewer_Msg_ScreenShotOK_EN =  "Chart Screen Shot has been generated successfully";
        public const string TrendViewer_Msg_ScreenShotOK_CH = "趋势图已截屏成功";
        public const string TrendViewer_Msg_ExportOK_EN =  "Export History Data has been generated successfully";
        public const string TrendViewer_Msg_ExportOK_CH =  "导出历史数据成功";
        public const string TrendViewer_Msg_ErrTitle_EN =  "Error";
        public const string TrendViewer_Msg_ErrTitle_CH =  "错误";
        public const string TrendViewer_Msg_WarningTitle_EN =  "Warning";
        public const string TrendViewer_Msg_WarningTitle_CH =  "警告";
        public const string TrendViewer_Msg_SelectConfigForLoad_EN =  "Please Select a Configuration to load";
        public const string TrendViewer_Msg_SelectConfigForLoad_CH =  "请选择要负载的一个配置";
        public const string TrendViewer_Msg_NoConn2OPC_EN =  "No connection to OPC Bridge";
        public const string TrendViewer_Msg_NoConn2OPC_CH = "无法连接到OPC Bridge.";
        public const string TrendViewer_Msg_AddOPCItemErr_EN = "Some data point can't be added to OPC Group";
        public const string TrendViewer_Msg_AddOPCItemErr_CH = "有些数据点不能加入OPC Group.";
        public const string TrendViewer_Msg_RemoveOPCItemErr_EN = "Cant's start real-time charting because got error when remove the old data point items from OPC group.";
        public const string TrendViewer_Msg_RemoveOPCItemErr_CH = "由于从OPC Group中删除旧数据点是出现错误，不能启动实时画图";
        public const string TrendViewer_Msg_NoDB_EN =  "Unable to open the datapoint selection dialog due to database error.";
        public const string TrendViewer_Msg_NoDB_CH =  "由于数据库错误，无法数据点选择页面";
        public const string TrendViewer_Msg_UnhandledError_EN = "Unhandled exception caught.";
        public const string TrendViewer_Msg_UnhandledError_CH = "捕获到一个未处理的异常";
        public const string TrendViewer_Msg_LoadDataError_EN = "Can not load data. No Connection to database.";
        public const string TrendViewer_Msg_LoadDataError_CH = "无法加载数据，数据库无连接。";
        public const string TrendViewer_Msg_SaveError_EN = "Save data unsuccessfully.";
        public const string TrendViewer_Msg_SaveError_CH = "保存数据失败！";
        public const string TrendViewer_Msg_ExportNoData_EN =  "No Data for current Datapoint History.";
        public const string TrendViewer_Msg_ExportNoData_CH =  "当前历史数据点无数据。";
        public const string TrendViewer_Msg_OPCNOK_EN =  "No connection to OPC Bridge.";
        public const string TrendViewer_Msg_OPCNOK_CH =  "OPC服务器连接不上";
        public const string TrendViewer_Msg_NoDB2_EN =  "No Connection to Database.";
        public const string TrendViewer_Msg_NoDB2_CH = "数据库连接不上";
        public const string TrendViewer_Msg_ScreenShotNOK_1_EN =  "Unable to generate the screen shot due to ";
        public const string TrendViewer_Msg_ScreenShotNOK_1_CH =  "无法截屏，因为：";
        public const string TrendViewer_Msg_NoConfigToLoad_EN =  "Please Select a Configuration to load.";
        public const string TrendViewer_Msg_NoConfigToLoad_CH =  "请选择要负载的一个配置";
        public const string TrendViewer_Msg_NoConfigToSave_EN =  "Please Select a Configuration to save.";
        public const string TrendViewer_Msg_NoConfigToSave_CH =  "请选择要保存的一个配置";
        public const string TrendViewer_Msg_StartDateMoreThenEnd_EN = "Start Date can not be more than End Date";
        public const string TrendViewer_Msg_StartDateMoreThenEnd_CH = "开始日期不能晚于结束日期";


        public static void InitLanguageType(LanguageType langType)
        {
            LanguageTypeHelper.GetInstance().SetLanaguageType(langType);
        }


        public static void InitAllStrings()
        {

            try
            {
                StringHelper.GetInstance().AddStringItem(TrendViewer_CancelBtn, TrendViewer_CancelBtn_EN, TrendViewer_CancelBtn_CH, "Cancel button");
                StringHelper.GetInstance().AddStringItem(TrendViewer_SubmitBtn,TrendViewer_SubmitBtn_EN,TrendViewer_SubmitBtn_CH, "Submit button");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ConfirmBtn,TrendViewer_ConfirmBtn_EN,TrendViewer_ConfirmBtn_CH, "OK button");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TotalPageLbl,TrendViewer_TotalPageLbl_EN,TrendViewer_TotalPageLbl_CH, "total page lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_LintTypeLine,TrendViewer_LintTypeLine_EN,TrendViewer_LintTypeLine_CH, "Line type item used in dropdown list");
                StringHelper.GetInstance().AddStringItem(TrendViewer_LintTypeSpline,TrendViewer_LintTypeSpline_EN,TrendViewer_LintTypeSpline_CH, "Line type item used in dropdown list");

                // for dp/histDP data form : todo need to check the Eng.
                StringHelper.GetInstance().AddStringItem(TrendViewer_EditHistDPTitle,TrendViewer_EditHistDPTitle_EN,TrendViewer_EditHistDPTitle_CH, "Edit historical datapoint form title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_EditDPTitle,TrendViewer_EditDPTitle_EN,TrendViewer_EditDPTitle_CH, "Edit datapoint form title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_AddDPTitle, TrendViewer_AddDPTitle_EN, TrendViewer_AddDPTitle_CH, "Add datapoint form title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_DPLbl,TrendViewer_DPLbl_EN,TrendViewer_DPLbl_CH, "Datapoint lable in data form");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColorLbl,TrendViewer_ColorLbl_EN,TrendViewer_ColorLbl_CH, "Color lable in data form");
                StringHelper.GetInstance().AddStringItem(TrendViewer_LineTypeLbl,TrendViewer_LineTypeLbl_EN,TrendViewer_LineTypeLbl_CH, "Line type lable in data form");
                StringHelper.GetInstance().AddStringItem(TrendViewer_SelectColorBtn,TrendViewer_SelectColorBtn_EN,TrendViewer_SelectColorBtn_CH, "Color select button");
                StringHelper.GetInstance().AddStringItem(TrendViewer_BrowseBtn,TrendViewer_BrowseBtn_EN,TrendViewer_BrowseBtn_CH, "Browse button");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ServerNameLbl,TrendViewer_ServerNameLbl_EN,TrendViewer_ServerNameLbl_CH, "Server name lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_LableLbl,TrendViewer_LableLbl_EN,TrendViewer_LableLbl_CH, "Legend label");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Host,TrendViewer_Host_EN,TrendViewer_Host_CH, "Host label");
                StringHelper.GetInstance().AddStringItem(TrendViewer_DPVisibleCb,TrendViewer_DPVisibleCb_EN,TrendViewer_DPVisibleCb_CH, "DP visible checkbox");
                StringHelper.GetInstance().AddStringItem(TrendViewer_LblEnabledCb,TrendViewer_LblEnabledCb_EN,TrendViewer_LblEnabledCb_CH, "Lable enabled checkbox");
                StringHelper.GetInstance().AddStringItem(TrendViewer_DPEnabledCb,TrendViewer_DPEnabledCb_EN,TrendViewer_DPEnabledCb_CH, "DP Enabled checkbox");

                //for hist dp data form:
                StringHelper.GetInstance().AddStringItem(TrendViewer_HistDataPointData_StartDateLbl,TrendViewer_HistDataPointData_StartDateLbl_EN,TrendViewer_HistDataPointData_StartDateLbl_CH, "history dp dataform start date lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_HistDataPointData_EndDateLbl,TrendViewer_HistDataPointData_EndDateLbl_EN,TrendViewer_HistDataPointData_EndDateLbl_CH, "history dp dataform end date lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_HistDataPointData_AddDPTitle,TrendViewer_HistDataPointData_AddDPTitle_EN,TrendViewer_HistDataPointData_AddDPTitle_CH, "history dp dataform add dp title");
                //marker data form:
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkData_EditMarkerTitle,TrendViewer_MarkData_EditMarkerTitle_EN,TrendViewer_MarkData_EditMarkerTitle_CH, "edit marker data title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkData_AddMarkerTitle,TrendViewer_MarkData_AddMarkerTitle_EN,TrendViewer_MarkData_AddMarkerTitle_CH, "add marker data title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkData_MarkerNameLbl,TrendViewer_MarkData_MarkerNameLbl_EN,TrendViewer_MarkData_MarkerNameLbl_CH, "marker name lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkData_MarkerBColorLbl,TrendViewer_MarkData_MarkerBColorLbl_EN,TrendViewer_MarkData_MarkerBColorLbl_CH, "marker background color lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkData_MarkerFColorLbl,TrendViewer_MarkData_MarkerFColorLbl_EN,TrendViewer_MarkData_MarkerFColorLbl_CH, "marker font color lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkData_StartValLbl,TrendViewer_MarkData_StartValLbl_EN,TrendViewer_MarkData_StartValLbl_CH, "marker start value lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkData_RangeLbl,TrendViewer_MarkData_RangeLbl_EN,TrendViewer_MarkData_RangeLbl_CH, "marker range lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkData_EnableMarkerCb,TrendViewer_MarkData_EnableMarkerCb_EN,TrendViewer_MarkData_EnableMarkerCb_CH, "marker enabled checkbox");
                //formula data form 
                StringHelper.GetInstance().AddStringItem(TrendViewer_FormulaData_FormulaLbl,TrendViewer_FormulaData_FormulaLbl_EN,TrendViewer_FormulaData_FormulaLbl_CH, "Formula:");
                StringHelper.GetInstance().AddStringItem(TrendViewer_FormulaData_EgLbl,TrendViewer_FormulaData_EgLbl_EN,TrendViewer_FormulaData_EgLbl_CH, "formula example label in formula data form");
                StringHelper.GetInstance().AddStringItem(TrendViewer_FormulaData_AddTitle,TrendViewer_FormulaData_AddTitle_EN,TrendViewer_FormulaData_AddTitle_CH, "Add a formula data form title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_FormulaData_EditTitle,TrendViewer_FormulaData_EditTitle_EN,TrendViewer_FormulaData_EditTitle_CH, "Edit a formula data form title");
                //EditChartForm
                StringHelper.GetInstance().AddStringItem(TrendViewer_EditChartForm_Title,TrendViewer_EditChartForm_Title_EN,TrendViewer_EditChartForm_Title_CH, "EditChartForm title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_EditChartForm_ChartTitleLbl,TrendViewer_EditChartForm_ChartTitleLbl_EN,TrendViewer_EditChartForm_ChartTitleLbl_CH, "EditChartForm chart title lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_EditChartForm_XTitleLbl,TrendViewer_EditChartForm_XTitleLbl_EN,TrendViewer_EditChartForm_XTitleLbl_CH, "EditChartForm x title lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_EditChartForm_YTitleLbl,TrendViewer_EditChartForm_YTitleLbl_EN,TrendViewer_EditChartForm_YTitleLbl_CH, "EditChartForm y title lable");
                 //formulalist form:
                StringHelper.GetInstance().AddStringItem(TrendViewer_FormulaList_Title,TrendViewer_FormulaList_Title_EN,TrendViewer_FormulaList_Title_CH, "formula list title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_FormulaList_GroupBox,TrendViewer_FormulaList_GroupBox_EN,TrendViewer_FormulaList_GroupBox_CH, "formula list groupbox");
                StringHelper.GetInstance().AddStringItem(TrendViewer_FormulaList_AddMenu,TrendViewer_FormulaList_AddMenu_EN,TrendViewer_FormulaList_AddMenu_CH, "formula list right click menu: add a formula");
                StringHelper.GetInstance().AddStringItem(TrendViewer_FormulaList_LoadMenu,TrendViewer_FormulaList_LoadMenu_EN,TrendViewer_FormulaList_LoadMenu_CH, "formula list right click menu: load a formula config");
                StringHelper.GetInstance().AddStringItem(TrendViewer_FormulaList_SaveMenu,TrendViewer_FormulaList_SaveMenu_EN,TrendViewer_FormulaList_SaveMenu_CH, "formula list right click menu: save a formula config");
                //markerlist form:
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkerList_Title,TrendViewer_MarkerList_Title_EN,TrendViewer_MarkerList_Title_CH, "marker list title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkerList_GroupBox,TrendViewer_MarkerList_GroupBox_EN,TrendViewer_MarkerList_GroupBox_CH, "marker list groupbox");
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkerList_AddMenu,TrendViewer_MarkerList_AddMenu_EN,TrendViewer_MarkerList_AddMenu_CH, "formula list right click menu: add a formula");
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkerList_LoadMenu,TrendViewer_MarkerList_LoadMenu_EN,TrendViewer_MarkerList_LoadMenu_CH, "formula list right click menu: load a formula config");
                StringHelper.GetInstance().AddStringItem(TrendViewer_MarkerList_SaveMenu,TrendViewer_MarkerList_SaveMenu_EN,TrendViewer_MarkerList_SaveMenu_CH, "formula list right click menu: save a formula config");
                StringHelper.GetInstance().AddStringItem(TrendViewer_LoadMarker_Title,TrendViewer_LoadMarker_Title_EN,TrendViewer_LoadMarker_Title_CH, "load marker form title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_SaveMarker_Title,TrendViewer_SaveMarker_Title_EN,TrendViewer_SaveMarker_Title_CH, "save marker form title");

                //hist dp list form
                StringHelper.GetInstance().AddStringItem(TrendViewer_HistDPList_Title,TrendViewer_HistDPList_Title_EN,TrendViewer_HistDPList_Title_CH, "hist dp list form title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_HistDPList_GroupBox,TrendViewer_HistDPList_GroupBox_EN,TrendViewer_HistDPList_GroupBox_CH, "hist dp list groupbox");
                StringHelper.GetInstance().AddStringItem(TrendViewer_HistDPList_AddMenu,TrendViewer_HistDPList_AddMenu_EN,TrendViewer_HistDPList_AddMenu_CH, "hisd dp list right click menu: add a hist dp");
                StringHelper.GetInstance().AddStringItem(TrendViewer_HistDPList_LoadMenu,TrendViewer_HistDPList_LoadMenu_EN,TrendViewer_HistDPList_LoadMenu_CH, "hist dp list right click menu: load a hist DP config");
                StringHelper.GetInstance().AddStringItem(TrendViewer_HistDPList_SaveMenu,TrendViewer_HistDPList_SaveMenu_EN,TrendViewer_HistDPList_SaveMenu_CH, "hsit dp list right click menu: save a hist DP config");
                 //dp list form
                StringHelper.GetInstance().AddStringItem(TrendViewer_DPList_Title,TrendViewer_DPList_Title_EN,TrendViewer_DPList_Title_CH, "dp list form title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_DPList_GroupBox,TrendViewer_DPList_GroupBox_EN,TrendViewer_DPList_GroupBox_CH, "dp list groupbox");
                StringHelper.GetInstance().AddStringItem(TrendViewer_DPList_AddMenu,TrendViewer_DPList_AddMenu_EN,TrendViewer_DPList_AddMenu_CH, "dp list right click menu: add a hist dp");
                StringHelper.GetInstance().AddStringItem(TrendViewer_DPList_LoadMenu,TrendViewer_DPList_LoadMenu_EN,TrendViewer_DPList_LoadMenu_CH, "dp list right click menu: load a hist DP config");
                StringHelper.GetInstance().AddStringItem(TrendViewer_DPList_SaveMenu,TrendViewer_DPList_SaveMenu_EN,TrendViewer_DPList_SaveMenu_CH, "dp list right click menu: save a hist DP config");
                //group form
                StringHelper.GetInstance().AddStringItem(TrendViewer_GroupConfigurationLbl,TrendViewer_GroupConfigurationLbl_EN,TrendViewer_GroupConfigurationLbl_CH, "group form configuration lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_HistDPGroup_TitleLoad,TrendViewer_HistDPGroup_TitleLoad_EN,TrendViewer_HistDPGroup_TitleLoad_CH, "hist dp Group form title: Load Historical Datapoint Configuration");
                StringHelper.GetInstance().AddStringItem(TrendViewer_HistDPGroup_TitleSave,TrendViewer_HistDPGroup_TitleSave_EN,TrendViewer_HistDPGroup_TitleSave_CH, "hist dp Group form title: Save Historical Datapoint Configuration");
                // about form
                StringHelper.GetInstance().AddStringItem(TrendViewer_AboutFormTitle,TrendViewer_AboutFormTitle_EN,TrendViewer_AboutFormTitle_CH, "about form title");
                // opc data selector form 
                StringHelper.GetInstance().AddStringItem(TrendViewer_OPCDataSelector_GrpBoxName,TrendViewer_OPCDataSelector_GrpBoxName_EN,TrendViewer_OPCDataSelector_GrpBoxName_CH, "opc data selector groupbox name: dp list");
                StringHelper.GetInstance().AddStringItem(TrendViewer_OPCDataSelector_OPCDPColumnName,TrendViewer_OPCDataSelector_OPCDPColumnName_EN,TrendViewer_OPCDataSelector_OPCDPColumnName_CH, "opc data selector form table column name: opc dp");
                StringHelper.GetInstance().AddStringItem(TrendViewer_OPCDataSelector_DesColumnName,TrendViewer_OPCDataSelector_DesColumnName_EN,TrendViewer_OPCDataSelector_DesColumnName_CH, "opc data selector form table column name: description");
                StringHelper.GetInstance().AddStringItem(TrendViewer_OPCDataSelector_OPCServerLbl,TrendViewer_OPCDataSelector_OPCServerLbl_EN,TrendViewer_OPCDataSelector_OPCServerLbl_CH, "opc server lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_OPCDataSelector_FormTitle,TrendViewer_OPCDataSelector_FormTitle_EN,TrendViewer_OPCDataSelector_FormTitle_CH, "opc data selector form title");
                 // Trendview form
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_Title,TrendViewer_TrendView_Title_EN,TrendViewer_TrendView_Title_CH,"TrendView form title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_Second,TrendViewer_TrendView_Second_EN,TrendViewer_TrendView_Second_CH,"Second - in dropdownlist ");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_Minute,TrendViewer_TrendView_Minute_EN,TrendViewer_TrendView_Minute_CH,"Minute - in dropdownlist ");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_Hour,TrendViewer_TrendView_Hour_EN,TrendViewer_TrendView_Hour_CH,"Hour - in dropdownlist ");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_Day,TrendViewer_TrendView_Day_EN,TrendViewer_TrendView_Day_CH,"Day - in dropdownlist ");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_Start,TrendViewer_TrendView_Start_EN,TrendViewer_TrendView_Start_CH, "Start button");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_Stop,TrendViewer_TrendView_Stop_EN,TrendViewer_TrendView_Stop_CH, "Stop button");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_GraphLblHist,TrendViewer_TrendView_GraphLblHist_EN,TrendViewer_TrendView_GraphLblHist_CH, "Historical lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_GraphLblRT,TrendViewer_TrendView_GraphLblRT_EN,TrendViewer_TrendView_GraphLblRT_CH, "Realtime lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_GraphLblMixed,TrendViewer_TrendView_GraphLblMixed_EN,TrendViewer_TrendView_GraphLblMixed_CH, "Mixed mode lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MixModeCb,TrendViewer_TrendView_MixModeCb_EN,TrendViewer_TrendView_MixModeCb_CH,"Mixed mode checkbox");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_PlotIntervalLbl,TrendViewer_TrendView_PlotIntervalLbl_EN,TrendViewer_TrendView_PlotIntervalLbl_CH,"plotting interval lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_TimeFrameLbl,TrendViewer_TrendView_TimeFrameLbl_EN,TrendViewer_TrendView_TimeFrameLbl_CH,"Time frame lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_3DModeCb,TrendViewer_TrendView_3DModeCb_EN,TrendViewer_TrendView_3DModeCb_CH, "3D checkbox");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_GrpBox,TrendViewer_TrendView_GrpBox_EN,TrendViewer_TrendView_GrpBox_CH, "TrendView Group box name");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_ChartTitle,TrendViewer_TrendView_ChartTitle_EN,TrendViewer_TrendView_ChartTitle_CH, "chart title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_StartTimeLbl,TrendViewer_TrendView_StartTimeLbl_EN,TrendViewer_TrendView_StartTimeLbl_CH, "start time lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_EndTimeLbl,TrendViewer_TrendView_EndTimeLbl_EN,TrendViewer_TrendView_EndTimeLbl_CH, "end time lable");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_RTDataGrpBox,TrendViewer_TrendView_RTDataGrpBox_EN,TrendViewer_TrendView_RTDataGrpBox_CH, "realtime datapoint groupbox");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_HistDataGrpBox,TrendViewer_TrendView_HistDataGrpBox_EN,TrendViewer_TrendView_HistDataGrpBox_CH, "Historical datapoint groupbox");
                //trendview menu item
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuUtiCommand,TrendViewer_TrendView_MenuUtiCommand_EN,TrendViewer_TrendView_MenuUtiCommand_CH,"Menu: Utility Command");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuExport,TrendViewer_TrendView_MenuExport_EN,TrendViewer_TrendView_MenuExport_CH,"Menu:Export History Data");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuScreenShort,TrendViewer_TrendView_MenuScreenShort_EN,TrendViewer_TrendView_MenuScreenShort_CH,"Menu:Screen Shot Trend");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuConfig,TrendViewer_TrendView_MenuConfig_EN,TrendViewer_TrendView_MenuConfig_CH,"Menu: Configuration");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuDPConfig,TrendViewer_TrendView_MenuDPConfig_EN,TrendViewer_TrendView_MenuDPConfig_CH, "Menu: Datapoint Configuration");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuHistDPConfig,TrendViewer_TrendView_MenuHistDPConfig_EN,TrendViewer_TrendView_MenuHistDPConfig_CH, "Menu: Historical Datapoint Configuration");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuMarkerConfig,TrendViewer_TrendView_MenuMarkerConfig_EN,TrendViewer_TrendView_MenuMarkerConfig_CH, "Menu: Marker Data Configuration");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuChangeChartTitle,TrendViewer_TrendView_MenuChangeChartTitle_EN,TrendViewer_TrendView_MenuChangeChartTitle_CH,"Menu:Change Chart Title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuSampleGrpConfig,TrendViewer_TrendView_MenuSampleGrpConfig_EN,TrendViewer_TrendView_MenuSampleGrpConfig_CH,"Menu: OPC DataLog Configuration");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuFormulaConfig,TrendViewer_TrendView_MenuFormulaConfig_EN,TrendViewer_TrendView_MenuFormulaConfig_CH,"Menu: Formula Configuration");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuPrint,TrendViewer_TrendView_MenuPrint_EN,TrendViewer_TrendView_MenuPrint_CH, "Menu: Print");
                StringHelper.GetInstance().AddStringItem(TrendViewer_TrendView_MenuAbout,TrendViewer_TrendView_MenuAbout_EN,TrendViewer_TrendView_MenuAbout_CH, "Menu: About");
                //list view column name:
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColDPEnabled,TrendViewer_ColDPEnabled_EN,TrendViewer_ColDPEnabled_CH, "column name : Enable Datapoint");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColLblEnabled,TrendViewer_ColLblEnabled_EN,TrendViewer_ColLblEnabled_CH, "column name: Enable Tag");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColDispDP,TrendViewer_ColDispDP_EN,TrendViewer_ColDispDP_CH, "column name : Display Datapoint");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColDPName,TrendViewer_ColDPName_EN,TrendViewer_ColDPName_CH, "column name: Datapoint Name");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColLengend,TrendViewer_ColLengend_EN,TrendViewer_ColLengend_CH, "column name: Legend");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColDPType,TrendViewer_ColDPType_EN,TrendViewer_ColDPType_CH, "column name: Line Type");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColDPColor,TrendViewer_ColDPColor_EN,TrendViewer_ColDPColor_CH,"column name: Color");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColServer,TrendViewer_ColServer_EN,TrendViewer_ColServer_CH, "column name: server");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColHost,TrendViewer_ColHost_EN,TrendViewer_ColHost_CH, "column name: host");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColEdit,TrendViewer_ColEdit_EN,TrendViewer_ColEdit_CH, "column name: edit");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColDelete,TrendViewer_ColDelete_EN,TrendViewer_ColDelete_CH, "column name: delete");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColStartDate,TrendViewer_ColStartDate_EN,TrendViewer_ColStartDate_CH, "column name: Start");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColEndDate,TrendViewer_ColEndDate_EN,TrendViewer_ColEndDate_CH, "column name: End");
                //for DataMarker viewer:
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColMarkerEnabled,TrendViewer_ColMarkerEnabled_EN,TrendViewer_ColMarkerEnabled_CH, "column name: Enable Marker");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColMarkerName,TrendViewer_ColMarkerName_EN,TrendViewer_ColMarkerName_CH, "column name: Marker Name");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColMarkerWidth,TrendViewer_ColMarkerWidth_EN,TrendViewer_ColMarkerWidth_CH, "column name: Range");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColMarkerBcolor,TrendViewer_ColMarkerBcolor_EN,TrendViewer_ColMarkerBcolor_CH,"column name: Background Color");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColMarkerFontColor,TrendViewer_ColMarkerFontColor_EN,TrendViewer_ColMarkerFontColor_CH, "column name: Font Color");
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColMarkerStartVal,TrendViewer_ColMarkerStartVal_EN,TrendViewer_ColMarkerStartVal_CH,"column name: Start Value");
                //for formula list:
                StringHelper.GetInstance().AddStringItem(TrendViewer_ColFormula,TrendViewer_ColFormula_EN,TrendViewer_ColFormula_CH,"column name: Formula");
                //message:
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_SysMsg,TrendViewer_Msg_SysMsg_EN,TrendViewer_Msg_SysMsg_CH, "Message box title:System Message");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_DelConfirm,TrendViewer_Msg_DelConfirm_EN,TrendViewer_Msg_DelConfirm_CH, "Msg: Delete Current Selection?");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_StopRT2Continue,TrendViewer_Msg_StopRT2Continue_EN,TrendViewer_Msg_StopRT2Continue_CH, "Msg: Please stop the realtime trend in order to continue");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_DuplicateName,TrendViewer_Msg_DuplicateName_EN,TrendViewer_Msg_DuplicateName_CH, "when user input a name which is already exist in the current configuration");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_EmptyName,TrendViewer_Msg_EmptyName_EN,TrendViewer_Msg_EmptyName_CH, "name shouldn't be empty");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_SelectConfirm,TrendViewer_Msg_SelectConfirm_EN,TrendViewer_Msg_SelectConfirm_CH, "");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_ExportOK,TrendViewer_Msg_ExportOK_EN,TrendViewer_Msg_ExportOK_CH, "Export Data Successfully!");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_ErrTitle,TrendViewer_Msg_ErrTitle_EN,TrendViewer_Msg_ErrTitle_CH, "Title: Error");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_WarningTitle,TrendViewer_Msg_WarningTitle_EN,TrendViewer_Msg_WarningTitle_CH, "warning msg title");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_SelectConfigForLoad,TrendViewer_Msg_SelectConfigForLoad_EN,TrendViewer_Msg_SelectConfigForLoad_CH, "erro when load without a configuration name");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_NoConn2OPC,TrendViewer_Msg_NoConn2OPC_EN,TrendViewer_Msg_NoConn2OPC_CH, "Err Msg:No connection to OPC Bridge. ");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_NoDB,TrendViewer_Msg_NoDB_EN,TrendViewer_Msg_NoDB_CH, "");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_UnhandledError, TrendViewer_Msg_UnhandledError_EN, TrendViewer_Msg_UnhandledError_CH, "");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_SaveError, TrendViewer_Msg_SaveError_EN, TrendViewer_Msg_SaveError_CH, "Save data unsuccessfully.");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_LoadDataError, TrendViewer_Msg_LoadDataError_EN, TrendViewer_Msg_LoadDataError_EN, "Can't load data due to DB error.");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_ExportNoData,TrendViewer_Msg_ExportNoData_EN,TrendViewer_Msg_ExportNoData_CH, "Err msg when export hist data");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_OPCNOK,TrendViewer_Msg_OPCNOK_EN,TrendViewer_Msg_OPCNOK_CH, "Err msg when OPC server can't be connected.");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_NoDB2,TrendViewer_Msg_NoDB2_EN,TrendViewer_Msg_NoDB2_CH, "DB error when first launched.");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_ScreenShotOK, TrendViewer_Msg_ScreenShotOK_EN, TrendViewer_Msg_ScreenShotOK_CH, "Screen Shot OK message.");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_ScreenShotNOK_1, TrendViewer_Msg_ScreenShotNOK_1_EN, TrendViewer_Msg_ScreenShotNOK_1_CH, "Screen Shot NOK message first part.");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_NoConfigToLoad, TrendViewer_Msg_NoConfigToLoad_EN, TrendViewer_Msg_NoConfigToLoad_CH, "error message when user didn't select any configuration to load.");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_NoConfigToSave, TrendViewer_Msg_NoConfigToSave_EN, TrendViewer_Msg_NoConfigToSave_CH, "error message when user didn't select any configuration to save.");
                StringHelper.GetInstance().AddStringItem(TrendViewer_Msg_StartDateMoreThenEnd, TrendViewer_Msg_StartDateMoreThenEnd_EN, TrendViewer_Msg_StartDateMoreThenEnd_CH, "Start Date can not be more than End Date");
               
                /*      
            public const string NAME_DUPLICATE = "名称不能与列表中的重复。";
            public const string MSG_OPC_SERVER_DOWN = "OPC 服务器关闭 原因:";
            public const string MSG_SERVER_OFF = "服务器关闭";
            public const string DP_NOT_CONNECTED = "不能连接到此数据点：";  //DP NOT_CONNECTED
            public const string DP_BAD_QUALITY = "质量不良. 质量代号为 ";//" doesn't have good quality. The Quality = "
            public const string OPC_GRP_NOT_ADDED = "OPC组添加到OPC服务器失败"; //"OPC group is not added to OPC server successfully."
                */

        }
            catch (System.Exception ex)
            {
            	
            }

   
            // for DataPointViewer.cs
            //msg
            
                      

        }
    }
}
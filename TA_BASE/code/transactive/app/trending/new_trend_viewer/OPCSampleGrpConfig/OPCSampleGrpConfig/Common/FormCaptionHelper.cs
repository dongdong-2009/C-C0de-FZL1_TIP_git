using System;
using System.Collections.Generic;
using System.Text;

using STEE.ISCS.MulLanguage;

namespace OPCSampleGrpConfig.Common
{
    /// <summary>
    /// This class is responsible for mapping the Display string of all the controls 
    /// in the project in different language based on its ID. 
    /// </summary>
    class FormCaptionHelper
    {
        private static FormCaptionHelper m_instance = null;

        //Control IDS - format - appName-Formname-Controlname
        public const string OPCSAMPLEGRPCONFIGSTART_LAB1 = "OPCSampleGrp_ConfigForm_Label1";
        public const string OPCSAMPLEGRPCONFIGSTART_LAB2 = "OPCSampleGrp_ConfigForm_Label2";
        public const string OPCSAMPLEGRPCONFIGSTART_LAB3 = "OPCSampleGrp_ConfigForm_Label3";
        public const string OPCSAMPLEGRPCONFIGSTART_LAB4 = "OPCSampleGrp_ConfigForm_Label4";
        public const string OPCSAMPLEGRPCONFIGSTART_LAB6 = "OPCSampleGrp_ConfigForm_Label6";
        public const string OPCSAMPLEGRPCONFIGSTART_LAB7 = "OPCSampleGrp_ConfigForm_Label7";
        public const string OPCSAMPLEGRPCONFIGSTART_TITLE = "OPCSampleGrp_ConfigForm_Title";
        public const string OPCSAMPLEGRPCONFIGSTART_TOTALPGLBL= "OPCSampleGrp_ConfigForm_TotalPageLabel";
        public const string OPCSAMPLEGRPCONFIGSTART_DISABLE_CHECKBOX = "OPCSampleGrp_ConfigForm_DisableCheckBox";
        public const string OPCSAMPLEGRPCONFIGSTART_CANCEL_BUTTON = "OPCSampleGrp_ConfigForm_CancelButton";
        public const string OPCSAMPLEGRPCONFIGSTART_SAVE_BUTTON = "OPCSampleGrp_ConfigForm_SaveButton";

        public const string OPCSAMPLEGRPNAME_COL_TEXT = "OPCSampleGrp_ConfigForm_NameCol";
        public const string OPCSAMPLEGRPDESC_COL_TEXT = "OPCSampleGrp_ConfigForm_DescCol";
        public const string OPCSAMPLEINTERVAL_COL_TEXT = "OPCSampleGrp_ConfigForm_IntevalCol";
        public const string OPCSAMPLEINTERVALTYPE_COL_TEXT = "OPCSampleGrp_ConfigForm_IntevalTypeCol";
        public const string OPCSAMPLESTARTTIME_COL_TEXT = "OPCSampleGrp_ConfigForm_StartTimeCol";
        public const string OPCSAMPLEDELTAVAL_COL_TEXT = "OPCSampleGrp_ConfigForm_DeltaCol";
        public const string OPCSAMPLEDISABLE_COL_TEXT = "OPCSampleGrp_ConfigForm_DisableCol";
        public const string OPCSAMPLEGRP_EDITCOLUMN_TEXT = "OPCSampleGrp_ConfigForm_EditCol";
        public const string OPCSAMPLEGRP_DELETECOL_TEXT = "OPCSampleGrp_ConfigForm_DeleteCol";
        public const string OPCSAMPLEGRP_CONFIGCOL_TEXT = "OPCSampleGrp_ConfigForm_ConfigCol";

        public const string OPCSAMPLEGRP_LOCATIONCOL_TEXT = "OPCSampleGrp_ConfigForm_LocationNameCol";

        public const string OPCSAMPLEGRP_SECOND_INTERVALTYPE = "OPCSampleGrp_ConfigForm_SecondIntervalType";
        public const string OPCSAMPLEGRP_MINUTE_INTERVALTYPE = "OPCSampleGrp_ConfigForm_MinuteIntervalType";
        public const string OPCSAMPLEGRP_HOUR_INTERVALTYPE = "OPCSampleGrp_ConfigForm_HourIntervalType";

        public const string OPCDATASELECTOR_SERVERBOX_TEXT = "OPCSampleGrp_DataSelectorForm_ServerInfoBox";
        public const string OPCDATASELECTOR_SERVERNAME_TEXT = "OPCSampleGrp_DataSelectorForm_ServerNameBox";
        public const string OPCDATASELECTOR_LABEL1 = "OPCSampleGrp_DataSelectorForm_Label1";
        public const string OPCDATASELECTOR_LABEL2 = "OPCSampleGrp_DataSelectorForm_Label2";
        public const string OPCDATASELECTOR_LABEL8 = "OPCSampleGrp_DataSelectorForm_Label8";
        public const string OPCDATASELECTOR_ADDMENU = "OPCSampleGrp_DataSelectorForm_AddMenu";
        public const string OPCDATASELECTOR_REMOVEMENU = "OPCSampleGrp_DataSelectorForm_RemoveMenu";
        public const string OPCDATASELECTOR_OK_BUTTON = "OPCSampleGrp_DataSelectorForm_OkButton";
        public const string OPCDATASELECTOR_GROUPBOX = "OPCSampleGrp_DataSelectorForm_GroupBox1";
        public const string OPCDATASELECTOR_CANCEL_BUTTON= "OPCSampleGrp_DataSelectorForm_CancelButton";
        public const string OPCDATASELECTOR_FORM_TITLE = "OPCSampleGrp_DataSelectorForm_Title";

        public const string OPCITEM_DATAPTNAME_COLTEXT = "OPCSampleGrp_DataSelectorForm_DataPointNameCol";
        public const string OPCITEM_DATAPTDESC_COLTEXT = "OPCSampleGrp_DataSelectorForm_DataPointDescCol";
        public const string OPCITEM_GRPID_COLTEXT = "OPCSampleGrp_DataSelectorForm_GrpIdCol";


        public const string OPCSAMPLECONFIG_DELETEMSGTITLE = "OPCSampleGrp_ConfigForm_DeleteMsgTitle";
        public const string OPCSAMPLECONFIG_DELETEMSG = "OPCSampleGrp_ConfigForm_DeleteMessage";
       // public const string OPCSAMPLECONFIG_DELETEMSG2 = "OPCSampleGrp_ConfigForm_DeleteMessage2";
        public const string OPCSAMPLECONFIG_GRPNAMEMISSING_MSG = "OPCSampleGrp_ConfigForm_GrpNameMSG";
        public const string OPCSAMPLECONFIG_INTERVALEVALMISS_MSG = "OPCSampleGrp_ConfigForm_IntervalValueMsg";
        public const string OPCSAMPLECONFIG_GRPEXISTS_MSG = "OPCSampleGrp_ConfigForm_GrpExistsMsg";
        public const string OPCSAMPLECONFIG_DELETEFAILED_MSG = "OPCSampleGrp_ConfigForm_DeleteFailedMsg";
        public const string OPCDATASELECTOR_SAVE_MSG = "OPCSampleGrp_DataSelectorForm_SaveMsg";

        public const string OPCDATASELECTOR_CANADDDP_MSG = "OPCSampleGrp_DataSelectorForm_CanAddDPMsg";
        public const string OPCDATASELECTOR_CANNOTADDDP_MSG = "OPCSampleGrp_DataSelectorForm_CannotAddDPMsg";

        public const string OPCSAMPLECONFIG_NODATABASECONN_MSG = "OPCSampleGrp_NoDatabase_Connection";

        public static FormCaptionHelper GetInstance()
        {
            if (m_instance == null)
            {
                m_instance = new FormCaptionHelper();
            }
            return m_instance;
        }

        public static void ReleaseInstance()
        {
            if (m_instance != null)
            {
                StringHelper.GetInstance().RemoveAllStringItem();
                m_instance = null;
            }
        }

        /// <summary>
        /// Adds control Text in different language to Multilanguage component class. 
        /// </summary>
        public void AddCaptionsToStringHelper()
        {
            //Form - OPCSampleGrpConfigStart
            //Edit group control captions
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB1, EnglishString.DP_GRP_NAME, ChineseString.DP_GRP_NAME, "Sample Grp Name Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB2, EnglishString.DP_GRP_DES, ChineseString.DP_GRP_DES, "Sample Grp desc Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB3, EnglishString.START_TIME, ChineseString.START_TIME, "Sample Grp starttime Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB4, EnglishString.DELTA_VALUE, ChineseString.DELTA_VALUE, "Sample Grp delta Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB6, EnglishString.INTERVAL_UNIT, ChineseString.INTERVAL_UNIT, "Sample Grp interval Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_LAB7, EnglishString.INTERVAL_LENGTH, ChineseString.INTERVAL_LENGTH, "Sample Grp intervaltype Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_DISABLE_CHECKBOX, EnglishString.DISABLE, ChineseString.DISABLE, "Sample Grp Disable Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_CANCEL_BUTTON, EnglishString.CANCLE, ChineseString.CANCLE, "Sample Grp cancel button");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_SAVE_BUTTON, EnglishString.SAVE, ChineseString.SAVE, "Sample Grp Save Button");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_TOTALPGLBL, EnglishString.TOTAL_PAGE, ChineseString.TOTAL_PAGE, "Sample Grp Count");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPCONFIGSTART_TITLE, EnglishString.TITLE, ChineseString.TITLE, "Sample Grp Form title");

            //DataGrid Column caption
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPNAME_COL_TEXT, EnglishString.OPCSAMPLEGRPNAME_COL_TEXT, ChineseString.DATA_GROUP_NAME, "Sample Grp Name Column ");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRPDESC_COL_TEXT, EnglishString.OPCSAMPLEGRPDESC_COL_TEXT, ChineseString.DESCRIPTION, "Sample Grp Description Column");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEINTERVAL_COL_TEXT, EnglishString.OPCSAMPLEINTERVAL_COL_TEXT, ChineseString.COLUMN_TIME_SPAN, "Sample Grp Interval Col");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEINTERVALTYPE_COL_TEXT, EnglishString.OPCSAMPLEINTERVALTYPE_COL_TEXT, ChineseString.COLUMN_TIME_FRAME, "Sample Grp IntervalType Column");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLESTARTTIME_COL_TEXT, EnglishString.OPCSAMPLESTARTTIME_COL_TEXT, ChineseString.COLUMN_START_TIME, "Sample Grp Starttime Column");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEDELTAVAL_COL_TEXT, EnglishString.OPCSAMPLEDELTAVAL_COL_TEXT, ChineseString.COLUMN_DELTA_VALUE, "Sample Grp Delta Column");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEDISABLE_COL_TEXT, EnglishString.OPCSAMPLEDISABLE_COL_TEXT, ChineseString.DISABLE, "Sample Grp Disable Column");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRP_EDITCOLUMN_TEXT, EnglishString.OPCSAMPLEGRP_EDITCOLUMN_NAME, ChineseString.COLUMN_EDIT, "Sample Grp Edit Column");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRP_DELETECOL_TEXT, EnglishString.OPCSAMPLEGRP_DELETECOL_NAME, ChineseString.COLUMN_DELETE, "Sample Grp Delete Column");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRP_CONFIGCOL_TEXT, EnglishString.OPCSAMPLEGRP_CONFIGCOL_NAME, ChineseString.COLUMN_CONFIG, "Sample Grp Config Column");


            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRP_LOCATIONCOL_TEXT, EnglishString.OPCSAMPLEGRP_LOCATIONCOL_NAME, ChineseString.COLUMN_CONFIG, "Sample Grp LocationName Column");


            //Interval type Combox items
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRP_SECOND_INTERVALTYPE, EnglishString.SECOND, ChineseString.SECOND, "Second intervaltype Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRP_MINUTE_INTERVALTYPE, EnglishString.MINITE, ChineseString.MINITE, "Minute intervaltype Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLEGRP_HOUR_INTERVALTYPE, EnglishString.HOUR, ChineseString.HOUR, "Hour intervaltype Label");

            //MsgBox captions
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLECONFIG_DELETEMSGTITLE, EnglishString.MSG_SYSTEM_TITLE, ChineseString.MSG_SYSTEM_TITLE, "DeleteMsgBox title");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLECONFIG_DELETEMSG, EnglishString.MSG_DELETE_CONFIRM, ChineseString.MSG_DELETE_CONFIRM, "DeleteMsg1");
            //StringHelper.GetInstance().AddStringItem(FormCaptionID.OPCSAMPLECONFIG_DELETEMSG2, EnglishString.MSG_DELETE_CONFIRM2, ChineseString.MSG_DELETE_CONFIRM2, "DeleteMsg2");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLECONFIG_GRPNAMEMISSING_MSG, EnglishString.MSG_FOR_GRP_NAME, ChineseString.MSG_FOR_GRP_NAME, "Sample Grp name missing msg");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLECONFIG_INTERVALEVALMISS_MSG, EnglishString.MSG_FOR_INTERVAL, ChineseString.MSG_FOR_INTERVAL, "Sample interval value invalid msg");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLECONFIG_GRPEXISTS_MSG, EnglishString.MSG_NAME_EXIST, ChineseString.MSG_NAME_EXIST, "Group already exists msg");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLECONFIG_DELETEFAILED_MSG, EnglishString.MSG_DELETE_MSG, ChineseString.MSG_DELETE_MSG, "Delete operation failed.");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCSAMPLECONFIG_NODATABASECONN_MSG, EnglishString.OPC_MSG_NODB, ChineseString.OPC_MSG_NODB, "No Database Connection.");

            
            


            //Form - OPCDataSelector 
            //Edit group control captions
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_LABEL1, EnglishString.DATA_GRP_NAME, ChineseString.DATA_GRP_NAME, "Sample Grp Name Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_LABEL2, EnglishString.DP_NAME_SELECT, ChineseString.DP_NAME_SELECT, " Datapoint name Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_LABEL8, EnglishString.OPC_SERVER_NAME, ChineseString.OPC_SERVER_NAME, "Server name Label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_ADDMENU, EnglishString.ADD, ChineseString.ADD, "addmenu");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_OK_BUTTON, EnglishString.SAVE, ChineseString.SAVE, "Ok button");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_SERVERNAME_TEXT, EnglishString.SERVER_NAME_LIST, ChineseString.SERVER_NAME_LIST, "ServerNameList");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_SERVERBOX_TEXT, EnglishString.DP_LIST, ChineseString.DP_LIST, "ServerNameSelection");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_REMOVEMENU, EnglishString.DELETE, ChineseString.DELETE, "Removemenu");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_GROUPBOX, EnglishString.SELECTED_DP, ChineseString.SELECTED_DP, "GroupBox label");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_CANCEL_BUTTON, EnglishString.CANCLE, ChineseString.CANCLE, "Cancel");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_FORM_TITLE, EnglishString.SELECT_OPC_DP, ChineseString.SELECT_OPC_DP, "Form title");

            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCITEM_DATAPTNAME_COLTEXT, EnglishString.OPC_DP_NAME, ChineseString.OPC_DP_NAME, "DataPoint name column");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCITEM_DATAPTDESC_COLTEXT, EnglishString.DESCRIPTION, ChineseString.DESCRIPTION, "DataPoint Description column");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCITEM_GRPID_COLTEXT, EnglishString.DATA_GROUP_NAME, ChineseString.DATA_GROUP_NAME, "DataPoint group name");

            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_SAVE_MSG, EnglishString.SAVE_SUSSCCESSFUL_MSG, ChineseString.SAVE_SUSSCCESSFUL_MSG, "Save msg");

            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_CANADDDP_MSG, EnglishString.CAN_ADD_DATAPOINT_MSG, ChineseString.CAN_ADD_DATAPOINT_MSG, "Add DP msg");
            StringHelper.GetInstance().AddStringItem(FormCaptionHelper.OPCDATASELECTOR_CANNOTADDDP_MSG, EnglishString.CANNOT_ADD_DATAPOINT_MSG, ChineseString.CANNOT_ADD_DATAPOINT_MSG, "cannot Add DP msg");                       
        }
    }
}

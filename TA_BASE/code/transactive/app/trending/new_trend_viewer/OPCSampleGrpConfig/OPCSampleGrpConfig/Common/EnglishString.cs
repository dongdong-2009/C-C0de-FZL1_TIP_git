using System;
using System.Collections.Generic;
using System.Text;

namespace OPCSampleGrpConfig.Common
{
    /// <summary>
    /// Contains all the Display string of the application
    /// in English Language.
    /// </summary>
    class EnglishString
    {
        //OPCSampleGrpConfigStart form
        public const string OPCSAMPLEGRPNAME_COL_TEXT = "Group Name";
        public const string OPCSAMPLEGRPDESC_COL_TEXT = "Description";
        public const string OPCSAMPLEINTERVAL_COL_TEXT = "Interval ";
        public const string OPCSAMPLEINTERVALTYPE_COL_TEXT = "Interval Type";
        public const string OPCSAMPLESTARTTIME_COL_TEXT = "Start Time";
        public const string OPCSAMPLEDELTAVAL_COL_TEXT = "Delta Value";
        public const string OPCSAMPLEDISABLE_COL_TEXT = "Disable";
        public const string OPCSAMPLEGRP_EDITCOLUMN_NAME = "Edit";
        public const string OPCSAMPLEGRP_DELETECOL_NAME = "Delete";
        public const string OPCSAMPLEGRP_CONFIGCOL_NAME = "Config";

        public const string OPCSAMPLEGRP_LOCATIONCOL_NAME = "Location";


        public const string SECOND = "Second";
        public const string MINITE = "Minute";
        public const string HOUR = "Hour";

        public const string INTERVAL_UNIT = "Interval Type :";
        public const string INTERVAL_LENGTH = "Interval :";
        public const string DELTA_VALUE = "Delta Value :";
        public const string DISABLE = "Disable";
        public const string START_TIME = "Start Time :";
        public const string CANCLE = "Cancel";
        public const string DP_GRP_DES = "Group Description :";
        public const string DP_GRP_NAME = "Group Name :";
        public const string TITLE = "OPC DataLogger Configuration";
        public const string TOTAL_PAGE = "Total Page :    ";
        public const string MSG_SYSTEM_TITLE = "";  
        
        public const string MSG_FOR_GRP_NAME = "Please Key In Group Name";
        public const string MSG_FOR_INTERVAL = "Interval must be more than 0";
        public const string MSG_NAME_EXIST = "Sample Group Name exists already!";
        public const string MSG_DELETE_CONFIRM = "Are you sure you want to delete {0} ?";
        public const string MSG_DELETE_MSG = "Delete operation failed.";

        //public const string MSG_DELETE_CONFIRM2 = "?";
            
            
        // for OPCDataSelector.cs
        public const string SAVE_SUSSCCESSFUL_MSG = "Successfully Save!";

        public const string CAN_ADD_DATAPOINT_MSG = "The Datapoint {0} is already configured to OCC Sample Group. Do you want to override it?";
        public const string CANNOT_ADD_DATAPOINT_MSG = "The Datapoint {0} belongs to different location!";


        // for OPCDataSelector.designer.cs
        public const string DP_LIST = "DataPoint List";
        public const string ADD = "Add";
        public const string SAVE = "Save";
        public const string SERVER_NAME_LIST = "serverNameBox";
        public const string OPC_SERVER_NAME =  "OPC Server Name :";
        public const string DELETE = "Remove";
        public const string SELECTED_DP = "Selected DataPoint";
        public const string DATA_GRP_NAME = "Sample Group Name :";
        public const string DP_NAME_SELECT = "Data Point Name Search:";
        public const string SELECT_OPC_DP = "Select OPC Data Point";

        public const string OPC_DP_NAME = "OPC DataPointName";
        public const string DESCRIPTION = "Description";
        public const string DATA_GROUP_NAME = "Group Name";

        public const string OPC_MSG_NODB = "lost connection with database, please contact with administrator";//"No Connection to Database.";

    }
}

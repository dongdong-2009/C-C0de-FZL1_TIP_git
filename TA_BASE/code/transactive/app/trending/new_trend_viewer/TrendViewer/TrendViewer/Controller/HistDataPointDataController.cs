using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using Entity.Trending;
using TrendViewer.Model;
using TrendViewer.View;
using System.Data;
using System.Windows.Forms;
using STEE.ISCS.MulLanguage;
using DAO.Trending.Common;
using STEE.ISCS.Log;
 

namespace TrendViewer.Controller
{
    public class HistDataPointDataController : IController
    {
        private const string CLASS_NAME = "HistDataPointDataController";
        private EtyHistDataPoint m_dp = new EtyHistDataPoint();
        private List<string> m_otherHistDataPointNames = new List<string>();
        private HistDataPointDataModel m_Model;
        private TrendViewer.View.HistDataPointData m_View;
        private string m_originalName = "";
        private FormType m_formType = FormType.Load;


        public void SetFormType(FormType type)
        {
            m_formType = type;
        }
        public FormType GetFormType()
        {
            return m_formType;
        }

        public override void AttachCallBack(IModel model, IView view)
        {
            m_Model = (HistDataPointDataModel)model;
            m_View = (TrendViewer.View.HistDataPointData)view;
        }

        public EtyHistDataPoint GetHistDataPoint()
        {
            return m_dp;
        }
        public override void NotifyObserver(NotifyObject notifyObj)
        {
            if (notifyObj.Name != m_View.ViewID) return;

            //will only handle the notification when the ViewID is matching.

            if (notifyObj.Type == DataNotificaitonConst.DPSelected)
            {
                EtyEntity dp = (EtyEntity)(notifyObj.Body);
                m_dp.EntityKey = dp.Pkey;  //sometimes it maybe doesn't have pkey(when the dp is not selected from the select page)
                m_dp.DPName = dp.Name;
                if (dp.Name.CompareTo("") != 0)
                {
                    m_dp.DPLblName = dp.Name.Length > 50 ? (dp.Name).Substring(0, 50) : dp.Name;
                }
                m_View.UpdateHistDPNameAndLabel(m_dp);
            }
        }
        public void Init(List<string> dpNameList) //for "Add"
        {
            if (m_formType == FormType.Add)
            {
                m_otherHistDataPointNames = dpNameList;
            }
            else  // for edit
            {
                
            }
        }

        public void Init(DataRow dpRow, List<string> dpNameList)  // for "Edit"
        {
            m_otherHistDataPointNames = dpNameList;

            m_originalName = dpRow["DATA_PT_NAME"].ToString();

            m_dp.DPHost = dpRow["DATA_PT_HOST"].ToString();
            m_dp.DPName = dpRow["DATA_PT_NAME"].ToString();
            m_dp.DPType = TrendViewerHelper.convertLineTypeToEnum(dpRow["DATA_PT_TYPE"].ToString());
            m_dp.DPColor = dpRow["DATA_PT_COLOR"].ToString();
            m_dp.DPServer = dpRow["DATA_PT_SERVER"].ToString();
            m_dp.DPLblName = dpRow["DATA_PT_LBL_NAME"].ToString();
            m_dp.DPEnabled = TrendViewerHelper.ChangeStrToBool(dpRow["DATA_PT_ENABLED"].ToString());
            m_dp.DPLblEnabled = TrendViewerHelper.ChangeStrToBool(dpRow["DATA_PT_LBL_ENABLED"].ToString());
            m_dp.DPStartDateTime = (DateTime)(dpRow["DATA_PT_DT"]);
            m_dp.DPEndDateTime = (DateTime)(dpRow["DATA_PT_DT_END"]);
            m_dp.EntityKey = Convert.ToUInt64(dpRow["ENTITYKEY"]);
            //m_dp. = TrendingHelper.ChangeStrToBool(dpRow["DATA_PT_VISIBLE"].ToString());


        }
        public bool HistDataPointNameValid(string name)
        {
            //check the new added dp's name, make sure it's not the same as any of the list
            foreach (string otherName in m_otherHistDataPointNames)
            {
                if (name == otherName)
                {
                    return false;
                }
            }
            return true;
        }

        public void AddHistDataPointData(object sender, EventArgs e)
        {
            string Function_Name = "AddHistDataPointData";
            if (m_View.GetStartDate() > m_View.GetEndDate())
            {
                MessageBoxDialog.Show(
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_StartDateMoreThenEnd, LanguageHelper.TrendViewer_Msg_StartDateMoreThenEnd_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_WarningTitle, LanguageHelper.TrendViewer_Msg_WarningTitle_EN),
                    MessageBoxButtons.OK, MessageBoxIcon.Warning
                 ); 
                return;
            }

            string dpName = m_View.GetHistDataPointName();

            if (dpName == "") return;

            if (!HistDataPointNameValid(dpName))  //means has duplicate names
            {
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DuplicateName, LanguageHelper.TrendViewer_Msg_DuplicateName_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN), MessageBoxButtons.OK, MessageBoxIcon.Warning);

                return;
            }

            EtyHistDataPoint histDP = m_View.GetNewHistDataPoint();

            try
            {
                histDP.EntityKey = m_Model.GetEntityKeyByName(histDP.DPName); //update the entityKey
            }
            catch (DatabaseConnectionException ex)
            {
//                 MessageBoxDialog.Show(
//                     StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_NoDB, LanguageHelper.TrendViewer_Msg_NoDB_EN),
//                     StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
//                     MessageBoxButtons.OK, MessageBoxIcon.Error);
                STEE.ISCS.Log.LogHelper.Error(CLASS_NAME, Function_Name, "Can't get entitykey for historical datapoint - "+histDP.DPName +", due to DB error.");
            }

            NotifyManager.GetInstance().Send(DataNotificaitonConst.AddHistDataPoint, m_View.ViewID, histDP);

            m_View.DestroyView();
        }

        public void ModifyHistDataPoint(object sender, EventArgs e)
        {
            string Function_Name = "ModifyHistDataPoint";
            if (m_View.GetStartDate() > m_View.GetEndDate())
            {
                MessageBoxDialog.Show(
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_StartDateMoreThenEnd, LanguageHelper.TrendViewer_Msg_StartDateMoreThenEnd_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_WarningTitle, LanguageHelper.TrendViewer_Msg_WarningTitle_EN),
                    MessageBoxButtons.OK, MessageBoxIcon.Warning
                 ); 
                return;
            }

            string dpName = m_View.GetHistDataPointName();

            if (dpName == "") return;

            if (!HistDataPointNameValid(dpName))  //means has duplicate names
            {
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DuplicateName, LanguageHelper.TrendViewer_Msg_DuplicateName_EN),
                    StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN), MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            
            EtyHistDataPoint histDP = m_View.GetNewHistDataPoint();
            try
            {
                histDP.EntityKey = m_Model.GetEntityKeyByName(histDP.DPName);  //update the entityKey
            }
            catch (DatabaseConnectionException ex)
            {
//                 MessageBoxDialog.Show(
//                     StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_NoDB, LanguageHelper.TrendViewer_Msg_NoDB_EN),
//                     StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_ErrTitle, LanguageHelper.TrendViewer_Msg_ErrTitle_EN),
//                     MessageBoxButtons.OK, MessageBoxIcon.Error);
                STEE.ISCS.Log.LogHelper.Error(CLASS_NAME, Function_Name, "Can't get entitykey for historical datapoint - " + histDP.DPName + ", due to DB error.");
            }
            
            KeyValuePair<string, EtyHistDataPoint> pair = new KeyValuePair<string, EtyHistDataPoint>(m_originalName, histDP);
            NotifyManager.GetInstance().Send(DataNotificaitonConst.ModifyHistDataPoint, m_View.ViewID, pair);
            m_View.DestroyView();
        }

        public void BrowseDP(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.OPCDataSelector, m_View.ViewID);

            OPCDataSelectorController ctl = (OPCDataSelectorController)(view.getController());

            ctl.InitDataNodeList();

            OPCDataSelector OpcDPview = (OPCDataSelector)(view);
            OpcDPview.Display(ctl.getDataNodeMap());
            OpcDPview.ShowView(true);


        }

    }
}

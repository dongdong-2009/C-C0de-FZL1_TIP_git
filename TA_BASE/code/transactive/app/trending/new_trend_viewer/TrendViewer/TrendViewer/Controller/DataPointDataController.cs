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

namespace TrendViewer.Controller
{
    public class DataPointDataController : IController
    {
        private EtyDataPoint m_dp = new EtyDataPoint();
        private List<string> m_otherDataPointNames = new List<string>();
        private DataPointDataModel m_Model;
        private TrendViewer.View.DataPointData m_View;
        private string m_originalName = "";  //used in Modify function. keep the original dp name for modification

        FormType m_formType = FormType.Load;

        /// <summary>
        /// set form type for this control : add/edit
        /// </summary>
        /// <param name="type">FormType, add / edit</param>
        public void SetFormType(FormType type)
        {
            m_formType = type;
        }

        /// <summary>
        /// get form type
        /// </summary>
        /// <returns>the Form type of the corresponding view</returns>
        public FormType GetFormType()
        {
            return m_formType;
        }


        public override void AttachCallBack(IModel model, IView view)
        {
            m_Model = (DataPointDataModel)model;
            m_View = (TrendViewer.View.DataPointData)view;
        }

        /// <summary>
        /// get current data point
        /// </summary>
        /// <returns>the data point</returns>
        public EtyDataPoint GetDataPoint()
        {
            return m_dp;
        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            if (notifyObj.Name != m_View.ViewID) return;

            //will only handle the notification when the ViewID is matching.

           if(notifyObj.Type == DataNotificaitonConst.DPSelected)
           {
               EtyEntity dp =(EtyEntity)(notifyObj.Body);
               m_dp.PKey = dp.Pkey;
               m_dp.DPName = dp.Name;
               if (dp.Name.CompareTo("") != 0)
               {
                   m_dp.DPLblName = dp.Name.Length > 50 ? (dp.Name).Substring(0, 50) : dp.Name;
               }
               m_View.UpdateDPNameAndLabel(m_dp);
           }
        }

        /// <summary>
        /// init the existed dp names for "add" function.
        /// </summary>
        /// <param name="dpNameList">dp name list</param>
        public void Init(List<string> dpNameList) //for "Add"
        {
            if (m_formType == FormType.Add)
            {
                m_otherDataPointNames = dpNameList;
            }
            else  // for edit
            {
                
            }
        }

        /// <summary>
        /// init the current datapoint for "Edit" function.
        /// </summary>
        /// <param name="dpRow">dataRow of a datapoint detail</param>
        /// <param name="dpNameList">other datapoint name list except the current one in the group</param>
        public void Init(DataRow dpRow, List<string> dpNameList)  // for "Edit"
        {
            m_otherDataPointNames = dpNameList;

            m_originalName = dpRow["DATA_PT_NAME"].ToString();

            m_dp.DPHost = dpRow["DATA_PT_HOST"].ToString();
            m_dp.DPName = dpRow["DATA_PT_NAME"].ToString();
            m_dp.DPType = TrendViewerHelper.convertLineTypeToEnum(dpRow["DATA_PT_TYPE"].ToString());
            m_dp.DPColor = dpRow["DATA_PT_COLOR"].ToString();
            m_dp.DPServer = dpRow["DATA_PT_SERVER"].ToString();
            m_dp.DPLblName = dpRow["DATA_PT_LBL_NAME"].ToString();
            m_dp.DPEnabled = TrendViewerHelper.ChangeStrToBool(dpRow["DATA_PT_ENABLED"].ToString());
            m_dp.DPLblEnabled = TrendViewerHelper.ChangeStrToBool(dpRow["DATA_PT_LBL_ENABLED"].ToString());
            m_dp.DPVisible = TrendViewerHelper.ChangeStrToBool(dpRow["DATA_PT_VISIBLE"].ToString());
        }

        /// <summary>
        /// check whether the current name is valid, i.e., it shouldn't be the same as any of the rest dp names.
        /// </summary>
        /// <param name="name">the current dp name</param>
        /// <returns>ture if the name is valid, false if not.</returns>
        public bool DataPointNameValid(string name)
        {
            //check the new added dp's name, make sure it's not the same as any of the list
            foreach (string otherName in m_otherDataPointNames)
            {
                if (name == otherName)
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// when user click "OK" on "Add Datapoint" form
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void AddDataPointData(object sender, EventArgs e)
        {
            string dpName = m_View.GetDataPointName();

            if (dpName == "") return;

            if (!DataPointNameValid(dpName))  //means has duplicate names
            {
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DuplicateName, LanguageHelper.TrendViewer_Msg_DuplicateName_EN)
                    , "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);

                return;
            }

            NotifyManager.GetInstance().Send(DataNotificaitonConst.AddDataPoint, m_View.ViewID, m_View.GetNewDataPoint());

            m_View.DestroyView();
        }

        /// <summary>
        /// when user click "OK" of the "Edit Datapoint" form
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void ModifyDataPoint(object sender, EventArgs e)
        {
            string dpName = m_View.GetDataPointName();

            if (dpName == "") return;

            if (!DataPointNameValid(dpName))  //means has duplicate names
            {
                MessageBoxDialog.Show(StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DuplicateName, LanguageHelper.TrendViewer_Msg_DuplicateName_EN)
                    , "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            KeyValuePair<string, EtyDataPoint> pair = new KeyValuePair<string, EtyDataPoint>(m_originalName, m_View.GetNewDataPoint());

            NotifyManager.GetInstance().Send(DataNotificaitonConst.ModifyDataPoint,m_View.ViewID , pair);
            m_View.DestroyView();
        }

        /// <summary>
        /// when user click "Browse" on the AddDataPoint/EditDatapoint form
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

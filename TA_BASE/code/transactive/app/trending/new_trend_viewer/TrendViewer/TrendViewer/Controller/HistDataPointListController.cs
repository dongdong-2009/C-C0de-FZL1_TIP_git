using System;
using System.Collections.Generic;
using System.Text;
using STEE.ISCS.MVC;
using TrendViewer.Model;
using System.Windows.Forms;
using System.Data;
using Entity.Trending;
using TrendViewer.View;
using STEE.ISCS.Log;
using System.Collections.Generic;
using STEE.ISCS.MulLanguage;

namespace TrendViewer.Controller
{
   public class HistDataPointListController : IController
    {
        private const string CLASS_NAME = "HistDataPointListController";
        private DataTable m_DataPointTable = null;
        private HistDataPointListModel m_Model;
        private TrendViewer.View.HistDataPointList m_View;
        private string m_grpName = "";

        public override void AttachCallBack(IModel model, IView view)
        {
            m_Model = (HistDataPointListModel)model;
            m_View = (TrendViewer.View.HistDataPointList)view;
        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            //throw new Exception("The method or operation is not implemented.");
            //  notifyObj.Name = can be a view id
            //  notifyObj.Type==Notify data type which is defined in the DataNotificationCost.cs file

            if (notifyObj.Name != m_View.ViewID) return;

            //will only handle the notification when the ViewID is matching.
            if (notifyObj.Type == DataNotificaitonConst.SelectHistDPGroupToLoad)
            {
                FillHistDataPointTable(m_Model.GetHistDListByGrp((string)(notifyObj.Body)));
                m_grpName = (string)(notifyObj.Body);
                m_View.PopulateDataPointListDataGridView(1,m_DataPointTable);
            }
            if (notifyObj.Type == DataNotificaitonConst.SaveHistDataPointToGroup)
            {
                m_grpName = (string)(notifyObj.Body);
                SaveHistDataPointListToGroup((string)(notifyObj.Body));  

            }
            if (notifyObj.Type == DataNotificaitonConst.AddHistDataPoint)
            {
                AddHistDataPointToList((EtyHistDataPoint)(notifyObj.Body));

            }
            if (notifyObj.Type == DataNotificaitonConst.ModifyHistDataPoint)
            {
                KeyValuePair<string, EtyHistDataPoint> pair = (KeyValuePair<string, EtyHistDataPoint>)(notifyObj.Body);
                ModifyHistDataPoint(pair.Key, pair.Value);  
            }
        }

        private void SetHistDataPointRowByEty(ref DataRow dpRow, EtyHistDataPoint dataPoint)
        {

            dpRow["DATA_PT_NAME"] = dataPoint.DPName;
            dpRow["DATA_PT_TYPE"] = TrendViewerHelper.convertLineTypeToDispLan(dataPoint.DPType);
            dpRow["DATA_PT_COLOR"] = dataPoint.DPColor;
            dpRow["DATA_PT_SERVER"] = dataPoint.DPServer;
            dpRow["DATA_PT_ENABLED"] = TrendViewerHelper.ChangeBoolToStr(dataPoint.DPEnabled);
            dpRow["DATA_PT_LBL_ENABLED"] = TrendViewerHelper.ChangeBoolToStr(dataPoint.DPLblEnabled);
            dpRow["DATA_PT_LBL_NAME"] = dataPoint.DPLblName;
            dpRow["DATA_PT_DT"] = dataPoint.DPStartDateTime;
            dpRow["DATA_PT_HOST"] = dataPoint.DPHost;      
            dpRow["DATA_PT_DT_END"] = dataPoint.DPEndDateTime;
            dpRow["ENTITYKEY"] = dataPoint.EntityKey;

         }

        public void SetHistDataPointEtyByRow(ref EtyHistDataPoint dataPoint, DataRow dpRow)
        {
            dataPoint.DPHost = dpRow["DATA_PT_HOST"].ToString();
            dataPoint.DPName = dpRow["DATA_PT_NAME"].ToString();
            dataPoint.DPType = TrendViewerHelper.convertLineTypeToEnum(dpRow["DATA_PT_TYPE"].ToString());
            dataPoint.DPColor = dpRow["DATA_PT_COLOR"].ToString();
            dataPoint.DPServer = dpRow["DATA_PT_SERVER"].ToString();
            dataPoint.DPLblName = dpRow["DATA_PT_LBL_NAME"].ToString();
            dataPoint.DPEnabled = TrendViewerHelper.ChangeStrToBool(dpRow["DATA_PT_ENABLED"].ToString());
            dataPoint.DPLblEnabled = TrendViewerHelper.ChangeStrToBool(dpRow["DATA_PT_LBL_ENABLED"].ToString());
            dataPoint.DPStartDateTime = (DateTime)(dpRow["DATA_PT_DT"]);
            dataPoint.DPEndDateTime = (DateTime)(dpRow["DATA_PT_DT_END"]);
            dataPoint.EntityKey = Convert.ToUInt64(dpRow["ENTITYKEY"]);
        }

        public void ModifyHistDataPoint(string dataPointName, EtyHistDataPoint DataPoint)
        {
            foreach (DataRow dpRow in m_DataPointTable.Rows)  //will only have 1 record
            {
                if (dataPointName == dpRow["DATA_PT_NAME"].ToString())
                {
                    DataRow row = dpRow;
                    SetHistDataPointRowByEty(ref row, DataPoint);
                }
            }

            m_View.PopulateDataPointListDataGridView(1, m_DataPointTable);
        }

        public void AddHistDataPointToList(EtyHistDataPoint dataPoint)
        {
            DataRow newDataPointDataPointRow = m_DataPointTable.NewRow();
            SetHistDataPointRowByEty(ref newDataPointDataPointRow, dataPoint);

            m_DataPointTable.Rows.Add(newDataPointDataPointRow);
            m_View.PopulateDataPointListDataGridView(1, m_DataPointTable);

        }

        public void SaveHistDataPointListToGroup(string grpName)
        {
            string Function_Name = "SaveHistDataPointListToGroup";

            List<EtyHistDataPoint> dataPointList = new List<EtyHistDataPoint>();
            foreach (DataRow dataPointRow in m_DataPointTable.Rows)
            {
                EtyHistDataPoint DataPoint = new EtyHistDataPoint();
                SetHistDataPointEtyByRow(ref DataPoint, dataPointRow);
                dataPointList.Add(DataPoint);
            }

            try
            {
                m_Model.SaveHistDPListToGroup(dataPointList, grpName);
            }
            catch (Exception ex)
            {
                TrendViewerHelper.HandleEx(ex);
            }
        }

        public void InitHistDataPointTableColumns()
        {
            if (m_DataPointTable != null) return;

            // Construct Data Point History Table
            m_DataPointTable = new DataTable("TRENDVIEWER_HIST_CONFIG");

            m_DataPointTable.Columns.Add("DATA_PT_HOST", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_NAME", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_TYPE", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_COLOR", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_SERVER", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_ENABLED", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_LBL_ENABLED", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_LBL_NAME", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_DT", System.Type.GetType("System.DateTime"));
            m_DataPointTable.Columns.Add("DATA_PT_DT_END", System.Type.GetType("System.DateTime"));
            m_DataPointTable.Columns.Add("ENTITYKEY", System.Type.GetType("System.UInt64"));
            
        }

        public DataTable GetHistDataPointTable()
        {
            return m_DataPointTable;
        }

        public void CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            string Function_Name = "CellDoubleClick";
            if (e.RowIndex < 0) return;

            string dataPointName = m_View.GetHistDataPointNameByRowIndex(e.RowIndex);

            try
            {
                if (e.ColumnIndex == 1)
                {
                    if (MessageBoxDialog.Show(
                                StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DelConfirm, LanguageHelper.TrendViewer_Msg_DelConfirm_EN),
                                StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_SysMsg, LanguageHelper.TrendViewer_Msg_SysMsg_EN), 
                                MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                    {
                        DeleteByHistDataPointName(dataPointName);
                    }

                }
                else if (e.ColumnIndex == 0)
                {
                    EditHistDataPoint(dataPointName);
                }

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        public void LoadHistDataPoint(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.LoadConfigHistDataPoint, m_View.ViewID);

            HistDataPointGroupController ctl = (HistDataPointGroupController)(view.getController());

            ctl.SetFormType(FormType.Load);

            ctl.InitHistDPGrp();
            ((HistDataPointGroup)(view)).SetFormType(FormType.Load);
            ((HistDataPointGroup)view).FillConfigNameBox(ctl.GetHistDataPointGrpNames());
            ((HistDataPointGroup)view).SetCurrentGrpName(m_grpName);
            view.ShowView(true);
        }

        public void SetGrpName(string grpName)
        {
            m_grpName = grpName;
        }

        public void FillHistDataPointTable(List<EtyHistDataPoint> dataPointList)
        {
            m_DataPointTable.Rows.Clear();
            DataRow newDataPointDataPointRow = null;

            foreach (EtyHistDataPoint dp in dataPointList)
            {
                newDataPointDataPointRow = m_DataPointTable.NewRow();
                SetHistDataPointRowByEty(ref newDataPointDataPointRow, dp);
                m_DataPointTable.Rows.Add(newDataPointDataPointRow);
            }


        }

        public void DeleteByHistDataPointName(string dataPointName)
        {

            foreach (DataRow dataRow in m_DataPointTable.Select("DATA_PT_NAME = '" + dataPointName + "'"))
            {
                dataRow.Delete();
            }
            m_DataPointTable.AcceptChanges();
            m_View.PopulateDataPointListDataGridView(1, m_DataPointTable);  

        }

        // this func is changed. 
        public void EditHistDataPoint(string DataPointName)
        {
            List<string> ohterDataPointNameList = new List<string>();

            foreach (DataRow DataPoint in m_DataPointTable.Select("DATA_PT_NAME <> '" + DataPointName + "'"))
            {
                ohterDataPointNameList.Add(DataPoint["DATA_PT_NAME"].ToString());
            }

            foreach (DataRow dataRow in m_DataPointTable.Select("DATA_PT_NAME = '" + DataPointName + "'")) //will only be one row, because we don't allow duplicate name
            {

                IView view = ViewManager.GetInstance().GetView(TrendViewConst.EditHistDataPointData, m_View.ViewID);

                HistDataPointDataController ctl = (HistDataPointDataController)(view.getController());

                ctl.SetFormType(FormType.Edit);

                ctl.Init(dataRow, ohterDataPointNameList);
                ((HistDataPointData)(view)).SetFormType(FormType.Edit);
                ((HistDataPointData)(view)).Display(ctl.GetHistDataPoint());
                view.ShowView(true);

            }
        }

        public void SaveHistDataPoint(object sender, EventArgs e)
        {
            string Function_Name = "SaveHistDataPoint";
            try
            {
                IView view = ViewManager.GetInstance().GetView(TrendViewConst.SaveConfigHistDataPoint, m_View.ViewID);

                HistDataPointGroupController ctl = (HistDataPointGroupController)(view.getController());

                ctl.SetFormType(FormType.Save);

                ctl.InitHistDPGrp();
                ((HistDataPointGroup)(view)).SetFormType(FormType.Save);
                ((HistDataPointGroup)view).FillConfigNameBox(ctl.GetHistDataPointGrpNames());
                ((HistDataPointGroup)view).SetCurrentGrpName(m_grpName);
                view.ShowView(true);

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }
        public void AddHistDataPoint(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.AddHistDataPointData, m_View.ViewID);

            HistDataPointDataController ctl = (HistDataPointDataController)(view.getController());

            ctl.SetFormType(FormType.Add);

            List<string> DataPointNameList = new List<string>();

            foreach (DataRow dataRow in m_DataPointTable.Rows)
            {
                DataPointNameList.Add(dataRow["DATA_PT_NAME"].ToString());
            }

            ctl.Init(DataPointNameList);
            ((HistDataPointData)(view)).SetFormType(FormType.Add);
            ((HistDataPointData)(view)).Display(ctl.GetHistDataPoint());
            view.ShowView(true);
        }

        public void HistDataPointListDataGridView_Sorted(object sender, EventArgs e)
        {
            m_View.PopulateDataPointListDataGridView(2, m_DataPointTable);
        }
        public void PageNumdataPointListDataGridView_ValueChanged(object sender, EventArgs e)
        {
            m_View.PopulateDataPointListDataGridView(2, m_DataPointTable);
        }

        public void FormClose(object sender, EventArgs e)
        {
            KeyValuePair<string, List<EtyHistDataPoint>> pair = new KeyValuePair<string, List<EtyHistDataPoint>>(m_grpName, ConvertDataTableToList());
            NotifyManager.GetInstance().Send(DataNotificaitonConst.HistDPListClosed, m_View.ViewID, pair);
        }
        public List<EtyHistDataPoint> ConvertDataTableToList()
        {
            List<EtyHistDataPoint> dataPointList = new List<EtyHistDataPoint>();
            foreach (DataRow dataPointRow in m_DataPointTable.Rows)
            {
                EtyHistDataPoint dataPoint = new EtyHistDataPoint();
                SetHistDataPointEtyByRow(ref dataPoint, dataPointRow);
                dataPointList.Add(dataPoint);
            }
            return dataPointList;
        }
    }
}

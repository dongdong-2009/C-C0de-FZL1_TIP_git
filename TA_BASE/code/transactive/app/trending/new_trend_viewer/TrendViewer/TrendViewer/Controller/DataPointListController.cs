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
 
using STEE.ISCS.MulLanguage;

namespace TrendViewer.Controller
{
    public class DataPointListController : IController
    {
        private const string CLASS_NAME = "DataPointListController";
        private DataTable m_DataPointTable = null;
        private DataPointListModel m_Model;
        private TrendViewer.View.DataPointList m_View;
        private string m_grpName = "";

        public override void AttachCallBack(IModel model, IView view)
        {
            m_Model = (DataPointListModel)model;
            m_View = (TrendViewer.View.DataPointList)view;
        }

        public override void NotifyObserver(NotifyObject notifyObj)
        {
            //throw new Exception("The method or operation is not implemented.");
            //  notifyObj.Name = can be a view id
            //  notifyObj.Type==Notify data type which is defined in the DataNotificationCost.cs file
            
            if (notifyObj.Name != m_View.ViewID) return;

            //will only handle the notification when the ViewID is matching.
            if (notifyObj.Type == DataNotificaitonConst.SelectDPGroupToLoad)
            {
                FillDataPointTable(m_Model.GetDPListByGrp((string)(notifyObj.Body)));
                m_grpName = (string)(notifyObj.Body);
                m_View.PopulateDataPointListDataGridView(1,m_DataPointTable);
            }
            if (notifyObj.Type == DataNotificaitonConst.SaveDataPointToGroup)
            {
                SaveDataPointListToGroup((string)(notifyObj.Body));
                m_grpName = (string)(notifyObj.Body);

            }
            if (notifyObj.Type == DataNotificaitonConst.AddDataPoint)
            {
                AddDataPointToList((EtyDataPoint)(notifyObj.Body));

            }
            if (notifyObj.Type == DataNotificaitonConst.ModifyDataPoint)
            {
                KeyValuePair<string, EtyDataPoint> pair = (KeyValuePair<string, EtyDataPoint>)(notifyObj.Body);
                ModifyDataPoint(pair.Key, pair.Value);
            }
        }

        /// <summary>
        /// set datapoint row by a given datapoint entity
        /// </summary>
        /// <param name="dpRow">the datarow to be set</param>
        /// <param name="dataPoint">the datapoint entiy</param>
        private void setDataPointRowByEty(ref DataRow dpRow, EtyDataPoint dataPoint)
        {
            dpRow["DATA_PT_HOST"] = dataPoint.DPHost;
            dpRow["DATA_PT_NAME"] = dataPoint.DPName;
            dpRow["DATA_PT_TYPE"] = TrendViewerHelper.convertLineTypeToDispLan(dataPoint.DPType);
            dpRow["DATA_PT_COLOR"] = dataPoint.DPColor; 
            dpRow["DATA_PT_SERVER"] = dataPoint.DPServer;
            dpRow["DATA_PT_LBL_NAME"] = dataPoint.DPLblName;
            dpRow["DATA_PT_ENABLED"] = TrendViewerHelper.ChangeBoolToStr(dataPoint.DPEnabled);
            dpRow["DATA_PT_LBL_ENABLED"] = TrendViewerHelper.ChangeBoolToStr(dataPoint.DPLblEnabled);
            dpRow["DATA_PT_VISIBLE"] = TrendViewerHelper.ChangeBoolToStr(dataPoint.DPVisible); 
        }

        /// <summary>
        /// set a datapoint entity by a datarow
        /// </summary>
        /// <param name="dataPoint">the datapoint entity to be set</param>
        /// <param name="dpRow">the datarow</param>
        public void SetDataPointEtyByRow(ref EtyDataPoint dataPoint, DataRow dpRow)
        {
            dataPoint.DPHost = dpRow["DATA_PT_HOST"].ToString();
            dataPoint.DPName = dpRow["DATA_PT_NAME"].ToString();
            dataPoint.DPType = TrendViewerHelper.convertLineTypeToEnum( dpRow["DATA_PT_TYPE"].ToString() );
            dataPoint.DPColor = dpRow["DATA_PT_COLOR"].ToString();
            dataPoint.DPServer = dpRow["DATA_PT_SERVER"].ToString();
            dataPoint.DPLblName = dpRow["DATA_PT_LBL_NAME"].ToString();
            dataPoint.DPEnabled = TrendViewerHelper.ChangeStrToBool(dpRow["DATA_PT_ENABLED"].ToString());
            dataPoint.DPLblEnabled = TrendViewerHelper.ChangeStrToBool(dpRow["DATA_PT_LBL_ENABLED"].ToString());
            dataPoint.DPVisible = TrendViewerHelper.ChangeStrToBool(dpRow["DATA_PT_VISIBLE"].ToString());
        }

        /// <summary>
        /// modify a specified datapoint in a datarow
        /// </summary>
        /// <param name="dataPointName">the name of the datapoint to be modified</param>
        /// <param name="DataPoint">the new entity of the datapoint</param>
        public void ModifyDataPoint(string dataPointName, EtyDataPoint DataPoint)
        {
            foreach (DataRow dpRow in m_DataPointTable.Rows)  //will only have 1 record
            {
                if (dataPointName == dpRow["DATA_PT_NAME"].ToString())
                {
                    DataRow row = dpRow;
                    setDataPointRowByEty(ref row, DataPoint);
                }
            }

            m_View.PopulateDataPointListDataGridView(1, m_DataPointTable);
        }

        /// <summary>
        /// add a new datapoint to data table
        /// </summary>
        /// <param name="dataPoint">the new datapoint to be added</param>
        public void AddDataPointToList(EtyDataPoint dataPoint)
        {
            DataRow newDataPointDataPointRow = m_DataPointTable.NewRow();
            setDataPointRowByEty(ref newDataPointDataPointRow, dataPoint);

            m_DataPointTable.Rows.Add(newDataPointDataPointRow);
            m_View.PopulateDataPointListDataGridView(1, m_DataPointTable);

        }
        
        /// <summary>
        /// convert the current data table to list
        /// </summary>
        /// <returns>the list of datapoint</returns>
        public List<EtyDataPoint> ConvertDataTableToList()
        {
            List<EtyDataPoint> dataPointList = new List<EtyDataPoint>();
            foreach (DataRow dataPointRow in m_DataPointTable.Rows)
            {
                EtyDataPoint dataPoint = new EtyDataPoint();
                SetDataPointEtyByRow(ref dataPoint, dataPointRow);
                dataPointList.Add(dataPoint);
            }
            return dataPointList;
        }

        /// <summary>
        /// save datapoint list to a configuration
        /// </summary>
        /// <param name="grpName">configuration name</param>
        public void SaveDataPointListToGroup(string grpName)
        {
            string Function_Name = "SaveDataPointListToGroup";
            try
            {
                m_Model.SaveDPListToGroup(ConvertDataTableToList(), grpName);
            }
            catch(Exception ex)
            {
                TrendViewerHelper.HandleEx(ex);
            }
            
        }

        /// <summary>
        /// initialize the datapoint table
        /// </summary>
        public void InitDataPointTableColumn()
        {
            if (m_DataPointTable != null) return;

            m_DataPointTable = new DataTable("TRENDVIEWER_CONFIG");

            m_DataPointTable.Columns.Add("DATA_PT_HOST", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_NAME", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_TYPE", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_COLOR", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_SERVER", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_ENABLED", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_LBL_ENABLED", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_LBL_NAME", System.Type.GetType("System.String"));
            m_DataPointTable.Columns.Add("DATA_PT_VISIBLE", System.Type.GetType("System.String"));

        }

        /// <summary>
        /// get the data point table
        /// </summary>
        /// <returns></returns>
        public DataTable GetDataPointTable()
        {
            return m_DataPointTable;
        }

        /// <summary>
        /// when user double click "Edit" or "delete" on the table column.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            string Function_Name = "CellDoubleClick";
            if (e.RowIndex < 0) return;

            string dataPointName = m_View.GetDataPointNameByRowIndex(e.RowIndex);

            try
            {
                if (e.ColumnIndex == 1)
                {
                    if (MessageBoxDialog.Show(
                        StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_DelConfirm, LanguageHelper.TrendViewer_Msg_DelConfirm_EN),
                        StringHelper.GetInstance().getStringValue(LanguageHelper.TrendViewer_Msg_SysMsg, LanguageHelper.TrendViewer_Msg_SysMsg_EN), 
                        MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                    {
                        DeleteByDataPointName(dataPointName);
                    }

                }
                else if (e.ColumnIndex == 0)
                {
                    EditDataPoint(dataPointName);
                }

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        /// <summary>
        /// when the user click menu "Load data point configuration"
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void LoadDataPoint(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.LoadConfigDataPoint, m_View.ViewID);

            DataPointGroupController ctl = (DataPointGroupController)(view.getController());

            ctl.SetFormType(FormType.Load);

            ctl.InitDPGrp();
            ((DataPointGroup)view).SetFormType(FormType.Load);
            ((DataPointGroup)view).FillConfigNameBox(ctl.GetDataPointGrpNames());
            ((DataPointGroup)view).SetCurrentGrpName(m_grpName);
            view.ShowView(true);
        }

        /// <summary>
        /// set the current group name
        /// </summary>
        /// <param name="grpName">the group name</param>
        public void SetGrpName(string grpName)
        {
            m_grpName = grpName;
        }

        /// <summary>
        /// fill the datapoint table by the given list.
        /// </summary>
        /// <param name="dataPointList">datapoint list</param>
        public void FillDataPointTable(List<EtyDataPoint> dataPointList)
        {
            m_DataPointTable.Rows.Clear();
            DataRow newDataPointDataPointRow = null;

            foreach ( EtyDataPoint dp in dataPointList)
            {
                newDataPointDataPointRow = m_DataPointTable.NewRow();
                setDataPointRowByEty(ref newDataPointDataPointRow, dp);
                m_DataPointTable.Rows.Add(newDataPointDataPointRow);
            }
        }

        /// <summary>
        /// delete datapoint by datapoint name
        /// </summary>
        /// <param name="dataPointName">datapoint name</param>
        public void DeleteByDataPointName(string dataPointName)
        {

            foreach (DataRow dataRow in m_DataPointTable.Select("DATA_PT_NAME = '" + dataPointName + "'"))
            {
                dataRow.Delete();
            }
            m_DataPointTable.AcceptChanges();
            m_View.PopulateDataPointListDataGridView(1, m_DataPointTable);  

        }

        /// <summary>
        /// when user clicked "Edit" in a list column
        /// </summary>
        /// <param name="DataPointName">the datapoint name the user selected to edit</param>
        public void EditDataPoint(string DataPointName)
        {
            List<string> ohterDataPointNameList = new List<string>();

            foreach (DataRow DataPoint in m_DataPointTable.Select("DATA_PT_NAME <> '" + DataPointName + "'"))
            {
                ohterDataPointNameList.Add(DataPoint["DATA_PT_NAME"].ToString());
            }

            foreach (DataRow dataRow in m_DataPointTable.Select("DATA_PT_NAME = '" + DataPointName + "'")) //will only be one row, because we don't allow duplicate name
            {

                IView view = ViewManager.GetInstance().GetView(TrendViewConst.EditDataPointData, m_View.ViewID);

                DataPointDataController ctl = (DataPointDataController)(view.getController());

                ctl.SetFormType(FormType.Edit);
                
                ctl.Init(dataRow, ohterDataPointNameList);
                ((DataPointData)(view)).SetFormType(FormType.Edit);
                ((DataPointData)(view)).Display(ctl.GetDataPoint());
                view.ShowView(true);
            }
        }

        /// <summary>
        /// when user clicked "save datapoint coinfiguration" menu
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void SaveDataPoint(object sender, EventArgs e)
        {
            string Function_Name = "SaveDataPoint";
            try
            {
                IView view = ViewManager.GetInstance().GetView(TrendViewConst.SaveConfigDataPoint, m_View.ViewID);

                DataPointGroupController ctl = (DataPointGroupController)(view.getController());

                ctl.SetFormType(FormType.Save);

                ctl.InitDPGrp();
                ((DataPointGroup)view).SetFormType(FormType.Save);
                ((DataPointGroup)view).FillConfigNameBox(ctl.GetDataPointGrpNames());
                ((DataPointGroup)view).SetCurrentGrpName(m_grpName);
                view.ShowView(true);

            }
            catch (Exception localException)
            {
                LogHelper.Error(CLASS_NAME, Function_Name,localException.ToString());
            }
        }

        /// <summary>
        /// when user click "add a datapoint" menu
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void AddDataPoint(object sender, EventArgs e)
        {
            IView view = ViewManager.GetInstance().GetView(TrendViewConst.AddDataPointData, m_View.ViewID);

            DataPointDataController ctl = (DataPointDataController)(view.getController());

            ctl.SetFormType(FormType.Add);

            List<string> DataPointNameList = new List<string>();

            foreach (DataRow dataRow in m_DataPointTable.Rows)
            {
                DataPointNameList.Add(dataRow["DATA_PT_NAME"].ToString());
            }

            ctl.Init(DataPointNameList);
            ((DataPointData)(view)).SetFormType(FormType.Add);
            ((DataPointData)(view)).Display(ctl.GetDataPoint());
            view.ShowView(true);
        }

        public void DataPointListDataGridView_Sorted(object sender, EventArgs e)
        {
            m_View.PopulateDataPointListDataGridView(2, m_DataPointTable);
        }
        public void PageNumdataPointListDataGridView_ValueChanged(object sender, EventArgs e)
        {
            m_View.PopulateDataPointListDataGridView(2, m_DataPointTable);
        }

        /// <summary>
        /// when the datapoint list form is closed. NotifyManager will send notification
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void FormClose(object sender, EventArgs e)
        {
            KeyValuePair<string, List<EtyDataPoint>> pair = new KeyValuePair<string, List<EtyDataPoint>>(m_grpName, ConvertDataTableToList());
            NotifyManager.GetInstance().Send(DataNotificaitonConst.DPListClosed, m_View.ViewID,pair);
        }
        
    }
}
